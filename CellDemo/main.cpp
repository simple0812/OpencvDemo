#include <opencv2/opencv.hpp>
#include <iostream>
using namespace std;
using namespace cv;

Mat img;
int threshval = 1;            //轨迹条滑块对应的值，给初值160  

int Otsu(Mat& src)
{
	int height = src.rows;
	int width = src.cols;

	float histogram[256] = { 0 };
	for (int i = 0; i < src.rows; i++)
		for (int j = 0; j < src.cols; j++) {
			histogram[src.at<uchar>(i, j)]++;
		}

	int size = height * width;
	for (int i = 0; i < 256; i++)
	{
		histogram[i] = histogram[i] / size;
	}

	float avgValue = 0;

	for (int i = 0; i < 256; i++)
	{
		avgValue += i * histogram[i];  //整幅图像的平均灰度 
	}

	int threshold;
	float maxVariance = 0;
	float w = 0, u = 0;

	for (int i = 0; i < 256; i++)
	{
		w += histogram[i];  //假设当前灰度i为阈值, 0~i 灰度的像素(假设像素值在此范围的像素叫做前景像素) 所占整幅图像的比例 
		u += i * histogram[i];  // 灰度i 之前的像素(0~i)的平均灰度值： 前景像素的平均灰度值 

		float t = avgValue * w - u;
		float variance = t * t / (w * (1 - w));

		if (variance > maxVariance)
		{
			maxVariance = variance;
			threshold = i;
		}
	}

	return threshold;
}

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
			newBImgData[i*step + j] = 255 - binaryData[i*step + j];
		}
	}

	return newBImg.clone();
}

static void on_trackbar(int xx, void*)
{
	Mat tmp;
	GaussianBlur(img, tmp, Size(5, 5), xx, xx);
	imshow("GaussianBlur", tmp);

	Mat aa;
	threshold(tmp, aa, Otsu(tmp), 255, CV_THRESH_BINARY);
	imshow("GaussianBlura", aa);
}

int main(int, char** argv)
{
	//char * filename = "e:/half_w.png";
	char * filename = "e:/cell1.jpg";
	Mat src = imread(filename);
	imshow("src", src);

	Mat gray;

	cvtColor(src, gray, CV_RGB2GRAY);
	imshow("gray", gray);

	Mat bin;
	threshold(gray, bin, Otsu(gray), 255, CV_THRESH_BINARY);
	Mat bw = inverseImg(bin);
	imshow("Binary Image", bw);
	img = bw;
	

	createTrackbar("Threshold", "Binary Image", &threshval, 255, on_trackbar);
	on_trackbar(threshval, 0);//轨迹条回调函数  


	/*Mat small, big;
	Mat kernal = Mat::ones(4,4, CV_8UC1);
	erode(bw, big, kernal);
	imshow("big Image ", big);
	kernal = Mat::ones(5, 5, CV_8UC1);
	dilate(big, small, kernal);

	bw = inverseImg(small);

	imshow("small Image ", bw);
*/
	//Mat canny_output;
	//vector<vector<Point> > contours;
	//vector<Vec4i> hierarchy;

	//Canny(bw, canny_output, 80, 255, 3);
	//imshow("canny_output", canny_output);
	//findContours(canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	//double maxarea = 0;
	//int maxAreaIdx = 0;
	//int minarea = 20;

	//for (int i = 0; i<contours.size(); i++)
	//{

	//	double tmparea = fabs(contourArea(contours[i]));
	//	if (tmparea>maxarea)
	//	{
	//		maxarea = tmparea;
	//		maxAreaIdx = i;
	//		continue;
	//	}

	//	if (tmparea < minarea)
	//	{
	//		//删除面积小于设定值的轮廓  
	//		contours.erase(contours.begin() + i);
	//		std::wcout << "delete a small area" << std::endl;
	//		continue;
	//	}
	//	//计算轮廓的直径宽高  
	//	//Rect aRect = boundingRect(contours[i]);
	//	//if ((aRect.width / aRect.height)<whRatio)
	//	//{
	//	//	//删除宽高比例小于设定值的轮廓  
	//	//	contours.erase(contours.begin() + i);
	//	//	std::wcout << "delete a unnomalRatio area" << std::endl;
	//	//	continue;
	//	//}
	//}
	//imshow("clean canny_output", canny_output);
	//cout << "count->" << contours.size() << endl;



	//Mat dist;
	//distanceTransform(bw, dist, DIST_L2, 5);
	//normalize(dist, dist, 0, .36, NORM_MINMAX);//需要手动设置
	//imshow("Distance Transform Image", dist);

	//threshold(dist, dist, 0.2, 1., CV_THRESH_BINARY);//需要收到设置
	//Mat kernel1 = Mat::ones(3, 3, CV_8UC1);
	//erode(dist, dist, kernel1);
	//imshow("Peaks", dist);


	//Mat dist_8u;
	//dist.convertTo(dist_8u, CV_8U);
	//vector<vector<Point> > contours;
	//findContours(dist_8u, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	//Mat markers = Mat::zeros(dist.size(), CV_32SC1);
	//for (size_t i = 0; i < contours.size(); i++)
	//	drawContours(markers, contours, static_cast<int>(i), Scalar::all(static_cast<int>(i) + 1), -1);
	//circle(markers, Point(5, 5), 3, CV_RGB(255, 255, 255), -1);
	//cout << "contours->" << contours.size() << endl;

	//watershed(src, markers);
	//Mat mark = Mat::zeros(markers.size(), CV_8UC1);
	//markers.convertTo(mark, CV_8UC1);
	//bitwise_not(mark, mark);
	//vector<Vec3b> colors;
	//for (size_t i = 0; i < contours.size(); i++)
	//{
	//	int b = theRNG().uniform(0, 255);
	//	int g = theRNG().uniform(0, 255);
	//	int r = theRNG().uniform(0, 255);
	//	colors.push_back(Vec3b((uchar)b, (uchar)g, (uchar)r));
	//}

	//Mat dst = Mat::zeros(markers.size(), CV_8UC3);

	//for (int i = 0; i < markers.rows; i++)
	//{
	//	for (int j = 0; j < markers.cols; j++)
	//	{
	//		int index = markers.at<int>(i, j);
	//		if (index > 0 && index <= static_cast<int>(contours.size()))
	//			dst.at<Vec3b>(i, j) = colors[index - 1];
	//		else
	//			dst.at<Vec3b>(i, j) = Vec3b(0, 0, 0);
	//	}
	//}
	//imshow("Final Result", dst);
	waitKey(0);
	return 0;
}