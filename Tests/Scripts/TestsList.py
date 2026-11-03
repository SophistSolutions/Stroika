"""The regression tests Tests/Tests-Description.txt lists, one a line - "[NN]<TAB>its name" - in its order.

Imported by PrintTestsList, PrintTestName and the Visual Studio projects' BuildProjectsFiles; not run directly.
"""

import os


def ReadTests():
    """[(number, name)] for each test, in Tests-Description.txt's order"""
    result = []
    with open(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "Tests-Description.txt"), encoding="utf-8") as f:
        for line in f:
            fields = line.strip().split("\t")
            if fields[0] != "":
                result.append((fields[0].replace("[", "", 1).replace("]", "", 1), fields[1] if len(fields) > 1 else ""))
    return result
