#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

#include "iostream"
using namespace std;

//CV_IMAGE_ELEM( img, T, y, x )
void GetMaxMin(IplImage*img, int *max, int *min)
{
	uchar tmp;
	int i = 0, j = 0;
	for (i = 0; i<img->width; i++)
		for (j = 0; j<img->height; j++)
		{
			tmp = CV_IMAGE_ELEM(img, uchar, j, i);
			if (tmp>*max) *max = tmp;
			if (tmp<*min) *min = tmp;
		}
}

void PaintColor(IplImage *img, int r = 255, int g = 255, int b = 255)
{
	int i = 0, j = 0;
	for (i = 0; i<img->width; i++)
		for (j = 0; j<img->height; j++) {
			CV_IMAGE_ELEM(img, uchar, j, i * 3) = b;
			CV_IMAGE_ELEM(img, uchar, j, i * 3 + 1) = g;
			CV_IMAGE_ELEM(img, uchar, j, i * 3 + 2) = r;
		}
}


int main(int argc, char* argv[])
{
	IplImage *src, *dst, *back, *gray, *fore, *dst_bw;

	//Step1: Load image to src and Show source image
	if (argc == 2) {
		src = cvLoadImage(argv[1]);
	}
	else {
		src = cvLoadImage("cell.bmp");
		//src = cvLoadImage("e:/cell_test.png");
	}

	if (src == 0) { cout << "Image doesn't exist"; return 0; }

	cvNamedWindow("Source image");
	cvShowImage("Source image", src);

	//Step 2:Get gray graph of source image.
	gray = cvCreateImage(cvGetSize(src), src->depth, 1);
	cvCvtColor(src, gray, CV_RGB2GRAY);
	if (gray != 0) {
		cvNamedWindow("Gray image");
		cvShowImage("Gray image", gray);
	}

	//Step 3:Get background color
	back = cvCreateImage(cvGetSize(gray), gray->depth, 1);
	cvDilate(gray, back, NULL, 10);

	if (back != 0) {
		cvNamedWindow("Background Color");
		cvShowImage("Background Color", back);
	}
	int back_max = 0, back_min = 255;
	GetMaxMin(back, &back_max, &back_min);
	//  cout<<"Statics in gray image of background image"<<endl;
	//   cout<<"Min="<<back_min<<"  Max="<<back_max<<endl;   

	//Step 4:Get foreground color
	fore = cvCreateImage(cvGetSize(gray), gray->depth, gray->nChannels);
	cvErode(gray, fore, NULL, 40);

	if (fore != 0) {
		cvNamedWindow("Foreground Color");
		cvShowImage("Foreground Color", fore);
	}


	int fore_min = 255, fore_max = 0;
	GetMaxMin(fore, &fore_max, &fore_min);
	//cout<<"Statics for foreground image"<<endl;
	//cout<<"Min="<<fore_min<<"  Max="<<fore_max;


	//Step 5: Create binary image
	dst_bw = cvCreateImage(cvGetSize(gray), gray->depth, gray->nChannels);
	cvThreshold(gray, dst_bw, (fore_max + back_min) / 2, 255, CV_THRESH_BINARY);
	cvNamedWindow("Binary image");
	cvShowImage("Binary image", dst_bw);


	int Number_Object = 0;
	int contour_area_sum = 0;
	int contour_area_ave = 0;
	int contour_area_min = 10000;
	int contour_area_max = 0;
	int contour_arc_max, contour_arc_min, contour_arc_ave, contour_arc_sum = 0;

	//Step 6:Find all contours in the image.
	CvMemStorage *stor = 0;
	CvSeq * cont = 0;
	CvSeq *maxcell = 0;
	CvSeq *mincell = 0;

	

	stor = cvCreateMemStorage(0);
	cont = cvCreateSeq(CV_SEQ_ELTYPE_POINT, sizeof(CvSeq), sizeof(CvPoint), stor);
	Number_Object = cvFindContours(dst_bw, stor, &cont, sizeof(CvContour), CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cvPoint(0, 0));
	cout << "Number of contours=" << Number_Object << endl;

	//Step 7: Looking for the largest and smallest cell, statistic area and arc length for all cells
	dst = cvCreateImage(cvGetSize(src), src->depth, 3);
	PaintColor(dst);

	int cellnum = 0;
	for (; cont->h_next; cont = cont->h_next);	//Skip the outer contour

	cont = cont->v_next;

	for (; cont; cont = cont->h_next)
	{
		cellnum++;
		cvDrawContours(dst, cont, CV_RGB(0, 255, 0), CV_RGB(255, 0, 0), 0, 1, 8, cvPoint(0, 0));//Draw contour

		int contour_area_tmp = fabs(cvContourArea(cont, CV_WHOLE_SEQ));
		int contour_arc_tmp = cvArcLength(cont, CV_WHOLE_SEQ);

		if (contour_area_tmp > contour_area_max)
		{
			contour_area_max = contour_area_tmp;
			contour_arc_max = contour_arc_tmp;
			maxcell = cont;

		}
		if (contour_area_tmp< contour_area_min)
		{
			contour_area_min = contour_area_tmp;
			contour_arc_min = contour_arc_tmp;
			mincell = cont;
		}

		contour_arc_sum += contour_arc_tmp;
		contour_area_sum += contour_area_tmp;

	}
	contour_area_ave = contour_area_sum / cellnum;
	contour_arc_ave = contour_arc_sum / cellnum;

	cout << "Number of Cell=" << cellnum << endl;
	//Step 8: Get rough center point
	CvRect rect1 = cvBoundingRect(maxcell, 0);
	CvRect rect2 = cvBoundingRect(mincell, 0);

	cout << "Largest Cell:" << endl;
	cout << "Area=" << contour_area_max << "\tArc Length=" << contour_arc_max << "\tDensity=" << (float)contour_area_max / (contour_arc_max*contour_arc_max) << endl;
	cout << "center at:(" << rect1.x + rect1.width / 2 << "," << rect1.y + rect1.height / 2 << ")" << endl;
	cout << "Smallest Cell:" << endl;
	cout << "Area=" << contour_area_min << "\tArc Length=" << contour_arc_min << "\tDensity=" << (float)contour_area_min / (contour_arc_min*contour_arc_min) << endl;
	cout << "center at:(" << rect2.x + rect2.width / 2 << "," << rect2.y + rect2.height / 2 << ")" << endl;

	//cvCircle(dst,cvPoint(rect1.x+rect1.width/2,rect1.y+rect1.height/2), 3, CV_RGB(0,0,255),-1,8,0);//Draw a circle at the center of largest/smallest cell
	//cvCircle(dst,cvPoint(rect2.x+rect2.width/2, rect2.y+rect2.height/2),3,CV_RGB(0,0,255),-1,8,0);

	cout << "Average area of cells=" << contour_area_ave << endl;
	cout << "Average arc length of cells=" << contour_arc_ave << endl;

	cvNamedWindow("Result Image");
	cvShowImage("Result Image", dst);



	cvWaitKey(-1); //Wait to exit

	cvReleaseImage(&src);
	cvReleaseImage(&gray);
	cvReleaseImage(&back);
	cvReleaseImage(&fore);
	cvReleaseImage(&dst_bw);
	cvReleaseImage(&dst);

	cvDestroyWindow("Source image");
	cvDestroyWindow("Background Color");
	cvDestroyWindow("Foreground Color");
	cvDestroyWindow("Gray image");
	cvDestroyWindow("Binary image");
	cvDestroyWindow("Result Image");



}


