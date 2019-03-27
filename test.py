import generateCMT as gCMT
import Encryption, Decryption
import generateCMT as gCMT
import json, os, filecmp, pickle, base64,struct

gCMT.generateCodeMapTable(5, 100, 100, 2**8,"CMT8")
# dataInt = data.hex()
# char = chr(int(dataInt))
#encrypted = Encryption(char,CMT)