#include "StdAfx.h"
#include "PythonExceptionSender.h"

void CPythonExceptionSender::Send()
{
	TraceError("%s", m_strExceptionString.c_str());
}

CPythonExceptionSender::CPythonExceptionSender()
{
}
CPythonExceptionSender::~CPythonExceptionSender()
{
}
