#include "StdAfx.h"
#include "PythonUtils.h"


IPythonExceptionSender * g_pkExceptionSender = nullptr;

bool __PyCallClassMemberFunc_ByCString(PyObject *poClass, const char *c_szFunc, PyObject *poArgs, PyObject ** poRet);
bool __PyCallClassMemberFunc_ByPyString(PyObject *poClass, PyObject *poFuncName, PyObject *poArgs, PyObject ** poRet);
bool __PyCallClassMemberFunc(PyObject *poClass, PyObject *poFunc, PyObject *poArgs, PyObject ** poRet);

PyObject *Py_BadArgument()
{
	PyErr_BadArgument();
	return NULL;
}

PyObject *Py_BuildException(const char *c_pszErr, ...)
{
	if (!c_pszErr)
		PyErr_Clear();
	else
	{
		char szErrBuf[512+1];
		va_list args;
		va_start(args, c_pszErr);
		vsnprintf(szErrBuf, sizeof(szErrBuf), c_pszErr, args);
		va_end(args);

		PyErr_SetString(PyExc_RuntimeError, szErrBuf);
	}

	return Py_BuildNone();
	//return NULL;
}

PyObject *Py_BuildNone()
{
	Py_INCREF(Py_None);
	return Py_None;
}

void Py_ReleaseNone()
{
	Py_DECREF(Py_None);
}

bool PyTuple_GetObject(PyObject *poArgs, int pos, PyObject ** ret)
{
	if (pos >= PyTuple_Size(poArgs))
		return false;

	PyObject *poItem = PyTuple_GetItem(poArgs, pos);

	if (!poItem)
		return false;

	*ret = poItem;
	return true;
}

bool PyTuple_GetLong(PyObject *poArgs, int pos, long* ret)
{
	if (!poArgs || !ret)
		return false;

	if (pos >= PyTuple_Size(poArgs))
		return false;

	PyObject *poItem = PyTuple_GetItem(poArgs, pos);
	if (!poItem)
		return false;

	if (PyInt_Check(poItem))
	{
		*ret = PyInt_AS_LONG(poItem);
		return true;
	}
	else if (PyLong_Check(poItem))
	{
		*ret = static_cast<long>(PyLong_AsLongLong(poItem));
		return true;
	}

	return false;
}

bool PyTuple_GetDouble(PyObject *poArgs, int pos, double *ret)
{
	if (!poArgs || !ret)
		return false;

	if (pos >= PyTuple_Size(poArgs))
		return false;

	PyObject *poItem = PyTuple_GetItem(poArgs, pos);
	if (!poItem)
		return false;

	if (PyFloat_Check(poItem))
	{
		*ret = PyFloat_AsDouble(poItem);
		return true;
	}
	else if (PyInt_Check(poItem))
	{
		*ret = static_cast<double>(PyInt_AS_LONG(poItem));
		return true;
	}
	else if (PyLong_Check(poItem))
	{
		*ret = PyLong_AsDouble(poItem);
		return true;
	}

	return false;
}

bool PyTuple_GetFloat(PyObject *poArgs, int pos, float *ret)
{
	if (!poArgs || !ret)
		return false;

	double d;
	if (PyTuple_GetDouble(poArgs, pos, &d))
	{
		*ret = static_cast<float>(d);
		return true;
	}

	return false;
}

bool PyTuple_GetByte(PyObject *poArgs, int pos, unsigned char *ret)
{
	int val;
	bool result = PyTuple_GetInteger(poArgs, pos, &val);
	*ret = static_cast<unsigned char>(val);
	return result;
}

bool PyTuple_GetInteger(PyObject *poArgs, int pos, unsigned char *ret)
{
	int val;
	bool result = PyTuple_GetInteger(poArgs, pos, &val);
	*ret = static_cast<unsigned char>(val);
	return result;
}

bool PyTuple_GetInteger(PyObject *poArgs, int pos, WORD* ret)
{
	int val;
	bool result = PyTuple_GetInteger(poArgs, pos, &val);
	*ret = static_cast<WORD>(val);
	return result;
}

bool PyTuple_GetInteger(PyObject *poArgs, int pos, int* ret)
{
	if (!poArgs || !ret)
		return false;

	if (pos >= PyTuple_Size(poArgs))
		return false;

	PyObject *poItem = PyTuple_GetItem(poArgs, pos);
	if (!poItem)
		return false;

	if (PyInt_Check(poItem))
	{
		*ret = static_cast<int>(PyInt_AS_LONG(poItem));
		return true;
	}
	else if (PyLong_Check(poItem))
	{
		*ret = static_cast<int>(PyLong_AsLongLong(poItem));
		return true;
	}

	return false;
}

bool PyTuple_GetUnsignedLong(PyObject *poArgs, int pos, unsigned long *ret)
{
	if (!poArgs || !ret)
		return false;

	if (pos >= PyTuple_Size(poArgs))
		return false;

	PyObject *poItem = PyTuple_GetItem(poArgs, pos);
	if (!poItem)
		return false;

	if (PyInt_Check(poItem))
	{
		*ret = PyInt_AsUnsignedLongMask(poItem);
		return true;
	}
	else if (PyLong_Check(poItem))
	{
		*ret = PyLong_AsUnsignedLongMask(poItem);
		return true;
	}

	return false;
}

#ifdef ENABLE_GOLD_LIMIT
bool PyTuple_GetLongLong(PyObject *poArgs, int pos, long long* ret)
{
	if (!poArgs || !ret)
		return false;

	if (pos >= PyTuple_Size(poArgs))
		return false;

	PyObject *poItem = PyTuple_GetItem(poArgs, pos);
	if (!poItem)
		return false;

	if (PyInt_Check(poItem))
	{
		*ret = static_cast<long long>(PyInt_AS_LONG(poItem));
		return true;
	}
	else if (PyLong_Check(poItem))
	{
		*ret = PyLong_AsLongLong(poItem);
		return true;
	}

	return false;
}
#endif

bool PyTuple_GetUnsignedInteger(PyObject *poArgs, int pos, unsigned int *ret)
{
	if (!poArgs || !ret)
		return false;

	if (pos >= PyTuple_Size(poArgs))
		return false;

	PyObject *poItem = PyTuple_GetItem(poArgs, pos);
	if (!poItem)
		return false;

	if (PyInt_Check(poItem))
	{
		*ret = static_cast<unsigned int>(PyInt_AsUnsignedLongMask(poItem));
		return true;
	}
	else if (PyLong_Check(poItem))
	{
		*ret = static_cast<unsigned int>(PyLong_AsUnsignedLongMask(poItem));
		return true;
	}

	return false;
}

bool PyTuple_GetString(PyObject *poArgs, int pos, char ** ret)
{
	if (!poArgs || !ret)
		return false;

	if (pos >= PyTuple_Size(poArgs))
		return false;

	PyObject *poItem = PyTuple_GetItem(poArgs, pos);
	if (!poItem)
		return false;

	if (!PyString_Check(poItem))
		return false;

	*ret = PyString_AsString(poItem);
	return true;
}

bool PyTuple_GetBoolean(PyObject *poArgs, int pos, bool *ret)
{
	if (!poArgs || !ret)
		return false;

	if (pos >= PyTuple_Size(poArgs))
		return false;

	PyObject *poItem = PyTuple_GetItem(poArgs, pos);
	if (!poItem)
		return false;

	if (PyBool_Check(poItem))
	{
		*ret = (poItem == Py_True);
		return true;
	}
	else if (PyInt_Check(poItem))
	{
		*ret = (PyInt_AS_LONG(poItem) != 0);
		return true;
	}
	else if (PyLong_Check(poItem))
	{
		*ret = (PyLong_AsLongLong(poItem) != 0);
		return true;
	}

	*ret = (PyObject_IsTrue(poItem) != 0);
	return true;
}

bool PyCallClassMemberFunc(PyObject *poClass, PyObject *poFunc, PyObject *poArgs)
{
	PyObject *poRet;

	if (!poClass)
	{
		Py_XDECREF(poArgs);
		return false;
	}

	if (!__PyCallClassMemberFunc(poClass, poFunc, poArgs, &poRet))
		return false;

	Py_DECREF(poRet);
	return true;
}

bool PyCallClassMemberFunc(PyObject *poClass, const char *c_szFunc, PyObject *poArgs)
{
	PyObject *poRet;

	if (!poClass)
	{
		Py_XDECREF(poArgs);
		return false;
	}

	if (!__PyCallClassMemberFunc_ByCString(poClass, c_szFunc, poArgs, &poRet))
		return false;

	Py_DECREF(poRet);
	return true;
}

bool PyCallClassMemberFunc_ByPyString(PyObject *poClass, PyObject *poFuncName, PyObject *poArgs)
{
	PyObject *poRet;

	if (!poClass)
	{
		Py_XDECREF(poArgs);
		return false;
	}

	if (!__PyCallClassMemberFunc_ByPyString(poClass, poFuncName, poArgs, &poRet))
		return false;

	Py_DECREF(poRet);
	return true;
}

bool PyCallClassMemberFunc(PyObject *poClass, const char *c_szFunc, PyObject *poArgs, bool *pisRet)
{
	PyObject *poRet;

	if (!__PyCallClassMemberFunc_ByCString(poClass, c_szFunc, poArgs, &poRet))
		return false;

	if (poRet && PyNumber_Check(poRet))
	{
		if (PyInt_Check(poRet))
			*pisRet = (PyInt_AS_LONG(poRet) != 0);
		else if (PyLong_Check(poRet))
			*pisRet = (PyLong_AsLongLong(poRet) != 0);
		else
			*pisRet = (PyObject_IsTrue(poRet) != 0);
	}
	else
		*pisRet = true;

	Py_XDECREF(poRet);
	return true;
}

bool PyCallClassMemberFunc(PyObject *poClass, const char *c_szFunc, PyObject *poArgs, long * plRetValue)
{
	PyObject *poRet;

	if (!__PyCallClassMemberFunc_ByCString(poClass, c_szFunc, poArgs, &poRet))
		return false;

	if (poRet && PyNumber_Check(poRet))
	{
		if (PyInt_Check(poRet))
			*plRetValue = PyInt_AS_LONG(poRet);
		else if (PyLong_Check(poRet))
			*plRetValue = static_cast<long>(PyLong_AsLongLong(poRet));
		else
			*plRetValue = 0;

		Py_DECREF(poRet);
		return true;
	}

	Py_XDECREF(poRet);
	return false;
}

bool __PyCallClassMemberFunc_ByCString(PyObject *poClass, const char *c_szFunc, PyObject *poArgs, PyObject ** ppoRet)
{
	if (!poClass)
	{
		Py_XDECREF(poArgs);
		return false;
	}

	PyObject *poFunc = PyObject_GetAttrString(poClass, (char *)c_szFunc);	// New Reference

	if (!poFunc)
	{
		PyErr_Clear();
		Py_XDECREF(poArgs);
		return false;
	}

	if (!PyCallable_Check(poFunc))
	{
		Py_DECREF(poFunc);
		Py_XDECREF(poArgs);
		return false;
	}

	PyObject *poRet = PyObject_CallObject(poFunc, poArgs);	// New Reference

	if (!poRet)
	{
		if (g_pkExceptionSender)
			g_pkExceptionSender->Clear();

		PyErr_Print();

		if (g_pkExceptionSender)
			g_pkExceptionSender->Send();

		Py_DECREF(poFunc);
		Py_XDECREF(poArgs);
		return false;
	}

	*ppoRet = poRet;

	Py_DECREF(poFunc);
	Py_XDECREF(poArgs);
	return true;
}

bool __PyCallClassMemberFunc_ByPyString(PyObject *poClass, PyObject *poFuncName, PyObject *poArgs, PyObject ** ppoRet)
{
	if (!poClass)
	{
		Py_XDECREF(poArgs);
		return false;
	}

	PyObject *poFunc = PyObject_GetAttr(poClass, poFuncName);	// New Reference

	if (!poFunc)
	{
		PyErr_Clear();
		Py_XDECREF(poArgs);
		return false;
	}

	if (!PyCallable_Check(poFunc))
	{
		Py_DECREF(poFunc);
		Py_XDECREF(poArgs);
		return false;
	}

	PyObject *poRet = PyObject_CallObject(poFunc, poArgs);	// New Reference

	if (!poRet)
	{
		if (g_pkExceptionSender)
			g_pkExceptionSender->Clear();

		PyErr_Print();

		if (g_pkExceptionSender)
			g_pkExceptionSender->Send();

		Py_DECREF(poFunc);
		Py_XDECREF(poArgs);
		return false;
	}

	*ppoRet = poRet;

	Py_DECREF(poFunc);
	Py_XDECREF(poArgs);
	return true;
}

bool __PyCallClassMemberFunc(PyObject *poClass, PyObject *poFunc, PyObject *poArgs, PyObject ** ppoRet)
{
	if (!poClass)
	{
		Py_XDECREF(poArgs);
		return false;
	}

	if (!poFunc)
	{
		PyErr_Clear();
		Py_XDECREF(poArgs);
		return false;
	}

	if (!PyCallable_Check(poFunc))
	{
		Py_DECREF(poFunc);
		Py_XDECREF(poArgs);
		return false;
	}

	PyObject *poRet = PyObject_CallObject(poFunc, poArgs);	// New Reference

	if (!poRet)
	{
		PyErr_Print();
		Py_DECREF(poFunc);
		Py_XDECREF(poArgs);
		return false;
	}

	*ppoRet = poRet;

	Py_DECREF(poFunc);
	Py_XDECREF(poArgs);
	return true;
}

bool PyTuple_GetPointer(PyObject *poArgs, int pos, void ** ret)
{
	if (!poArgs || !ret)
		return false;

	*ret = nullptr;

	if (pos >= PyTuple_Size(poArgs))
		return false;

	PyObject *poItem = PyTuple_GetItem(poArgs, pos);
	if (!poItem || poItem == Py_None)
	{
		*ret = nullptr;
		return true;
	}

	if (PyLong_Check(poItem))
	{
		*ret = PyLong_AsVoidPtr(poItem);
		return true;
	}
	else if (PyInt_Check(poItem))
	{
		long val = PyInt_AS_LONG(poItem);
		if (val == 0)
		{
			*ret = nullptr;
			return true;
		}
		*ret = (void*)(uintptr_t)(unsigned long)PyInt_AsUnsignedLongMask(poItem);
		return true;
	}

	return false;
}