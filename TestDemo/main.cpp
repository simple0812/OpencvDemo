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

void removeBlackBg(Mat& src, int th) 
{
	for (int i = 0; i<src.rows; i++)
	{
		for (int j = 0; j<src.cols; j++)
		{

			double r = src.at<Vec3b>(i, j)[0];
			double g = src.at<Vec3b>(i, j)[1];
			double b = src.at<Vec3b>(i, j)[2];

			if (r <= th && g <= th && b <= th) {
				src.at<Vec3b>(i, j)[0] = 255;
				src.at<Vec3b>(i, j)[1] = 255;
				src.at<Vec3b>(i, j)[2] = 255;
			}
		}
	}

	imshow("removeBlackBg", src);
}


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

int main(int argc, char** argv)
{
	Mat src, canny_output, dst;
	char * filename = "e:/cell_test.png";
	src = imread(filename);
	imshow("src", src);

	removeBlackBg(src, 18);
	

	cvtColor(src, gray, CV_RGB2GRAY);
	medianBlur(gray, gray, 3);
	medianBlur(gray, gray, 3);
	medianBlur(gray, gray, 3);
	medianBlur(gray, gray, 3);
	/*GaussianBlur(gray, gray, Size(3, 3), 0, 0);
	GaussianBlur(gray, gray, Size(3, 3), 0, 0);
	GaussianBlur(gray, gray, Size(3, 3), 0, 0);
	GaussianBlur(gray, gray, Size(3, 3), 0, 0);
	*/
	imshow("gray", gray);

	createTrackbar("Threshold", "gray", &threshval, 255, on_trackbar);
	createTrackbar("Threshold2", "gray", &threshval2, 255, on_trackbar1);
	on_trackbar(threshval, 0);//轨迹条回调函数  

	Mat temp;
	threshold(gray, temp, Otsu(gray), 255, CV_THRESH_BINARY);

	imshow("temp", temp);

	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	Canny(temp, canny_output, 0, 255, 3);


		//pointPolygonTest()
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