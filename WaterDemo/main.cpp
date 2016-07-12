
#include <opencv2/opencv.hpp>
#include <iostream>
using namespace std;
using namespace cv;

int getContoursByCplus(Mat& src, double minarea = 100, double whRatio = 1)
{
	Mat dst, canny_output;
	/// Load source image and convert it to gray  

	if (!src.data)
	{
		std::cout << "read data error!" << std::endl;
		return -1;
	}
	blur(src, src, Size(3, 3));
	imshow("xx", src);

	//the pram. for findContours,  
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	/// Detect edges using canny  
	Canny(src, canny_output, 80, 255, 3);
	/// Find contours  
	findContours(canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	//CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE  

	double maxarea = 0;
	int maxAreaIdx = 0;

	for (int i = 0; i<contours.size(); i++)
	{

		double tmparea = fabs(contourArea(contours[i]));
		if (tmparea>maxarea)
		{
			maxarea = tmparea;
			maxAreaIdx = i;
			continue;
		}

		if (tmparea < minarea)
		{
			//删除面积小于设定值的轮廓  
			contours.erase(contours.begin() + i);
			std::wcout << "delete a small area" << std::endl;
			continue;
		}
		//计算轮廓的直径宽高  
		Rect aRect = boundingRect(contours[i]);
		if ((aRect.width / aRect.height)<whRatio)
		{
			//删除宽高比例小于设定值的轮廓  
			contours.erase(contours.begin() + i);
			std::wcout << "delete a unnomalRatio area" << std::endl;
			continue;
		}
	}
	/// Draw contours,彩色轮廓  
	dst = Mat::zeros(canny_output.size(), CV_8UC3);

	cout << "count->" << contours.size() << endl;

	for (int i = 0; i< contours.size(); i++)
	{
		RNG rng;
		//随机颜色  
		Scalar color = Scalar(255, 255, 0);
		//Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		drawContours(dst, contours, i, color, 2, 8, hierarchy, 0, Point());
	}

	// Create Window  
	char* source_window = "countors";
	namedWindow(source_window, CV_WINDOW_AUTOSIZE);
	imshow(source_window, dst);
	//waitKey(0);

	return 0;
}


//反色 bitwise_xor(image,Scalar(255),image);
Mat inverseImg(Mat im)
{
	Mat newBImg(im.rows, im.cols, im.type());
	uchar* newBImgData = newBImg.data;
	uchar* binaryData = im.data;
	int step = im.step / sizeof(uchar);
	for (int i = 0; i<im.rows; i++)
	{
		for (int j = 0; j<im.cols; j++)
		{
			newBImgData[i*step + j] = newBImgData[i*step + j] > 100 ? 0 : 255;
		}
	}

	imshow("newBImg", newBImg);

	return newBImg.clone();
}


int main(int, char** argv)
{
	Mat src = imread("e:/red.jpg");
	if (!src.data)
		return -1;

	imshow("Source Image", src);

	Mat kernelx = Mat::ones(3, 3, CV_8UC1);
	dilate(src, src, kernelx);
	dilate(src, src, kernelx);
	dilate(src, src, kernelx);

	/*blur(src, src, Size(3, 3));
	blur(src, src, Size(3, 3));
	blur(src, src, Size(3, 3));*/

	medianBlur(src, src, 1);
	medianBlur(src, src, 1);
	medianBlur(src, src, 1);
	//GaussianBlur(src, src, Size(5, 5), 200, 200);
	//GaussianBlur(src, src, Size(5, 5), 200, 200);
	//GaussianBlur(src, src, Size(5, 5), 200, 200);

	//erode(src, src, kernelx);
	//erode(src, src, kernelx);
	//erode(src, src, kernelx);

	imshow("Source Imagxexx", src);

	//for (int x = 0; x < src.rows; x++) {
	//	for (int y = 0; y < src.cols; y++) {
	//		if (src.at<Vec3b>(x, y) == Vec3b(255, 255, 255)) {
	//			src.at<Vec3b>(x, y)[0] = 0;
	//			src.at<Vec3b>(x, y)[1] = 0;
	//			src.at<Vec3b>(x, y)[2] = 0;
	//		}
	//	}
	//}
	//waitKey(0);
	//return 0;

	//imshow("Black Background Image", src);
	//Mat kernel = (Mat_<float>(3, 3) <<
	//	1, 1, 1,
	//	1, -8, 1,
	//	1, 1, 1);

	//Mat imgLaplacian;
	//Mat sharp = src;
	//filter2D(sharp, imgLaplacian, CV_32F, kernel);
	//src.convertTo(sharp, CV_32F);
	//Mat imgResult = sharp - imgLaplacian;
	//imgResult.convertTo(imgResult, CV_8UC3);
	//imgLaplacian.convertTo(imgLaplacian, CV_8UC3);
	//imshow("New Sharped Image", imgResult);
	//src = imgResult;

	Mat bw;
	cvtColor(src, bw, CV_BGR2GRAY);
	threshold(bw, bw, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
	//imshow("Binary Image", bw);
	Mat dist;
	distanceTransform(bw, dist, CV_DIST_L2, 3);
	normalize(dist, dist, 0, 1., NORM_MINMAX);
	//imshow("Distance Transform Image", dist);


	threshold(dist, dist, 0, 1., CV_THRESH_BINARY);
	Mat kernel1 = Mat::ones(3, 3, CV_8UC1);
	dilate(dist, dist, kernel1);
	//imshow("Peaks", dist);


	Mat dist_8u;
	dist.convertTo(dist_8u, CV_8U);
	vector<vector<Point> > contours;
	
	findContours(dist_8u, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	Mat markers = Mat::zeros(dist.size(), CV_32SC1);
	for (size_t i = 0; i < contours.size(); i++) {
		//drawContours(markers, contours, i, Scalar(255,0,0), 0);
		drawContours(markers, contours, i, Scalar::all(i + 1), -1);
	}

	/// Detect edges using canny  
	/// Find contours  
	imshow("Markers", markers * 1000000);

	watershed(src, markers);

	Mat mark = Mat::zeros(markers.size(), CV_8UC1);
	markers.convertTo(mark, CV_8UC1);
	bitwise_not(mark, mark);
	vector<Vec3b> colors;
	for (size_t i = 0; i < contours.size(); i++)
	{
		int b = theRNG().uniform(0, 255);
		int g = theRNG().uniform(0, 255);
		int r = theRNG().uniform(0, 255);
		colors.push_back(Vec3b((uchar)b, (uchar)g, (uchar)r));
	}

	Mat dst = Mat::zeros(markers.size(), CV_8UC3);
	for (int i = 0; i < markers.rows; i++)
	{
		for (int j = 0; j < markers.cols; j++)
		{
			int index = markers.at<int>(i, j);
			if (index > 0 && index <= static_cast<int>(contours.size()))
				dst.at<Vec3b>(i, j) = colors[index - 1];
			else
				dst.at<Vec3b>(i, j) = Vec3b(0, 0, 0);
		}
	}
	imshow("Final Result", dst);

	cvtColor(dst, dst, CV_RGB2GRAY);
	//threshold(dst, dst,0, 0, CV_THRESH_BINARY | CV_THRESH_OTSU);

	//GaussianBlur(dst, dst, Size(5, 5), 200, 200);

	imshow("Final Resultyyy", dst);

	imwrite("temp.jpg", dst);

	Mat xxx = imread("temp.jpg");

	getContoursByCplus(xxx, 1000, 10);
	waitKey(0);
	return 0;
}