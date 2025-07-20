"""World's Worst Assembler for the SAP-Plus instruction set
"""

import argparse
import os

__version__="1.3"

epilog = """
A C++ header file is created with the code bytes.  If the input file is named test.asm then
the output file will be pgmTest.h.  Note that the header file does not support multiple,
non-contiguous code blocks.  Multiple blocks are supported in the output of the --monitor command.
"""

parser = argparse.ArgumentParser(description="World's Worst Assembler", epilog=epilog)
parser.add_argument('-a', '--arrays', action='store_true', help="write memory as Python arrays")
parser.add_argument('-l', '--list', action='store_true', help="write a human-readale listing")
parser.add_argument('-m', '--monitor', action='store_true', help="write Insert commands for Monitor CLI")
parser.add_argument('-v', '--version', action='version', version="%(prog)s v" +__version__+"")
parser.add_argument("filename", help="Input file name")
args = parser.parse_args()


ins = {'NOP': '00', 'OUT': '01', 'LAI': '02', 'LAM': '03', 'SAM': '04', 'TAS': '05', 'TSA': '06', 'INA': '07', 
       'DCA': '08', 'NOT': '09', 'ASL': '0a', 'TST': '0b', 'CLF': '0c', 'SEF': '0d', 'LAX': '0e', 'SAX': '0f', 
       'JMP': '10', 'JC':  '11', 'JGE': '11', 'JZ':  '12', 'JEQ': '12', 'JNC': '13', 'JLT': '13', 'JNZ': '14', 
       'JNE': '14', 'PHA': '15', 'PLA': '16', 'JSR': '17', 'RTS': '18', 'RC':  '19', 'RGE': '19', 'RZ':  '1a', 
       'REQ': '1a', 'RNC': '1b', 'RLT': '1b', 'RNZ': '1c', 'RNE': '1c', 'INS': '1d', 'DCS': '1e', 'ADI': '20', 
       'ADM': '21', 'SBI': '22', 'SBM': '23', 'ACI': '24', 'ACM': '25', 'SCI': '26', 'SCM': '27', 'CPI': '28', 
       'CPM': '29', 'CYN': '3f'}


class AssemblerError(Exception):
    def __init__(self, lineNum, msg, arg=None):
        # Custom attributes
        self.lineNum = lineNum
        self.msg = msg
        self.arg = arg
        super().__init__(self.msg)  # Call the base class constructor

    def __str__(self):
        s = f"ERROR on line {self.lineNum}: {self.msg}"
        if self.arg:
            s = f"{s}'{self.arg}'"
        return s


# assemble
#
# Two pass assembler for the SAP-Plus instruction set.  The first pass resolves all of the label values, allowing
# forward references.  The second pass outputs the code as a C++ header file that can be included in the Loader.
#
# An optional list file can be printed to stdout.
#
# The mem arrary is maintined as a dictionary of lists of bytes indexed by a starting address.  This can be
# printed out as a series of Insert commands for the Loader/Monitor.
def assemble(pass2):
    code = 0
    data = 0
    memAddr = 0
    mem[memAddr]= []
    isCode = True
    lineNum = 0
    srcDir, srcFile = os.path.split(args.filename)
    f, _ = os.path.splitext(srcFile)
    f = f[0].upper() + f[1:]
    hFilename = os.path.join(srcDir, 'pgm' + f + '.h')
    with open(args.filename, 'r') as asmFile, open(hFilename, 'w') as hFile:
        print("static const uint8_t pgm{}[] = {{".format(f), file=hFile)
        for line in asmFile:
            printLine = line.rstrip()
            lineNum += 1
            label = None
            cppBytes =  ""
            listBytes = ""
            lineAddr = "  "

            ix = line.find(';')
            if ix >= 0:
                line = line[:ix]

            parts = line.split()
            if parts:
                if len(parts) == 3 and parts[1].lower() == 'equ':
                    # Special case because the label in an EQU does not have a colon.  The later
                    # command processing code will add the value to the labels table
                    label = parts.pop(0).upper()
                elif parts[0][-1] == ':':
                    label = parts.pop(0)[0:-1].upper()
                    if not pass2:
                        # store label in the table if pass 1
                        labels[label] = code if isCode else data
                op = parts.pop(0).upper() if parts else None
                arg = parts.pop(0).upper() if parts else None
                argVal = None
                if arg:

                    if arg[0].isdigit() or arg[0] == '-':
                        argVal = int(arg, 0)
                        if argVal > 255 or argVal < -128:
                            raise AssemblerError(lineNum, "argument out of range -128..255, value=", argVal)
                        argVal = 256 + argVal if argVal < 0 else argVal
                    else:
                        argVal = labels.get(arg)
                        if argVal == None:
                            if pass2:
                                raise AssemblerError(lineNum, "label not found, label=", arg)
                            else:
                                argVal = 0 # don't fail a missing label on pass 1
                #if asmPass == 1:
                #    print("label={} op={} arg={}".format(label, op, arg))
                if op == 'DATA':
                    isCode = False
                    data = argVal if arg else data
                elif op == 'CODE':
                    isCode = True
                    code = argVal if arg else code
                    memAddr = code
                    mem[memAddr] = []
                elif op == 'EQU':
                    labels[label] = argVal
                    lineAddr = "{:02x}".format(argVal)
                elif op == 'BYTE':
                    if isCode:
                        lineAddr = "{:02x}".format(code)
                        code += 1
                    else:
                        lineAddr = "{:02x}".format(data)
                        data += 1
                elif op:
                    h = ins.get(op)
                    if not h:
                        raise AssemblerError(lineNum, "unknown instruction name, name=", op)
                    mem[memAddr].append(h) 
                    lineAddr = "{:02x}".format(code)
                    if arg:
                        cppBytes = "0x{}, 0x{:02x},".format(h, argVal)
                        listBytes = "{} {:02x}".format(h, argVal)
                        mem[memAddr].append(format(argVal, '02X')) 
                        code += 2
                    else:
                        cppBytes = "0x{},".format(h)
                        listBytes = "{}".format(h)
                        code += 1

            if pass2:
                print("  {:12}// {} {}".format(cppBytes, lineAddr, printLine), file=hFile)
                if args.list:
                    sep = '  ' if lineAddr[0] == ' ' else ': '
                    print("{}{}{:8}{}".format(lineAddr, sep, listBytes, printLine))

        print('};', file=hFile)


# printMonitor
#
# Print the code memory contents as a series of Insert commands for the Loader/Monitor.
# Multiple, non-contiguous code blocks are supported.
def printMonitor(mem):
    step = 8
    for key in mem:
        l = mem[key]
        if l:
            for x in range(0, len(l), step):
                print("i{} {}".format(format(key+x, '02X'), ' '.join(l[x:x+step])))

# printArrays
#
# Print the code memory contents as a series of Insert commands for the Loader/Monitor.
# Multiple, non-contiguous code blocks are supported.
def printArrays(mem):
    step = 8
    for key in mem:
        print("mem{:02x} = (".format(key))
        l = mem[key]
        if l:
            for x in range(0, len(l), step):
                print("    {},".format(', '.join(['0x'+y for y in l[x:x+step]])))
        print(")")


labels = {}
mem = {}
try:
    assemble(False)
    assemble(True)
except AssemblerError as e:
    print(e)
    exit(-1)
#print(labels)
if args.monitor:
    printMonitor(mem)

if args.arrays:
    printArrays(mem)
