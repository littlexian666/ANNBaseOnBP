#include "src.h"
#include "stdafx.h"
#include "iostream"
#include "io.h"
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp> 


using namespace std;
using namespace cv;
/*·��*/
string SrcTestOne(int i)
{
	char buffer[100];
	string src1 = "H:\\��ѧ\\����ϵͳ\\Mnist-image\\test\\";
	sprintf_s(buffer, "%d", i);
	string src3 = src1 + buffer;
	return src3;
}

string SrcTrainOne(int i)
{
	char buffer[100];
	string src1 = "H:\\��ѧ\\����ϵͳ\\Mnist-image\\train\\";
	sprintf_s(buffer, "%d", i);
	string src3 = src1 + buffer;
	return src3;
}

char * SrcTrainAll(int i) {
	char src[100];
	string src2 = "\\*.png";
	string src_pictures = SrcTrainOne(i) + src2;
	strcpy_s(src, src_pictures.c_str());
	return src;
}

char * SrcTestAll(int i) {
	char src[100];
	string src2 = "\\*.png";
	string src_pictures = SrcTestOne(i) + src2;
	strcpy_s(src, src_pictures.c_str());
	return src;
}

/*���㲢����ͼƬ���ظ���*/
/*����ÿ��ͼƬ�Ĵ�С��һ����,����������ֵ������ֵ*/
int NumOfPixel()
{
	string src_picture= SrcTrainOne(0)+"\\0_1.png";
	Mat img = imread(src_picture, 0);
	return img.rows*img.cols;
}