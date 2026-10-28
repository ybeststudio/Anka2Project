#include "StdAfx.h"
#include "../eterPack/EterPackManager.h"
#include "../eterBase/tea.h"

DWORD g_adwEncryptKey[4];
DWORD g_adwDecryptKey[4];

#include "AccountConnector.h"

#ifdef ENABLE_PYTHON_DYNAMIC_MODULE_NAME
	#include "PythonDynamicModuleNames.h"
#endif

inline const BYTE* GetKey_20050304Myevan()
{
	volatile static DWORD s_adwKey[1938];

	volatile DWORD seed=1491971513;
	for (UINT i=0; i<BYTE(seed); i++)
	{
		seed^=2148941891;
		seed+=3592385981;
		s_adwKey[i]=seed;
	}

	return (const BYTE*)s_adwKey;
}

void CAccountConnector::__BuildClientKey_20050304Myevan()
{
	const BYTE * c_pszKey = GetKey_20050304Myevan();
	memcpy(g_adwEncryptKey, c_pszKey+157, 16);

	for (DWORD i = 0; i < 4; ++i)
		g_adwEncryptKey[i] = random();

	tea_encrypt((DWORD *) g_adwDecryptKey, (const DWORD *) g_adwEncryptKey, (const DWORD *) (c_pszKey+37), 16);
}

PyObject *packExist(PyObject *poSelf, PyObject *poArgs)
{
	char *strFileName;

	if (!PyTuple_GetString(poArgs, 0, &strFileName))
		return Py_BuildException();

	return Py_BuildValue("i", CEterPackManager::Instance().isExist(strFileName)?1:0);
}

PyObject *packGet(PyObject *poSelf, PyObject *poArgs)
{
	char *strFileName;

	if (!PyTuple_GetString(poArgs, 0, &strFileName))
		return Py_BuildException();

	const char *pcExt = strrchr(strFileName, '.');
	if (pcExt)
	{
		if ((stricmp(pcExt, ".py") == 0) ||
			(stricmp(pcExt, ".pyc") == 0) ||
			(stricmp(pcExt, ".txt") == 0))
		{
			CMappedFile file;
			const void * pData = nullptr;

			if (CEterPackManager::Instance().Get(file,strFileName,&pData))
				return Py_BuildValue("s#",pData, file.Size());
		}
	}

	return Py_BuildException();
}

void initpack()
{
	static PyMethodDef s_methods[] =
	{
		{ "Exist",		packExist,		METH_VARARGS },
		{ "Get",		packGet,		METH_VARARGS },
		{ nullptr, nullptr },
	};

#ifdef ENABLE_PYTHON_DYNAMIC_MODULE_NAME
	Py_InitModule(GetModuleName(PACK_MODULE).c_str(), s_methods);
#else
	Py_InitModule("pack", s_methods);
#endif
}