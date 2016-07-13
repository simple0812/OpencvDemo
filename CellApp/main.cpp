#include <opencv2/opencv.hpp>
#include <iostream>
using namespace std;
using namespace cv;

Mat dist, src;
int threshval = 55;

static  void on_trackbar(int th, void*);

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

void fillHole(const Mat srcBw, Mat &dstBw)
{
	Size m_Size = srcBw.size();
	Mat Temp = Mat::zeros(m_Size.height + 2, m_Size.width + 2, srcBw.type());//延展图像
	srcBw.copyTo(Temp(Range(1, m_Size.height + 1), Range(1, m_Size.width + 1)));

	cv::floodFill(Temp, Point(0, 0), Scalar(255));

	Mat cutImg;//裁剪延展的图像
	Temp(Range(1, m_Size.height + 1), Range(1, m_Size.width + 1)).copyTo(cutImg);

	dstBw = srcBw | (~cutImg);
}

int main(int, char** argv)
{
	char * filename = "e:/cell4.jpg";
	src = imread(filename);
	imshow("src", src);

	Mat gray;

	cvtColor(src, gray, CV_RGB2GRAY);
	GaussianBlur(gray, gray, Size(3, 3), 0, 0);
	imshow("gray", gray);

	Mat bin;
	threshold(gray, bin, Otsu(gray), 255, CV_THRESH_BINARY);

	Mat bw;
	bitwise_xor(bin, Scalar(255), bw);
	imshow("Binary Image", bw);//黑底

	fillHole(bw, bw);
	imshow("tempx", bw);

	distanceTransform(bw, dist, DIST_L2, 5);
	normalize(dist, dist, 0, 1, NORM_MINMAX);//需要手动设置
	imshow("Distance Transform Image", dist);

	createTrackbar("Threshold", "Distance Transform Image", &threshval, 255, on_trackbar);
	on_trackbar(threshval, 0);//轨迹条回调函数  

	waitKey(0);

	while (1)
	{
		int key = waitKey(10);

		if (key == -1) continue;

		switch (key)
		{
		case 'a':
		case 2424832:
			threshval -= 1;
			createTrackbar("Threshold", "Distance Transform Image", &threshval, 255, on_trackbar);
			on_trackbar(threshval, 0);//轨迹条回调函数 
			break;

		case 'd':
		case 2555904:
			threshval += 1;
			createTrackbar("Threshold", "Distance Transform Image", &threshval, 255, on_trackbar);
			on_trackbar(threshval, 0);//轨迹条回调函数 
			break;
		}



	}
	return 0;
}

void on_trackbar(int th, void*)
{
	Mat tmp;
	double x = (double)th / 100;
	threshold(dist, tmp, x, 1., CV_THRESH_BINARY);//需要手动设置
	Mat kernel1 = Mat::ones(3, 3, CV_8UC1);
	erode(tmp, tmp, kernel1);
	imshow("Peaks", tmp);


	Mat dist_8u;
	tmp.convertTo(dist_8u, CV_8U);
	vector<vector<Point> > contours;
	findContours(dist_8u, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	Mat markers = Mat::zeros(tmp.size(), CV_32SC1);
	for (size_t i = 0; i < contours.size(); i++)
		drawContours(markers, contours, static_cast<int>(i), Scalar::all(static_cast<int>(i) + 1), -1);
	//circle(markers, Point(5, 5), 3, CV_RGB(255, 255, 255), -1);
	cout << "threshval->" << x << " contours->" << contours.size() << endl;

	watershed(src, markers);
	Mat mark = Mat::zeros(markers.size(), CV_8UC1);
	markers.convertTo(mark, CV_8UC1);
	//imshow("markers", mark);
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
}
