#include <opencv2/opencv.hpp>
#include <iostream>
using namespace std;
using namespace cv;

int threshval = 0;
int threshval2 = 255;
Mat gray;


void on_trackbar(int th, void*)
{

	Mat bin;
	threshold(gray, bin, threshval, threshval2, CV_THRESH_BINARY);

	Mat bw = bin;
	//Mat bw = inverseImg(bin);
	imshow("Binary Image", bw);
}

void on_trackbar1(int th, void*)
{

	Mat bin;

	threshold(gray, bin, threshval, threshval2, CV_THRESH_BINARY);

	Mat bw = bin;
	//Mat bw = inverseImg(bin);
	imshow("Binary Image", bw);
}

int main(int argc, char** argv)
{
	Mat src, canny_output, dst;
	char * filename = "e:/cell3.jpg";
	src = imread(filename);
	//imshow("src", src);
	

	cvtColor(src, gray, CV_RGB2GRAY);
	GaussianBlur(gray, gray, Size(3, 3), 0, 0);
	imshow("gray", gray);

	//createTrackbar("Threshold", "gray", &threshval, 255, on_trackbar);
	//createTrackbar("Threshold2", "gray", &threshval2, 255, on_trackbar1);
	//on_trackbar(threshval, 0);//轨迹条回调函数  

	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	Canny(gray, canny_output, 1, 255, 3);
	/// Find contours  
	findContours(canny_output, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	cout << contours.size() << endl;
	dst = Mat::zeros(canny_output.size(), CV_8UC3);
	for (int i = 0; i< contours.size(); i++)
	{
		RNG rng;
		//随机颜色  
		Scalar color = Scalar(255, 255, 0);
		//Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		drawContours(dst, contours, i, color, 1, 8, hierarchy, 0, Point());
	}

//	src.copyTo(dst, hierarchy);

	imshow("dst", dst);
	waitKey(0);
}

void splitChannels()
{
	vector<Mat> channels;
	Mat imageBlueChannel;
	Mat imageGreenChannel;
	Mat imageRedChannel;

	Mat srcImage4 = imread("e:/cell3.jpg");
	// 把一个3通道图像转换成3个单通道图像  
	split(srcImage4, channels);//分离色彩通道  
	imageBlueChannel = channels.at(0);
	imageGreenChannel = channels.at(1);
	imageRedChannel = channels.at(2);

	imshow("imageBlueChannel", imageBlueChannel);
	imshow("imageGreenChannel", imageGreenChannel);
	imshow("imageRedChannel", imageRedChannel);
}