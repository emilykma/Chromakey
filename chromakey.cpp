/*********************************************************************
 File:     chromakey.cpp

 Author:   Emily Ma

 Email address: ekma@usc.edu

 Usage: program_name in.bmp background.bmp dist_threshold out1.bmp out2.bmp

 Notes:
 This program performs the chroma key operation on an input 
 using two different methods.

 Method 1 Utilize a user-defined distance threshold from the
          chromakey value as a discriminator

 Method 2 Devise a method that to determine the chromakey mask
          that doesn't require a user-input threshold

********************************************************************/

#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include "bmplib.h"

using namespace std;

// Prototypes
// IMPORTANT: you must exactly use these input types, function names, and 
// return types. Otherwise the grader can't test them.
void method1(unsigned char inImage[][SIZE][RGB], 
	     bool mask[][SIZE],
	     double threshold);

void method2(unsigned char inImage[][SIZE][RGB], 
	     bool mask[][SIZE]);

void replace(bool mask[][SIZE],
	     unsigned char inImage[][SIZE][RGB],
	     unsigned char bgImage[][SIZE][RGB],
	     unsigned char outImage[][SIZE][RGB]);

int main(int argc, char *argv[])
{
  // Image data array
  // Note:  DON'T use the static keyword except where we expressly say so.
  //        It puts the large array in a separate, fixed, area of memory. 
  //        It is bad practice. But useful until we have dynamic allocation.
  static unsigned char inputImage[SIZE][SIZE][RGB];
  static unsigned char bgrndImage[SIZE][SIZE][RGB];
  static unsigned char outputImage[SIZE][SIZE][RGB];
  static bool chromaMask[SIZE][SIZE];

  double threshold;

  if (argc < 6) {
    cerr << "usage: program_name in.bmp background.bmp dist_threshold " 
         << "out1.bmp out2.bmp" << endl;
    return 0;
  }
	
  if (readRGBBMP(argv[1], inputImage)) {
    cerr << "Error reading file: " << argv[1] << endl;
    return 1;
  }

  if (readRGBBMP(argv[2], bgrndImage)) {
    cout << "Error reading file: " << argv[2] << endl;
    return 1;
  }
  
  // Write code to convert the threshold (argv[3])
  // from string format to a double and assign the 'threshold'
  threshold = atof(argv[3]);
 
  // Call Method 1 Function
  method1(inputImage, chromaMask, threshold);

  // Produce the output by calling replace()
  replace(chromaMask, inputImage, bgrndImage, outputImage);
 
  // Write the output image to a file using the filename argv[4] (don't touch)
  if (writeRGBBMP(argv[4], outputImage)) {
    cout << "Error writing file: " << argv[4] << endl;
    exit(1);
  }	

  // Call Method 2 Function
  method2(inputImage,chromaMask);

  // Produce the output by calling replace()
  replace(chromaMask, inputImage, bgrndImage, outputImage);

  // Write the output image to a file using the filename argv[5]
  if (writeRGBBMP(argv[5], outputImage)) {
    cout << "Error writing file: " << argv[5] << endl;
    exit(1);
  }	

  return 0;
}



// Use user-provided threshold for chroma-key distance
// The "output" of this method is to produce a valid mask array
//  where entries in the mask array are 1 for foreground image
//  and 0 for 'background'

// find distance, if dist > threshold, mask at pixel = true, otherwise false

void method1(unsigned char inImage[][SIZE][RGB], 
	     bool mask[][SIZE],
	     double threshold)
{
  int red;
  int green;
  int blue;

  // average RGB values of each pixel in top row and left column to determine chromakey values
  int avgRed = 0;
  int avgGreen = 0;
  int avgBlue = 0;
  int distance;

    for (int r = 0; r < SIZE; r++) { //iterating through column 0 to find RGB values
      red += inImage[r][0][0];
      green += inImage[r][0][1];
      blue += inImage[r][0][2];
    }
    for (int c = 1; c < SIZE; c++) { //iterating through row 0 to find RGB values
      red += inImage[0][c][0];
      green += inImage[0][c][1];
      blue += inImage[0][c][2];
    }
    // finding the average of each color
    avgRed += red / (SIZE + (SIZE - 1));  
    avgGreen += green / (SIZE + (SIZE - 1));
    avgBlue += blue / (SIZE + (SIZE - 1));

    // setting the mask array after iterating over the entire image computing
    // the distance between each pixel and the chromakey value 
    for (int i = 0; i < SIZE; i++) {
      for (int j = 0; j < SIZE; j++) {
        distance = sqrt(pow((inImage[i][j][0] - avgRed), 2) + pow((inImage[i][j][1] - avgGreen), 2) + pow((inImage[i][j][2] - avgBlue), 2));
        if (distance > threshold) {
          mask[i][j] = true;
        }
        else {
          mask[i][j] = false;
        }
      }
    }

}

// Devise a method to automatically come up with a threshold
//  for the chroma key determination
// The "output" of this method is to produce a valid mask array
//  where entries in the mask array are 1 for foreground image
//  and 0 for 'background'
void method2(unsigned char inImage[][SIZE][RGB], 
	     bool mask[][SIZE])  
{
  double distance;
  double threshold = 512;
  double avgDist = 0;
	int count = 0;
  int red;
  int green;
  int blue;
	int avgRed = 0;
	int avgGreen = 0;
	int avgBlue = 0;  

  //choosing specific pixels in the left top border instead of entire image (former approach)

  for (int r = 0; r < SIZE; r++) { //iterating through column 0 to find RGB values
      red += inImage[r][0][0];
      green += inImage[r][0][1];
      blue += inImage[r][0][2];
    }
    for (int c = 1; c < SIZE; c++) { //iterating through row 0 to find RGB values
      red += inImage[0][c][0];
      green += inImage[0][c][1];
      blue += inImage[0][c][2];
    }
    // finding the average of each color
    avgRed += red / (SIZE + (SIZE - 1));  
    avgGreen += green / (SIZE + (SIZE - 1));
    avgBlue += blue / (SIZE + (SIZE - 1));

 //Finding the average RGB distance of all pixels using the distance from each pixel(i,j) to chromakey
	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++){
			distance = sqrt(pow((inImage[i][j][0] - avgRed), 2) + pow((inImage[i][j][1] - avgGreen), 2) + pow((inImage[i][j][2] - avgBlue), 2));
			avgDist += distance;
			count ++;
		}
	}

	avgDist /= count; // dividing by total pixels

// Setting automatic threshold to calculated individual pixel distance if it is larger than avgDist
	for (int i = 0; i < SIZE; i++){
		for (int j = 0; j < SIZE; j++){
			distance = sqrt(pow((inImage[i][j][0] - avgRed), 2) + pow((inImage[i][j][1] - avgGreen), 2) + pow((inImage[i][j][2] - avgBlue), 2));
			if ((distance > avgDist) && (distance < threshold)){
				threshold = distance;
        //cout << threshold << " ";
			}
		}
	}
  
	threshold = threshold * 1.2; // for colbert, image including too much of bg, meaning threshold too small (so we increase it)

	for (int i=0; i < SIZE; i++){
		for (int j=0; j < SIZE; j++){
			distance = sqrt(pow((inImage[i][j][0] - avgRed), 2) + pow((inImage[i][j][1] - avgGreen), 2) + pow((inImage[i][j][2] - avgBlue), 2));
			if (distance > threshold){
				mask[i][j] = 1;
			}
			else {
				mask[i][j] = 0;
			}
		}
	}
}

// If mask[i][j] = 1 use the input image pixel for the output image
// Else if mask[i][j] = 0 use the background image pixel

// Create the output image using the mask to determine
//  whether to use the pixel from the Input or Background image
// if mask true, outImage = inImage, else, outImage = bgImage***
void replace(bool mask[SIZE][SIZE],
	     unsigned char inImage[SIZE][SIZE][RGB],
	     unsigned char bgImage[SIZE][SIZE][RGB],
	     unsigned char outImage[SIZE][SIZE][RGB])
{
  for (int i = 0; i < SIZE; i++) {
    for (int j = 0; j < SIZE; j++) {
      if (mask[i][j] == true) {
        outImage[i][j][0] = inImage[i][j][0];
        outImage[i][j][1] = inImage[i][j][1];
        outImage[i][j][2] = inImage[i][j][2];
      }
      else { // mask[i][j] is false (= 0)
        outImage[i][j][0] = bgImage[i][j][0];
        outImage[i][j][1] = bgImage[i][j][1];
        outImage[i][j][2] = bgImage[i][j][2];
      }
    }
  }

}
