/* 
 * File:   utilities.cpp
 * Author: mbustreo
 *
 * Created on 07 July 2015
 *
 * Implementation of the algorithm described in "A two-stage correlation method for stereoscopic depth estimation" 
 * by Nils Einecke, and Julian Eggert [DICTA, page 227-234. IEEE Computer Society, (2010)].
 *
 * Utility functions.
 *
 */

#include "utilities.h"

using namespace cv;
using namespace std;

namespace dispev_utils
{
	void imagesc(string winName, const Mat &inImm, const Mat &mask, int colormap)
	{
		Mat u8Imm;
		normalize(inImm, u8Imm, 0, 255, NORM_MINMAX, CV_8U, mask);

		if(colormap>=0)
		{
			Mat u8ImmCol;
			applyColorMap(u8Imm, u8ImmCol, COLORMAP_JET);
			swap(u8Imm, u8ImmCol);
		}
		
		imshow(winName, u8Imm);
	}

	void translateImg(const Mat &img, Mat &outImg, int offsetx, int offsety)
	{
		Mat trans_mat = (Mat_<double>(2,3) << 1, 0, offsetx, 0, 1, offsety);
		warpAffine(img, outImg, trans_mat, img.size());
	}

	void translateImgXWithKernel(const Mat &img, Mat &outImg, int offsetx)
	{
		int kernel_size = 1 + abs(offsetx) * 2; //Center pixel plus displacement diameter (=radius * 2)
		Mat kernel = Mat::zeros( 1, kernel_size, CV_32F );
		kernel.at<float>(0,abs(offsetx)-offsetx) = 1.0f; // Indices are zero-based, not relative

		//Mat kernShift = Mat_<char>(1,2*abs(offsetx)+1);
		//kernShift = 0;
		//kernShift.at<char>(0,abs(offsetx)+offsetx) = 1;

		filter2D(img, outImg, CV_64F, kernel, Point(-1,-1), true, BORDER_REFLECT);
	}
}