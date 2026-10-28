#pragma once

#ifdef ENABLE_DIRECTX9_UPDATE
#include <d3d9.h>
#else
#include <d3d8.h>
#endif
#include <string>

#ifdef ENABLE_DIRECTX9_UPDATE
typedef BOOL (*PFNCONFIRMDEVICE) (D3DCAPS9& rkD3DCaps, UINT uBehavior, D3DFORMAT eD3DFmt);
#else
typedef BOOL (*PFNCONFIRMDEVICE) (D3DCAPS8& rkD3DCaps, UINT uBehavior, D3DFORMAT eD3DFmt);
#endif

enum
{
	D3DDEVICETYPE_HAL,
	D3DDEVICETYPE_REF,
	D3DDEVICETYPE_NUM,
};

struct D3D_SModeInfo
{
	UINT m_uScrWidth;
	UINT m_uScrHeight;
	UINT m_uScrDepthBit;
 	UINT m_dwD3DBehavior;
	D3DFORMAT m_eD3DFmtPixel;
	D3DFORMAT m_eD3DFmtDepthStencil;

	VOID GetString(std::string* pstEnumList);
};

class D3D_CAdapterDisplayModeList
{
	public:
		D3D_CAdapterDisplayModeList() {}
		~D3D_CAdapterDisplayModeList() {}

#ifdef ENABLE_DIRECTX9_UPDATE
		VOID Build(IDirect3D9& rkD3D, D3DFORMAT eD3DFmtDefault, UINT iAdapter);
#else
		VOID Build(IDirect3D8& rkD3D, D3DFORMAT eD3DFmtDefault, UINT iAdapter);
#endif

		UINT GetDisplayModeNum();
		UINT GetPixelFormatNum();

		const D3DDISPLAYMODE&	GetDisplayModer(UINT iD3DDM);
		const D3DFORMAT&		GetPixelFormatr(UINT iD3DFmt);

	protected:
		enum
		{
			D3DDISPLAYMODE_MAX = 100,
			D3DFORMAT_MAX = 20,

			FILTEROUT_LOWRESOLUTION_WIDTH = 640,
			FILTEROUT_LOWRESOLUTION_HEIGHT = 480,
		};

	protected:
		D3DDISPLAYMODE m_akD3DDM[D3DDISPLAYMODE_MAX];
		D3DFORMAT m_aeD3DFmt[D3DFORMAT_MAX];

		UINT m_uD3DDMNum;
		UINT m_uD3DFmtNum;

};

class D3D_CDeviceInfo
{
	public:
		D3D_CDeviceInfo() {}
		~D3D_CDeviceInfo() {}

#ifdef ENABLE_DIRECTX9_UPDATE
		BOOL Build(IDirect3D9& rkD3D, UINT iAdapter, UINT iDevType, D3D_CAdapterDisplayModeList& rkD3DADMList, PFNCONFIRMDEVICE pfnConfirmDevice);
#else
		BOOL Build(IDirect3D8& rkD3D, UINT iAdapter, UINT iDevType, D3D_CAdapterDisplayModeList& rkD3DADMList, PFNCONFIRMDEVICE pfnConfirmDevice);
#endif

		BOOL Find(UINT uScrWidth, UINT uScrHeight, UINT uScrDepthBits, BOOL isWindowed, UINT* piD3DModeInfo);

		UINT GetD3DModeInfoNum();

		VOID GetString(std::string* pstEnumList);

#ifdef ENABLE_DIRECTX9_UPDATE
		BOOL FindDepthStencilFormat(IDirect3D9& rkD3D, UINT iAdapter, D3DDEVTYPE DeviceType, D3DFORMAT TargetFormat, D3DFORMAT* pDepthStencilFormat);
#else
		BOOL FindDepthStencilFormat(IDirect3D8& rkD3D, UINT iAdapter, D3DDEVTYPE DeviceType, D3DFORMAT TargetFormat, D3DFORMAT* pDepthStencilFormat);
#endif

		D3D_SModeInfo& GetD3DModeInfor(UINT iD3DModeInfo);
		D3D_SModeInfo* GetD3DModeInfop(UINT iD3DModeInfo);

	protected:
		enum
		{
			D3DMODEINFO_NUM = 150,
		};


	protected:
		const TCHAR* m_szDevDesc;

		D3DDEVTYPE	m_eD3DDevType;
#ifdef ENABLE_DIRECTX9_UPDATE
		D3DCAPS9	m_kD3DCaps;
#else
		D3DCAPS8	m_kD3DCaps;
#endif
		BOOL		m_canDoWindowed;

		UINT		m_iCurD3DModeInfo;
		UINT		m_uD3DModeInfoNum;
		D3D_SModeInfo	m_akD3DModeInfo[D3DMODEINFO_NUM];


		BOOL		m_isWindowed;

		D3DMULTISAMPLE_TYPE m_eD3DMSTWindowed;
		D3DMULTISAMPLE_TYPE m_eD3DMSTFullscreen;


	protected:
		static const CHAR*		msc_aszD3DDevDesc[D3DDEVICETYPE_NUM];
		static const D3DDEVTYPE msc_aeD3DDevType[D3DDEVICETYPE_NUM];
};

class D3D_CAdapterInfo
{
	public:
		D3D_CAdapterInfo() {}
		~D3D_CAdapterInfo() {}
		BOOL Find(UINT uScrWidth, UINT uScrHeight, UINT uScrDepthBits, BOOL isWindowed, UINT* piD3DModeInfo, UINT* piD3DDevInfo);

#ifdef ENABLE_DIRECTX9_UPDATE
		BOOL Build(IDirect3D9& rkD3D, UINT iAdapter, PFNCONFIRMDEVICE pfnConfirmDevice);
#else
		BOOL Build(IDirect3D8& rkD3D, UINT iAdapter, PFNCONFIRMDEVICE pfnConfirmDevice);
#endif

		VOID GetString(std::string* pstEnumList);

#ifdef ENABLE_DIRECTX9_UPDATE
		D3DADAPTER_IDENTIFIER9& GetIdentifier()
#else
		D3DADAPTER_IDENTIFIER8& GetIdentifier()
#endif
		{
			return m_kD3DAdapterIdentifier;
		}

		D3DDISPLAYMODE&	GetDesktopD3DDisplayModer();
		D3DDISPLAYMODE*	GetDesktopD3DDisplayModep();

		D3D_CDeviceInfo*	GetD3DDeviceInfop(UINT iD3DDevInfo);
		D3D_SModeInfo*		GetD3DModeInfop(UINT iD3DDevInfo, UINT iD3DModeInfo);

	protected:
		enum
		{
			D3DDEVICEINFO_NUM = 5,
		};


	protected:
#ifdef ENABLE_DIRECTX9_UPDATE
		D3DADAPTER_IDENTIFIER9	m_kD3DAdapterIdentifier;
#else
		D3DADAPTER_IDENTIFIER8	m_kD3DAdapterIdentifier;
#endif
		D3DDISPLAYMODE			m_kD3DDMDesktop;

		UINT			m_iCurD3DDevInfo;
		UINT			m_uD3DDevInfoNum;
		D3D_CDeviceInfo	m_akD3DDevInfo[D3DDEVICEINFO_NUM];

};

class D3D_CDisplayModeAutoDetector
{
	public:
		D3D_CDisplayModeAutoDetector();
		~D3D_CDisplayModeAutoDetector();

		BOOL Find(UINT uScrWidth, UINT uScrHeight, UINT uScrDepthBits, BOOL isWindowed, UINT* piD3DModeInfo, UINT* piD3DDevInfo, UINT* piD3DAdapterInfo);

#ifdef ENABLE_DIRECTX9_UPDATE
		BOOL Build(IDirect3D9& rkD3D, PFNCONFIRMDEVICE pfnConfirmDevice);
#else
		BOOL Build(IDirect3D8& rkD3D, PFNCONFIRMDEVICE pfnConfirmDevice);
#endif

		D3D_CAdapterInfo*	GetD3DAdapterInfop(UINT iD3DAdapterInfo);
		D3D_SModeInfo*		GetD3DModeInfop(UINT iD3DAdapterInfo, UINT iD3DDevInfo, UINT iD3DModeInfo);

		VOID GetString(std::string* pstEnumList);

	protected:
		enum
		{
			D3DADAPTERINFO_NUM = 10,
		};

	protected:
		D3D_CAdapterInfo	m_akD3DAdapterInfo[D3DADAPTERINFO_NUM];
		UINT				m_uD3DAdapterInfoCount;
};