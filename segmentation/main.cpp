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

static int getContoursByC(char* Imgname, double minarea = 100, double whRatio = 1)
{
	IplImage* src = cvLoadImage(Imgname, CV_LOAD_IMAGE_GRAYSCALE);
	if (!src)
	{
		printf("read data error!\n");
		return -1;
	}
	IplImage* dst = cvCreateImage(cvGetSize(src), 8, 3);

	//the parm. for cvFindContours  
	CvMemStorage* storage = cvCreateMemStorage(0);
	CvSeq* contour = 0;
	double maxarea = 0;

	//for display  
	cvNamedWindow("Source", CV_WINDOW_NORMAL);
	cvShowImage("Source", src);

	//��ֵ��  
	cvThreshold(src, src, 120, 255, CV_THRESH_BINARY);

	//��ȡ����  
	cvFindContours(src, storage, &contour, sizeof(CvContour), CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
	cvZero(dst);//�������  

				/*CvSeq* _contourΪ�˱�����������ָ��λ�ã���Ϊ���contour����������*/
	CvSeq* _contour = contour;


	int maxAreaIdx = -1, iteratorIdx = 0, xIndex = 0;//nΪ����������������mΪ��������  
	for (int iteratorIdx = 0; contour != 0; contour = contour->h_next, iteratorIdx++/*���µ�������*/)
	{

		double tmparea = fabs(cvContourArea(contour));
		if (tmparea > maxarea)
		{
			xIndex++;
			maxarea = tmparea;
			maxAreaIdx = iteratorIdx;
			continue;
		}
		if (tmparea < minarea)
		{
			//ɾ�����С���趨ֵ������  
			cvSeqRemove(contour, 0);
			continue;
		}
		CvRect aRect = cvBoundingRect(contour, 0);
		if ((aRect.width / aRect.height)<whRatio)
		{
			//ɾ����߱���С���趨ֵ������  
			cvSeqRemove(contour, 0);
			continue;
		}
		//CvScalar color = CV_RGB( rand()&255, rand()&255, rand()&255 );//����һ��ɫ��ֵ  
		//CvScalar color = CV_RGB(0, 255, 255);  

		//max_level �������������ȼ�������ȼ�Ϊ0�����Ƶ��������������Ϊ1��������������������ͬ�ļ�����������  
		//���ֵΪ2�����е�����������ȼ�Ϊ2����������ͬ�����������е�һ��������������֡�  
		//���ֵΪ����������������ͬ�������������������ֱ������Ϊabs(max_level)-1����������   
		//cvDrawContours(dst, contour, color, color, -1, 1, 8);//�����ⲿ���ڲ�������  
	}
	contour = _contour; /*int k=0;*/
						//ͳ��ʣ��������������������������  
	int count = 0;

	while (contour != 0)
	{
		contour++;
		double tmparea = fabs(cvContourArea(contour));
		if (tmparea == maxarea)
		{
			CvScalar color = CV_RGB(255, 0, 0);
			try
			{
				cvDrawContours(dst, contour, color, color, -1, 1, 8);

			}
			catch (const std::exception&)
			{
				//
			}
		}
		else
		{
			CvScalar color = CV_RGB(255, 255, 0);
			try
			{
				cvDrawContours(dst, contour, color, color, -1, 1, 8);

			}
			catch (const std::exception&)
			{
				//
			}
		}
	}

	for (; contour != 0; contour = contour->h_next)
	{
		count++;
		double tmparea = fabs(cvContourArea(contour));
		if (tmparea == maxarea /*k==n*/)
		{
			CvScalar color = CV_RGB(255, 0, 0);
			cvDrawContours(dst, contour, color, color, -1, 1, 8);
		}
		else
		{
			CvScalar color = CV_RGB(255, 255, 0);
			cvDrawContours(dst, contour, color, color, -1, 1, 8);
		}
		/*k++;*/
	}

	printf("The total number of contours is:%d", count);
	cvNamedWindow("Components", CV_WINDOW_AUTOSIZE);
	cvShowImage("Components", dst);
	cvSaveImage("dst.jpg", dst);
	//roateProcess(dst);  
	cvWaitKey(0);
	//���ٴ��ں�ͼ��洢  
	cvDestroyWindow("Source");
	cvReleaseImage(&src);
	cvDestroyWindow("Components");
	cvReleaseImage(&dst);
	return 0;
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

int main(int argc, char **argv)
{
	//char * filename = "e:/coins.jpg";
	char * filename = "e:/xxx.png";
	Mat img = imread(filename);
	Mat gray;

	cvtColor(img, gray, CV_RGB2GRAY);

	imshow("img", img);
	imshow("gray", gray);

	Mat bin;
	threshold(gray, bin, Otsu(gray), 255, CV_THRESH_BINARY);
	imshow("bin", bin);

	Mat opening, sure_bg, sure_fg, dist_transform, unknown;
	
	Mat kernel(3, 3, CV_8U);

	morphologyEx(bin, opening, 1, kernel, Point(-1,-1), 3);
	dilate(opening, sure_bg, kernel,Point(-1,-1), 2);
	distanceTransform(opening, dist_transform, DIST_L2, 5);
	threshold(dist_transform, sure_fg, 200*0.7, 255, 0);
	//convertScaleAbs(sure_fg, sure_fg);//����ʽתΪunit8

	sure_fg.convertTo(sure_fg, CV_8UC3);
	subtract(sure_bg, sure_fg, unknown);

	imshow("opening", opening);
	imshow("sure_bg", sure_bg);
	imshow("sure_fg", sure_fg);
	imshow("dist_transform", dist_transform);
	imshow("unknown", unknown);

	waitKey(0);

	return 0;
}


