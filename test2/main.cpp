#include "opencv2/opencv.hpp"
#include "iostream"

using namespace cv;
using namespace std;


int Otsu(Mat& src)
{
	int height = src.rows;
	int width = src.cols;

	//histogram   
	float histogram[256] = { 0 };
	for (int i = 0; i < src.rows; i++)
		for (int j = 0; j < src.cols; j++) {
			histogram[src.at<uchar>(i, j)]++;
		}

	//normalize histogram   
	int size = height * width;
	for (int i = 0; i < 256; i++)
	{
		histogram[i] = histogram[i] / size;
	}

	//average pixel value   
	float avgValue = 0;

	for (int i = 0; i < 256; i++)
	{
		avgValue += i * histogram[i];  //����ͼ���ƽ���Ҷ� 
	}

	int threshold;
	float maxVariance = 0;
	float w = 0, u = 0;

	for (int i = 0; i < 256; i++)
	{
		w += histogram[i];  //���赱ǰ�Ҷ�iΪ��ֵ, 0~i �Ҷȵ�����(��������ֵ�ڴ˷�Χ�����ؽ���ǰ������) ��ռ����ͼ��ı��� 
		u += i * histogram[i];  // �Ҷ�i ֮ǰ������(0~i)��ƽ���Ҷ�ֵ�� ǰ�����ص�ƽ���Ҷ�ֵ 

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

int getContoursByCplus(char* Imgname, double minarea = 100, double whRatio = 1)
{
	Mat src, dst, canny_output;
	/// Load source image and convert it to gray  
	src = imread(Imgname, 0);

	if (!src.data)
	{
		std::cout << "read data error!" << std::endl;
		return -1;
	}
	blur(src, src, Size(3, 3));


	//the pram. for findContours,  
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	/// Detect edges using canny  
	Canny(src, canny_output, Otsu(src), 255, 3);
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
			//ɾ�����С���趨ֵ������  
			contours.erase(contours.begin() + i);
			std::wcout << "delete a small area" << std::endl;
			continue;
		}
		//����������ֱ�����  
		Rect aRect = boundingRect(contours[i]);
		if ((aRect.width / aRect.height)<whRatio)
		{
			//ɾ����߱���С���趨ֵ������  
			contours.erase(contours.begin() + i);
			std::wcout << "delete a unnomalRatio area" << std::endl;
			continue;
		}
	}
	/// Draw contours,��ɫ����  
	dst = Mat::zeros(canny_output.size(), CV_8UC3);
	
	cout << "count->" << contours.size() << endl;

	for (int i = 0; i< contours.size(); i++)
	{
		RNG rng;
		//�����ɫ  
		Scalar color = Scalar(255,255,0);
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

int main(int argc, char **argv)
{
	char* filename = "e:/cell3.jpg";
	Mat mat = imread(filename);
	
	Mat src, dst, canny_output;

	bitwise_xor(mat, Scalar(255), src);
	imshow("xxxx", src);

	//the pram. for findContours,  
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	/// Detect edges using canny  
	Canny(src, canny_output, 80, 255, 3);
	/// Find contours  
	findContours(canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	//CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE  

	dst = Mat::zeros(canny_output.size(), CV_8UC3);
	for (int i = 0; i< contours.size(); i++)
	{
		RNG rng;
		//�����ɫ  
		Scalar color = Scalar(255, 255, 0);
		//Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		drawContours(dst, contours, i, color, 2, 8, hierarchy, 0, Point());
	}

	// Create Window  
	char* source_window = "countors";
	namedWindow(source_window, CV_WINDOW_AUTOSIZE);
	imshow(source_window, dst);


	waitKey(0);
}


