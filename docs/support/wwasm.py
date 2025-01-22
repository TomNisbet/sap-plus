"""World's Worst Assembler
"""

import argparse

__version__="1.0"

parser = argparse.ArgumentParser("World's Worst Assembler")
parser.add_argument('-c', '--code', action='store_true', help="write C code for Arduino Loader")
parser.add_argument('-m', '--monitor', action='store_true', help="write Insert commands for Monitor CLI")
parser.add_argument("filename", help="Input file name")
args = parser.parse_args()

ins = {'NOP': '00', 'OUT': '01', 'LAI': '02', 'LAM': '03', 'SAM': '04', 'TAS': '05', 'TSA': '06', 
       'INA': '07', 'DCA': '08', 'NOT': '09', 'ASL': '0a', 'TST': '0b', 'CLF': '0c', 'SEF': '0d', 
       'JMP': '10', 'JC':  '11', 'JZ':  '12', 'JNC': '13', 'JNZ': '14', 'PHA': '15', 'PLA': '16', 
       'JSR': '17', 'RTS': '18', 'RC':  '19', 'RZ':  '1a', 'RNC': '1b', 'RNZ': '1c', 
       'ADI': '20', 'ADM': '21', 'SBI': '22', 'SBM': '23', 'ACI': '24', 'ACM': '25', 'SCI': '26', 
       'SCM': '27', 'CPI': '28', 'CPM': '29'}

def assemble(pass2):
    code = 0
    data = 0
    mem = {}
    memAddr = 0
    mem[memAddr]= []
    isCode = True
    lineNum = 0
    with open(args.filename, 'r') as asmFile:
        for line in asmFile:
            lineNum += 1
            label = None
            comment = None
            ix = line.find(';')
            if ix >= 0:
                comment = line[ix+1:].rstrip()
                line = line[:ix]
                if pass2 and args.code and not len(line.strip()):
                    print("//{}".format(comment))
            parts = line.split()
            if not parts:
                continue

            if len(parts) == 3 and parts[1].lower() == 'equ':
                # Special case because the label in an EQU does not have a colon.  The later
                # command processing code will add the value to the lables table
                label = parts.pop(0).upper()
            elif parts[0][-1] == ':':
                label = parts.pop(0)[0:-1].upper()
                if not pass2:
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

            outLine = None
            if not op:
                continue
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
            elif op == 'BYTE':
                if isCode:
                    code += 1
                else:
                    data += 1
            else:
                h = ins[op]
                mem[memAddr].append(h) 
                if arg:
                    outLine = "    N_{:8} {},".format(op+',', hex(argVal))
                    mem[memAddr].append(format(argVal, '02X')) 
                    code += 2
                else:
                    outLine = "    N_{},".format(op)
                    code += 1

            if outLine and args.code and pass2:
                if comment:
                    outLine = "{:24} //{}".format(outLine, comment)
                print(outLine)

    if pass2 and args.monitor:
        printMem(mem)

def printMem(mem):
    step = 8
    for key in mem:
        l = mem[key]
        if l:
            for x in range(0, len(l), step):
                print("i{} {}".format(format(key+x, '02X'), ' '.join(l[x:x+step])))


labels = {}
assemble(False)
assemble(True)
#print(labels)
