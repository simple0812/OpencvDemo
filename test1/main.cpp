#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

#include "iostream"
using namespace std;

/*------------------------------------------------------------------*/

int main(int argc, char **argv)
{

	/*-------------------------------------------//
	//���ܣ�����ͼ��
	//ԭ�ͣ�IplImage* cvLoadImage( const char* filename, int flags=CV_LOAD_IMAGE_COLOR );
	//������
	flags��ָ������ͼ�����ɫ�����,ָ������ɫ�ɽ�����ͼ��ת��Ϊ�������ָ�ʽ
	3ͨ��(CV_LOAD_IMAGE_COLOR)Ҳ����ɫ��>0��,
	���ŵ� (CV_LOAD_IMAGE_GRAYSCALE)Ҳ����ɫ��=0��,
	���ֲ���(CV_LOAD_IMAGE_ANYCOLOR)(<0)
	���ָ�������ͼ���Ƿ�תΪÿ����ɫ�ŵ�ÿ����8λ
	�����Ҫ��������ʵ��ͼ��ѡ��CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_ANYCOLOR
	//-------------------------------------------*/
	char * fileName = argc == 2 ? argv[1] : (char *)"rice.png";   //"lena.jpg"; 
	IplImage * src = 0;
	//  ԭɫ��
	src = cvLoadImage(fileName, -1);
	//  3ͨ��
	//    src=cvLoadImage(fileName,1);
	//  �Ҷ�
	//    src=cvLoadImage(fileName,0);    
	//  
	if (!src)
	{
		cout << "Could not load image file!" << endl;
		return -1;
		//exit(0);
	}

	/*-------------------------------------------//
	//���ܣ���������
	//ԭ�ͣ�int cvNamedWindow( const char* name, int flags=CV_WINDOW_AUTOSIZE );
	//������CV_WINDOW_AUTOSIZEΪ1,��ʾ�����Զ��������ʺ�ͼ��
	Ϊ0ʱ,��ʾ�Զ�����һ��ʹ�õĴ��ڳߴ�
	//-------------------------------------------*/
	cvNamedWindow("src", 1);
	cvNamedWindow("dst", 1);

	/*-------------------------------------------//
	//���ܣ��ı䴰�ڴ�С
	//ԭ�ͣ�void cvResizeWindow( const char* name, int width, int height );
	//������\\
	//-------------------------------------------*/
	
	cvResizeWindow("src", 512, 512);
	cvResizeWindow("dst", 512, 512);

	/*-------------------------------------------//
	//���ܣ��ƶ�����,���ص�
	//ԭ�ͣ�void cvMoveWindow( const char* name, int x, int y );
	//������x��y���������Ͻǵ�x��y����
	//-------------------------------------------*/
	cvMoveWindow("src", 0, 0);
	cvMoveWindow("dst", 200, 200);

	/*-------------------------------------------//
	//���ܣ�ָ����������ʾͼ��
	//ԭ�ͣ�void cvShowImage( const char* name, const CvArr* image );
	//-------------------------------------------*/
	cvShowImage("src", src);

	/*-------------------------------------------//
	//���ܣ�����ͼ��
	//ԭ�ͣ�int cvSaveImage( const char* filename, const CvArr* image );
	//������ͼ���ʽ�ĵ�ȡ������չ��
	//-------------------------------------------*/
	cvSaveImage("rice.jpg", src);
	cvSaveImage("rice.bmp", src);

	/*-------------------------------------------//
	//���ܣ�ͼ��ת
	//˵�������ǲ�����
	//-------------------------------------------*/
	//  ���幤��λͼ
	IplImage * dst = cvCreateImage(cvGetSize(src), src->depth, src->nChannels);
	cvCopy(src, dst);   //dst�Ѿ�����
						//  ��ȡͼ����Ϣ
	int height, width, step, channels;
	uchar * data;
	height = dst->height;
	width = dst->width;
	step = dst->widthStep;//���е����ֽ���
	channels = dst->nChannels;
	data = (uchar *)dst->imageData;//ͼ������ char *imageData; 
								   //  ��תͼ��
	for (int i = 0; i<height; i++)
	{
		for (int j = 0; j<width; j++)
		{
			for (int k = 0; k<channels; k++)
			{
				data[i*step + j*channels + k] = 255 - data[i*step + j*channels + k];
			}
		}
	}
	//  ��ʾ
	cvShowImage("dst", dst);
	//  �ͷ���Դ
	cvReleaseImage(&dst);

	/*-------------------------------------------//
	//���ܣ�ͼ�񱳾�����
	//˵����
	//-------------------------------------------*/
	//  ��������λͼ
	IplImage *tmp = 0;  //������ʱͼ��ָ��
	IplImage *src_back = 0; //����Դͼ�񱳾�ָ��
	tmp = cvCreateImage(cvGetSize(src), src->depth, src->nChannels);
	src_back = cvCreateImage(cvGetSize(src), src->depth, src->nChannels);
	//  �����ṹԪ��
	IplConvKernel *element = 0; //������̬ѧ�ṹָ��
	element = cvCreateStructuringElementEx(4, 4, 1, 1, CV_SHAPE_ELLIPSE, 0);
	//  �øýṹ��Դͼ�������ѧ��̬ѧ�Ŀ������󣬹��Ʊ�������    
	cvErode(src, tmp, element, 10);
	cvDilate(tmp, src_back, element, 10);
	cvNamedWindow("src_back", CV_WINDOW_AUTOSIZE);
	cvShowImage("src_back", src_back);

	/*-------------------------------------------//
	//���ܣ���Դͼ���м�ȥ����ͼ��
	//˵����
	//-------------------------------------------*/
	IplImage *dst_gray = 0; //����Դ�ļ�ȥ���������Ŀ��Ҷ�ͼ��ָ��
	dst_gray = cvCreateImage(cvGetSize(src), src->depth, src->nChannels);
	cvSub(src, src_back, dst_gray, 0);
	cvNamedWindow("dst_gray", CV_WINDOW_AUTOSIZE);
	cvShowImage("dst_gray", dst_gray);

	/*-------------------------------------------//
	//���ܣ�ʹ�÷�ֵ������ͼ��ת��Ϊ��ֵͼ��
	//˵����
	//-------------------------------------------*/
	IplImage *dst_bw = 0; //����Դ�ļ�ȥ���������Ŀ���ֵͼ��ָ��
	dst_bw = cvCreateImage(cvGetSize(src), src->depth, src->nChannels);
	cvThreshold(dst_gray, dst_bw, 50, 255, CV_THRESH_BINARY); //ȡ��ֵΪ50��ͼ��תΪ��ֵͼ��

															  //cvAdaptiveThreshold( dst_gray, dst_bw, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 3, 5 );

	cvNamedWindow("dst_bw", CV_WINDOW_AUTOSIZE);
	cvShowImage("dst_bw", dst_bw);

	/*-------------------------------------------//
	//���ܣ����ͼ���е�Ŀ���������
	//˵����
	//-------------------------------------------*/
	int Number_Object = 0; //����Ŀ���������
	CvMemStorage *stor = 0;
	CvSeq * cont = 0;
	CvContourScanner contour_scanner;
	CvSeq * a_contour = 0;
	stor = cvCreateMemStorage(0);
	cont = cvCreateSeq(CV_SEQ_ELTYPE_POINT, sizeof(CvSeq), sizeof(CvPoint), stor);
	Number_Object = cvFindContours(dst_bw, stor, &cont, sizeof(CvContour), \
		CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cvPoint(0, 0)); //�ҵ���������

	printf("Number_Object: %d\n", Number_Object);

	/*-------------------------------------------//
	//���ܣ�����ͼ���ж����ͳ������
	//˵����
	//-------------------------------------------*/
	IplImage *dst_contours = 0; //��������ͼ��ָ��
	int contour_area_tmp = 0; //����Ŀ����������ʱ�Ĵ���    
	int contour_area_sum = 0; //����Ŀ�����ж�������ĺ�    
	int contour_area_ave = 0; //����Ŀ��������ƽ��ֵ    
	int contour_area_max = 0; //����Ŀ�����������ֵ
	dst_contours = cvCreateImage(cvGetSize(src), src->depth, src->nChannels);
	cvThreshold(dst_contours, dst_contours, 0, 255, CV_THRESH_BINARY_INV); //�ڻ�����ǰ�Ȱ�ͼ���ɰ�ɫ

	for (; cont; cont = cont->h_next)
	{
		cvDrawContours(dst_contours, cont, CV_RGB(255, 0, 0), CV_RGB(255, 0, 0), 0, 1, 8, cvPoint(0, 0)); //���Ƶ�ǰ����

		contour_area_tmp = fabs(cvContourArea(cont, CV_WHOLE_SEQ)); //��ȡ��ǰ�������

		if (contour_area_tmp > contour_area_max)
		{
			contour_area_max = contour_area_tmp; //�ҵ������������

		}
		contour_area_sum += contour_area_tmp; //�����������������

	}
	contour_area_ave = contour_area_sum / Number_Object; //�������������ƽ��ֵ

	printf("contour_area_ave: %d\n", contour_area_ave);
	printf("contour_area_max: %d\n", contour_area_max);
	cvNamedWindow("dst_contours", CV_WINDOW_AUTOSIZE);
	cvShowImage("dst_contours", dst_contours);

	/*-------------------------------------------//
	//���ܣ��ȴ�
	//ԭ�ͣ�int cvWaitKey( int delay=0 );
	//����������<=0��ʾ���Զ�����
	//ע�⣺��Ҫ���ڵر����ã�����HighGUI������ĳЩ�ܹ������¼��Ļ����С�����MFC�����£����������������
	//-------------------------------------------*/
	cvWaitKey(0);

	/*-------------------------------------------//
	//���ܣ����ٴ��ڡ��ͷ���Դ
	//ԭ�ͣ�void cvDestroyAllWindows(void);
	//-------------------------------------------*/
	cvDestroyAllWindows();
	cvReleaseImage(&src);
	cvReleaseImage(&tmp);
	cvReleaseImage(&src_back);
	cvReleaseImage(&dst_gray);
	cvReleaseImage(&dst_bw);
	cvReleaseImage(&dst_contours);
	cvReleaseMemStorage(&stor);
	cvDestroyWindow("src");
	cvDestroyWindow("src_back");
	cvDestroyWindow("dst_gray");
	cvDestroyWindow("dst_bw");
	cvDestroyWindow("dst_contours");

	return 0;

	system("pause");

}