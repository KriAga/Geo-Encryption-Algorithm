import numpy as np
import cv2
import base64

# cap = cv2.VideoCapture(0)
# capture frame by frame
# ret, frame = cap.read()
# encode frame
frame = "D:\Vit\Semester_8\China\Data.png"
encoded_string = base64.b64encode(frame)
# decode frame
decoded_string = base64.b64decode(encoded_string)
decoded_img = np.fromstring(decoded_string, dtype=np.uint8)
# decoded_img = decoded_img.reshape(frame.shape)
# show decoded frame
cv2.imshow("decoded", decoded_img)