// createArrayToFile.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <stdlib.h>
#include <time.h>
#include "iostream"
#include <fstream>
#include "ArrayFile.h"
using namespace std;
//�����������,д���ļ���
void writeRandomArray(char *fileName,int num_rows, int num_cols)
{
	FILE *pf = NULL;
	errno_t err;
	fstream file;
	file.open(fileName, ios::in);
	if (file)
	{
		return;
	}
	else {
		err = fopen_s(&pf, fileName, "w");
		//��������
		double **weight = new double*[num_rows];
		for (int j = 0; j < num_rows; j++) {
			weight[j] = new double[num_cols];
		}
		srand((unsigned)time(NULL));
		for (int i = 0; i<num_rows; i++)
			for (int j = 0; j < num_cols; j++) {
				weight[i][j] = (rand() % 200)*0.01 - 1;
			}
		//��ʽ��������ļ���
		for (int i = 0; i<num_rows; i++)
		{
			for (int j = 0; j<num_cols; j++)
			{
				fprintf_s(pf, "%.2lf", weight[i][j]);
			}
			fprintf(pf, "\n");
		}
		fclose(pf);
	}
	
}
//
//��ȡ��������
double ** readArray(char *fileName,int num_rows, int num_cols)
{
	FILE *pf;
	errno_t err;
	double **weight = new double*[num_rows];
	for (int j = 0; j < num_rows; j++) {
		weight[j] = new double[num_cols];
	}
	if ((err = fopen_s(&pf, fileName, "r")) != 0)
	{
		printf("Error\n");
		system("PAUSE");
		exit(1);
	}
	//��ȡ�ļ����ݵ�����
	for (int i = 0; i<num_rows; i++)
	{
		for (int j = 0; j<num_cols; j++)
		{
			fscanf_s(pf, "%lf", &weight[i][j]);
		}
		fscanf_s(pf, "\n");
	}
	fclose(pf);
	
	return weight;
}
double ** randomArray(char *fileName,int num_rows, int num_cols) 
{
	writeRandomArray(fileName, num_rows, num_cols);
	return readArray(fileName,num_rows,num_cols);
}

/*��������������һ����ά���顾-1��1��*/
double ** randomArray(int num_rows, int num_cols) {

	double **weight = new double*[num_rows];
	for (int j = 0; j < num_rows; j++) {
		weight[j] = new double[num_cols];
	}
	srand((unsigned)time(NULL));
	for (int i = 0; i<num_rows; i++)
		for (int j = 0; j < num_cols; j++) {
			weight[i][j] = (rand() % 200)*0.01 - 1;
		}
	return weight;
}



