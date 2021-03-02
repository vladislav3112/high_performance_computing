#include <iostream>
#include <vector>
#include<time.h>
#include "omp.h"
using namespace std;

const int THREADS_NUM = 4;

//matrix*matrix
void mult(vector <vector <double>> A, vector <vector <double>> B,
	vector <vector <double>> &R, int n)
{
	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++)
			for (int k = 0; k < n; k++)
				R[i][j] += A[i][k] * B[k][j];
}

void omp_stripped_mult(vector <vector <double>> A, vector <vector <double>> B,
	vector <vector <double>> &R, int n)
{
	int i,j,k;
	#pragma omp parallel for private(i,j,k)
	for (i = 0; i < n; i++) 
	{
		for (j = 0; j < n; j++)
			for (k = 0; k < n; k++)
				R[i][j] += A[i][k] * B[k][j];
	}
}



//matrix*vector
void mult(vector <vector <double>> A, vector <double> b,
	vector <double> &res, int n)
{
	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++)
				res[i] += A[i][j] * b[j];
}

void omp_row_mult(vector <vector <double>> A, vector <double> b,
	vector <double> &res, int n)
{
	int j;
	for (int i = 0; i < n; i++)
		#pragma omp parallel for private (j)
		for (j = 0; j < n; j++)
		{
			res[i] += A[i][j] * b[j];
		}
}

void omp_col_mult(vector <vector <double>> A, vector <double> b,
	vector <double> &res, int n)
{
	int i, j;
	#pragma omp parallel for private (i,j)
	for (i = 0; i < n; i++)
	{
		for (j = 0; j < n; j++)
			res[i] += A[i][j] * b[j];
	}
}

void show(vector <vector <double>> A, int n)
{
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			cout << "\t" << A[i][j] << "\t";
		}
		cout << endl;
	}
}

int main()
{
	const double n = 200; //size of matrixes

	vector <vector <double>> A(n), B(n), R(n); // matrixes A and B, A*B = R
	vector <double> b(n); //column vector
	vector <double> res_vector(n); //result of A*b
	srand(time(NULL));

	omp_set_num_threads(THREADS_NUM);
	//initialisation can be changed for double values
	for (int i = 0; i < n; i++)
	{
		b[i] = 10 * rand() / RAND_MAX;
		for (int j = 0; j < n; j++)
		{
			R[i].push_back(0);
			A[i].push_back(10 * rand() / RAND_MAX);
			B[i].push_back(10 * rand() / RAND_MAX);
		}
	}
	//cout << "Fisrt matrix - A" << endl;
	//show(A, n);
	//cout << "Second matrix - B" << endl;
	//show(B, n);

	//A*B
	double start_t, end_t, dt;

	start_t = omp_get_wtime();
	mult(A, B, R, n);
	end_t = omp_get_wtime();
	dt = end_t - start_t;

	cout << "A*B matrix" << endl;
	//show(R, n);
	cout << "seq calc time:" << dt << endl;
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			R[i][j] = 0;
		}
	}
	start_t = omp_get_wtime();
	omp_stripped_mult(A, B, R, n);
	end_t = omp_get_wtime();
	dt = end_t - start_t;
	cout << "stripped calc time:" << dt << endl;

	//A*b
	for (int i = 0; i < n; i++) res_vector[i] = 0;
	//cout << "column vector - b: " << endl;
	//for (int i = 0; i < n; i++) cout << b[i] << " " << endl;

	start_t = omp_get_wtime();
	mult(A, b, res_vector, n);
	end_t = omp_get_wtime();
	dt = end_t - start_t;

	cout << "A*b result" << endl;
	//for (int i = 0; i < n; i++) cout << res_vector[i] << " " << endl;
	cout << "seq calc time:" << dt << endl;
	
	for (int i = 0; i < n; i++) res_vector[i] = 0;
	start_t = omp_get_wtime();
	omp_col_mult(A, b, res_vector, n);
	end_t = omp_get_wtime();
	dt = end_t - start_t;
	cout << "column calc time:" << dt<< endl;

	for (int i = 0; i < n; i++) res_vector[i] = 0;
	start_t = omp_get_wtime();
	omp_row_mult(A, b, res_vector, n);
	end_t = omp_get_wtime();
	dt = end_t - start_t;
	cout << "row calc time:" << dt  << endl;
	return 0;
}