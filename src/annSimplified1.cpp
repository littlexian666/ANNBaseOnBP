#include "stdafx.h"
#include "windows.h"
#include "iostream"
#include "ctime"
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include "io.h"
#include "ArrayFile.h"
#include "src.h"

using namespace std;
using namespace cv;


/*反向传播*/
void reverseTransmission(int label,int numOfInput,int numOfOutput, int numOfHide1, int numOfHide2, int* input,double* hideLayer1, double* hideLayer2, double* output, double** weightOfInput, double** weightOfHide1, double** weightOfHide2, double** weightHide1Init, double** weightHide2Init, double* outputModel)
{
	//学习率
	double learnRate = 0.1;
	double influenceRate = 0;
	double *sumOfHide2 = new double[numOfHide2] {0};
	double *sumOfHide1 = new double[numOfHide1] {0};
	double temp_lastweight;
	double temp = 0, temp1 = 0;
	/*-------更新隐层2的权值-------*/
	for (int h = 0; h < numOfHide2; h++)
	{
		for (int q = 0; q < numOfOutput; q++)
		{
			temp = (-1) * (outputModel[q] - output[q]) * output[q] * (1 - output[q]);
			//更新隐层2的权值,加了一个冲量项
			weightOfHide2[h][q] -= learnRate * temp * hideLayer2[h] + influenceRate * weightOfHide2[h][q];
			//为前一层(隐层1)权值的更新做准备
			sumOfHide2[h] += temp * weightHide2Init[h][q];
		}
	}
	/*-------更新隐层1的权值-------*/
	for (int k = 0; k < numOfHide1; k++)
	{
		for (int h = 0; h < numOfHide2; h++)
		{
			temp1 = hideLayer2[h] * (1 - hideLayer2[h]);
			//更新隐层1的权值,加了一个冲量项
			weightOfHide1[k][h] -= learnRate * sumOfHide2[h] * temp1 * hideLayer1[k] + influenceRate * weightOfHide1[k][h];
			//为前一层(输入层)权值的更新做准备
			sumOfHide1[k] += sumOfHide2[h] * temp1 * weightHide1Init[k][h];
		}
	}
	/*-------更新输入层的权值-------*/
	for (int j = 0; j < numOfInput; j++)
	{
		for (int k = 0; k < numOfHide1; k++)
		{
			//更新输入层的权值,加了一个冲量项
			weightOfInput[j][k] -= learnRate* sumOfHide1[k] *(hideLayer1[k] * (1 - hideLayer1[k]))*input[j] + influenceRate * weightOfInput[j][k];
		}
	}
}

/*初始化数组，包括隐层和输出数组*/
void arrayInit(double *hideLayer1, double *hideLayer2, double *output_buffer, int numOfHide1, int numOfHide2, int numOfOutput)
{
	for (int i = 0; i < numOfHide1; i++)
	{
		hideLayer1[i] = 0;
	}
	for (int i = 0; i < numOfHide2; i++)
	{
		hideLayer2[i] = 0;
	}
	for (int i = 0; i < numOfOutput; i++)
	{
		output_buffer[i] = 0;
	}
}

/*正向传播*/
void forwordTransmission(int *input,double* hideLayer1,double* hideLayer2,double* output,double** weightOfInput,double** weightOfHide1,double** weightOfHide2,int numOfInput,int numOfHide1,int numOfHide2,int numOfOutput,double b1,double b2,double b3)
{
	//计算隐层1的节点值及其输出值
	for (int i = 0; i < numOfHide1; i++)
	{
		for (int j = 0; j < numOfInput; j++) {
			hideLayer1[i] += input[j] * weightOfInput[j][i];
		}
		hideLayer1[i] += b1;
		hideLayer1[i] = 1 / (1 + exp((-1)*hideLayer1[i]));
	}
	//计算隐层2的节点值及其输出值
	for (int i = 0; i < numOfHide2; i++)
	{
		for (int j = 0; j < numOfHide1; j++) {
			hideLayer2[i] += hideLayer1[j] * weightOfHide1[j][i];
		}
		hideLayer2[i] += b2;
		hideLayer2[i] = 1 / (1 + exp((-1)*hideLayer2[i]));
	}
	//计算输出层的节点值及其输出值
	for (int i = 0; i < numOfOutput; i++)
	{
		for (int j = 0; j < numOfHide2; j++) {
			output[i] += hideLayer2[j] * weightOfHide2[j][i];
		}
		output[i] += b3;
		output[i] = 1 / (1 + exp((-1)*output[i]));
	}
}

/*读取图片将其像素值放入数组input中*/
void ReadPicture(string src_picture, int * input)
{
	// 读入一张图片  
	Mat img = imread(src_picture, 0);
	int i = 0;
	for (int nrow = 0; nrow < img.rows; nrow++)
	{
		uchar* data = img.ptr<uchar>(nrow);
		for (int ncol = 0; ncol < img.cols; ncol++)
		{
			input[i++] = (int)data[ncol];
		}
	}
}

/*测试*/
int Test(int label, double ** weightOfInput, double ** weightOfHide1, double ** weightOfHide2, int numOfInput, int numOfHide1, int numOfHide2, int numOfOutput, double b1, double b2, double b3)
{
	//定义numOfTest为每个label的测试图片个数
	int numOfTest = 0;
	//定义b为正确个数
	int numOfTestRight = 0;
	//定义输入层节点数组
	int *input = new int[numOfInput];
	//定义隐层1节点数组
	double *hideLayer1 = new double[numOfHide1];
	//定义隐层2节点数组
	double *hideLayer2 = new double[numOfHide2];
	//定义参考输出数组
	double* outputModel = new double[numOfOutput];
	for (int j = 0; j<numOfOutput; j++) {
		outputModel[j] = 0.01 / 9;
		if (j == label) {
			outputModel[j] = 0.99;
		}
	}
	//定义输出层节点数组
	double* output = new double[numOfOutput];
	//误差数组
	double * E = new double[numOfOutput];
	//总误差
	double ETotal = 0;
	//构建路径，开始测试
	struct _finddata_t file;
	intptr_t lf = 0;
	string src_picture;
	if ((lf = _findfirst(SrcTestAll(label), &file)) == -1L) {
		cout << "文件没有找到；";
	}
	else {
		do {
			//构建当前读取的文件的路径
			src_picture = SrcTestOne(label) + "\\" + file.name;
			//读取文件放入input数组
			ReadPicture(src_picture, input);
			//初始化除了输入层以外的层的值
			arrayInit(hideLayer1, hideLayer2, output, numOfHide1, numOfHide2, numOfOutput);
			//正向传播
			forwordTransmission(input, hideLayer1, hideLayer2, output, weightOfInput, weightOfHide1, weightOfHide2, numOfInput, numOfHide1, numOfHide2, numOfOutput, b1, b2, b3);
			//计算总误差（小于某个值认为是正确的），统计正确个数
			for (int j = 0; j < numOfOutput; j++)
			{
				E[j] = pow((outputModel[j] - output[j]), 2) / 2;
				ETotal += E[j];
			}
			if (ETotal < 0.01)
			{
				numOfTestRight++;
			}
			cout <<"测试图片"<<label<<"，总误差:"<< ETotal << endl;
			ETotal = 0;
			numOfTest++;
			if (numOfTest == 10)
				break;
		} while (_findnext(lf, &file) == 0);
	}
	_findclose(lf);
	return numOfTestRight;
}

/*训练*/
void Train(int label, double ** weightOfInput, double ** weightOfHide1, double ** weightOfHide2, int numOfInput, int numOfHide1, int numOfHide2, int numOfOutput, double b1, double b2, double b3)
{
	//每个label的训练图片个数
	int  NumOfTrain = 0;
	//定义输入层节点数组
	int *input = new int[numOfInput];
	//定义隐层1节点数组
	double *hideLayer1 = new double[numOfHide1];
	//定义隐层2节点数组
	double *hideLayer2 = new double[numOfHide2];
	//定义参考输出
	double* outputModel = new double[numOfOutput];
	for (int j = 0; j<numOfOutput; j++) {
		outputModel[j] = 0.01 / 9;
		if (j == label) {
			outputModel[j] = 0.99;
		}
	}
	//定义输出层节点数组
	double* output = new double[numOfOutput];
	//定义原始权重数组，值为上次更新得到的权重值
	double **weightHide1Init = weightOfHide1;
	double **weightHide2Init = weightOfHide2;
	//构造路径，开始遍历文件夹
	struct _finddata_t file;
	intptr_t lf = 0;
	string src_picture;
	if ((lf = _findfirst(SrcTrainAll(label), &file)) == -1L) {
		cout << "文件没有找到；";
	}
	else {
		do {
			//每遍历一张图片，即每修改一次权重，更换反向传播使用的原始权重
			weightHide1Init = weightOfHide1;
			weightHide2Init = weightOfHide2;
			//cout << a << " ";	
			//构建当前读到的文件的路径
			src_picture = SrcTrainOne(label) + "\\" + file.name;
			//读取该图片，将其像素点的值放入数组input中
			ReadPicture(src_picture, input);
			//初始化，将隐层数组和输出数组的值都设为0
			arrayInit(hideLayer1, hideLayer2, output, numOfHide1, numOfHide2, numOfOutput);
			//前向推导，根据input、权重和截距项的值改变各层的值
			forwordTransmission(input, hideLayer1, hideLayer2, output, weightOfInput, weightOfHide1, weightOfHide2, numOfInput, numOfHide1, numOfHide2, numOfOutput, b1, b2, b3);
			//反向传播，根据参考输出和真实输出之间的差来反向更新权值
			reverseTransmission(label, numOfInput,numOfOutput, numOfHide1, numOfHide2, input, hideLayer1, hideLayer2, output ,weightOfInput, weightOfHide1, weightOfHide2, weightHide1Init, weightHide2Init, outputModel);
			
			//numOfTest++;
			////遍历numOfTest个图片以后停止遍历，开始下一个文件夹的遍历
			//if (numOfTest == 200)
			//	break;

		} while (_findnext(lf, &file) == 0);
	}
	_findclose(lf);
}

int main() {
	//定义label为数字号；
	int label = 0;
	//定义训练遍历次数 
	int NumOfTraversal =100;
	//定义三个截距项
	//srand((unsigned)time(NULL));
	//double b1 = (rand() % 100) *0.01;
	//double b2 = (rand() % 100) *0.01;
	//double b3 = (rand() % 100) *0.01;
	double b1 = 0.54, b2 = 0.67, b3 = 0.60;
	//获得像素个数，方便定义节点个数
	int numOfInput = NumOfPixel();
	int numOfHide1 = numOfInput / 8;
	int numOfHide2 = numOfInput / 16;
	int numOfOutput = 10;
	//定义输入层各个节点的权重数组
	double **weightOfInput = randomArray("weightOfInput.txt", numOfInput, numOfHide1);
	//Sleep(1000);
	//定义隐层1各个节点的权重数组
	double **weightOfHide1 = randomArray("weightOfHide1.txt", numOfHide1, numOfHide2);
	//Sleep(1000);
	//定义隐层2各个节点的权重数组
	double **weightOfHide2 = randomArray("weightOfHide2.txt", numOfHide2, numOfOutput);
	//逐个文件夹逐个文件训练
	for (int j = 0; j < NumOfTraversal; j++)
	{
		cout << "Traversaltimes:" << j << endl;
		while (label != 10) {
			Train(label, weightOfInput, weightOfHide1, weightOfHide2, numOfInput, numOfHide1, numOfHide2, numOfOutput, b1, b2, b3);
			label++;
		}
		label = 0;
	}

	cout << endl << "训练完成." << endl;

	double sum = 0;
	while (label != 10) {
		sum += Test(label, weightOfInput, weightOfHide1, weightOfHide2, numOfInput, numOfHide1, numOfHide2, numOfOutput, b1, b2, b3);
		label++;
	}

	cout << endl << "测试完毕，准确率为：" << sum / 100 * 100 << "%" << endl;

	system("pause");
}

