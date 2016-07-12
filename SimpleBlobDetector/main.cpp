#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;


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

int main(int argc, char** argv)
{

	// Read image
	Mat im = imread("e://temp.jpg", IMREAD_GRAYSCALE);
	im = inverseImg(im);
	imshow("src", im);

	SimpleBlobDetector::Params params;

	// Change thresholds
	//params.minThreshold = 4;
	//params.maxThreshold = 40;

	// Filter by Area.
	/*params.filterByArea = true;
	params.minArea = 1500;*/

	// Filter by Circularity
	//params.filterByCircularity = true;
	//params.minCircularity = 0.1;

	// Filter by Convexity
	params.filterByConvexity = true;
	params.minConvexity = 0.87;

	// Filter by Inertia
	//params.filterByInertia = true;
	//params.minInertiaRatio = 0.01;


	// Storage for blobs
	vector<KeyPoint> keypoints;


#if CV_MAJOR_VERSION < 3   // If you are using OpenCV 2

	// Set up detector with params
	SimpleBlobDetector detector(params);

	// Detect blobs
	detector.detect(im, keypoints);
#else 

	// Set up detector with params
	Ptr<SimpleBlobDetector> detector = SimpleBlobDetector::create(params);

	// Detect blobs
	detector->detect(im, keypoints);
#endif 

	// Draw detected blobs as red circles.
	// DrawMatchesFlags::DRAW_RICH_KEYPOINTS flag ensures
	// the size of the circle corresponds to the size of blob

	Mat im_with_keypoints;
	Mat im_with_keypointxs;
	Mat newBImg(im.rows, im.cols, im.type());

	drawKeypoints(newBImg, keypoints, im_with_keypoints, Scalar(255, 0, 0), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
	drawKeypoints(im, keypoints, im_with_keypointxs, Scalar(255, 0, 0), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

	// Show blobs
	imshow("keypoints", im_with_keypoints);
	imshow("keypointsxx", im_with_keypointxs);
	cout << "keypoints->" << keypoints.size() << endl;
	waitKey(0);

}