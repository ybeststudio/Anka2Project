import glob
import os
os.chdir("xml/")

files = []
for file in glob.glob("*.xml"):
    #os.system("filearchiver xml/" + file)
	files.append(file)
	# os.system( "filearchiver xml\\" +file)

os.chdir("..")

filearchiver_cmdline = "archiver"

for myFile in files:
	filearchiver_cmdline += " xml\\" + myFile

os.system( filearchiver_cmdline )