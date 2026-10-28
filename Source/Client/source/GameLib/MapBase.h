#pragma once

class CMapBase : public CScreen
{
	public:
		enum EMAPTYPE
		{
			MAPTYPE_INVALID,
			MAPTYPE_INDOOR,
			MAPTYPE_OUTDOOR,
		};

	public:
		CMapBase();
		virtual ~CMapBase();

		virtual void	Clear();
		virtual bool	Initialize() = 0;
		virtual bool	Destroy() = 0;
		virtual bool	Load(float x, float y, float z) = 0;

		virtual bool	Update(float fX, float fY, float fZ) = 0;
		virtual void	UpdateAroundAmbience(float fX, float fY, float fZ) = 0;
		virtual float	GetHeight(float fx, float fy) = 0;
		virtual void	OnBeginEnvironment() = 0;

#ifdef ENABLE_DIRECTX9_UPDATE
        virtual void ApplyLight(DWORD dwVersion, const D3DLIGHT9& c_rkLight) = 0;
#else
        virtual void ApplyLight(DWORD dwVersion, const D3DLIGHT8& c_rkLight) = 0;
#endif

	protected:
		virtual void	OnRender() = 0;
		virtual void	OnSetEnvironmentDataPtr() = 0;
		virtual void	OnResetEnvironmentDataPtr() = 0;

	public:
		void			Render();
		void			SetEnvironmentDataPtr(const TEnvironmentData * c_pEnvironmentData);
		void			ResetEnvironmentDataPtr(const TEnvironmentData * c_pEnvironmentData);
		bool			Enter();
		bool			Leave();
		bool			IsReady()	{ return m_bReady; }

		bool			LoadProperty();

		//////////////////////////////////////////////////////////////////////////
		// Setting
		//////////////////////////////////////////////////////////////////////////
		EMAPTYPE GetType() const { return m_eType; }
		void SetType(EMAPTYPE eType) { m_eType = eType; }

		const std::string & GetName() const { return m_strName; }
		void SetName(const std::string & cr_strName) { m_strName = cr_strName; }

		bool IsCopiedMap() const { return m_strParentMapName.length() > 0; }
		const std::string& GetParentMapName() const { return m_strParentMapName; }
		const std::string& GetMapDataDirectory() const { return IsCopiedMap() ? m_strParentMapName : m_strName; }

	protected:
		EMAPTYPE				m_eType;
		std::string				m_strName;
		std::string				m_strParentMapName;
		bool					m_bReady;

		const TEnvironmentData *	mc_pEnvironmentData;
};
