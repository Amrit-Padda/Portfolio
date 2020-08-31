from PIL import Image
import pytesseract
from pytesseract import Output
import cv2
import re
import os 
import shutil
import numpy as np
from random import randint
import time

dir = 'C:\\Users\\Amrit\\Desktop\\Scanned files\\'

def rotate(image):
    im = Image.open(image)
    try:
        angle=360-int(re.search('(?<=Rotate: )\d+', pytesseract.image_to_osd(im)).group(0))
    except:
        angle = 0
  
    im = im.rotate(angle)
    im.save(path, "PNG")
   

def preprocess(image):
    img = cv2.imread(image)
    # Convert to gray
    img = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)

    # Apply dilation and erosion to remove some noise
    kernel = np.ones((1, 1), np.uint8)
    img = cv2.dilate(img, kernel, iterations=1)
    img = cv2.erode(img, kernel, iterations=1)    # Apply blur to smooth out the edges
    img = cv2.GaussianBlur(img, (5, 5), 0)
    img = cv2.threshold(img, 0, 255, cv2.THRESH_BINARY + cv2.THRESH_OTSU)[1]

    #cv2.imshow('img', img)
    #cv2.waitKey(0)
    return img

def makedirs():
    moveto = dir + str(randint(0, 1000000))
    os.mkdir(moveto)
    dirs = ["\\receipt", "\\info", "\\comm", "\\purchase", "\\CPAPcheck", "\\download", "\\sleepstudy", "\\consult", "\\other", "\\questionaire" ]
    
    for directory in dirs:
        os.mkdir(moveto + directory)
  
    return moveto 
move = makedirs()


for file in os.listdir(dir):
    path = os.path.join(dir, file)

    if os.path.isdir(path):
        continue
    
    rotate(path)  
  
    try:
        data = pytesseract.image_to_string(preprocess(path), output_type=Output.DICT)
        #data = pytesseract.image_to_string(im, output_type=Output.DICT)
    except:
        print(data)
        shutil.move(path, move + file)
    
    data = data["text"].replace('\\n', '\n')
  
    if "Billed" and "Facturé" in data:
        shutil.move(path, move + '\\receipt\\' + file)
 
    elif "Information du patient" and "Patient Information" in data:
        shutil.move(path, move + '\\info\\' + file)
  
    elif "Communication Log" in data:
        shutil.move(path, move + '\\comm\\' + file)

    elif "CPAP CHECK" in data:
        shutil.move(path, move + '\\CPAPcheck\\' + file)
    
    elif "Therapy Data Summary - All Data" in data:
        shutil.move(path, move + '\\download\\' + file)
    
    elif "consult" in data or "consultation" in data or "Consultation" in data or "CONSULTATION" in data:
        shutil.move(path, move + '\\consult\\' + file)
         
    elif "CLIENT: I have read and understand the warranties. I agree that I need the system BIPAP/nCPAP, I" in data:
        shutil.move(path, move + '\\purchase\\' + file)
    
    elif "Section 1" in data or "Section 2" in data or "Section 3" in data or "Section 4" in data  or "Section 5" in data  or "Questionnaire" in data or "questionnaire" in data or "I’échelle" in data or "Encerclez" in data:
        shutil.move(path, move + '\\questionaire\\' + file)
    
    elif "SLEEP STUDY" in data or "étude" in data or "STARDUST" in data or "TARDUST" in data or "RDUST" in data:
        shutil.move(path, move + '\\sleepstudy\\' + file)

    else:
        print(data)
        print("----------------------------------")
        shutil.move(path, move + '\\other\\' + file)


    '''
    im = cv2.imread(os.path.join(dir, file))
    data = pytesseract.image_to_data(im, output_type=Output.DICT)
    keys = list(data.keys())

    n_boxes = len(data['text'])
    for i in range(n_boxes):
        if int(data['conf'][i]) > 60:
            (x, y, w, h) = (data['left'][i], data['top'][i], data['width'][i], data['height'][i])
            im = cv2.rectangle(im, (x, y), (x + w, y + h), (0, 255, 0), 2)

    cv2.imshow('img', im)
    cv2.waitKey(0)
    '''

