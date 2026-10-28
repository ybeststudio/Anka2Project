if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))

import sys
import dbg
import traceback

def GetExceptionString(excTitle):
	(excType, excMsg, excTraceBack) = sys.exc_info()

	excText = ""
	excText += chr(10)

	traceLineList = traceback.extract_tb(excTraceBack)

	for traceLine in traceLineList:
		if traceLine[3]:
			excText += "%s(line:%d) %s - %s" % (traceLine[0], traceLine[1], traceLine[2], traceLine[3])
		else:
			excText += "%s(line:%d) %s" % (traceLine[0], traceLine[1], traceLine[2])

		excText+=chr(10)

	excText += chr(10)
	excText += "%s - %s:%s" % (excTitle, excType, excMsg)
	excText += chr(10)

	return excText

def Abort(excTitle):
	excText = GetExceptionString(excTitle)
	dbg.TraceError(excText)
	app.Abort()
	sys.exit()
	return 0