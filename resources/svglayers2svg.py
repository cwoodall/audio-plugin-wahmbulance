#! /usr/bin/python3
import xml.etree.ElementTree as ET
import os
import copy
from typing import List
import argparse
import pathlib


def main():
    """
    Converts a svg file with layers into multiple svg files with only one layer visible in
    each layer. The output files are named after the layer name and the input file name.
    """

    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("input", help="Input svg file", type=pathlib.Path)
    args = parser.parse_args()

    # Script taken from and edited (https://github.com/james-bird/layer-to-svg/edit/master/layer2svg.py)
    svg_filename: pathlib.Path = pathlib.Path(args.input.stem)
    tree: ET.ElementTree = ET.parse(args.input)
    root: ET.Element = tree.getroot()
    layers: List[str] = []

    # Add these here to prevent the ns0: and other prefixes from being added. This is required to give
    # the xml writer the correct namespace associations. Without this juce will not be able to properly
    # parse the output svg file
    ET.register_namespace("", "http://www.w3.org/2000/svg")
    ET.register_namespace(
        "sodipodi", "http://sodipodi.sourceforge.net/DTD/sodipodi-0.dtd"
    )
    ET.register_namespace("inkscape", "http://www.inkscape.org/namespaces/inkscape")
    ET.register_namespace("xlink", "http://www.w3.org/1999/xlink")
    ET.register_namespace("inkpad", "http://inkpad.envoza.com/svg_extensions")

    # Get all of the layer names from the inskapce:label attribute
    for g in root.findall("{http://www.w3.org/2000/svg}g"):
        name = g.get("{http://www.inkscape.org/namespaces/inkscape}label")
        if name is None:
            raise Exception("Layer has no name")
        else:
            layers.append(name)

    # Create the export folder if it doesn't exist
    os.makedirs(svg_filename, exist_ok=True)

    # Remove the background layer if it exists
    try:
        layers.remove("background")
    except ValueError:
        pass

    for layer in layers:
        if len(layer) == 1:
            layer = "char_" + str(ord(layer))
        temp_tree = copy.deepcopy(tree)
        temp_root = temp_tree.getroot()
        for g in temp_root.findall("{http://www.w3.org/2000/svg}g"):
            name = g.get("{http://www.inkscape.org/namespaces/inkscape}label")
            if name in layers and name != layer:
                temp_root.remove(g)
            else:
                style = g.get("style")
                if isinstance(style, str):
                    style = style.replace("display:none", "display:inline")
                    g.set("style", style)
        layer_filename = svg_filename.joinpath(f"{svg_filename}_{layer}.svg")
        temp_tree.write(
            layer_filename,
            xml_declaration=True,
        )

        with open(layer_filename, "a") as f:
            f.write("\n")


if __name__ == "__main__":
    main()
