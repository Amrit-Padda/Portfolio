Project Readme

How to run:
- Change the path in outputdir and imagedir on lines 214 and 215
- Run the script as normal
- Press any button to close the windows that pop up
- If you accidentally hit the x to close, the script will be stuck.
- To resolve this you can kill the terminal and try again
- Occasionaly RANSAC will not be able to compute a Homography which will throw an error.
- If this occurs you can try again, it usually works on the second try, however it may take more tries

-----------------------------------------Steps 1 and 2------------------------------------------------
- My feature detector didnt work in assignment 2, so I used the builtin SIFT implementaion instead.
- The output folder contains the image 1a.png showing the sift keypoints in boxes.png
- The output folder also contains the images 1a.png and 1b.png
- I used my feature matcher from assignment 2 for step 2
- The output is in output/2.png

Code explanation:
1. Ignoring the methods at the top for now, lines 212-220 contain arrays which store the filepaths for the images 
2. We begin by finding the keypoints in boxes.png on line 226
3. The method findKeypoints on line 146 is the SIFT implementation, it takes as input an array of images 
   and it returns the imgs, keypoints and descriptors
4. We then output the img 
5. We then begin looping over the rainier images on line 233. Outputting and saving the images with keypoints
6. For step 2, we first find the matches between the two rainier images on line 245
7. line 244 is commented out since it uses the cv2 BF matcher
8. On line 36 feature matcher loops over the descriptors, calculates the distance using SSD and performs the ratio test
9. The method will loop over each element of descriptor1 and look for matches in descriptor 2
10. For each pair of descriptors a distance is calculated and compared to the current best values
11. The best match is added to the matches array
12. We also ignore distances over the current best in order to save time
13. On line 66 the SSD implementation will also ignore values over the threshold, returning an arbritrary large number to feature matcher
14. featurematcher will also sort the matches based on distance and return the 200 best matches
15. We then output the matched images on line 248



-----------------------------------------Steps 3 and 4------------------------------------------------
- My RANSAC implementation sometimes cannot compute a homography, in this case the code will throw an error.
- In this case, you can rerun the code and it should find a homography eventually

Code explanation:
1. On line 257 we set the inlier threshold, I found 10 to be a good value
2. We then call the RANSAC method on line 258 with 200 iterations
3. On line 115 I implement the Ransac method as described in the project pdf
4. I start by extracting the points from the matches array, I then take a sample of 4 points
5. I use this sample to calculate a homography
6. I then loop in order to find the best homography
7. Once we've looped for the number of iterations, I recalulate the homography on line 140 using 
   all the inliers found. 
8. My function returnInliers on line takes the matches, projects them onto the image and calculates the distances
9. If the distance is below the threshold it adds them to the inliers array
10. Once we find the inliers we calculate the inverse homography on line 142
11. We then draw the matches on line 262 and save the image
12. For step 4, we first call the stitch method.
13. On line 190 the stitch method calls the createimage helper function
14. On line 162 the create image function will project the four corners of both images and determine the stitched image required
15. If the width of projected image 2 exceeds the width of image 1, we resize to image 2 width
16. If any of the values are negative, this means the image is projected to the left of the origin, therefore we add its absolute value to the width
17. The same applies to the height
18. In stitch we create the blank image and copy over the ranier1 image in the first loop. 

 

