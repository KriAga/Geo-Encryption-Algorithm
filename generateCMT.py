import secrets, struct, json
import pickle
import json

def generateFPPDigits(FPPB):
    maxD = 2**FPPB
    FPPDigits = len(str(maxD))
    return FPPDigits
    
def generateCodeMapTable(width,numberOfSectorsAcrossX,numberOfSectorsAcrossY,totalChars,filename):
    CMT = dict()
    CMT['config'] = dict()
    CMT['data'] = dict()
    CMT['config']['width'] = width
    CMT['config']['totalChars'] = totalChars
    CMT['config']['FPPB'] = len(str(bin(totalChars-1))[2:])
    CMT['config']['FPPDigits'] = generateFPPDigits(CMT['config']['FPPB'])
    CMT['config']['FPPDecimal'] = 10**CMT['config']['FPPDigits']
    ymin = 0
    for j in range(numberOfSectorsAcrossY):
        xmin = 0
        for i in range(numberOfSectorsAcrossX):
            if(xmin not in CMT['data']):
                CMT['data'][xmin] = dict()
            CMT['data'][xmin][ymin] = str(secrets.randbits(CMT['config']['FPPB'] * 2))
            xmin+=width
        ymin+=width
    with open(filename+".pickle","wb+") as fp:
        pickle.dump(CMT, fp)
    with open(filename+'.json', 'w') as fp:
        json.dump(CMT, fp,sort_keys=True,indent=4)