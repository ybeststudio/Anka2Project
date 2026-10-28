# -*- coding: utf-8 -*-
import os
import sys

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
XML_DIR = os.path.join(BASE_DIR, "make_xml")
ARCHIVER = os.path.join(BASE_DIR, "tools", "Archiver.exe")

xml_files = [
    os.path.join("make_xml", f)
    for f in os.listdir(XML_DIR)
    if f.endswith(".xml")
]

if not xml_files:
    print "ERROR: XML yok"
    sys.exit(1)

os.chdir(BASE_DIR)

cmd = '"%s" %s' % (ARCHIVER, " ".join(xml_files))
print "RUN:", cmd

ret = os.system(cmd)

if ret != 0:
    print "Archiver FAILED"
    sys.exit(1)

print "Archiver finished (output kontrol et)"
