# ============================================================================
# >  Anka2 Client VS Files Cleaner
# >  Professional PowerShell Clean Tool
# >  Version: 1.0
# ============================================================================

param(
    [Parameter(Position=0, Mandatory=$false)]
    [ValidateSet("all", "debug", "release", "distribute", "help", "")]
    [string]$Target = "all",
    
    [Parameter(Mandatory=$false)]
    [switch]$Force,
    
    [Parameter(Mandatory=$false)]
    [switch]$Verbose
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
$VsFilesDir = Join-Path $ScriptDir "vs_files"

# >> KULLANIM YARDIMI <<
function Show-Usage {
    Write-Host ""
    Write-Header "VS FILES CLEANER"
    Write-Host ""
    Write-Info "KULLANIM:"
    Write-Host "  .\clear_vsfiles.ps1 [hedef] [secenekler]"
    Write-Host ""
    Write-Info "HEDEFLER:"
    Write-Host "  all         - Tum klasorleri temizler (Debug, Release, Distribute) [varsayilan]"
    Write-Host "  debug       - Sadece Debug klasorlerini temizler"
    Write-Host "  release     - Sadece Release klasorlerini temizler"
    Write-Host "  distribute  - Sadece Distribute klasorlerini temizler"
    Write-Host "  help        - Bu yardim mesajini gosterir"
    Write-Host ""
    Write-Info "SECENEKLER:"
    Write-Host "  -Force      - Onay istemeden direkt temizler"
    Write-Host "  -Verbose    - Detayli cikti gosterir"
    Write-Host ""
    Write-Info "ORNEKLER:"
    Write-Host "  .\clear_vsfiles.ps1                    - Tum klasorleri temizler"
    Write-Host "  .\clear_vsfiles.ps1 all                - Tum klasorleri temizler"
    Write-Host "  .\clear_vsfiles.ps1 debug              - Sadece Debug klasorlerini temizler"
    Write-Host "  .\clear_vsfiles.ps1 release            - Sadece Release klasorlerini temizler"
    Write-Host "  .\clear_vsfiles.ps1 all -Force         - Onaysiz tum klasorleri temizler"
    Write-Host ""
    Write-Info "NOT:"
    Write-Host "  - Bu islem geri alinamaz!"
    Write-Host "  - vs_files klasorundeki tum projelerin ilgili klasorleri temizlenir"
    Write-Host ""
    Write-ColorOutput Cyan "============================================================================"
    Write-Host ""
}

if ($Target -eq "help") {
    Show-Usage
    exit 0
}

# >> KLASOR VARLIK KONTROLU <<
if (-not (Test-Path $VsFilesDir)) {
    Write-Error "vs_files klasoru bulunamadi: $VsFilesDir"
    exit 1
}

# >> TEMIZLENECEK KLASORLERI BELIRLEME <<
$FoldersToClean = @()
switch ($Target.ToLower()) {
    "all" {
        $FoldersToClean = @("Debug", "Release", "Distribute")
    }
    "debug" {
        $FoldersToClean = @("Debug")
    }
    "release" {
        $FoldersToClean = @("Release")
    }
    "distribute" {
        $FoldersToClean = @("Distribute")
    }
    default {
        $FoldersToClean = @("Debug", "Release", "Distribute")
    }
}

# >> BASLANGIC BILGILENDIRMESI <<
Write-Host ""
Write-Header "VS FILES CLEANER"
Write-Host ""
Write-Info "Hedef dizin: $VsFilesDir"
Write-Info "Temizlenecek klasorler: $($FoldersToClean -join ', ')"
Write-Host ""

# >> ONAY ISTEME <<
if (-not $Force) {
    Write-Warning "Bu islem geri alinamaz!"
    Write-Host ""
    $Confirm = Read-Host "Devam etmek istiyor musunuz? (E/H)"
    if ($Confirm -ne "E" -and $Confirm -ne "e") {
        Write-Host ""
        Write-Info "Islem iptal edildi."
        exit 0
    }
    Write-Host ""
}

# >> TEMIZLEME ISLEMI <<
$StartTime = Get-Date
$TotalFolders = 0
$TotalSize = 0

# >> HER PROJE KLASORUNDE GEZ <<
$ProjectDirs = Get-ChildItem -Path $VsFilesDir -Directory | Where-Object { $_.Name -ne "desktop.ini" }

foreach ($ProjectDir in $ProjectDirs) {
    $ProjectName = $ProjectDir.Name
    
    foreach ($FolderName in $FoldersToClean) {
        $CleanFolder = Join-Path $ProjectDir.FullName $FolderName
        
        if (Test-Path $CleanFolder) {
            Write-Info "[TEMIZLENIYOR] $ProjectName\$FolderName"
            
            # >> KLASOR BOYUTU HESAPLAMA <<
            $FolderSize = 0
            try {
                $FolderSize = (Get-ChildItem -Path $CleanFolder -Recurse -ErrorAction SilentlyContinue | 
                    Measure-Object -Property Length -Sum -ErrorAction SilentlyContinue).Sum
                $TotalSize += $FolderSize
                
                if ($Verbose -and $FolderSize -gt 0) {
                    $SizeMB = [math]::Round($FolderSize / 1MB, 2)
                    Write-Info "  Boyut: $SizeMB MB"
                }
            } catch {
                # Boyut hesaplanamazsa devam et
            }
            
            # >> KLASOR SILME <<
            try {
                Remove-Item -Path $CleanFolder -Recurse -Force -ErrorAction Stop
                Write-Success "[SILINDI] $ProjectName\$FolderName"
                $TotalFolders++
            } catch {
                Write-Error "[HATA] $ProjectName\$FolderName silinemedi: $_"
            }
        }
    }
}

# >> BITIS BILGILENDIRMESI <<
$EndTime = Get-Date
$Duration = $EndTime - $StartTime

Write-Host ""
Write-ColorOutput Green "============================================================================"
Write-ColorOutput Green "                    TEMIZLEME TAMAMLANDI!"
Write-ColorOutput Green "============================================================================"
Write-Host ""
Write-Success "Temizleme islemi tamamlandi!"
Write-Info "Baslangic: $($StartTime.ToString('HH:mm:ss'))"
Write-Info "Bitis: $($EndTime.ToString('HH:mm:ss'))"
Write-Info "Sure: $($Duration.ToString('mm\:ss'))"
Write-Info "Temizlenen klasor sayisi: $TotalFolders"

if ($TotalSize -gt 0) {
    $TotalSizeMB = [math]::Round($TotalSize / 1MB, 2)
    Write-Info "Toplam temizlenen boyut: $TotalSizeMB MB"
}

Write-Host ""

