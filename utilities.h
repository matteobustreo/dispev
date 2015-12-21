/* 
 * File:   utilities.h
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

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/contrib/contrib.hpp"

namespace dispev_utils
{
	void imagesc(std::string winName, const cv::Mat &inImm, const cv::Mat &mask = cv::Mat(), int colormap=-1);
	void translateImg(const cv::Mat &img, cv::Mat &outImg, int offsetx, int offsety);
	void translateImgXWithKernel(const cv::Mat &img, cv::Mat &outImg, int offsetx);
}
