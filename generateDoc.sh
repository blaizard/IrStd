#!/bin/bash

doxygen doxygen.doxyfile
python bin/doxygenXMLToMarkdown.py doc/xml/ docs/
