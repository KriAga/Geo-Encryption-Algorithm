def splitPoint(point,decimalPrecision):
    integer = int(point*decimalPrecision/decimalPrecision)
    fraction = round((point*decimalPrecision) - integer*decimalPrecision)
    # print("integer,fraction", integer,fraction, (point*decimalPrecision), integer*decimalPrecision)
    return integer,fraction


def decrypt(x,y,CMT):
    FPPB = CMT['config']['FPPB']
    FPPDigits = CMT['config']['FPPDigits']
    FPPDecimal = 10**FPPDigits
    integerX,fractionX = splitPoint(x,FPPDecimal)
    integerY,fractionY = splitPoint(y,FPPDecimal)
    xmin = integerX - (integerX  % CMT['config']['width'])
    ymin = integerY - (integerY % CMT['config']['width'])
    key = CMT['data'][xmin][ymin]
    result =  ((((fractionX << FPPB) + fractionY)^key)%CMT['config']["totalChars"])
    # print("decrypted, x, y, integerX,fractionX,integerY,fractionY,xmin,ymin,key,result", x, y, integerX,fractionX,integerY,fractionY,xmin,ymin,key,result)
    return chr(result)

def decryptString(String, CMT):
    # print("String: ", String)
    data = String.split(" ")
    decryptedString = ""
    # print(data)
    # print(len(data) - 1)
    i = 0
    while i < len(data) - 1:
        x, y = float(data[i]), float(data[i+1])
        # print("x, y: ", x, y)
        i += 2
        decryptedString += decrypt(x, y, CMT)
    # for i in range(len(data) - 1):
    #     x, y = float(data[i]), float(data[i+1])
    #     print("x, y: ", x, y)
    #     i += 2
    #     decryptedString += decrypt(x, y, CMT)
    return decryptedString 
def decryptToByte(x,y,CMT):
    totalChars=256
    FPPB = CMT['config']['FPPB']
    FPPDigits = CMT['config']['FPPDigits']
    FPPDecimal = 10**FPPDigits
    integerX,fractionX = splitPoint(x,FPPDecimal)
    integerY,fractionY = splitPoint(y,FPPDecimal)
    xmin = integerX - (integerX  % CMT['config']['width'])
    ymin = integerY - (integerY % CMT['config']['width'])
    key = CMT['data'][xmin][ymin]
    result =  ((((fractionX << FPPB) + fractionY)^key)%totalChars)
    # print("decrypted, x, y, integerX,fractionX,integerY,fractionY,xmin,ymin,key,result", x, y, integerX,fractionX,integerY,fractionY,xmin,ymin,key,result)
    return result
def decryptToByteArray(String, CMT):
    # print("String: ", String)
    data = String.split(" ")
    decryptedArray = []
    # print(data)
    # print(len(data) - 1)
    i = 0
    while i < len(data) - 1:
        x, y = float(data[i]), float(data[i+1])
        print("x, y: ", x, y)
        i += 2
        decryptedArray.append(decryptToByte(x, y, CMT))
    # for i in range(len(data) - 1):
    #     x, y = float(data[i]), float(data[i+1])
    #     print("x, y: ", x, y)
    #     i += 2
    #     decryptedString += decrypt(x, y, CMT)
    return decryptedArray 