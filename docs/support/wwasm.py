"""World's Worst Assembler
"""

import argparse

__version__="1.1"

parser = argparse.ArgumentParser("World's Worst Assembler")
parser.add_argument('-c', '--code', action='store_true', help="write C code for Arduino Loader")
parser.add_argument('-l', '--list', action='store_true', help="write a human-readale listing - don't use with -c")
parser.add_argument('-m', '--monitor', action='store_true', help="write Insert commands for Monitor CLI")
parser.add_argument("filename", help="Input file name")
args = parser.parse_args()

ins = {'NOP': '00', 'OUT': '01', 'LAI': '02', 'LAM': '03', 'SAM': '04', 'TAS': '05', 'TSA': '06', 
       'INA': '07', 'DCA': '08', 'NOT': '09', 'ASL': '0a', 'TST': '0b', 'CLF': '0c', 'SEF': '0d', 
       'JMP': '10', 'JC':  '11', 'JZ':  '12', 'JNC': '13', 'JNZ': '14', 'PHA': '15', 'PLA': '16', 
       'JSR': '17', 'RTS': '18', 'RC':  '19', 'RZ':  '1a', 'RNC': '1b', 'RNZ': '1c', 
       'ADI': '20', 'ADM': '21', 'SBI': '22', 'SBM': '23', 'ACI': '24', 'ACM': '25', 'SCI': '26', 
       'SCM': '27', 'CPI': '28', 'CPM': '29', 'CYN': '3f' }

def assemble(pass2):
    code = 0
    data = 0
    memAddr = 0
    mem[memAddr]= []
    isCode = True
    lineNum = 0
    with open(args.filename, 'r') as asmFile:
        for line in asmFile:
            lineNum += 1
            outLine = None
            listLine = None
            label = None
            printLabel = ""
            comment = None
            if len(line.strip()) == 0:
                # preserve empty lines in list file
                listLine = ""
            ix = line.find(';')
            if ix >= 0:
                comment = line[ix+1:].rstrip()
                line = line[:ix]
                if pass2 and not len(line.strip()):
                    # line contains only a comment
                    outLine = "//{}".format(comment)
                    listLine = ";{}".format(comment)

            parts = line.split()
            if parts:
                if len(parts) == 3 and parts[1].lower() == 'equ':
                    # Special case because the label in an EQU does not have a colon.  The later
                    # command processing code will add the value to the labels table
                    label = parts.pop(0).upper()
                    printLabel = label
                elif parts[0][-1] == ':':
                    label = parts.pop(0)[0:-1].upper()
                    printLabel = label+':'
                    if not pass2:
                        # store label in the table if pass 1
                        labels[label] = code if isCode else data
                op = parts.pop(0).upper() if parts else None
                arg = parts.pop(0).upper() if parts else None
                argVal = None
                if arg:
                    if arg[0].isdigit():
                        argVal = int(arg, 0)
                    else:
                        argVal = labels.get(arg)
                        if argVal == None:
                            if pass2:
                                print("{}: label not found: {}".format(lineNum, arg))
                            else:
                                argVal = 0 # don't fail a missing label on pass 1
                #if asmPass == 1:
                #    print("label={} op={} arg={}".format(label, op, arg))
                if not op:
                    if printLabel:
                        listLine = "           {}".format(printLabel)
                elif op == 'DATA':
                    isCode = False
                    data = argVal if arg else data
                elif op == 'CODE':
                    isCode = True
                    code = argVal if arg else code
                    memAddr = code
                    mem[memAddr] = []
                elif op == 'EQU':
                    labels[label] = argVal
                    listLine = "{:02X}          {:8}{:8} {}".format(argVal, printLabel, op, arg)
                elif op == 'BYTE':
                    if isCode:
                        listLine = "{:02X}:         {:8}{:8}".format(code, printLabel, op)
                        code += 1
                    else:
                        listLine = "{:02X}:         {:8}{:8}".format(data, printLabel, op)
                        data += 1
                else:
                    h = ins[op]
                    mem[memAddr].append(h) 
                    if arg:
                        outLine = "    N_{:8} {},".format(op+',', hex(argVal))
                        listLine = "{:02X}: {} {:02X}   {:8}{:8} {}".format(code, h, argVal, printLabel, op, arg)
                        mem[memAddr].append(format(argVal, '02X')) 
                        code += 2
                    else:
                        outLine = "    N_{},".format(op)
                        listLine = "{:02X}: {}      {:8}{:8}".format(code, h, printLabel, op)
                        code += 1

            if outLine and args.code and pass2:
                if comment:
                    outLine = "{:24}//{}".format(outLine, comment)
                print(outLine)

            if listLine != None and args.list and pass2:
                if comment:
                    listLine = "{:36};{}".format(listLine, comment)
                print(listLine)


def printMem(mem):
    step = 8
    for key in mem:
        l = mem[key]
        if l:
            for x in range(0, len(l), step):
                print("i{} {}".format(format(key+x, '02X'), ' '.join(l[x:x+step])))


labels = {}
mem = {}
assemble(False)
assemble(True)
#print(labels)
if args.monitor:
    printMem(mem)

