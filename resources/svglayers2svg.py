#! /usr/bin/python3
import xml.etree.ElementTree as ET
import os
import copy
import sys


# Script taken from and edited (https://github.com/james-bird/layer-to-svg/edit/master/layer2svg.py)
export_folder = os.path.splitext(sys.argv[1])[0]
tree = ET.parse(sys.argv[1])
root = tree.getroot()
listoflayers=[]

# Add these here to prevent the ns0: and other prefixes from being added. This is required to give
# the xml writer the correct namespace associations. Without this juce will not be able to properly 
# parse the output svg file
ET.register_namespace("","http://www.w3.org/2000/svg")
ET.register_namespace("sodipodi","http://sodipodi.sourceforge.net/DTD/sodipodi-0.dtd")
ET.register_namespace("inkscape", "http://www.inkscape.org/namespaces/inkscape")
ET.register_namespace("xlink", "http://www.w3.org/1999/xlink")
ET.register_namespace("inkpad", "http://inkpad.envoza.com/svg_extensions")

for g in root.findall('{http://www.w3.org/2000/svg}g'):
	name = g.get('{http://www.inkscape.org/namespaces/inkscape}label')
	listoflayers.append(name)
print(listoflayers)

if not os.path.exists(export_folder):
	os.makedirs(export_folder)

try:
	listoflayers.remove('background')
except ValueError:
	print("No background")
for counter in range(len(listoflayers)):
	lname = listoflayers[counter]
	if len( lname ) == 1:
		lname = 'char_' + str(ord( lname ))
	james=listoflayers[:]
	temp_tree = copy.deepcopy(tree)
	del james[counter]
	temp_root = temp_tree.getroot()
	for g in temp_root.findall('{http://www.w3.org/2000/svg}g'):
		name = g.get('{http://www.inkscape.org/namespaces/inkscape}label')
		if name in james:
			temp_root.remove(g)
		else:
			style = g.get('style')
			if type(style) is str:
				style = style.replace( 'display:none', 'display:inline' )
				g.set('style', style)
	temp_tree.write( os.path.join( export_folder, export_folder+"_"+lname +'.svg' ) , xml_declaration=True)
