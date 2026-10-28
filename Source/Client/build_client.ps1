# ============================================================================
# >  Anka2 Client Build System
# >  Professional PowerShell Build Tool
# >  Version: 1.0
# ============================================================================

param(
    [Parameter(Position=0, Mandatory=$false)]
    [ValidateSet("build", "rebuild", "clean", "info", "help", "")]
    [string]$Command = "build",
    
    [Parameter(Position=1, Mandatory=$false)]
    [ValidateSet("Debug", "Release")]
    [string]$Config = "Release",
    
    [Parameter(Position=2, Mandatory=$false)]
    [ValidateSet("x64", "Win32")]
    [string]$Platform = "x64",

    # -Verbose PowerShell'in ortak parametresidir; ayrica tanimlanirsa script hic calismaz
    [Parameter(Mandatory=$false)]
    [switch]$NoLogo
)

# >> RENK FONKSIYONLARI <<
function Write-ColorOutput($ForegroundColor) {
    $fc = $host.UI.RawUI.ForegroundColor
    $host.UI.RawUI.ForegroundColor = $ForegroundColor
    if ($args) {
        Write-Output $args
    }
    $host.UI.RawUI.ForegroundColor = $fc
}

function Write-Header($text) {
    Write-ColorOutput Cyan "============================================================================"
    Write-ColorOutput Cyan "                    $text"
    Write-ColorOutput Cyan "============================================================================"
}

function Write-Success($text) {
    Write-ColorOutput Green "[BASARILI] $text"
}

function Write-Error($text) {
    Write-ColorOutput Red "[HATA] $text"
}

function Write-Info($text) {
    Write-ColorOutput Yellow "[BILGI] $text"
}

function Write-Warning($text) {
    Write-ColorOutput Yellow "[UYARI] $text"
}

# >> AYARLAR <<
$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$SolutionFile = Join-Path $ScriptDir "client.sln"
$BuildLogDir = Join-Path $ScriptDir "build_logs"
$DefaultConfig = "Release"
$DefaultPlatform = "x64"

# UserInterface.vcxproj TargetName: Win32 -> Metin2<Config>.exe, x64 -> Metin2<Config>_x64.exe
function Get-ClientExePath($Cfg, $Plat) {
    $Suffix = if ($Plat -eq "x64") { "_x64" } else { "" }
    return Join-Path $ScriptDir "binary\Metin2${Cfg}${Suffix}.exe"
}

# >> KULLANIM YARDIMI <<
function Show-Usage {
    Write-Host ""
    Write-Header "ANKA2 CLIENT BUILD SYSTEM"
    Write-Host ""
    Write-Info "KULLANIM:"
    Write-Host "  .\build_client.ps1 [komut] [konfigurasyon] [platform] [secenekler]"
    Write-Host ""
    Write-Info "KOMUTLAR:"
    Write-Host "  build      - Client'i derler"
    Write-Host "  rebuild    - Once temizler, sonra derler"
    Write-Host "  clean      - Derleme dosyalarini temizler"
    Write-Host "  info       - Build sistem bilgilerini gosterir"
    Write-Host "  help       - Bu yardim mesajini gosterir"
    Write-Host ""
    Write-Info "KONFIGURASYONLAR:"
    Write-Host "  Debug      - Debug build"
    Write-Host "  Release    - Release build (varsayilan)"
    Write-Host ""
    Write-Info "PLATFORM:"
    Write-Host "  x64        - 64-bit build (varsayilan)"
    Write-Host "  Win32      - 32-bit build"
    Write-Host ""
    Write-Info "SECENEKLER:"
    Write-Host "  -Verbose   - Detayli cikti gosterir"
    Write-Host "  -NoLogo    - MSBuild logosunu gizler"
    Write-Host ""
    Write-Info "ORNEKLER:"
    Write-Host "  .\build_client.ps1 build                - Release x64 build yapar"
    Write-Host "  .\build_client.ps1 build Release Win32  - Release 32-bit build yapar"
    Write-Host "  .\build_client.ps1 build Debug          - Debug build yapar"
    Write-Host "  .\build_client.ps1 rebuild Release      - Release rebuild yapar"
    Write-Host "  .\build_client.ps1 clean Debug          - Debug temizler"
    Write-Host "  .\build_client.ps1 info                 - Sistem bilgilerini gosterir"
    Write-Host ""
    Write-ColorOutput Cyan "============================================================================"
    Write-Host ""
}

if ($Command -eq "help" -or $Command -eq "") {
    Show-Usage
    exit 0
}

# >> LOG DIZINI OLUSTURMA <<
if (-not (Test-Path $BuildLogDir)) {
    New-Item -ItemType Directory -Path $BuildLogDir -Force | Out-Null
}

# >> TIMESTAMP <<
$Timestamp = Get-Date -Format "yyyyMMdd_HHmmss"
$LogFile = Join-Path $BuildLogDir "build_${Config}_${Timestamp}.log"

# >> MSBUILD PATH BULMA <<
# Projeler v145 toolset kullanir (Visual Studio 2026 = "18"). Once vswhere ile en yeni
# MSBuild aranir, bulunamazsa bilinen kurulum yollari denenir.
$MsBuildPath = $null

$VsWhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
if (Test-Path $VsWhere) {
    $Found = & $VsWhere -latest -prerelease -products * -requires Microsoft.Component.MSBuild -find "MSBuild\**\Bin\MSBuild.exe" 2>$null | Select-Object -First 1
    if ($Found -and (Test-Path $Found)) {
        $MsBuildPath = $Found
    }
}

if ($null -eq $MsBuildPath) {
    $MsBuildPaths = @()
    foreach ($Version in @("18", "2022")) {
        foreach ($Edition in @("Community", "Professional", "Enterprise", "BuildTools")) {
            $MsBuildPaths += "${env:ProgramFiles}\Microsoft Visual Studio\$Version\$Edition\MSBuild\Current\Bin\MSBuild.exe"
            $MsBuildPaths += "${env:ProgramFiles(x86)}\Microsoft Visual Studio\$Version\$Edition\MSBuild\Current\Bin\MSBuild.exe"
        }
    }

    foreach ($path in $MsBuildPaths) {
        if (Test-Path $path) {
            $MsBuildPath = $path
            break
        }
    }
}

if ($null -eq $MsBuildPath) {
    Write-Error "MSBuild bulunamadi!"
    Write-Info "Visual Studio 2026 (v145 toolset) yuklu oldugundan emin olun."
    exit 1
}

if ($MsBuildPath -match "\\2022\\") {
    Write-Warning "Visual Studio 2022 MSBuild bulundu; projeler v145 (VS 2026) toolset kullanir, build basarisiz olabilir."
}

# >> MSBUILD PARAMETRELERI <<
$Verbosity = if ($PSBoundParameters.ContainsKey("Verbose")) { "detailed" } else { "normal" }
$LoggerParams = "/fl /flp:logfile=$LogFile;verbosity=detailed"
$NoLogoParam = if ($NoLogo) { "/nologo" } else { "" }

# >> KOMUT ISLEME <<
switch ($Command.ToLower()) {
    "clean" {
        Write-Host ""
        Write-Header "CLIENT CLEAN: $Config $Platform"
        Write-Host ""
        
        $StartTime = Get-Date
        $Process = Start-Process -FilePath $MsBuildPath `
            -ArgumentList "`"$SolutionFile`"", "/t:Clean", "/p:Configuration=$Config", "/p:Platform=$Platform", "/v:minimal", $NoLogoParam `
            -NoNewWindow -Wait -PassThru
        
        if ($Process.ExitCode -ne 0) {
            Write-Error "Temizleme basarisiz oldu!"
            exit 1
        }
        
        $EndTime = Get-Date
        $Duration = $EndTime - $StartTime
        Write-Host ""
        Write-Success "Temizleme tamamlandi! (Sure: $($Duration.ToString('mm\:ss')))"
        Write-Host ""
    }
    
    "build" {
        Write-Host ""
        Write-Header "CLIENT BUILD: $Config $Platform"
        Write-Host ""
        Write-Info "Solution: $SolutionFile"
        Write-Info "Log Dosyasi: $LogFile"
        Write-Info "MSBuild: $MsBuildPath"
        Write-Host ""
        
        $StartTime = Get-Date
        $Process = Start-Process -FilePath $MsBuildPath `
            -ArgumentList "`"$SolutionFile`"", "/t:Build", "/p:Configuration=$Config", "/p:Platform=$Platform", "/v:$Verbosity", "/m", $LoggerParams, $NoLogoParam `
            -NoNewWindow -Wait -PassThru
        
        if ($Process.ExitCode -ne 0) {
            Write-Host ""
            Write-Error "Build basarisiz oldu!"
            Write-Info "Detayli log: $LogFile"
            exit 1
        }
        
        $EndTime = Get-Date
        $Duration = $EndTime - $StartTime
        
        $OutputFile = Get-ClientExePath $Config $Platform
        Write-Host ""
        Write-ColorOutput Green "============================================================================"
        Write-ColorOutput Green "                    BUILD BASARILI!"
        Write-ColorOutput Green "============================================================================"
        Write-Host ""
        Write-Success "Build tamamlandi!"
        Write-Info "Baslangic: $($StartTime.ToString('HH:mm:ss'))"
        Write-Info "Bitis: $($EndTime.ToString('HH:mm:ss'))"
        Write-Info "Sure: $($Duration.ToString('mm\:ss'))"
        Write-Info "Log Dosyasi: $LogFile"
        if (Test-Path $OutputFile) {
            $FileSize = (Get-Item $OutputFile).Length / 1MB
            Write-Info "Output: $OutputFile ($([math]::Round($FileSize, 2)) MB)"
        } else {
            Write-Warning "Output dosyasi bulunamadi: $OutputFile"
        }
        Write-Host ""
    }
    
    "rebuild" {
        Write-Host ""
        Write-Header "CLIENT REBUILD: $Config $Platform"
        Write-Host ""
        
        Write-Info "[ADIM 1/2] Temizleme yapiliyor..."
        $Process = Start-Process -FilePath $MsBuildPath `
            -ArgumentList "`"$SolutionFile`"", "/t:Clean", "/p:Configuration=$Config", "/p:Platform=$Platform", "/v:minimal", $NoLogoParam `
            -NoNewWindow -Wait -PassThru
        
        if ($Process.ExitCode -ne 0) {
            Write-Host ""
            Write-Error "Temizleme basarisiz oldu!"
            exit 1
        }
        
        Write-Success "[ADIM 1/2] Temizleme tamamlandi!"
        Write-Host ""
        Write-Info "[ADIM 2/2] Build yapiliyor..."
        Write-Host ""
        
        $StartTime = Get-Date
        $Process = Start-Process -FilePath $MsBuildPath `
            -ArgumentList "`"$SolutionFile`"", "/t:Build", "/p:Configuration=$Config", "/p:Platform=$Platform", "/v:$Verbosity", "/m", $LoggerParams, $NoLogoParam `
            -NoNewWindow -Wait -PassThru
        
        if ($Process.ExitCode -ne 0) {
            Write-Host ""
            Write-Error "Build basarisiz oldu!"
            Write-Info "Detayli log: $LogFile"
            exit 1
        }
        
        $EndTime = Get-Date
        $Duration = $EndTime - $StartTime
        
        $OutputFile = Get-ClientExePath $Config $Platform
        Write-Host ""
        Write-ColorOutput Green "============================================================================"
        Write-ColorOutput Green "                    REBUILD BASARILI!"
        Write-ColorOutput Green "============================================================================"
        Write-Host ""
        Write-Success "Rebuild tamamlandi!"
        Write-Info "Baslangic: $($StartTime.ToString('HH:mm:ss'))"
        Write-Info "Bitis: $($EndTime.ToString('HH:mm:ss'))"
        Write-Info "Sure: $($Duration.ToString('mm\:ss'))"
        Write-Info "Log Dosyasi: $LogFile"
        if (Test-Path $OutputFile) {
            $FileSize = (Get-Item $OutputFile).Length / 1MB
            Write-Info "Output: $OutputFile ($([math]::Round($FileSize, 2)) MB)"
        } else {
            Write-Warning "Output dosyasi bulunamadi: $OutputFile"
        }
        Write-Host ""
    }
    
    "info" {
        Write-Host ""
        Write-Header "CLIENT BUILD SYSTEM INFO"
        Write-Host ""
        Write-Host "Solution Dosyasi: " -NoNewline
        Write-ColorOutput White $SolutionFile
        Write-Host "MSBuild Path: " -NoNewline
        Write-ColorOutput White $MsBuildPath
        Write-Host "Log Dizini: " -NoNewline
        Write-ColorOutput White $BuildLogDir
        Write-Host "Varsayilan Konfigurasyon: " -NoNewline
        Write-ColorOutput White $DefaultConfig
        Write-Host "Varsayilan Platform: " -NoNewline
        Write-ColorOutput White $DefaultPlatform
        Write-Host ""
        
        foreach ($Plat in @("x64", "Win32")) {
            foreach ($Cfg in @("Release", "Debug")) {
                $Exe = Get-ClientExePath $Cfg $Plat
                if (Test-Path $Exe) {
                    $FileSize = (Get-Item $Exe).Length / 1MB
                    $FileDate = (Get-Item $Exe).LastWriteTime
                    Write-Success "$Cfg $Plat build bulundu: $Exe ($([math]::Round($FileSize, 2)) MB, $($FileDate.ToString('yyyy-MM-dd HH:mm:ss')))"
                } else {
                    Write-Info "$Cfg $Plat build yok"
                }
            }
        }
        Write-Host ""
    }
    
    default {
        Write-Error "Gecersiz komut: $Command"
        Show-Usage
        exit 1
    }
}

