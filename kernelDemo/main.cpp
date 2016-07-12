#include "opencv2/opencv.hpp"
#include "iostream"
#include <iostream>
#include <functional>

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



#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define MAX_NUM 5000  // Ԥ�����ϸ������
#define WIDTH 444  //ͼƬ���
#define HEIGHT 773   //ͼƬ�߶�

int label[HEIGHT][WIDTH];  //ÿ�����ص�ı�ǣ����С����ϸ�����
uchar pixel[HEIGHT][WIDTH];  //��¼ÿ�����ص�ĻҶ�ֵ
int area[MAX_NUM], perimeter[MAX_NUM];  // ��¼ÿ��ϸ����������ܳ�
int flag_cell[MAX_NUM];   //���ÿ��ϸ���Ƿ�����
float degree[MAX_NUM];  //��¼ÿ��ϸ�����ܼ���

int peried[MAX_NUM];     //���ĳ��ϸ���Ƿ񱻼�����ܳ���0��ʾû��
int beginx[2];   //��¼��ʼ���ܳ��ĳ�ʼ������(i,j)

int main()
{
	int cell_num = 0;  // ��¼ϸ������
	int counter = 0;   // ��ǩ����
	int max;  //����ϸ���ı��
	int min;  //��С��ϸ���ı��

	float average_degree = 0;  //ƽ���ܼ���
	float sum_degree = 0;
	float max_cell_degree, min_cell_degree;  //���ϸ������Сϸ�����ܼ���
	long sum_area = 0;  //����ϸ��������ܺ�
	long sum_perimeter = 0;  //����ϸ�����ܳ��ܺ�
	int average_area;  //ƽ�����
	int average_perimeter;  //ƽ���ܳ�

	int max_cell_center_x, max_cell_center_y;  //���ϸ�������ĵ�����
	int min_cell_center_x, min_cell_center_y;  //��Сϸ�������ĵ�����

	void DoLabel(int i, int j, int counter);  //���ÿ�����ص�ϸ�����
	void DoPerimeter(int i, int j, int b, int tmp);    // ����һ��ϸ�����ܳ�,�ݹ麯��
	int find_max(int first, int last);       // �����������ϸ��
	int find_min(int first, int last);       // ���������С��ϸ��
	int find_center_x(int cell);    //����ϸ���е������
	int find_center_y(int cell);    //����ϸ���е�������

	IplImage* image = cvLoadImage("e:/fs.png", 0);   // �ԻҶȷ�ʽ��ͼ��
	if (!image) return -1;
	IplImage* image2 = image;       //�Ҷ�ͼƬ
	cvSaveImage("cell2.bmp", image2);

	for (int i = 0; i<HEIGHT; i++)
		for (int j = 0; j<WIDTH; j++)
		{
			label[i][j] = 0;
			uchar* pix = &CV_IMAGE_ELEM(image2, uchar, i, j);
			pixel[i][j] = *pix;
		}

	for (int i = 0; i<HEIGHT; i++)
		for (int j = 0; j<WIDTH; j++)
		{
			if (pixel[i][j] <170 && !label[i][j])
			{
				counter++;
				DoLabel(i, j, counter);
			}

		}
	cell_num = counter;

	for (int i = 0; i<HEIGHT; i++)               //Ѱ�ұ߽��
		for (int j = 0; j<WIDTH; j++)
		{
			int tmp = label[i][j];
			if (tmp != 0)
			{
				if (!peried[tmp])
				{
					beginx[0] = i;
					beginx[1] = j;
					DoPerimeter(i, j, 0, tmp);
					peried[tmp] = 1;
				}
			}
		}

	for (int c = 1; c <= cell_num; c++)
	{
		if (area[c]<400)
		{
			flag_cell[c] = 0;
			counter--;
		}
		else flag_cell[c] = 1;
	}

	max = find_max(1, cell_num);
	min = find_min(1, cell_num);
	max_cell_center_x = find_center_x(max);
	max_cell_center_y = find_center_y(max);
	min_cell_center_x = find_center_x(min);
	min_cell_center_y = find_center_y(min);

	for (int c = 1; c <= cell_num; c++)
	{
		if (flag_cell[c] == 1)
		{
			degree[c] = (float)area[c] / (perimeter[c] * perimeter[c]);
			sum_degree += degree[c];
			sum_area += area[c];
			sum_perimeter += perimeter[c];
		}
	}

	cell_num = counter;
	average_degree = sum_degree / cell_num;
	average_area = sum_area / cell_num;
	average_perimeter = sum_perimeter / cell_num;
	max_cell_degree = (float)area[max] / (perimeter[max] * perimeter[max]);
	min_cell_degree = (float)area[min] / (perimeter[min] * perimeter[min]);

	printf("the cell number is : %d\n", cell_num);
	printf("\nthe biggest cell :\n");
	printf("      center:( %d , %d )\n", max_cell_center_x, max_cell_center_y);
	printf("      the area is : %d\n", area[max]);
	printf("      the perimeter is : %d\n", perimeter[max]);
	printf("      the degree is : %f\n", max_cell_degree);

	printf("\nthe smallest cell :\n");
	printf("      center:( %d , %d )\n", min_cell_center_x, min_cell_center_y);
	printf("      the area is : %d\n", area[min]);
	printf("      the perimeter is : %d\n", perimeter[min]);
	printf("      the degree is : %f\n", min_cell_degree);

	printf("\nthe average degree is : %f\n", average_degree);
	printf("the average area of all cells is : %d\n", average_area);
	printf("the average perimeter of all cells is : %d\n", average_perimeter);

	for (int i = 0; i<HEIGHT; i++)
		for (int j = 0; j<WIDTH; j++)
		{
			uchar* pix = &CV_IMAGE_ELEM(image2, uchar, i, j);
			if (label[i][j] == max || label[i][j] == min)
				*pix = 0;
		}
	cvSaveImage("cell3.bmp", image2);
	cvNamedWindow("test", 1);
	cvShowImage("test", image2);
	cvWaitKey();
	cvReleaseImage(&image);
	cvReleaseImage(&image2);
	cvDestroyWindow("test");
	return 0;
	system("pause");
}

void DoLabel(int i, int j, int counter)
{
	if (pixel[i][j] >  210) return;                  //��һЩ�����ĵ�Ҳ�����
	if (label[i][j] == 0)
	{
		label[i][j] = counter;
		area[counter]++;
	}
	else return;

	if (i > 0) DoLabel(i - 1, j, counter);
	if (j > 0) DoLabel(i, j - 1, counter);
	if (i < HEIGHT - 1) DoLabel(i + 1, j, counter);
	if (j < WIDTH - 1) DoLabel(i, j + 1, counter);

	return;
}

int next(int i, int j, int b)
{
	if (b == 0)
	{
		if (j>0)
		{
			if (label[i][j - 1]) return 0;
		}
		if (i<HEIGHT - 1)
		{
			if (label[i + 1][j]) return 1;
		}
		if (j<WIDTH - 1)
		{
			if (label[i][j + 1]) return 2;
		}
		if (i>0)
		{
			if (label[i - 1][j]) return 3;
		}
	}
	else if (b == 1)
	{
		if (i<HEIGHT - 1)
		{
			if (label[i + 1][j]) return 1;
		}
		if (j<WIDTH - 1)
		{
			if (label[i][j + 1]) return 2;
		}
		if (i>0)
		{
			if (label[i - 1][j]) return 3;
		}
		if (j>0)
		{
			if (label[i][j - 1]) return 0;
		}
	}
	else if (b == 2)
	{
		if (j<WIDTH - 1)
		{
			if (label[i][j + 1]) return 2;
		}
		if (i>0)
		{
			if (label[i - 1][j]) return 3;
		}
		if (j>0)
		{
			if (label[i][j - 1]) return 0;
		}
		if (i<HEIGHT - 1)
		{
			if (label[i + 1][j]) return 1;
		}
	}
	else if (b == 3)
	{
		if (i>0)
		{
			if (label[i - 1][j]) return 3;
		}
		if (j>0)
		{
			if (label[i][j - 1]) return 0;
		}
		if (i<HEIGHT - 1)
		{
			if (label[i + 1][j]) return 1;
		}
		if (j<WIDTH - 1)
		{
			if (label[i][j + 1]) return 2;
		}
	}
}

void DoPerimeter(int i, int j, int b, int tmp)
{
	int f;
	int t = next(i, j, b);
	f = (t + 3) % 4;

	switch (t)
	{
	case 0:
		if (i != beginx[0] || j - 1 != beginx[1])    // �ж��Ƿ�����ʼ���غ�
		{
			perimeter[tmp]++;
			DoPerimeter(i, j - 1, f, tmp);
		}
		break;
	case 1:
		if (i + 1 != beginx[0] || j != beginx[1])
		{
			perimeter[tmp]++;
			DoPerimeter(i + 1, j, f, tmp);
		}
		break;
	case 2:
		if (i != beginx[0] || j + 1 != beginx[1])
		{
			perimeter[tmp]++;
			DoPerimeter(i, j + 1, f, tmp);
		}
		break;
	case 3:
		if (i - 1 != beginx[0] || j != beginx[1])
		{
			perimeter[tmp]++;
			DoPerimeter(i - 1, j, f, tmp);
		}
		break;
	default:
		break;
	}
}


int find_max(int first, int last)
{
	int i, max;

	max = first;
	for (i = first; i <= last; i++)
	{
		if (flag_cell[i] == 1)
			if (area[i]>area[max])
				max = i;
	}
	return max;
}

int find_min(int first, int last)
{
	int i, min;

	for (i = 1; i<last; i++)
		if (flag_cell[i] == 1)
		{
			min = i;
			break;
		}
	for (i = first; i <= last; i++)
	{
		if (flag_cell[i] == 1)
			if (area[i]<area[min])
				min = i;
	}
	return min;
}

int find_center_x(int cell)
{
	long sum_x = 0;
	int center_x;

	for (int i = 0; i<HEIGHT; i++)
		for (int j = 0; j<WIDTH; j++)
		{
			if (label[i][j] == cell)
			{
				sum_x += i;
			}
		}
	center_x = sum_x / area[cell];
	return center_x;
}

int find_center_y(int cell)
{
	long sum_y = 0;
	int center_y;

	for (int i = 0; i<HEIGHT; i++)
		for (int j = 0; j<WIDTH; j++)
		{
			if (label[i][j] == cell)
			{
				sum_y += j;
			}
		}
	center_y = sum_y / area[cell];
	return center_y;
}

//int main() 
//{
//	Mat source = imread("e://111.jpg", CV_LOAD_IMAGE_COLOR);
//	imshow("Source image", source);
//
//	// Get gray graph of source image.  
//	Mat gray;
//	cvtColor(source, gray, CV_RGB2GRAY);
//	imshow("Gray image", gray);
//
//	int thresholdn = Otsu(gray);
//	Mat bin;
//	threshold(gray, bin, thresholdn, 255, CV_THRESH_BINARY);
//	imshow("gray", bin);
//
//
//	vector<vector<Point>> contours;
//	vector<Vec4i> hierarchy;
//	
//
//	Mat cont = Mat::zeros(bin.size(), CV_8UC1);
//	Mat ellp = Mat::zeros(bin.size(), CV_8UC1);
//
//	findContours(bin, contours, hierarchy,
//		CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
//
//	vector<double> area;
//	double totalarea;
//
//	for (int i = 0; i < contours.size(); i++) {
//		area[i] = contourArea(contours[i]);
//		totalarea += area[i];
//	}
//
//	int counter = contours.size();
//	int average = totalarea / counter;
//
//
//
//
//	int minarea = cont.rows*cont.cols, maxarea = 0, mini, maxi;
//
//	for (int i = 0; i < contours.size(); i++) {
//		if (area[i]<average * 0.3) {
//			counter--;
//		}
//		else {
//			if (area[i]<minarea) {
//				minarea = area[i];
//				mini = i;
//			}
//
//			if (area[i]>maxarea) {
//				maxarea = area[i];
//				maxi = i;
//			}
//		}
//	}
//
//	CvPoint center;
//
//	CvSize size;
//
//	CvBox2D32f* box;
//
//	CvPoint* PointArray;
//
//	CvPoint2D32f* PointArray2D32f;
//
//
//
//	PointArray = (CvPoint*)malloc(count * sizeof(CvPoint));
//
//	PointArray2D32f = (CvPoint2D32f*)malloc(count * sizeof(CvPoint2D32f));
//
//
//
//	// Alloc memory for ellipse data.  
//
//	box = (CvBox2D32f*)malloc(sizeof(CvBox2D32f));
//
//
//
//
//
//	// Convert CvPoint set to CvBox2D32f set.  
//
//	for (int j = 0; j<count; j++) {
//
//		PointArray2D32f[j].x = (float)contours[i][j].x;
//
//		PointArray2D32f[j].y = (float)contours[i][j].y;
//
//	}
//
//
//
//	// Fits ellipse to current contour.  
//
//	cvFitEllipse(PointArray2D32f, count, box);
//
//
//
//	// Convert ellipse data from float to integer representation.  
//
//
//
//	center.x = cvRound(box->center.x);
//
//	center.y = cvRound(box->center.y);
//
//	size.width = cvRound(box->size.width*0.5);
//
//	size.height = cvRound(box->size.height*0.5);
//
//
//
//	drawContours(cont, contours, i, 255);
//
//	ellipse(ellp, center, size, box->angle, 0, 360, 255, 1, CV_AA, 0);
//
//
//
//
//
//	if (i == mini)
//
//		cout << "��Сϸ�����Ϊ" << area[i] << ',' << "�ܳ���" << count << ','
//
//		<< "������X+��ɣ�" << 180 - box->angle << "�Ƚ�," << "����" << '(' << center.x << ',' << center.y << ");" << endl;
//
//	if (i == maxi)
//
//		cout << "���ϸ�����Ϊ" << area[i] << ',' << "�ܳ���" << count << ','
//
//		<< "������X+��ɣ�" << 180 - box->angle << "�Ƚ�," << "����" << '(' << center.x << ',' << center.y << ");" << endl;
//
//	// Free memory.            
//
//	free(PointArray);
//
//	free(PointArray2D32f);
//
//	free(box);
//
//	
//	
//	waitKey(0);
//	return 0;
//
//}
