import sys
libpath = '..\\..\\Extern\\Py2Lib\\lib'
sys.path.append(libpath)

import imp
fp, pathname, description = imp.find_module('utils', [libpath])
utils = imp.load_module('utils', fp, pathname, description)
import cythonizer

pys = utils.findMatchedFiles(".", "*.py")
pys.remove('RootlibCythonizer.py')
moduleLst = cythonizer.run(pys, forceRecompile=TRUE)
moduleNameLst = []
sourceFileLst = []
import os.path
for m in moduleLst:
	for source in m.sources:
		base, ext = os.path.splitext(source)
		moduleName = base.split('/')[-1]
		moduleNameLst.append(moduleName)
		sourceFileLst.append(base + (".cpp" if "c++" == m.language else ".c"))

import sourceWriter

sourceFileName = sourceWriter.run(moduleNameLst, 'rootlib')
print "%s create successful." % sourceFileName
