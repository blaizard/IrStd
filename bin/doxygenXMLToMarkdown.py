from optparse import OptionParser
import xml.etree.ElementTree
import sys
import re
import os

NEWLINE = "\n<br/>"
index = {}

def generateLink(ref):
	m = re.search('(.+)_[^_]+', ref)
	fileName = m.group(1) + ".md"
	return "%s#%s" % (fileName, ref)

"""
<parameterlist kind="param"><parameteritem>
<parameternamelist>
<parametername>str</parametername>
</parameternamelist>
<parameterdescription>
<para>(optional) Json serialized string </para></parameterdescription>
</parameteritem>
"""

def getDoc(doc):
	paraList = []

	for item in doc:
		string = ""

		if item.text:
			string += item.text

		for ref in item:

			# Handle links
			if ref.tag == 'ref':
				link = ref.get('refid')
				string += "[%s](%s)" % (ref.text, generateLink(link))

			# Handle parameter list
			elif ref.tag == 'parameterlist':
				string += "<br/>**Parameters**" + NEWLINE

				for item in ref:
					nameList = []
					for name in item.find('parameternamelist'):
						nameList.append(name.text)
					description = getDoc(item.find('parameterdescription'))

					string += "`%s` %s%s" % (", ".join(nameList), description, NEWLINE)

			# Handle return
			elif ref.tag == 'simplesect' and ref.get('kind') == 'return':
				string += "<br/>**Return**" + NEWLINE
				string += getDoc(ref)

			# Handle note
			elif ref.tag == 'simplesect' and ref.get('kind') == 'note':
				string += "`%s`" % (getDoc(ref))

			# Handle see
			elif ref.tag == 'simplesect' and ref.get('kind') == 'see':
				string += "`%s`" % (getDoc(ref))

			# Handle bold
			elif ref.tag == 'bold':
				string += "**%s**" % (ref.text)

			else:
				sys.exit("Unknown item type '" + ref.tag + "'")

			if ref.tail:
				string += ref.tail

		paraList.append(string)
	return "\n".join(paraList)

def escapeStr(str):
	newStr = str.replace("*", "\\*")
	return newStr

# Print members
def listMembers(section, memberIndex):

	output = ""

	for member in section.findall('memberdef'):
		kind = member.get('kind')
		name = member.find('name').text
		if member.find('definition') != None:
			name = member.find('definition').text
		anchor = member.get('id')

		protection = member.get('prot')
		isExplicit = True if member.get('explicit') == 'yes' else False
		isVirtual = True if member.get('virt') == 'virtual' else False
		isStatic = True if member.get('static') == 'yes' else False
		isConst = True if member.get('const') == 'yes' else False
		isInline = True if member.get('inline') == 'yes' else False

		# Check if template
		templateStr = ""
		template = member.find('templateparamlist')
		if template != None:
			templateList = []
			for param in template.findall('param'):
				templateList.append("%s %s" % (param.find('type').text, param.find('defname').text))
			templateStr = "template <%s>" % (", ".join(templateList))

		# Make the argument string
		argStr = ""
		if member.find('argsstring') != None:
			argStr = member.find('argsstring').text
		else:
			argList = []
			# Look for function arguments
			for param in member.findall('param'):
				paramName = param.find('defname')
				if paramName != None:
					argList.append(paramName.text)
			argStr = "(%s)" % (", ".join(argList))

		# Generate the documentation
		doc = ""
		brief = member.find('briefdescription')
		if brief != None:
			doc += getDoc(brief)
		detail = member.find('detaileddescription')
		if detail != None:
			if doc != "":
				doc += "\n"
			doc += getDoc(detail)

		# Update the memeber index
		memberSignature = name + argStr
		memberIndex[anchor] = memberSignature

		# Print the member
		memberName = ""
		if protection:
			memberName += "`%s` " % (protection)
		if templateStr != "":
			memberName += "`%s` " % (templateStr)
		if isVirtual:
			memberName += "`virtual` "
		if isExplicit:
			memberName += "`explicit` "
		if isStatic:
			memberName += "`static` "
		if isInline:
			memberName += "`inline` "
		memberName += "**%s**" % (escapeStr(memberSignature))

		output += "\n\n<a name='%s'></a> %s\n\n%s\n\n" % (anchor, memberName, doc)

	return output

def processFile(fileName, outputDir):
	e = xml.etree.ElementTree.parse(fileName).getroot()

	# Sanity check
	if e.tag != "doxygen":
		sys.exit("This is not a valid doxygen format, it should start with 'doxygen' tag, not '" + e.tag + "'")
	documentation = e.find('compounddef')
	if documentation == False:
		sys.exit("'compounddef' tag not found")

	anchor = documentation.get('id')
	category = documentation.find('compoundname').text

	output = "# " + category + "\n"

	# Generate the documentation
	doc = ""
	brief = documentation.find('briefdescription')
	if brief != None:
		doc += getDoc(brief)
	detail = documentation.find('detaileddescription')
	if detail != None:
		if doc != "":
			doc += "\n"
		doc += getDoc(detail)
	output += doc

	memberIndex = {}

	# List all sections
	memberStr = ""
	for section in documentation.findall('sectiondef'):
		sectionKind = section.get('kind')
		if sectionKind == 'user-defined':
			sectionName = section.find('header')
			if sectionName != None:
				memberStr += "## " + sectionName.text + "\n"
			memberStr += listMembers(section, memberIndex)
		elif sectionKind == 'define':
			memberStr += "## Define(s)\n"
			memberStr += listMembers(section, memberIndex)
		elif sectionKind == 'func':
			memberStr += "## Function(s)\n"
			memberStr += listMembers(section, memberIndex)
		else:
			sys.exit("Unknown section type '" + sectionKind + "'")

	output += "# Member(s)\n"
	for attr, value in memberIndex.iteritems():
		output += "- [%s](#%s)\n" % (value, attr)

	output += memberStr

	outputPath = outputDir + "/" + anchor + ".md"
	print "Processing " + fileName + " -> " + outputPath
	with open(outputPath, 'w') as f:
		f.write(output)

	# Update the index
	index[anchor + ".md"] = category

parser = OptionParser()
parser.add_option("-f", "--file", dest="filename",
                  help="write report to FILE", metavar="FILE")
parser.add_option("-q", "--quiet",
                  action="store_false", dest="verbose", default=True,
                  help="don't print status messages to stdout")

(options, args) = parser.parse_args()

if len(args) != 2:
	sys.exit("Usage: <XML directory> <output>")

xmlDirectory = args[0]
outputDir = args[1]

if not os.path.exists(outputDir):
	os.makedirs(outputDir)

for fileName in os.listdir(xmlDirectory):
	if fileName.startswith("group__"): 
		processFile(xmlDirectory + "/" + fileName, outputDir)

# Generate the index
with open(outputDir + "/index.md", 'w') as f:
	indexStr = "# Index\n"
	for attr, value in index.iteritems():
		indexStr += "- [%s](%s)\n" % (value, attr)
	f.write(indexStr)
