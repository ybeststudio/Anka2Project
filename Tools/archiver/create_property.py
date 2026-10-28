import glob
import os

os.system( "PropertyGen.exe ../binary_unpack/property" )
os.system( "move property.xml ../binary_unpack/root/property.xml" )