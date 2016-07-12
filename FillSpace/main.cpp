#include "opencv2/opencv.hpp"
#include "iostream"
#include <math.h>

#define min(x,y) (x<y?x:y)
#define R_THRESHHOLD 125
#define S_THRESHHOLD 60

using namespace cv;
using namespace std;


//RGB+HSI��ɫģ��
void colorModel(IplImage *src, IplImage * dst) {
	int step = NULL;
	int rows = src->height;
	int cols = src->width;
	for (int i = 0; i < rows; i++) {
		//uchar* dataS = src.ptr<uchar>(i);
		//uchar* dataD = dst.ptr<uchar>(i);
		uchar *dataS = (uchar*)src->imageData;
		uchar *dataD = (uchar*)dst->imageData;
		for (int j = 0; j < cols; j++) {
			step = i*src->widthStep + j*src->nChannels;;
			float S;
			float b = dataS[step] / 255.0;
			float g = dataS[step + 1] / 255.0;
			float r = dataS[step + 2] / 255.0;
			float minRGB = min(min(r, g), b);
			float den = r + g + b;
			if (den == 0)	//��ĸ����Ϊ0
				S = 0;
			else
				S = (1 - 3 * minRGB / den) * 100;
			if (dataS[step + 2] <= R_THRESHHOLD || dataS[step + 2] < 165) {
				dataD[step] = 0;
				dataD[step + 1] = 0;
				dataD[step + 2] = 0;
			}
			else if (dataS[step + 2] <= dataS[step + 1] || dataS[step + 1] <= dataS[step]) {
				dataD[step] = 0;
				dataD[step + 1] = 0;
				dataD[step + 2] = 0;
			}
			else if (S <= (float)(S_THRESHHOLD*(255 - dataS[step + 2])) / R_THRESHHOLD) {
				dataD[step] = 0;
				dataD[step + 1] = 0;
				dataD[step + 2] = 0;
			}
			else {
				dataD[step] = dataS[step];
				dataD[step + 1] = dataS[step + 1];
				dataD[step + 2] = dataS[step + 2];
			}
		}
	}
}

//���ݷָ���ȷ�����������
void fillSeg(IplImage *src, IplImage *tempdst)
{
	CvSeq * contour = NULL;
	CvMemStorage * storage = cvCreateMemStorage();
	//�ڶ�ֵͼ����Ѱ������,CV_CHAIN_APPROX_SIMPLE - ѹ��ˮƽ����ֱ�ͶԽǷָ������ֻ����ĩ�˵����ص�
	cvFindContours(src, storage, &contour, sizeof(CvContour), CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
	cvZero(tempdst);
	for (contour; contour != 0; contour = contour->h_next)
	{
		//�����ķ���Ӱ������ķ��š���˺���Ҳ��᷵�ظ��Ľ����Ӧ�ú��� fabs() �õ�����ľ���ֵ�� 
		double area = cvContourArea(contour, CV_WHOLE_SEQ);
		//�������������򲿷����������
		if (fabs(area) < 10)
		{
			continue;
		}
		//	CvScalar color = CV_RGB( 255, 255, 255 );
		CvPoint *point = new CvPoint[contour->total];
		CvPoint *Point;

		//printf("ͼ��ָ�contour->total\t%d\n",contour->total);
		for (int i = 0; i<contour->total; i++)
		{
			Point = (CvPoint*)cvGetSeqElem(contour, i);
			point[i].x = Point->x;
			point[i].y = Point->y;
		}
		int pts[1] = { contour->total };
		cvFillPoly(tempdst, &point, pts, 1, CV_RGB(255, 255, 255));//��������ڲ� 
	}
}

int main() {
	IplImage *img = NULL;		//����ͼ��8bit 3ͨ��
	IplImage *colTemp = NULL;	//��ɫ�ָ��(���ڲ��ն�)�Ļ���ͼƬ
	IplImage *gray = NULL;		//�Ҷ�ͼ
	IplImage *mask = NULL;		//��ֵͼ�����ڸ���ͼ�����Ĥ
	IplImage *dst = NULL;		//�����������ͼ��8bit��3ͨ��

	img = cvLoadImage("E:/fire.jpg");					//����ԭʼͼƬ
	colTemp = cvCreateImage(cvGetSize(img), img->depth, img->nChannels);//������ɫ�ָ��(���ڲ��ն�)�Ļ���ͼƬ
	gray = cvCreateImage(cvGetSize(img), img->depth, 1);
	mask = cvCreateImage(cvGetSize(img), img->depth, 1);
	dst = cvCreateImage(cvGetSize(img), img->depth, img->nChannels);		//�������Ļ���ͼƬ
	cvZero(dst);
	colorModel(img, colTemp);
	cvCvtColor(colTemp, gray, CV_BGR2GRAY);
	//ʹ��cvFindContours������cvFillPoly�����ͨ���ڲ��ն�
	fillSeg(gray, mask);
	cvCopy(img, dst, mask);
	cvShowImage("ԭʼͼƬ", img);
	cvShowImage("��ɫ�ָ��", colTemp);
	cvShowImage("��䴦��ͼƬ", dst);
	cvShowImage("mask", mask);
	cvWaitKey();
}

//void fillHole(const Mat srcBw, Mat &dstBw)
//{
//	Size m_Size = srcBw.size();
//	Mat Temp = Mat::zeros(m_Size.height + 2, m_Size.width + 2, srcBw.type());//��չͼ��  
//	srcBw.copyTo(Temp(Range(1, m_Size.height + 1), Range(1, m_Size.width + 1)));
//
//	cv::floodFill(Temp, Point(0, 0), Scalar(255));
//
//	Mat cutImg;//�ü���չ��ͼ��  
//	Temp(Range(1, m_Size.height + 1), Range(1, m_Size.width + 1)).copyTo(cutImg);
//
//	dstBw = srcBw | (~cutImg);
//}
//
//int main()
//{
//	Mat img = cv::imread("e:/test.jpg");
//
//	Mat gray;
//	cv::cvtColor(img, gray, CV_RGB2GRAY);
//
//	Mat bw;
//	cv::threshold(gray, bw, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
//
//	Mat bwFill;
//	fillHole(bw, bwFill);
//
//	imshow("���ǰ", gray);
//	imshow("����", bwFill);
//	waitKey();
//	return 0;
//}