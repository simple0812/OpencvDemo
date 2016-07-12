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

	//二值化  
	cvThreshold(src, src, 120, 255, CV_THRESH_BINARY);

	//提取轮廓  
	cvFindContours(src, storage, &contour, sizeof(CvContour), CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
	cvZero(dst);//清空数组  

				/*CvSeq* _contour为了保存轮廓的首指针位置，因为随后contour将用来迭代*/
	CvSeq* _contour = contour;


	int maxAreaIdx = -1, iteratorIdx = 0, xIndex = 0;//n为面积最大轮廓索引，m为迭代索引  
	for (int iteratorIdx = 0; contour != 0; contour = contour->h_next, iteratorIdx++/*更新迭代索引*/)
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
			//删除面积小于设定值的轮廓  
			cvSeqRemove(contour, 0);
			continue;
		}
		CvRect aRect = cvBoundingRect(contour, 0);
		if ((aRect.width / aRect.height)<whRatio)
		{
			//删除宽高比例小于设定值的轮廓  
			cvSeqRemove(contour, 0);
			continue;
		}
		//CvScalar color = CV_RGB( rand()&255, rand()&255, rand()&255 );//创建一个色彩值  
		//CvScalar color = CV_RGB(0, 255, 255);  

		//max_level 绘制轮廓的最大等级。如果等级为0，绘制单独的轮廓。如果为1，绘制轮廓及在其后的相同的级别下轮廓。  
		//如果值为2，所有的轮廓。如果等级为2，绘制所有同级轮廓及所有低一级轮廓，诸此种种。  
		//如果值为负数，函数不绘制同级轮廓，但会升序绘制直到级别为abs(max_level)-1的子轮廓。   
		//cvDrawContours(dst, contour, color, color, -1, 1, 8);//绘制外部和内部的轮廓  
	}
	contour = _contour; /*int k=0;*/
						//统计剩余轮廓，并画出最大面积的轮廓  
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
	//销毁窗口和图像存储  
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
	//convertScaleAbs(sure_fg, sure_fg);//将格式转为unit8

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


