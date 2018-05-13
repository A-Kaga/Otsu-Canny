/*
基于Otsu算法的自适应Canny边缘检测
最后修改	2018/4/26
AKaga Presents
*/

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

Mat g_SrcImage = imread("highway.jpg", 0);
Mat g_ResImage;
double g_p[256];
int g_count[256] = { 0 };
double g_variance;
int g_MaxThreshold;
int g_MinThreshold;

void ImageScan();
double ImageExpectation(int, int);
double ImageVariance(double, double, double, double, int, int);
void FindMaxVariance();
void procession();

int main()
{
	procession();
	imshow("source", g_SrcImage);
	Canny(g_SrcImage, g_ResImage, g_MinThreshold, g_MaxThreshold, 3);
	cout << g_MinThreshold << ' ' << g_MaxThreshold;
	imshow("Test", g_ResImage);

	waitKey(0);
	return 0;
}

void ImageScan()
{
	int GrayValue;
	int i, j;
	for (i = 0; i < g_SrcImage.rows; ++i)
	{
		for (j = 0; j < g_SrcImage.cols; ++j)
		{
			GrayValue = (int)g_SrcImage.at<uchar>(i, j);
			g_count[GrayValue]++;
		}
	}

	for (i = 0; i < 255; i++)
	{
		int pixel = g_SrcImage.cols * g_SrcImage.rows;
		g_p[i] = (double)(g_count[i]) / pixel;
	}
}

double ImageExpectation(int Min, int Max) //求数学期望
{
	double Ek = 0.0, Ekm = 0.0, Em = 0.0, Eav = 0.0;

	for (int i = 0; i < Min; i++)
		Ek += i * g_p[i];
	for (int i = Min; i < Max; i++)
		Ekm += i * g_p[i];
	for (int i = Max; i < 255; i++)
		Em += i * g_p[i];
	for (int i = 0; i < 255; i++)
		Eav += i * g_p[i];

	return ImageVariance(Ek, Ekm, Em, Eav, Min, Max);
}

double ImageVariance(double ek, double ekm, double em, double eav, int k, int m) //求方差	
{
	double p_km = 0.0;
	for (int i = k; i < m; i++)
		p_km += g_p[i];
	double variance = (ek - eav) * (ek - eav) * g_p[k] +
		(ekm - eav) * (ekm - eav) * p_km +
		(em - eav) * (em - eav) * g_p[m];

	return variance;
}

void FindMaxVariance() //求满足最大方差的阈值
{
	double MaxVariance = 0.0;
	int MinThreshold, MaxThreshold;
	for (MinThreshold = 0; MinThreshold < 255; MinThreshold++)
	{
		for (MaxThreshold = MinThreshold; MaxThreshold < 255; MaxThreshold++)
		{
			double temp = ImageExpectation(MinThreshold, MaxThreshold);
			if (MaxVariance < temp)
			{
				MaxVariance = temp;
				g_MaxThreshold = MaxThreshold;
				g_MinThreshold = MinThreshold;
			}
		}
	}
}

void procession()
{
	ImageScan();
	FindMaxVariance();
}