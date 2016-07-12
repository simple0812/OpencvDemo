#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

#include "iostream"
using namespace std;

/*------------------------------------------------------------------*/

int main(int argc, char **argv)
{

	/*-------------------------------------------//
	//功能：载入图像
	//原型：IplImage* cvLoadImage( const char* filename, int flags=CV_LOAD_IMAGE_COLOR );
	//参数：
	flags：指定读入图像的颜色和深度,指定的颜色可将输入图像转换为以下三种格式
	3通道(CV_LOAD_IMAGE_COLOR)也即彩色（>0）,
	单信道 (CV_LOAD_IMAGE_GRAYSCALE)也即灰色（=0）,
	保持不变(CV_LOAD_IMAGE_ANYCOLOR)(<0)
	深度指定输入的图像是否转为每个颜色信道每象素8位
	如果想要载入最真实的图像，选择CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_ANYCOLOR
	//-------------------------------------------*/
	char * fileName = argc == 2 ? argv[1] : (char *)"rice.png";   //"lena.jpg"; 
	IplImage * src = 0;
	//  原色彩
	src = cvLoadImage(fileName, -1);
	//  3通道
	//    src=cvLoadImage(fileName,1);
	//  灰度
	//    src=cvLoadImage(fileName,0);    
	//  
	if (!src)
	{
		cout << "Could not load image file!" << endl;
		return -1;
		//exit(0);
	}

	/*-------------------------------------------//
	//功能：创建窗口
	//原型：int cvNamedWindow( const char* name, int flags=CV_WINDOW_AUTOSIZE );
	//参数：CV_WINDOW_AUTOSIZE为1,表示窗口自动调整以适合图像
	为0时,表示自动按上一次使用的窗口尺寸
	//-------------------------------------------*/
	cvNamedWindow("src", 1);
	cvNamedWindow("dst", 1);

	/*-------------------------------------------//
	//功能：改变窗口大小
	//原型：void cvResizeWindow( const char* name, int width, int height );
	//参数：\\
	//-------------------------------------------*/
	
	cvResizeWindow("src", 512, 512);
	cvResizeWindow("dst", 512, 512);

	/*-------------------------------------------//
	//功能：移动窗口,不重叠
	//原型：void cvMoveWindow( const char* name, int x, int y );
	//参数：x、y：窗口左上角的x、y坐标
	//-------------------------------------------*/
	cvMoveWindow("src", 0, 0);
	cvMoveWindow("dst", 200, 200);

	/*-------------------------------------------//
	//功能：指定窗口中显示图像
	//原型：void cvShowImage( const char* name, const CvArr* image );
	//-------------------------------------------*/
	cvShowImage("src", src);

	/*-------------------------------------------//
	//功能：保存图像
	//原型：int cvSaveImage( const char* filename, const CvArr* image );
	//参数：图像格式的的取决于扩展名
	//-------------------------------------------*/
	cvSaveImage("rice.jpg", src);
	cvSaveImage("rice.bmp", src);

	/*-------------------------------------------//
	//功能：图像反转
	//说明：仅是测试用
	//-------------------------------------------*/
	//  定义工作位图
	IplImage * dst = cvCreateImage(cvGetSize(src), src->depth, src->nChannels);
	cvCopy(src, dst);   //dst已经创建
						//  获取图像信息
	int height, width, step, channels;
	uchar * data;
	height = dst->height;
	width = dst->width;
	step = dst->widthStep;//排列的行字节数
	channels = dst->nChannels;
	data = (uchar *)dst->imageData;//图像数据 char *imageData; 
								   //  反转图像
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
	//  显示
	cvShowImage("dst", dst);
	//  释放资源
	cvReleaseImage(&dst);

	/*-------------------------------------------//
	//功能：图像背景估计
	//说明：
	//-------------------------------------------*/
	//  创建工作位图
	IplImage *tmp = 0;  //定义临时图像指针
	IplImage *src_back = 0; //定义源图像背景指针
	tmp = cvCreateImage(cvGetSize(src), src->depth, src->nChannels);
	src_back = cvCreateImage(cvGetSize(src), src->depth, src->nChannels);
	//  创建结构元素
	IplConvKernel *element = 0; //定义形态学结构指针
	element = cvCreateStructuringElementEx(4, 4, 1, 1, CV_SHAPE_ELLIPSE, 0);
	//  用该结构对源图象进行数学形态学的开操作后，估计背景亮度    
	cvErode(src, tmp, element, 10);
	cvDilate(tmp, src_back, element, 10);
	cvNamedWindow("src_back", CV_WINDOW_AUTOSIZE);
	cvShowImage("src_back", src_back);

	/*-------------------------------------------//
	//功能：从源图象中减去背景图像
	//说明：
	//-------------------------------------------*/
	IplImage *dst_gray = 0; //定义源文件去掉背景后的目标灰度图像指针
	dst_gray = cvCreateImage(cvGetSize(src), src->depth, src->nChannels);
	cvSub(src, src_back, dst_gray, 0);
	cvNamedWindow("dst_gray", CV_WINDOW_AUTOSIZE);
	cvShowImage("dst_gray", dst_gray);

	/*-------------------------------------------//
	//功能：使用阀值操作将图像转换为二值图像
	//说明：
	//-------------------------------------------*/
	IplImage *dst_bw = 0; //定义源文件去掉背景后的目标二值图像指针
	dst_bw = cvCreateImage(cvGetSize(src), src->depth, src->nChannels);
	cvThreshold(dst_gray, dst_bw, 50, 255, CV_THRESH_BINARY); //取阀值为50把图像转为二值图像

															  //cvAdaptiveThreshold( dst_gray, dst_bw, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 3, 5 );

	cvNamedWindow("dst_bw", CV_WINDOW_AUTOSIZE);
	cvShowImage("dst_bw", dst_bw);

	/*-------------------------------------------//
	//功能：检查图像中的目标对象数量
	//说明：
	//-------------------------------------------*/
	int Number_Object = 0; //定义目标对象数量
	CvMemStorage *stor = 0;
	CvSeq * cont = 0;
	CvContourScanner contour_scanner;
	CvSeq * a_contour = 0;
	stor = cvCreateMemStorage(0);
	cont = cvCreateSeq(CV_SEQ_ELTYPE_POINT, sizeof(CvSeq), sizeof(CvPoint), stor);
	Number_Object = cvFindContours(dst_bw, stor, &cont, sizeof(CvContour), \
		CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cvPoint(0, 0)); //找到所有轮廓

	printf("Number_Object: %d\n", Number_Object);

	/*-------------------------------------------//
	//功能：计算图像中对象的统计属性
	//说明：
	//-------------------------------------------*/
	IplImage *dst_contours = 0; //定义轮廓图像指针
	int contour_area_tmp = 0; //定义目标对象面积临时寄存器    
	int contour_area_sum = 0; //定义目标所有对象面积的和    
	int contour_area_ave = 0; //定义目标对象面积平均值    
	int contour_area_max = 0; //定义目标对象面积最大值
	dst_contours = cvCreateImage(cvGetSize(src), src->depth, src->nChannels);
	cvThreshold(dst_contours, dst_contours, 0, 255, CV_THRESH_BINARY_INV); //在画轮廓前先把图像变成白色

	for (; cont; cont = cont->h_next)
	{
		cvDrawContours(dst_contours, cont, CV_RGB(255, 0, 0), CV_RGB(255, 0, 0), 0, 1, 8, cvPoint(0, 0)); //绘制当前轮廓

		contour_area_tmp = fabs(cvContourArea(cont, CV_WHOLE_SEQ)); //获取当前轮廓面积

		if (contour_area_tmp > contour_area_max)
		{
			contour_area_max = contour_area_tmp; //找到面积最大的轮廓

		}
		contour_area_sum += contour_area_tmp; //求所有轮廓的面积和

	}
	contour_area_ave = contour_area_sum / Number_Object; //求出所有轮廓的平均值

	printf("contour_area_ave: %d\n", contour_area_ave);
	printf("contour_area_max: %d\n", contour_area_max);
	cvNamedWindow("dst_contours", CV_WINDOW_AUTOSIZE);
	cvShowImage("dst_contours", dst_contours);

	/*-------------------------------------------//
	//功能：等待
	//原型：int cvWaitKey( int delay=0 );
	//参数：参数<=0表示不自动返回
	//注意：需要周期地被调用，除非HighGUI被用在某些能够处理事件的环境中。如在MFC环境下，这个函数不起作用
	//-------------------------------------------*/
	cvWaitKey(0);

	/*-------------------------------------------//
	//功能：销毁窗口、释放资源
	//原型：void cvDestroyAllWindows(void);
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