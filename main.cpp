#include <cstdio>
#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <time.h>

using namespace cv;
using namespace std;

float gasdev()
{
	//float ran1(long *idum);
	static int iset = 0;
	static float gset;
	float fac, rsq, v1, v2;
	double u;

	if (iset == 0) {
		do {
			//srand(time(NULL));
			u = rand() / (double)RAND_MAX;
			v1 = 2.0 * u - 1.0;
			v2 = 2.0 * u - 1.0;
			rsq = v1 * v1 + v2 * v2;
		} while (rsq >= 1.0 || rsq == 0.0);
		fac = sqrt(-2.0 * log(rsq) / rsq);
		gset = v1 * fac;
		iset = 1;
		return v2 * fac;
	}
	else {
		iset = 0;
		return gset;
	}
}

float rand_normal(float mean, float stddev) {
	static float n2 = 0.0;
	static int n2_cached = 0;
	if (!n2_cached)	{
		float x, y, r;
		do {
			x = 2.0 * rand() / RAND_MAX - 1; //亂數產生 AND_MAX=32767
			y = 2.0 * rand() / RAND_MAX - 1;

			r = x * x + y * y;
		} while (r == 0.0 || r > 1.0);
		{
			float d = sqrt(-2.0*log(r) / r);
			float n1 = x * d;
			n2 = y * d;
			double result = n1 * stddev + mean;	//增加期望值與標準差
			n2_cached = 1;
			return result;
		}
	}
	else {
		n2_cached = 0;
		return (n2*stddev + mean);
	}
}

int main() {
	srand(time(NULL));	//隨機亂數種子
	float no;
	for (int i = 1; i <= 10; i++) {	//測試亂數
		no = rand_normal(0, 1000);
		cout << no << endl;
	}

	const char* imageName = "lena.jpg";	//讀取圖片檔
	Mat image, image2, image3, image4;
	image = imread(imageName, 0);
	image2 = imread(imageName, 0);
	image3 = imread(imageName, 0);
	image4 = imread(imageName, 0);
	int h = image.rows + 2;
	int l = image.cols + 2;
	//Mat img1(image.cols, image.rows, CV_8U,Scalar(100));
	for (int i = 0; i < image.rows;i++) {	//把亂數加入原本圖片產生雜訊
		for (int j = 0; j < image.cols; j++) {
			no = rand_normal(0, 30);
			if (image.at<uchar>(i, j) + no < 0)
				image2.at<uchar>(i, j) = 0;
			else
				image2.at<uchar>(i, j) = image.at<uchar>(i, j) + no;
			if (image2.at<uchar>(i, j) >= 255)
				image2.at<uchar>(i, j) = 255;
			if (image2.at<uchar>(i, j) <= 0)
				image2.at<uchar>(i, j) = 0;
		}
	}
	for (int i = 2; i < image.rows - 2; i++) {	//進行Adaptive local noise reduction filter
		for (int j = 2; j < image.cols - 2; j++) {
			//使用 2X2 Adaptive local noise reduction filter
			image3.at<uchar>(i, j) = image2.at<uchar>(i, j) - ( (image2.at<uchar>(i, j) -
				(image2.at<uchar>(i, j) + image2.at<uchar>(i - 1, j - 1) +
					image2.at<uchar>(i - 1, j) + image2.at<uchar>(i, j - 1)) / 4) );
			//使用 3X3 Adaptive local noise reduction filter
			image4.at<uchar>(i, j) = image2.at<uchar>(i, j) - ( (image2.at<uchar>(i, j) -
				(image2.at<uchar>(i, j) + image2.at<uchar>(i - 1, j - 1) +
					image2.at<uchar>(i - 1, j) + image2.at<uchar>(i, j - 1) +
					image2.at<uchar>(i - 2, j - 2) + image2.at<uchar>(i - 1, j - 2) +
					image2.at<uchar>(i - 2, j - 1) + image2.at<uchar>(i - 2, j) +
					image2.at<uchar>(i, j - 2)) / 9) );
		}
	}
	//輸出檔案
	imwrite("ori.png", image);
	namedWindow("ori", CV_WINDOW_NORMAL);
	imshow("ori", image);
	imwrite("big banana.png", image2);
	namedWindow("src", CV_WINDOW_NORMAL);
	imshow("src", image2);
	imwrite("big banana2.png", image3);
	namedWindow("dst 2x2", CV_WINDOW_NORMAL);
	imshow("dst 2x2", image3);
	imwrite("big banana3.png", image4);
	namedWindow("dst 3x3", CV_WINDOW_NORMAL);
	imshow("dst 3x3", image4);
	waitKey(0);

	return 0;
}
