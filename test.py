import generateCMT as gCMT
import Encryption, Decryption
import generateCMT as gCMT
import json, os, filecmp, pickle, base64,struct

gCMT.generateCodeMapTable(5, 100, 100, 1114112)
CMT = pickle.load(open("CMT.pickle","rb"))
dataFile = open("Data.png",'rb')
data = (dataFile.read())
intArray = []
for i in data:
    intArray.append(i)
print(len(intArray))
def stringToBinary(string):
    binData =[ bytes(chr(i),encoding='utf-8') for i in string]
    print(binData)
    return (b"".join(binData[:]))

# dataInt = data.hex()
# char = chr(int(dataInt))
#encrypted = Encryption(char,CMT)
# print(char)