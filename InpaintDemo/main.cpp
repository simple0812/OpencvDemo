#include <opencv2/opencv.hpp>

#include <opencv2/highgui/highgui.hpp>

#include <opencv2/imgproc/imgproc.hpp>



#include <iostream>

#include <string>



using namespace std;

using namespace cv;



#pragma comment( lib, "vfw32.lib" ) 

#pragma comment( lib, "comctl32.lib" )



int main()

{

	cv::Mat src = cv::imread("e:/111.jpg");

	if (src.empty())

		return -1;

	namedWindow("original image", CV_WINDOW_AUTOSIZE);

	imshow("original image", src);





	cv::Mat mask;

	cv::cvtColor(src, mask, CV_BGR2GRAY);

	cv::threshold(mask, mask, 220, 255, CV_THRESH_BINARY);

	namedWindow("threshed image", CV_WINDOW_AUTOSIZE);

	imshow("threshed image", mask);



	std::vector<std::vector<cv::Point> > contours;

	cv::findContours(mask.clone(), contours, CV_RETR_EXTERNAL,

		CV_CHAIN_APPROX_SIMPLE);

	for (int i = 0; i < contours.size(); i++)

	{

		cv::Rect r = cv::boundingRect(contours[i]);



		if (std::abs(1 - (src.cols / r.width)) > 0.2)

			cv::drawContours(mask, contours, i, CV_RGB(0, 0, 0), CV_FILLED);

	}

	namedWindow("filtered image", CV_WINDOW_AUTOSIZE);

	imshow("filtered image", mask);



	cv::Mat dst;

	cv::inpaint(src, mask, dst, 1, cv::INPAINT_TELEA);

	namedWindow("inpainted image", CV_WINDOW_AUTOSIZE);

	imshow("inpainted image", dst);



	waitKey();

	return 0;

}