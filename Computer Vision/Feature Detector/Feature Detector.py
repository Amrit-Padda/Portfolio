import cv2
import numpy as np
import math

def featurematcher(descriptor1, descriptor2):
    matchvector = []
    bestmatch = 0
    secondbestmatch = 1
    bestindexes = (0,0)
    secondbestindexes = (0,0)
    
    for vector1 in descriptor1[1]:
        for vector2 in descriptor2[1]:           
            tempmatch = SSD(vector2, vector1)
            tempratio = bestmatch / secondbestmatch

            if tempmatch > bestmatch and tempratio < 1:
                secondbestmatch = bestmatch
                secondbestindexes = bestindexes
                bestmatch = tempmatch
                bestindexes = (descriptor1.index(vector1),descriptor2.index(vector2))

        newmatch = cv2.DMatch(bestindexes[0], bestindexes[1], bestmatch)
        matchvector.append(newmatch)
                
    return matchvector

def SSD(v1, v2):
    distance = 0   
    for f1 in range((v1[0].size)-1):
            distance = distance + (v1[1][f1] - v1[1][f1])**2
    return distance

def orientation(image, corner):
    x, y = corner
    height, width = image.shape
    if (x + 1 >= width or x - 1 < 0):
        return 0

    elif (y + 1>= height or y - 1 < 0):
        return 0

    else:
        #magnitude = math.sqrt((image[y][x+1] - image[y][x-1])**2 + (image[y+1][x] - image[y-1][x])**2 )
        theta =  math.degrees(math.atan2((image[y + 1][x] - image[y-1][x]), (image[y][x+1] - image[y][x-1])))
        return theta

def descriptor(image, keypoint):
    bin = np.array([0,0,0,0,0,0,0,0])
    descriptorvector = []
    npempty = np.zeros(2)
    ctr = 0
    imheight, imwidth = image.shape
    x,y = keypoint
    ybounds = (0,0)
    xbounds = (0,0)

    for yiter in range(y-8, y+8):
        for xiter in range(x-8, y+8):
            binelem = int(orientation(image, (xiter,yiter)) / 45)
            bin[binelem] = bin[binelem] + 1 
            if xiter % 4 == 0 and yiter % 4 == 0 and ctr <=15:
                if ctr < 8:
                    print(ctr)
                    descriptorvector.append(bin)
                    bin.fill(0)
                    ctr = ctr + 1
                else:
                    ctr = 0

    return descriptorvector



def getResponse(filepath):
    Gaussianksize = (5,5)
    kpvector = []
    featurevector = []
    epsilon = 10
    sobelx = np.array(
        [[-1, 0, 1],
        [-2, 0, 2],
        [-1, 0, 1]])

    sobely = np.array(
        [[1, 2, 1], 
        [0, 0, 0], 
        [-1, -2, -1]])

    gaussian = np.array(
        [[0, 1, 0], 
        [1, -4, 1], 
        [0, 1, 0]])

    imageoriginal = cv2.imread(filepath, cv2.IMREAD_UNCHANGED )
    imagegrey = cv2.imread(filepath, cv2.IMREAD_GRAYSCALE)

    ix = cv2.filter2D(imagegrey, -1, sobelx)
    iy = cv2.filter2D(imagegrey, -1, sobely)

    ixx = ix**2
    ixy = iy*ix
    iyy = iy**2

    ixx = cv2.GaussianBlur(ixx, Gaussianksize,cv2.BORDER_DEFAULT)
    ixy = cv2.GaussianBlur(ixy, Gaussianksize,cv2.BORDER_DEFAULT)
    iyy = cv2.GaussianBlur(iyy, Gaussianksize,cv2.BORDER_DEFAULT)


    #cv2.imshow('original', imageoriginal)
    #cv2.imshow('ix', ix)
    #cv2.imshow('iy', iy)
    cv2.waitKey()
    
    imageheight, imagewidth = imagegrey.shape
    cH = np.empty((imageheight, imagewidth))

    for y in range(1, imageheight - 1):
        for x in range(1, imagewidth - 1):
            ixxtmp = np.sum(ixx[y-1:y+2 , x-1:x+2])
            iyytmp = np.sum(iyy[y-1:y+2 , x-1:x+2])
            ixytmp = np.sum(ixy[y-1:y+2 , x-1:x+2])

            determinanttmp = (ixxtmp * iyytmp) - (ixytmp**2) 
            tracetmp = ixxtmp + iyytmp 
            c = determinanttmp / tracetmp
            cH[y,x] = c
    
    responseheight, responsewidth = cH.shape
    threshold = 200
   
    for y in range (1, responseheight-1):
        for x in range (1, responsewidth-1):
            localmax = max(cH[y-1][x-1], cH[y][x-1], cH[y+1][x-1],cH[y-1][x], cH[y+1][x], cH[y][x], cH[y][x+1], cH[y-1][x+1],cH[y+1][x+1])
            if cH[y][x] > threshold:
                if cH[y][x] == localmax: 
                    temporientation = orientation(imagegrey, (x,y))
                    temp = cv2.KeyPoint(x,y, 1,temporientation)
                    kpvector.append(temp)
                    featurevector.append(descriptor(imagegrey,(x,y)))
                #temp = cv2.KeyPoint(x,y, 1,-1)
                #featurevector.append(temp)


    kpimage = np.array([])
    kpimage = cv2.drawKeypoints(imageoriginal, kpvector, kpimage, 1)
    return kpimage, imageoriginal, featurevector, kpvector

imagedir = 'C:\\Users\\Amrit\\Downloads\\COMP-425\\assignment2\\image_sets\\'

yosemite = [(imagedir + 'yosemite\\Yosemite1.jpg'),(imagedir + 'yosemite\\Yosemite2.jpg') ]
graf = [(imagedir + 'graf\\img1.ppm'),(imagedir + 'graf\\img2.ppm'), (imagedir + 'graf\\img4.ppm') ]
pano = [(imagedir + 'panorama\\pano1_0008.png'),(imagedir + 'panorama\\pano1_0009.png'),(imagedir + 'panorama\\pano1_0010.png'),(imagedir + 'panorama\\pano1_0011.png')]

yosoutputs = []
for yos in yosemite:
    #image1kp, image1original = getResponse(images[0])
    yosoutputs.append(getResponse(yos))

for kpsimage, original, features, kps in yosoutputs:
    cv2.imshow('Key points', kpsimage)
    cv2.imshow('Original', original)
    cv2.waitKey()
yomatches = featurematcher(yosoutputs[0][2],yosoutputs[1][2])
yosmatchimg = np.array([])
yosmatchimg = cv2.drawMatches(yosoutputs[0][1], yosoutputs[0][3],yosoutputs[1][1], yosoutputs[1][3], yomatches, yosmatchimg)
cv2.imshow('Matches', yosmatchimg)
cv2.waitKey()

grafoutputs = []
for gra in graf:
    #image1kp, image1original = getResponse(images[0])
    grafoutputs.append(getResponse(gra))

for kpsimage, original, features, kps in grafoutputs:
    cv2.imshow('Key points', kpsimage)
    cv2.imshow('Original', original)
    cv2.waitKey()
grafmatches = featurematcher(grafoutputs[0][2],grafoutputs[1][2])
grafmatches1 = featurematcher(grafoutputs[1][2],grafoutputs[2][2])
grafmatchimg = np.array([])
grafmatchimg1 = np.array([])
grafmatchimg = cv2.drawMatches(grafoutputs[0][1], grafoutputs[0][3],grafoutputs[1][1], grafoutputs[1][3], grafmatches, grafmatchimg)
grafmatchimg1 = cv2.drawMatches(grafoutputs[1][1], grafoutputs[1][3],grafoutputs[2][1], grafoutputs[2][3], grafmatches1, grafmatchimg1)
cv2.imshow('Matches', grafmatchimg)
cv2.imshow('Matches', grafmatchimg1)
cv2.waitKey()

panooutputs = []
for pan in pano:
    #image1kp, image1original = getResponse(images[0])
    panooutputs.append(getResponse(pan))

for kpsimage, original, features, kps in panooutputs:
    cv2.imshow('Key points', kpsimage)
    cv2.imshow('Original', original)
    cv2.waitKey()
panomatches = featurematcher(panooutputs[0][2],panooutputs[1][2])
panomatches1 = featurematcher(panooutputs[1][2],panooutputs[2][2])
panomatchimg = np.array([])
panomatchimg1 = np.array([])
panomatchimg = cv2.drawMatches(panooutputs[0][1], panooutputs[0][3],panooutputs[1][1], panooutputs[1][3], panomatches, panomatchimg)
panomatchimg1 = cv2.drawMatches(panooutputs[1][1], panooutputs[1][3],panooutputs[2][1], panooutputs[2][3], panomatches1, panomatchimg1)
cv2.imshow('Matches', panomatchimg)
cv2.imshow('Matches', panomatchimg1)
cv2.waitKey()
