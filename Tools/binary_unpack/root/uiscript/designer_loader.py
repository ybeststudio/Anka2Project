import ui
if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))

import dbg
import os

class DesignerTestWindow(ui.Window):
    def __init__(self):
        ui.Window.__init__(self)
        self.BuildWindow()

    def __del__(self):
        ui.Window.__del__(self)

    def BuildWindow(self):
        try:
            pyScrLoader = ui.PythonScriptLoader()
            pyScrLoader.LoadScriptFile(self, 'uiscript/ui_designer_export.py')
            dbg.LogBox('M2 Designer: Arayuz basariyla yuklendi.')
        except Exception as e:
            import sys
            import traceback
            dbg.TraceError('M2 Designer Error: %s' % str(e))
            dbg.TraceError(traceback.format_exc())

# Auto-Start for testing
test_win = DesignerTestWindow()
test_win.Show()
