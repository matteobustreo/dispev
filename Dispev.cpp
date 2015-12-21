/* 
 * File:   Dispev.cpp
 * Author: mbustreo
 *
 * Created on 07 July 2015
 *
 * Implementation of the algorithm described in "A two-stage correlation method for stereoscopic depth estimation" 
 * by Nils Einecke, and Julian Eggert [DICTA, page 227-234. IEEE Computer Society, (2010)]
 *
 * Note that algorithm can be strongly speeded-up.
 *
 */

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/contrib/contrib.hpp"
#include <iostream>
#include <fstream>

#include "utilities.h"

using namespace cv;
using namespace std;

struct Inputs
{
	string imgNameL;
	string imgNameR;
	 
	int maxDisp;
	int minDisp;

	int boxFilterSizeX;
	int boxFilterSizeY;
	int sumFilterSizeX;
	int sumFilterSizeY;
};

void help()
{
	cout << "Usage Example: Dispev.exe C:\\leftPic.jpg C:\\rightPic.jpg  maxDisp minDisp boxFilterSizeX boxFilterSizeY sumFilterSizeX sumFilterSizeX" << endl;
	
	cout << " - maxDisp and minDisp represent the disparity interval the algorithm will search for correspondence." << endl;
	cout << "   -- DEFAULT VALUES: If not inserted maxDisp and minDisp are set to 60 and 1;" << endl;
	
	cout << " - (boxFilterSizeX, boxFilterSizeY) represent the correlation filter size." << endl;
	cout << "   -- DEFAULT VALUES: If not inserted (boxFilterSizeX, boxFilterSizeY) is set to (3,3);" << endl;
	
	cout << " - (sumFilterSizeX, sumFilterSizeY) represent the summation filter size." << endl;
	cout << "   -- DEFAULT VALUES: If not inserted (sumFilterSizeX, sumFilterSizeY) is set to (5,9);" << endl;
}

int readArgs ( int argc, char** argv, Inputs &in )
{
	cout << "Reading inputs...." << endl << endl;
	
	if( argc < 3)	
	{
		help();
		return -1;
	}

	in.maxDisp = 60;		
	in.minDisp = 1;	

	in.boxFilterSizeX = 3;
	in.boxFilterSizeY = 3;
	in.sumFilterSizeX = 5;
	in.sumFilterSizeY = 9;

	switch (argc)
	{
		default:
		case 9:
			in.sumFilterSizeY = atoi(argv[8]);
		case 8:
			in.sumFilterSizeX = atoi(argv[7]);
		case 7:
			in.boxFilterSizeY = atoi(argv[6]);	
		case 6:
			in.boxFilterSizeX = atoi(argv[5]);	
		case 5:
			in.minDisp = atoi(argv[4]);	
		case 4:
			in.maxDisp = atoi(argv[3]);	
		case 3:
			in.imgNameL = argv[1];
			in.imgNameR = argv[2];
			break;
	}

	// -- CHECKING INPUTS VALIDITY
	
	// Verifying if disparity threashold are reasonable
    if(in.minDisp >= in.maxDisp ) 
	{
        cout << "minDisp should be less than maxDisp!" << std::endl ;
        return -1;
    }
	
	// Verifying correlation window sizes are odd, otherwise increment by 1
	if(in.boxFilterSizeX%2==0) 
	{
        cout << "Correlation window X size dimensions should be odd! Automatically increased of 1." << std::endl ;
        in.boxFilterSizeX++;
    }
	
	if(in.boxFilterSizeY%2==0) 
	{
        cout << "Correlation window Y size dimensions should be odd! Automatically increased of 1." << std::endl ;
        in.boxFilterSizeY++;
    }

	// Verifying summation window sizes are odd, otherwise increment by 1
	if(in.sumFilterSizeX%2==0) 
	{
        cout << "Correlation window X size dimensions should be odd! Automatically increased of 1." << std::endl ;
        in.sumFilterSizeX++;
    }
	
	if(in.sumFilterSizeY%2==0) 
	{
        cout << "Correlation window Y size dimensions should be odd! Automatically increased of 1." << std::endl ;
        in.sumFilterSizeY++;
    }
	
	cout << " - maxDisp set to... " << in.maxDisp << endl;
	cout << " - minDisp set to... " << in.minDisp << endl;
	cout << " - (boxFilterSizeX, boxFilterSizeY) set to... (" << in.boxFilterSizeX << ", " << in.boxFilterSizeY << ")" << endl;
	cout << " - (sumFilterSizeX, sumFilterSizeY) set to... (" << in.sumFilterSizeX << ", " << in.sumFilterSizeY << ")" << endl;
	cout << endl;

	return 0;
}

int main( int argc, char** argv )
{
	// Reading inputs arguments
	Inputs in;
	int retVal = readArgs(argc, argv, in);

	if( retVal < 0 )	return retVal;

	setUseOptimized(true);

	// Loading images
    Mat imgL, imgR;
    imgL = imread(in.imgNameL, CV_LOAD_IMAGE_GRAYSCALE ); 
	imgR = imread(in.imgNameR, CV_LOAD_IMAGE_GRAYSCALE ); 

	// Verifying if images has been correctly loaded
    if(! imgL.data ) 
    {
        cout << "Left image impossible to read" << std::endl ;
        return -1;
    }

	if(! imgR.data ) 
    {
        cout << "Right image impossible to read" << std::endl ;
        return -1;
    }

	// Verifying images are of the same size
	if(imgR.rows != imgL.rows || imgR.cols != imgL.cols) 
    {
        cout << "Size of images have to be equal!" << std::endl ;
        return -1;
    }

	// Visualizing input images
	namedWindow( "Left Image", WINDOW_NORMAL); 
	cv::resizeWindow("Left Image", 640, 480);
    imshow( "Left Image", imgL ); 

	namedWindow( "Right Image", WINDOW_NORMAL); 
	cv::resizeWindow("Right Image", 640, 480);
    imshow( "Right Image", imgR ); 

	// --> 
	
	// Converting input images to double precision
	Mat imgL64, imgR64;
	imgL.convertTo(imgL64, CV_64F);
	imgR.convertTo(imgR64, CV_64F);

	// ** SNCC INIT **
	Mat meanL, meanR, meanLL, meanRR;
	Mat sigmaL, sigmaR;

	// Calculating meanL and meanR using separable filters
	boxFilter(imgL64, meanL, CV_64F, Size(in.boxFilterSizeX, in.boxFilterSizeY));		// #SNCC Init (1)
	boxFilter(imgR64, meanR, CV_64F, Size(in.boxFilterSizeX, in.boxFilterSizeY));		// #SNCC Init (2)

	// Calculating sigmaL and sigmaR using separable filters
	boxFilter(imgL64.mul(imgL64), meanLL, CV_64F, Size(in.boxFilterSizeX, in.boxFilterSizeY));				
	boxFilter(imgR64.mul(imgR64), meanRR, CV_64F, Size(in.boxFilterSizeX, in.boxFilterSizeY));				

	sqrt((meanLL - meanL.mul(meanL)), sigmaL);			// #SNCC Init (3)
	sqrt((meanRR - meanR.mul(meanR)), sigmaR);			// #SNCC Init (4)

	// Initializing disparity map and matrix containing best correlation values
	Mat_<int> dispMap(imgL.rows, imgL.cols);
	dispMap = 0;

	Mat bestCorrVals(imgL.rows, imgL.cols, CV_64F);
	bestCorrVals = -std::numeric_limits<double>::max();

	// Evaluating elapsed time
	double t = (double)getTickCount(); 

	// ** SNCC LOOP ** 
	for(int d=in.minDisp; d<in.maxDisp; d++)
	{
		if(d%10==0)	cout << "Evaluating disparity value " << d << "..." << endl;

		if(d==0)	continue;

		// generating the translated version of imgR, meanR and sigmaR
		Mat imgR64d, meanRd, sigmaRd, goodValsMaskd;

		dispev_utils::translateImg(imgR64, imgR64d, d, 0);
		dispev_utils::translateImg(meanR,  meanRd,  d, 0);
		dispev_utils::translateImg(sigmaR, sigmaRd, d, 0);

		//translateImgXWithKernel(imgR64,		imgR64d, d);
		//translateImgXWithKernel(meanR,		meanRd,	 d);
		//translateImgXWithKernel(sigmaR,		sigmaRd, d);

		// #SNCC Loop (1.1)
		Mat boxFiltLRd;
		boxFilter(imgL64.mul(imgR64d), boxFiltLRd, CV_64F, Size(in.boxFilterSizeX, in.boxFilterSizeY));
		Mat corrVal = (boxFiltLRd - meanL.mul(meanRd))/(sigmaL.mul(sigmaRd));

		// #SNCC Loop (1.2)
		Mat meanCorrVal;
		boxFilter(corrVal, meanCorrVal, CV_64F, Size(in.sumFilterSizeX, in.sumFilterSizeY));

		// Looking for pixels where meanCorrVal is bigger than bestCorrVals
		Mat biggerMask(imgL.rows, imgL.cols, CV_64F);
		biggerMask = meanCorrVal > bestCorrVals;
		
		// Updating disparity map and matrix containing best correlation values
		dispMap.setTo(d, biggerMask);
		meanCorrVal.copyTo(bestCorrVals, biggerMask);
	}

	// Evaluating elapsed time
	t = ((double)getTickCount() - t)/getTickFrequency(); 
	cout << "Times passed in seconds: " << t << std::endl;

	// Saving results
	char fileName[1000];
	
	sprintf(fileName, "%s_dispMap.txt", in.imgNameL.c_str());
	ofstream dispMapFile;
	dispMapFile.open(fileName);
	dispMapFile << cv::format(dispMap,"CSV") << endl;
	dispMapFile.close();

	sprintf(fileName, "%s_bestCorrVals.txt", in.imgNameL.c_str());
	ofstream bestCVFile;
	bestCVFile.open(fileName);
	bestCVFile << cv::format(bestCorrVals,"CSV") << endl;
	bestCVFile.close();

	sprintf(fileName, "%s_elapsedTime.txt", in.imgNameL.c_str());
	ofstream elTimeFile;
	elTimeFile.open(fileName);
	elTimeFile << "Elapsed Time: " << t << endl;
	elTimeFile << "img size: " << imgL.rows << "x" << imgL.cols << endl;
	elTimeFile << "n: " << imgL.rows*imgL.cols << endl;
	elTimeFile << "d: " << in.maxDisp - in.minDisp << endl;
	elTimeFile << "maxDisp - minDisp: " << in.maxDisp << " - " << in.minDisp << endl;
	elTimeFile.close();

	sprintf(fileName, "%s_dispMap.png", in.imgNameL.c_str());
	imwrite(fileName, dispMap);

	dispev_utils::imagesc("3D4", dispMap, Mat(), COLORMAP_JET);
	cvWaitKey(3000);
	
	return 0;
}