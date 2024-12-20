#!/usr/bin/python
"""
"""

__version__ = '1.0'

import codecs
import csv
from datetime import datetime


def fixBom(inFile):
    bom = inFile.read(3)
    if bom == codecs.BOM_UTF16_LE or bom == codecs.BOM_UTF16_BE:
        # UTF16 BOM is only 2 bytes, so go back
        print('UTF-16')
        inFile.seek(2)
    elif bom != codecs.BOM_UTF8:
        # No BOM to skip
        print('no BOM')
        inFile.seek(0)


class SpecificInstructionRecord:
    def __init__(self, row, line=0):
        self.line = line
        self.name = row['Name'].strip()
        self.opcode = row['Opcode'].strip().lower()
        self.description = row['Description'].strip()
        self.bytes = row['Bytes'].strip()
        self.cycles = row['Cycles'].strip()
        self.carry = row['Carry'].strip()
        self.zero = row['Zero'].strip()
        self.text = row['Text'].strip()

    def __str__(self):
        return "{} {} {} {} {} {} {}".format(self.name, self.opcode, self.description, self.bytes, self.cycles, self.carry, self.zero)

    def tableRow(self):
        return '|' + '|'.join([self.name, self.opcode, self.description, self.bytes, self.cycles, self.carry, self.zero]) + '|'

    @staticmethod
    def tableHeader():
        return '|' + '|'.join(['Name', 'Opcode', 'Description', 'Bytes', 'Cycles', 'Carry', 'Zero']) + '|'

    @staticmethod
    def tableAlign():
        return '|' + '|'.join([':---', ':---', '---:', '---:', '---:', ':---', '|---']) +'|'


def writeFileHeader(f, title, link, excerpt):
    f.write('---\n')
    f.write('title: "{}"\n'.format(title))
    f.write('permalink: /docs/{}/\n'.format(link))
    f.write('excerpt: "{}"\n'.format(excerpt))
    f.write('---\n\n')

def writeFileFooter(f):
    dt = datetime.now().strftime("%d-%b-%Y %H:%M:%S")
    f.write('\n*this file was generated by sap-plus-instructions.py at ' + dt + '*\n')


def detailsLink(name):
        # This is a specific instruction in the form AX_LDA.  Write the specific as the
        # printable text, but use the general instruction for the link.
    linkName = name[3:6] if len(name) == 6 else name
    return "[{}](../{}#{})".format(name, detailsName, linkName.lower())



def makeEnums(filename):
    print("writing", filename)
    with open(filename, 'w') as f:
        f.write("// Instruction opcodes.\n")
        f.write('enum {\n')
        for opcode in sorted(opcodes):
            si = opcodes[opcode]
            comment = '  ' + si.description
            f.write("    N_{} = {},  // {}\n".format(si.name, opcode, comment))
        f.write('};\n')

def makeOpcodes(filename):
    print("writing", filename)
    with open(filename, 'w') as f:
        f.write("// Opcode comments - aid in verification of microcode tables\n")

        for x in range(0, 256):
            opcode = hex2(x)
            si = opcodes.get('0x' + opcode)
            if not si:
                f.write("    // {}\n".format(opcode))
            else:
                comment = ' '
                if si.cycles == 'x':
                    comment += '$'
                f.write("    // {} {} {} {}\n".format(opcode, si.name, comment, si.cycles))
        f.write('};\n')


detailHeader = """## SAP-Plus Instructions

The SAP-Plus has a single user-accessable register A that functions as the accumulator for ALU operations.
If no stack operations are needed, then the SP can also be used as a general purpose register.

The memory features two 256 byte areas that are addressed by the Memory Address Register.  The Program Memory
area stores the instructions and instruction arguments.  The data area is used for the stack and for data
storage with the Load, Store, and arithmetic operations.

The stack pointer must be initialized before the stack is used.  This can be done using LAI to load a value
into A and TAS to move it to the SP.  Because the stack grows downward, it is recomended that the SP be
initialized to the value 255 so it can use the top section of the data memory.  General data storage can
then start at zero and grow upward.
"""

def makeInstructionSummaries(filename):
    print("writing", filename)

    with open(filename, 'w') as f:
        writeFileHeader(f, 'SAP-Plus Instructions', 'in-summary','Instruction set summaries for the SAP-Plus Computer')
        f.write('\n## Instructions by Name\n\n')
        f.write('|' + '|'.join(['Name', 'Opcode', 'Description']) + '|\n')
        f.write('|' + '|'.join([':---', ':---', ':---']) + '|\n')
        for name in sorted(instructions):
            gi = instructions[name]
            f.write('|' + '|'.join([detailsLink(gi.name), gi.opcode, gi.description]) + '|\n')
        f.write('\n')

        f.write('\n## Instructions by Opcode\n\n')
        f.write('|' + '|'.join(['Opcode', 'Name', 'Description']) + '|\n')
        f.write('|' + '|'.join([':---', ':---', ':---']) + '|\n')
        for name in sorted(opcodes):
            gi = opcodes[name]
            f.write('|' + '|'.join([gi.opcode, detailsLink(gi.name), gi.description]) + '|\n')
        f.write('\n')
        writeFileFooter(f)

def makeInstructionDetails(filename):
    print("writing", filename)
    with open(filename, 'w') as f:
        writeFileHeader(f, 'SAP-Plus Instructions', 'in-details','Instruction set for the SAP-Plus Computer')
        f.write(detailHeader)
        f.write('\n\n')

        # Write a table of links to the instructions
        n = 0
        cols = 8
        f.write('|' + '           |' * cols + '\n')
        f.write('|' + ':---:      |' * cols)
        for name in sorted(instructions):
            gi = instructions[name]
            if n % cols == 0:
                f.write('\n|')
            f.write("[{}](#{})|".format(name, name.lower()))
            n = n + 1
        f.write('\n\n')

        # Write a detailed description of each instruction
        for name in sorted(instructions):
            gi = instructions[name]
            f.write('## ' + gi.name + '\n\n')
            f.write(gi.description + '\n\n')
            f.write(gi.text + '\n\n')
            f.write('**Carry Flag:** ' + gi.carry + '\n\n')
            f.write('**Zero Flag:** ' + gi.zero + '\n\n')
            f.write('|Name|Opcode|Bytes|Cycles|\n')
            f.write(':--- |:---: |:---:|:---: |\n')
            f.write("|{}|{}|{}|{}|\n".format(gi.name, gi.opcode, gi.bytes, gi.cycles))
            f.write('\n')
        writeFileFooter(f)


def hex2(i):
    return '{:02x}'.format(int(i))




# -----------------------------------------------------------------------------------------------
# -----------------------------------------------------------------------------------------------
instructions = {}
opcodes = {}

specificFile = 'instruction-set.csv'
detailsName = 'in-details'

#
wip = 0
with open(specificFile, 'r') as inFile:
    #fixBom(inFile)
    reader = csv.DictReader(inFile)
    rows = 1
    for row in reader:
        rows += 1
        si = SpecificInstructionRecord(row, rows)
        if opcodes.get(si.opcode):
            print("Duplicate opcode", si)
        opcodes[si.opcode] = si
        instructions[si.name] = si
        if si.cycles == '':
            wip += 1

makeEnums('in-enums.cpp')
makeOpcodes('in-opcodes.cpp')
makeInstructionSummaries('../_docs/in-10-summary.md')
makeInstructionDetails('../_docs/in-20-details.md')
print("opcodes={}: implemented={}, wip={}".format(len(opcodes), len(opcodes) - wip, wip))
