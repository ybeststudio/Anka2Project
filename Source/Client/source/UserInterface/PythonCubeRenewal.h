#pragma once

#include "StdAfx.h"

#ifdef ENABLE_RENEWAL_CUBE

#include "Packet.h"

class CPythonCubeRenewal : public CSingleton<CPythonCubeRenewal>
{

	public:
		typedef std::vector<TInfoDateCubeRenewal> TInfoStrucCubeRenewal;

	public:
		CPythonCubeRenewal();
		virtual ~CPythonCubeRenewal();


		void LoadingList();
		void ClearList();

		void ReceiveList(const TInfoDateCubeRenewal & TInfoCR);
		int GetCountList();


		const TInfoStrucCubeRenewal & GetList();

		void SetCubeRenewalHandler(PyObject *_CubeRenewalHandler){m_CubeRenewalHandler = _CubeRenewalHandler;}

	private:
		TInfoStrucCubeRenewal cube_renewal_list;
		PyObject *m_CubeRenewalHandler;
};
#endif