import Encryption, Decryption
import CMT as CMT
import json, os, filecmp, pickle, base64,struct

CMT.generateCodeMapTable(5, 100, 100, 2**64,"CMT64")
# dataInt = data.hex()
# char = chr(int(dataInt))
#encrypted = Encryption(char,CMT)
