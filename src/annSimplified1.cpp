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


/*���򴫲�*/
void reverseTransmission(int label,int numOfInput,int numOfOutput, int numOfHide1, int numOfHide2, int* input,double* hideLayer1, double* hideLayer2, double* output, double** weightOfInput, double** weightOfHide1, double** weightOfHide2, double** weightHide1Init, double** weightHide2Init, double* outputModel)
{
	//ѧϰ��
	double learnRate = 0.1;
	double influenceRate = 0;
	double *sumOfHide2 = new double[numOfHide2] {0};
	double *sumOfHide1 = new double[numOfHide1] {0};
	double temp_lastweight;
	double temp = 0, temp1 = 0;
	/*-------��������2��Ȩֵ-------*/
	for (int h = 0; h < numOfHide2; h++)
	{
		for (int q = 0; q < numOfOutput; q++)
		{
			temp = (-1) * (outputModel[q] - output[q]) * output[q] * (1 - output[q]);
			//��������2��Ȩֵ,����һ��������
			weightOfHide2[h][q] -= learnRate * temp * hideLayer2[h] + influenceRate * weightOfHide2[h][q];
			//Ϊǰһ��(����1)Ȩֵ�ĸ�����׼��
			sumOfHide2[h] += temp * weightHide2Init[h][q];
		}
	}
	/*-------��������1��Ȩֵ-------*/
	for (int k = 0; k < numOfHide1; k++)
	{
		for (int h = 0; h < numOfHide2; h++)
		{
			temp1 = hideLayer2[h] * (1 - hideLayer2[h]);
			//��������1��Ȩֵ,����һ��������
			weightOfHide1[k][h] -= learnRate * sumOfHide2[h] * temp1 * hideLayer1[k] + influenceRate * weightOfHide1[k][h];
			//Ϊǰһ��(�����)Ȩֵ�ĸ�����׼��
			sumOfHide1[k] += sumOfHide2[h] * temp1 * weightHide1Init[k][h];
		}
	}
	/*-------����������Ȩֵ-------*/
	for (int j = 0; j < numOfInput; j++)
	{
		for (int k = 0; k < numOfHide1; k++)
		{
			//����������Ȩֵ,����һ��������
			weightOfInput[j][k] -= learnRate* sumOfHide1[k] *(hideLayer1[k] * (1 - hideLayer1[k]))*input[j] + influenceRate * weightOfInput[j][k];
		}
	}
}

/*��ʼ�����飬����������������*/
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

/*���򴫲�*/
void forwordTransmission(int *input,double* hideLayer1,double* hideLayer2,double* output,double** weightOfInput,double** weightOfHide1,double** weightOfHide2,int numOfInput,int numOfHide1,int numOfHide2,int numOfOutput,double b1,double b2,double b3)
{
	//��������1�Ľڵ�ֵ�������ֵ
	for (int i = 0; i < numOfHide1; i++)
	{
		for (int j = 0; j < numOfInput; j++) {
			hideLayer1[i] += input[j] * weightOfInput[j][i];
		}
		hideLayer1[i] += b1;
		hideLayer1[i] = 1 / (1 + exp((-1)*hideLayer1[i]));
	}
	//��������2�Ľڵ�ֵ�������ֵ
	for (int i = 0; i < numOfHide2; i++)
	{
		for (int j = 0; j < numOfHide1; j++) {
			hideLayer2[i] += hideLayer1[j] * weightOfHide1[j][i];
		}
		hideLayer2[i] += b2;
		hideLayer2[i] = 1 / (1 + exp((-1)*hideLayer2[i]));
	}
	//���������Ľڵ�ֵ�������ֵ
	for (int i = 0; i < numOfOutput; i++)
	{
		for (int j = 0; j < numOfHide2; j++) {
			output[i] += hideLayer2[j] * weightOfHide2[j][i];
		}
		output[i] += b3;
		output[i] = 1 / (1 + exp((-1)*output[i]));
	}
}

/*��ȡͼƬ��������ֵ��������input��*/
void ReadPicture(string src_picture, int * input)
{
	// ����һ��ͼƬ  
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

/*����*/
int Test(int label, double ** weightOfInput, double ** weightOfHide1, double ** weightOfHide2, int numOfInput, int numOfHide1, int numOfHide2, int numOfOutput, double b1, double b2, double b3)
{
	//����numOfTestΪÿ��label�Ĳ���ͼƬ����
	int numOfTest = 0;
	//����bΪ��ȷ����
	int numOfTestRight = 0;
	//���������ڵ�����
	int *input = new int[numOfInput];
	//��������1�ڵ�����
	double *hideLayer1 = new double[numOfHide1];
	//��������2�ڵ�����
	double *hideLayer2 = new double[numOfHide2];
	//����ο��������
	double* outputModel = new double[numOfOutput];
	for (int j = 0; j<numOfOutput; j++) {
		outputModel[j] = 0.01 / 9;
		if (j == label) {
			outputModel[j] = 0.99;
		}
	}
	//���������ڵ�����
	double* output = new double[numOfOutput];
	//�������
	double * E = new double[numOfOutput];
	//�����
	double ETotal = 0;
	//����·������ʼ����
	struct _finddata_t file;
	intptr_t lf = 0;
	string src_picture;
	if ((lf = _findfirst(SrcTestAll(label), &file)) == -1L) {
		cout << "�ļ�û���ҵ���";
	}
	else {
		do {
			//������ǰ��ȡ���ļ���·��
			src_picture = SrcTestOne(label) + "\\" + file.name;
			//��ȡ�ļ�����input����
			ReadPicture(src_picture, input);
			//��ʼ���������������Ĳ��ֵ
			arrayInit(hideLayer1, hideLayer2, output, numOfHide1, numOfHide2, numOfOutput);
			//���򴫲�
			forwordTransmission(input, hideLayer1, hideLayer2, output, weightOfInput, weightOfHide1, weightOfHide2, numOfInput, numOfHide1, numOfHide2, numOfOutput, b1, b2, b3);
			//��������С��ĳ��ֵ��Ϊ����ȷ�ģ���ͳ����ȷ����
			for (int j = 0; j < numOfOutput; j++)
			{
				E[j] = pow((outputModel[j] - output[j]), 2) / 2;
				ETotal += E[j];
			}
			if (ETotal < 0.01)
			{
				numOfTestRight++;
			}
			cout <<"����ͼƬ"<<label<<"�������:"<< ETotal << endl;
			ETotal = 0;
			numOfTest++;
			if (numOfTest == 10)
				break;
		} while (_findnext(lf, &file) == 0);
	}
	_findclose(lf);
	return numOfTestRight;
}

/*ѵ��*/
void Train(int label, double ** weightOfInput, double ** weightOfHide1, double ** weightOfHide2, int numOfInput, int numOfHide1, int numOfHide2, int numOfOutput, double b1, double b2, double b3)
{
	//ÿ��label��ѵ��ͼƬ����
	int  NumOfTrain = 0;
	//���������ڵ�����
	int *input = new int[numOfInput];
	//��������1�ڵ�����
	double *hideLayer1 = new double[numOfHide1];
	//��������2�ڵ�����
	double *hideLayer2 = new double[numOfHide2];
	//����ο����
	double* outputModel = new double[numOfOutput];
	for (int j = 0; j<numOfOutput; j++) {
		outputModel[j] = 0.01 / 9;
		if (j == label) {
			outputModel[j] = 0.99;
		}
	}
	//���������ڵ�����
	double* output = new double[numOfOutput];
	//����ԭʼȨ�����飬ֵΪ�ϴθ��µõ���Ȩ��ֵ
	double **weightHide1Init = weightOfHide1;
	double **weightHide2Init = weightOfHide2;
	//����·������ʼ�����ļ���
	struct _finddata_t file;
	intptr_t lf = 0;
	string src_picture;
	if ((lf = _findfirst(SrcTrainAll(label), &file)) == -1L) {
		cout << "�ļ�û���ҵ���";
	}
	else {
		do {
			//ÿ����һ��ͼƬ����ÿ�޸�һ��Ȩ�أ��������򴫲�ʹ�õ�ԭʼȨ��
			weightHide1Init = weightOfHide1;
			weightHide2Init = weightOfHide2;
			//cout << a << " ";	
			//������ǰ�������ļ���·��
			src_picture = SrcTrainOne(label) + "\\" + file.name;
			//��ȡ��ͼƬ���������ص��ֵ��������input��
			ReadPicture(src_picture, input);
			//��ʼ�����������������������ֵ����Ϊ0
			arrayInit(hideLayer1, hideLayer2, output, numOfHide1, numOfHide2, numOfOutput);
			//ǰ���Ƶ�������input��Ȩ�غͽؾ����ֵ�ı�����ֵ
			forwordTransmission(input, hideLayer1, hideLayer2, output, weightOfInput, weightOfHide1, weightOfHide2, numOfInput, numOfHide1, numOfHide2, numOfOutput, b1, b2, b3);
			//���򴫲������ݲο��������ʵ���֮��Ĳ����������Ȩֵ
			reverseTransmission(label, numOfInput,numOfOutput, numOfHide1, numOfHide2, input, hideLayer1, hideLayer2, output ,weightOfInput, weightOfHide1, weightOfHide2, weightHide1Init, weightHide2Init, outputModel);
			
			//numOfTest++;
			////����numOfTest��ͼƬ�Ժ�ֹͣ��������ʼ��һ���ļ��еı���
			//if (numOfTest == 200)
			//	break;

		} while (_findnext(lf, &file) == 0);
	}
	_findclose(lf);
}

int main() {
	//����labelΪ���ֺţ�
	int label = 0;
	//����ѵ���������� 
	int NumOfTraversal =100;
	//���������ؾ���
	//srand((unsigned)time(NULL));
	//double b1 = (rand() % 100) *0.01;
	//double b2 = (rand() % 100) *0.01;
	//double b3 = (rand() % 100) *0.01;
	double b1 = 0.54, b2 = 0.67, b3 = 0.60;
	//������ظ��������㶨��ڵ����
	int numOfInput = NumOfPixel();
	int numOfHide1 = numOfInput / 8;
	int numOfHide2 = numOfInput / 16;
	int numOfOutput = 10;
	//�������������ڵ��Ȩ������
	double **weightOfInput = randomArray("weightOfInput.txt", numOfInput, numOfHide1);
	//Sleep(1000);
	//��������1�����ڵ��Ȩ������
	double **weightOfHide1 = randomArray("weightOfHide1.txt", numOfHide1, numOfHide2);
	//Sleep(1000);
	//��������2�����ڵ��Ȩ������
	double **weightOfHide2 = randomArray("weightOfHide2.txt", numOfHide2, numOfOutput);
	//����ļ�������ļ�ѵ��
	for (int j = 0; j < NumOfTraversal; j++)
	{
		cout << "Traversaltimes:" << j << endl;
		while (label != 10) {
			Train(label, weightOfInput, weightOfHide1, weightOfHide2, numOfInput, numOfHide1, numOfHide2, numOfOutput, b1, b2, b3);
			label++;
		}
		label = 0;
	}

	cout << endl << "ѵ�����." << endl;

	double sum = 0;
	while (label != 10) {
		sum += Test(label, weightOfInput, weightOfHide1, weightOfHide2, numOfInput, numOfHide1, numOfHide2, numOfOutput, b1, b2, b3);
		label++;
	}

	cout << endl << "������ϣ�׼ȷ��Ϊ��" << sum / 100 * 100 << "%" << endl;

	system("pause");
}

