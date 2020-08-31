import cv2 
import numpy as np
import math
import random

placeholder = 0


""" This method is a helper to generate filenames.
    It takes as input indexes and returns a string with the filename"""
def getfilename(val, val1):
    letters = ["a", "b", "c", "d", "e", "f", "g", "h", "i", "j"  ]
    return outputdir + str(val) + letters[val1] + '.png'

""" This is an implementation of the builtin bf.match method, it was used for testing.
    It takes the descriptors from 2 images and returns an array of matches """
def matcher(img1, img2):
    bf = cv2.BFMatcher()
    matches = bf.match(img1, img2)
    matches = sorted(matches, key = lambda x:x.distance)
    return matches[:200]

""" This is my modified feature matcher from assignment 2.
    It takes the descriptors from both images and returns an array of matches.
    It matches using SSD and a ratio test"""
def featureMatcher(descriptor1, descriptor2):
    matchvector = []
    bestdistance = 100000
    secondbestmatch = 100000
    bestindexes = (0,0)
    secondbestindexes = (0,0)

    for x in range(descriptor1.shape[0] - 1):       
        for y in range(descriptor2.shape[0] - 1):
            distance= SSD(descriptor1[x], descriptor2[y])
           
            #if the distance is greater than the current best, ignore it and continue
            if distance > bestdistance: 
                continue
            
            ratio = bestdistance / secondbestmatch
           
            if distance < bestdistance and ratio <= 1:
                secondbestmatch = bestdistance
                secondbestindexes = bestindexes
                bestdistance = distance
                bestindexes = (x,y)
        #resetting the best values for the next iteration
        bestx, besty = bestindexes
        newmatch = cv2.DMatch(bestx, besty, bestdistance)
        matchvector.append(newmatch)     
        bestdistance = 100000
        secondbestmatch = 100000
        bestindexes = (0,0)
        secondbestindexes = (0,0)
        
    matchvector = sorted(matchvector, key = lambda x:x.distance)       
    return matchvector[:200]

""" Implemetation of SSD for the feature matcher."""
def SSD(v1, v2):
    distance = 0   
    threshold = 10000
    for f1 in range(v1.shape[0]-1):
            distance = distance + math.pow((v1[f1] - v2[f1]), 2)
            if distance > threshold:
                return 100000
    return distance

""" Projects the point x1, y1 using the homography H."""
def project(x1,y1,H):
    pt = np.array(
        [[x1],
         [y1],
         [1]])
    
    matrix = H.dot(pt)
    
    x = matrix[0][0] / matrix[2][0]
    y = matrix[1][0] / matrix[2][0]
    return x,y

""" This function computes the number of inliers for the given matches and homography."""
def computeInlierCount(H, matches, img1kp, img2kp, inlierThreshold):
    inliermatches = []
    nummatches = 0 
    for m in matches:
        inliermatches.append((img1kp[m.queryIdx].pt, img2kp[m.trainIdx].pt))

    for m,n in inliermatches:
        x, y = m
        x1, y1 = n
        projx, projy = project(x,y,H)
        projx1, projy1 = project(x1,y1,H)

        if (math.sqrt( (projx1-projx)**2 + (projy1-projy)**2 )) < inlierThreshold:
            nummatches = nummatches + 1

    return nummatches

""" Same as above, however here we return the inliers instead of the count """
def returnInliers(H, matches, img1kp, img2kp, inlierThreshold):
    inliers = []
    for m in matches:
        x, y = img1kp[m.queryIdx].pt
        x1, y1 = img2kp[m.trainIdx].pt
        projx, projy = project(x,y,H)
        projx1, projy1 = project(x1,y1,H)

        if (math.sqrt( (projx1-projx)**2 + (projy1-projy)**2 )) < inlierThreshold:
            inliers.append(m)
    return inliers

""" RANSAC implementation according to the project document """
def RANSAC(matches, img1kp, img2kp, numIterations, inlierThreshold):
    best_match = 0
    best_homography = 0

    
    for x in range(numIterations):
        sample = random.sample(matches, 4)

        pts_src = np.float32([img1kp[m.queryIdx].pt for m in sample]).reshape(-1, 1, 2)
        pts_dst = np.float32([img2kp[m.trainIdx].pt for m in sample]).reshape(-1, 1, 2)

        H, status = cv2.findHomography(pts_src, pts_dst)
       
        inliers = computeInlierCount(H, matches, img1kp, img2kp, inlierThreshold)

        if  inliers > best_match:
            best_match = inliers
            best_homography = H
            
    
    highest_inliers = returnInliers(best_homography, matches, img1kp, img2kp, inlierThreshold)
    refined_pts_src = np.float32([img1kp[h.queryIdx].pt for h in highest_inliers]).reshape(-1, 1, 2)
    refined_pts_dst = np.float32([img2kp[h.trainIdx].pt for h in highest_inliers]).reshape(-1, 1, 2)
    
    
    hom, refinedstatus = cv2.findHomography(refined_pts_src, refined_pts_dst, 0)

    hominv = np.linalg.inv(hom)
    return hom, hominv, highest_inliers
   
""" SIFT implementation that returns the key points in an image """
def findKeypoints(inputimages):
    imgs = []
    kps = []
    dess = []
    sift = cv2.xfeatures2d.SIFT_create()
    for img in inputimages:
        img = cv2.imread(img)
        gray= cv2.cvtColor(img,cv2.COLOR_BGR2GRAY) 
        kp, des = sift.detectAndCompute(gray,None)
        img=cv2.drawKeypoints(gray,kp,img)
        imgs.append(img)
        kps.append(kp)
        dess.append(des)
    return imgs, kps, dess

""" Contains logic to calculate output image size """
def createimage(img1, img2, H):
    baseimgwidth = img1.shape[0]
    baseimgheight = img1.shape[1]
    projimgwidth = img2.shape[0]
    projimgheight = img2.shape[1]

    x, y = project(0, 0, H)
    x1, y1 = project(projimgwidth, 0, H)
    x2, y2 = project(projimgwidth,  projimgheight, H)
    x3, y3 = project(0,  projimgheight, H)

    outimgwidth = 0
    outimgheight = 0 

    if(x < 0 or x3 < 0):
        outimgwidth = max(x1,x2) - min(x,x3)
    else:
        outimgwidth = max(x1,x2)

    if(y < 0 or y3 < 0):
        outimgheight = max(y1,y2) - min(y,y3)
    else:
        outimgheight = max(y1,y2)

    return int(outimgwidth), int(outimgheight)

""" Stitch method as described in assignment document """
def stitch(img1, img2, hom, hominv):
    outimgy, outimgx =  createimage(img1, img2, hominv)
    
    outimg = np.zeros((outimgx, outimgy,3), np.uint8)
  
    for d in range(3):
        for y in range(int(img1.shape[1]) - 1):
            for x in range(int(img1.shape[0]) - 1):
                outimg[x,y,d] = img1[x,y,d]
    cv2.imwrite('colorimg.jpg', outimg, )
    cv2.imshow("stitched", outimg)
    cv2.waitKey()

    for d in range(3):
        for y in range(int(outimg.shape[1]) - 1):
            for x in range(int(outimg.shape[0]) - 1):
                projx, projy = project(x, y, hom)
                #outimg[x,y,d] = 
                output = np.array([])
                output = cv2.getRectSubPix(img2, (1,1), (projx,projy))
                input = 0
                #img1[projx,projy,d]

"""----------------------------Image file paths---------------------------------"""

outputdir = 'C:\\Users\\Amrit\\Downloads\\COMP-425\\assignment3\\output\\'
imagedir = 'C:\\Users\\Amrit\\Downloads\\COMP-425\\assignment3\\project_images\\'
melwaka = [(imagedir + 'MelakwaLake1.png'), (imagedir + 'MelakwaLake2.png'), (imagedir + 'MelakwaLake3.png'), (imagedir + 'MelakwaLake4.png')]
hanging = [(imagedir + 'Hanging1.png'), (imagedir + 'Hanging2.png')]
nd = [(imagedir + 'ND1.png'), (imagedir + 'ND2.png')]
rainier = [(imagedir + 'Rainier1.png'), (imagedir + 'Rainier2.png'), (imagedir + 'Rainier3.png'), (imagedir + 'Rainier4.png'), (imagedir + 'Rainier5.png'), (imagedir + 'Rainier6.png')]
boxes = [(imagedir + 'Boxes.png')]


"""---------FEATURE DETECTION AND MATCHING [Steps 1 and 2] - assignment 2--------
------------------------------------ Step 1 --------------------------------- """

box_imgs, box_kps, box_desc = findKeypoints(boxes)
cv2.imshow('sift_keypoints.jpg', box_imgs[0])
cv2.imwrite(getfilename(1, placeholder), box_imgs[0])
cv2.waitKey(0)

placeholder = placeholder + 1

rainier_imgs, rainier_kps, rainier_desc = findKeypoints(rainier)
for rainier_img in rainier_imgs:
    cv2.imshow('sift_keypoints.jpg', rainier_img)
    cv2.imwrite(getfilename(1, placeholder), rainier_img)
    placeholder = placeholder + 1
    cv2.waitKey(0)
cv2.destroyAllWindows()


"""------------------------------------ Step 2 ---------------------------------"""

#matches = matcher(rainier_desc[0], rainier_desc[1])
matches = featureMatcher(rainier_desc[0], rainier_desc[1])
img3 = np.array([])
img3 = cv2.drawMatches(cv2.imread(rainier[0]), rainier_kps[0], cv2.imread(rainier[1]), rainier_kps[1], matches, img3, None, flags=2)
cv2.imshow('Matches.jpg', img3)
cv2.imwrite(outputdir + '2.png', img3)
cv2.waitKey(0)


"""---------- PANORAMA MOSAIC STITCHING [Steps 3 and 4] - assignment 23 --------
----------------------------------- Step 3 -------------------------------------
- Methods project(), computerInlierCount(), and RANSAC() are defined above --"""

inthres = 10
homography, hominv, inliermatches = RANSAC(matches, rainier_kps[0], rainier_kps[1], 200, inthres)

img4 = np.array([])
img4 = cv2.drawMatches(cv2.imread(rainier[0]), rainier_kps[0], cv2.imread(rainier[1]), rainier_kps[1], inliermatches, img4, None, flags=2)
cv2.imshow('InlierMatches.jpg', img4)
cv2.imwrite(outputdir + '3.png', img4)
cv2.waitKey(0)
cv2.destroyAllWindows()

"""---------------------------------- Step 4 -----------------------------------"""
stitch(cv2.imread(rainier[0]), cv2.imread(rainier[1]), homography, hominv) 