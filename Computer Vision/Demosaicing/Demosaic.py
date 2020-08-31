import cv2
import numpy as np

kernelR = np.array(
    [[0, 1],
    [1, 0]]) / 4

kernelG = np.array(
    [[0, 0],
    [0, 1]]) / 4

kernelB = np.array(
    [[1, 0],
    [0, 0]]) / 4

images = [('C:\\Users\\Amrit\\Downloads\\COMP 425\\assignment1\\crayons_mosaic.bmp', 'C:\\Users\\Amrit\\Downloads\\COMP 425\\assignment1\\crayons.bmp'),
          ('C:\\Users\\Amrit\\Downloads\\COMP 425\\assignment1\\oldwell_mosaic.bmp', 'C:\\Users\\Amrit\\Downloads\\COMP 425\\assignment1\\oldwell.bmp'),
          ('C:\\Users\\Amrit\\Downloads\\COMP 425\\assignment1\\pencils_mosaic.bmp', 'C:\\Users\\Amrit\\Downloads\\COMP 425\\assignment1\\pencils.bmp')]
for mosaic, original in images:
    image = cv2.imread(mosaic, cv2.IMREAD_GRAYSCALE)

    r = cv2.filter2D(image, -1, kernelR)
    g = cv2.filter2D(image, -1, kernelG)
    b = cv2.filter2D(image, -1, kernelB)

    mergedImage = cv2.merge((b, g, r))
    cv2.imshow('RGB image', mergedImage)
    cv2.waitKey()