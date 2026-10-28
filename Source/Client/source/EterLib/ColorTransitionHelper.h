#pragma once

class CColorTransitionHelper
{
	public:
		CColorTransitionHelper();
		~CColorTransitionHelper();

		void Clear(const float & c_rfRed,
			const float & c_rfGreen,
			const float & c_rfBlue,
			const float & c_rfAlpha);
		void SetSrcColor(const float & c_rfRed,
			const float & c_rfGreen,
			const float & c_rfBlue,
			const float & c_rfAlpha);
		void SetTransition(const float & c_rfRed,
			const float & c_rfGreen,
			const float & c_rfBlue,
			const float & c_rfAlpha,
			const DWORD & dwDuration);
		const D3DCOLOR & GetCurColor();// { return m_dwCurColor; }

		void StartTransition();
		bool Update();

		bool isTransitionStarted() { return m_bTransitionStarted; }

	private:
 		D3DCOLOR m_dwCurColor;

		DWORD m_dwStartTime;
		DWORD m_dwDuration;

		bool m_bTransitionStarted;

		float m_fSrcRed, m_fSrcGreen, m_fSrcBlue, m_fSrcAlpha;
		float m_fDstRed, m_fDstGreen, m_fDstBlue, m_fDstAlpha;
};
