import generateCMT as gCMT
import Encryption, Decryption
import generateCMT as gCMT
import json, os, filecmp, pickle, base64

gCMT.generateCodeMapTable(5, 100, 100, 1114112)
CMT = pickle.load(open("CMT.pickle","rb"))
# print(CMT)
# with open("Data.png", "rb") as image_file:
    # Data = str(base64.b64encode(image_file.read()))
    # print(type(base64.b64encode(image_file.read())))


# from array import array
# output_file = open('Encrypted', 'wb')
# float_array = array('d', encryptedData)
# float_array.tofile(output_file)
# output_file.close()
# with open("Encrypted.pickle","wb") as fe:
#     pickle.dump(encryptedData, fe)
# for i in range(100000):
Data = open("Data.txt", "r").read()
# print("Data: ", Data)
encryptedData = Encryption.encryptString(Data, CMT)
# print("encryptedData", encryptedData)

EncryptedFile = open("Encrypted.txt","w+")
EncryptedFile.write(encryptedData)
EncryptedFile.close()

EncryptedData = open("Encrypted.txt", "r").read()
# print("EncryptedData", EncryptedData)
decryptedData = Decryption.decryptString(EncryptedData, CMT)

# print("decryptedData", decryptedData)


# fh = open("Decrypted.png", "wb")
# fh.write(decryptedData.decode('base64'))
# fh.close()
DecryptedFile = open("Decrypted.txt","w+")
# print(decryptedData)
DecryptedFile.write(decryptedData)
DecryptedFile.close()
# print(bytearray(bytes(decryptedData, encoding='ascii'), encoding='ascii'))
# decryptedData = decryptedData[2:-1]
# decryptedDataBytes = bytes(decryptedData, encoding='ascii')
# print(decryptedDataBytes)
# with open("Decrypted.png", "wb") as fh:
    # fh.write(decryptedDataBytes)
if filecmp.cmp('Data.txt', 'Decrypted.txt'):
    print("File Matched")
else:
    print("File not Matched")

if(Data != decryptedData):
    print("Jagteeeee rahooooooo")
else:
    print("Yipeeeeeee Hurrraayyyyyy")