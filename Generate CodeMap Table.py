import secrets
import struct

keyLength = 16
width = 5
numberOfSectorsAcrossX = 10
numberOfSectorsAcrossY = 10
totalChars = 256

def generateCodeMapTable(keyLength,width,numberOfSectorsAcrossX,numberOfSectorsAcrossY,totalChars):
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
            if(xmin not in CMT):
                CMT['data'][xmin] = dict()
            CMT['data'][xmin][ymin] = secrets.randbits(keyLength)
            xmin+=width
        ymin+=width
    return CMT        

def generateFPPDigits(FPPB):
    maxD = 2**FPPB
    FPPDigits = len(str(maxD))
    return FPPDigits

def generateFPPDigitsDecrypt(FPPB):
    maxD = 2**FPPB
    FPPDigits = len(str(maxD))-1
    return FPPDigits

def splitPoint(point,decimalPrecision):
    integer = int(point*decimalPrecision//decimalPrecision)
    fraction = int(point*decimalPrecision - integer*decimalPrecision)
    return integer,fraction


def decrypt(x,y,CMT):
    FPPB = CMT['config']['FPPB']
    FPPDigits = generateFPPDigits(FPPB)
    FPPDecimal = 10**FPPDigits
    integerX,fractionX = splitPoint(x,FPPDecimal)
    integerY,fractionY = splitPoint(y,FPPDecimal)
    xmin = integerX - (integerX  % CMT['config']['width'])
    ymin = integerY - (integerY % CMT['config']['width'])
    key = CMT['data'][xmin][ymin]
    result =  ((((fractionX << FPPB) + fractionY)^key)%CMT['config']["totalChars"])
    # print("decrypted, integerX,fractionX,integerY,fractionY,xmin,ymin,key,result", integerX,fractionX,integerY,fractionY,xmin,ymin,key,result)
    return chr(result)


def encrypt(character, CMT):
    totalCharacters = CMT['config']['totalChars']
    bitLength = CMT['config']['FPPB']
    integerX = secrets.choice(list(CMT['data'].keys()))
    integerY = secrets.choice(list(CMT['data'][0].keys()))
    floatingX = 0
    floatingY = 0
    for i in range(CMT['config']['FPPDigits'] - 1):
        floatingX = (10 * floatingX) + secrets.randbelow(9)
        floatingY = (10 * floatingY) + secrets.randbelow(9)
    # print("floatingX, floatingY", floatingX, floatingY)
    xmin = integerX - (integerX  % CMT['config']['width'])
    ymin = integerY - (integerY % CMT['config']['width'])
    key = CMT['data'][xmin][ymin]
    asciiChar = ord(character)
    #floatingX = int(str(x).split('.')[-1])
    #floatingY = int(str(y).split('.')[-1])
    # floatingX = int((x-integerX)*(int(CMT['config']['FPPDecimal']/10)))
    # floatingY = int((y-integerY)*(int(CMT['config']['FPPDecimal']/10)))
    # print("encrypt,integerX,floatingX,integerY,floatingY",integerX,floatingX,integerY,floatingY)
    # recievedFormatter = '{0:0' + str(bitLength) + 'b}'
    # binFloatingX = recievedFormatter.format(floatingX)
    # binFloatingY = recievedFormatter.format(floatingY)
    # binFloatingXYCord = binFloatingX+binFloatingY
    # print("binFloatingXYCord", binFloatingXYCord)
    # binKey = bin(int(key))
    # xoredFloatingXYCord = int(binFloatingXYCord,2) ^ int(binKey,2)
    # modification = totalCharacters - (xoredFloatingXYCord % totalCharacters) + asciiChar
    # print("modification", modification)
    # encryptedFloatingXYCord = xoredFloatingXYCord + modification
    # binencryptedFloatingXYCord = bin(encryptedFloatingXYCord)
    # newXYCord = int(binencryptedFloatingXYCord,2) ^ int(binKey,2)
    xoredFloatingXYCord = (((floatingX << CMT['config']['FPPB']) + floatingY)^key)
    beta = xoredFloatingXYCord % totalCharacters
    if (beta > asciiChar):
        modification = totalCharacters - beta + asciiChar
    else:
        modification = asciiChar - beta
    encryptedFloatingXYCord = xoredFloatingXYCord + modification
    newXYCord = encryptedFloatingXYCord ^ key
    # modification = beta > asciiChar ? (totalCharacters - beta + asciiChar) : (asciiChar - beta)
    # encodedFormatter = '{0:0' + str(bitLength*2) + 'b}'
    # binNewXYCord = encodedFormatter.format(newXYCord)
    encryptedFloatingX = newXYCord >> CMT['config']['FPPB']
    encryptedFloatingY = newXYCord & ((2**CMT['config']['FPPB'])-1)
    # floatingBinEncryptedX, floatingBinEncryptedY = binNewXYCord[:len(binNewXYCord)//2], binNewXYCord[len(binNewXYCord)//2:]
    # encryptedFloatingX, encryptedFloatingY = int(floatingBinEncryptedX, 2), int(floatingBinEncryptedY, 2)
    encryptedFloatingX = float(encryptedFloatingX / CMT['config']['FPPDecimal'])
    encryptedFloatingY = float(encryptedFloatingY / CMT['config']['FPPDecimal'])
    # floatFormatter = '{0:0' + str(CMT['config']['FPPDigits']) + '}'
    # encryptedFloatingX, encryptedFloatingY = floatFormatter.format(encryptedFloatingX), floatFormatter.format(encryptedFloatingY)
    # encryptedX, encryptedY = float(str(integerX) + '.' + encryptedFloatingX), float(str(integerY) +  '.' + encryptedFloatingY)
    encryptedX, encryptedY = float(integerX + encryptedFloatingX), float(integerY + encryptedFloatingY)
    # print("encryptedFloatingX, encryptedFloatingY, encryptedX, encryptedY",encryptedFloatingX, encryptedFloatingY, encryptedX, encryptedY)
    return(encryptedX, encryptedY,key)


# CMT = generateCodeMapTable(keyLength,width,numberOfSectorsAcrossX,numberOfSectorsAcrossY,totalChars)
# for i in range(100000):
#     x,y,key = encrypt('K',CMT)
#     # print(x,y,key)
#     if (decrypt(x,y,CMT)) != 'K':
#         print("Decrypt: x, y,", x, y, decrypt(x,y,CMT))

#splitPoint(10.23,100)
#decrypt(13.32,16.21,CMT,8)