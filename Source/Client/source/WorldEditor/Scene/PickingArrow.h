#pragma once

class CPickingArrows : public CScreen
{
	public:
		enum
		{
			DIRECTION_X,
			DIRECTION_Y,
			DIRECTION_Z,
			DIRECTION_XY,
			DIRECTION_YZ,
			DIRECTION_ZX,
		};

	public:
		CPickingArrows();
		~CPickingArrows();

		void SetCenterPosition (const D3DXVECTOR3 & rVector);
		void SetScale (float fScale);

		void GetCenterPosition (D3DXVECTOR3 * pv3Position);

		int Picking();
		void Unpicking();
		bool Move();
		void Render();

		static void SetArrowSets (int dwIndex);

	protected:
		float m_fScale;
		D3DXVECTOR3 m_v3Position;

		// Holding
		int m_iHoldingDirection;
		D3DXVECTOR3 m_v3HoldingPosition;
		static int m_dwIndex;
};
