import secrets
import struct
import generateCMT as gCMT

keyLength = 16
width = 5
numberOfSectorsAcrossX = 10
numberOfSectorsAcrossY = 10
totalChars = 256

def encrypt(character, CMT):
    totalCharacters = CMT['config']['totalChars']
    FPPB = CMT['config']['FPPB']
    integerX = secrets.choice(list(CMT['data'].keys()))
    integerY = secrets.choice(list(CMT['data'][0].keys()))
    floatingX = 0
    floatingY = 0
    for i in range(CMT['config']['FPPDigits'] - 1):
        floatingX = (10 * floatingX) + secrets.randbelow(9)
        floatingY = (10 * floatingY) + secrets.randbelow(9)
    xmin = integerX - (integerX  % CMT['config']['width'])
    ymin = integerY - (integerY % CMT['config']['width'])
    key = CMT['data'][xmin][ymin]
    # print(character)
    asciiChar = ord(str(character))
    xoredFloatingXYCord = (((floatingX << FPPB) + floatingY)^key)
    beta = xoredFloatingXYCord % totalCharacters
    if (beta > asciiChar):
        modification = totalCharacters - beta + asciiChar
    else:
        modification = asciiChar - beta
    encryptedFloatingXYCord = xoredFloatingXYCord + modification
    newXYCord = encryptedFloatingXYCord ^ key
    encryptedFloatingX = newXYCord >> FPPB
    encryptedFloatingY = newXYCord & ((2**FPPB)-1)
    encryptedFloatingX = float(encryptedFloatingX / CMT['config']['FPPDecimal'])
    encryptedFloatingY = float(encryptedFloatingY / CMT['config']['FPPDecimal'])
    encryptedX, encryptedY = float(integerX + encryptedFloatingX), float(integerY + encryptedFloatingY)
    return(encryptedX, encryptedY)


# CMT = gCMT.generateCodeMapTable(keyLength,width,numberOfSectorsAcrossX,numberOfSectorsAcrossY,totalChars)
# for i in range(10000):
#     x,y,key = encrypt('K',CMT)
#     print(x,y,key)
#     if (decrypt(x,y,CMT)) != 'K':
#         print("Decrypt: x, y,", x, y, decrypt(x,y,CMT))

def encryptByte(byte, CMT):
    totalCharacters = 256
    FPPB = CMT['config']['FPPB']
    integerX = secrets.choice(list(CMT['data'].keys()))
    integerY = secrets.choice(list(CMT['data'][0].keys()))
    floatingX = 0
    floatingY = 0
    for i in range(CMT['config']['FPPDigits'] - 1):
        floatingX = (10 * floatingX) + secrets.randbelow(9)
        floatingY = (10 * floatingY) + secrets.randbelow(9)
    xmin = integerX - (integerX  % CMT['config']['width'])
    ymin = integerY - (integerY % CMT['config']['width'])
    key = CMT['data'][xmin][ymin]
    # print(character)
    asciiChar = byte
    xoredFloatingXYCord = (((floatingX << FPPB) + floatingY)^key)
    beta = xoredFloatingXYCord % totalCharacters
    if (beta > asciiChar):
        modification = totalCharacters - beta + asciiChar
    else:
        modification = asciiChar - beta
    encryptedFloatingXYCord = xoredFloatingXYCord + modification
    newXYCord = encryptedFloatingXYCord ^ key
    encryptedFloatingX = newXYCord >> FPPB
    encryptedFloatingY = newXYCord & ((2**FPPB)-1)
    encryptedFloatingX = float(encryptedFloatingX / CMT['config']['FPPDecimal'])
    encryptedFloatingY = float(encryptedFloatingY / CMT['config']['FPPDecimal'])
    encryptedX, encryptedY = float(integerX + encryptedFloatingX), float(integerY + encryptedFloatingY)
    return(encryptedX, encryptedY)

def encryptString(String, CMT):
    encrypted = ""
    # print("String: ", String)
    for char in String:
        # print("char: ", char)
        x, y = encrypt(char, CMT)
        encrypted += str(x) + " " + str(y) + " "
        # encrypted.append(x)
        # encrypted.append(y)
    return encrypted
def encryptByteArray(Bytes,CMT):
    encrypted =""
    for byte in Bytes:
        x,y = encryptByte(byte,CMT)
        encrypted += str(x) + " " + str(y) + " "
    return encrypted