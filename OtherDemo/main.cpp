#include <opencv2/opencv.hpp>
#include <iostream>
using namespace std;
using namespace cv;

typedef struct
{
	CvPoint cP;
	int height;
	int width;

} RecP;
//�Զ���������  
namespace my
{
	bool less(const RecP& s1, const RecP& s2)
	{
		//if(s1.cP.x < s2.cP.x && s1.cP.y < s2.cP.y)  
		return s1.cP.x < s2.cP.x;          //��������  
	}
}

void PrintVector(vector<RecP> & vec)
{
	for (vector<RecP>::iterator n = vec.begin(); n != vec.end(); n++)
	{
		cout << n->cP.x << '\t' << n->cP.y << '\t' << n->height << '\t' << n->width << endl;
	}
}

IplImage* src;
IplImage* img;
IplImage* dst;
IplImage* bianyuan;
CvMemStorage* storage = NULL;

int thresh = 50;

void on_trackbar(int pos)
{
	CvSeq* contour = 0;
	if (storage == NULL)
	{
		dst = cvCreateImage(cvGetSize(bianyuan), 8, 3);
		storage = cvCreateMemStorage(0);
	}
	else
	{
		cvClearMemStorage(storage);
	}
	cvSmooth(bianyuan, bianyuan, CV_GAUSSIAN, 3, 3, 0, 0);
	cvThreshold(bianyuan, img, thresh, 200, CV_THRESH_BINARY);

	cvNamedWindow("threshold", 1);
	cvShowImage("threshold", img);

	cvFindContours(img, storage, &contour, sizeof(CvContour), CV_RETR_CCOMP, CV_CHAIN_APPROX_NONE, cvPoint(0, 0));  //��������  
	cvZero(dst);     //������������ͨ��������Ԫ�ص�ֵ������Ϊ0  

	vector<RecP> vecP;

	int n = 0;
	for (; contour; contour = contour->h_next)
	{
		CvRect rect = cvBoundingRect(contour, 1);       // ��ȡ���α߽��   

		if (abs(rect.width - rect.height)>3)
		{
			rect.width = 0;
			rect.height = 0;
			rect.x = rect.x + 640;
			rect.y = rect.y + 480;
		}

		CvPoint pt1 = cvPoint(rect.x, rect.y), pt2 = cvPoint(rect.x + rect.width, rect.y + rect.height);   //������ζԶ���  

		cvRectangle(dst, pt1, pt2, CV_RGB(255, 0, 0), 1, CV_AA, 0);      //���ƾ��α߿�  
		cvLine(dst, pt1, pt2, CV_RGB(0, 255, 0), 1, CV_AA, 0);           //���ζԽ�������  

		pt1 = cvPoint(rect.x, rect.y + rect.height),
		pt2 = cvPoint(rect.x + rect.width, rect.y);

		cvLine(dst, pt1, pt2, CV_RGB(0, 255, 0), 1, CV_AA, 0);            //���ζԽ�������  

		RecP tmp;
		CvPoint p1;
		p1 = cvPoint(rect.x + rect.width / 2, rect.y + rect.height / 2);   //������������  

		tmp.cP = p1;
		tmp.height = rect.height;
		tmp.width = rect.width;
		vecP.push_back(tmp);
		//printf("(%d,%d)\n", p1);  
		sort(vecP.begin(), vecP.end(), my::less);    //��������  
													 //printf("(%d,%d):(%d,%d)\n", vecP[n].cP, vecP[n].height, vecP[n].width);  
		n++;
	}
	PrintVector(vecP);

	cvShowImage("Components", dst);
}

int main()
{
	const char* a = "e:/qi.jpg";
	src = cvLoadImage(a, 0);
	cvSmooth(src, src, CV_GAUSSIAN, 5, 5, 0, 0);
	cvNamedWindow("Source0000", 1);
	cvShowImage("Source0000", src);

	IplImage* bw = NULL;
	IplImage* color = NULL;
	IplImage* jh = NULL;
	IplImage* sm = NULL;
	if (!src)
		return -1;
	jh = cvCreateImage(cvGetSize(src), 8, 1);
	sm = cvCreateImage(cvGetSize(src), 8, 1);
	bw = cvCreateImage(cvGetSize(src), 8, 1);
	color = cvCreateImage(cvGetSize(src), 8, 3);
	cvEqualizeHist(src, jh);
	cvSmooth(jh, sm, CV_MEDIAN, 3, 3, 0, 0);
	
	cvCanny(sm, bw, 200, 600, 3);
	cvCvtColor(bw, color, CV_GRAY2BGR);
	cvSaveImage("color.bmp", color);

	const char* b = "color.bmp";
	bianyuan = cvLoadImage(b, 0);
	img = cvCreateImage(cvGetSize(bianyuan), 8, 1);

	cvNamedWindow("Source", 1);
	cvShowImage("Source", bianyuan);

	cvNamedWindow("Components", 1);

	on_trackbar(0);

	cvWaitKey(0);
	cvDestroyWindow("sorce");
	cvDestroyWindow("threshold");
	cvDestroyWindow("Components");
	cvReleaseImage(&src);
	cvReleaseImage(&img);
	cvReleaseImage(&dst);
	cvReleaseMemStorage(&storage);
	return 0;
}