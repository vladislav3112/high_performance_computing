#include <iostream>
#include <vector>
#include<time.h>
#include "omp.h"
using namespace std;

const int THREADS_NUM = 4;

//matrix * matrix (both square)
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
	#pragma omp parallel for
	for (int i = 0; i < n; i++) 
	{
		for (int j = 0; j < n; j++)
			for (int k = 0; k < n; k++)
				R[i][j] += A[i][k] * B[k][j];
	}
}
void block_mat_mult(vector <vector <double>> A, vector <vector <double>> B, vector <vector <double>> &R, int n, int block_index)
{
	int block_size = n / 2;
	int block_row_start1 = block_index / 4;
	int block_col_start1 = block_index % 2;
	int block_col_start2 = block_index < 4 ? block_index / 2 : (block_index - 4) / 2;

	for (int i = block_row_start1 * block_size; i < (block_row_start1 + 1) * block_size; ++i)
	{
		for (int j = block_col_start2 * block_size; j < (block_col_start2 + 1) * block_size; ++j)
		{
			int sum = 0;
			for (int l = block_col_start1 * block_size; l < (block_col_start1 + 1) * block_size; ++l)
			{
				sum += A[i][l] * B[l][j];
			}
			R[i][j] += sum;
		}
	}
}


/* block matrix-matrix multiplication
   assume  n is even */
void omp_matrix_matrix_block(vector <vector <double>> A, vector <vector <double>> B, vector <vector <double>> &R, int n)
{
	const int block_num = 8; 
	#pragma omp parallel for shared(R)
	for (int i = 0; i < block_num; i++)
	{
		block_mat_mult(A, B, R, n, i);
	}
}


//matrix*vector (square matrix)
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
	int i, j;
	#pragma omp parallel for private (i,j)
	for (i = 0; i < n; i++) 
	{
		for (j = 0; j < n; j++)
		{
			res[i] += A[i][j] * b[j];
		}
	}
}
void omp_col_mult(vector <vector <double>> A, vector <double> b,
	vector <double> &res, int n)
{
	int i, j;
	double IterSum = 0;
	for (i = 0; i < n; i++) 
	{
		IterSum = 0;
		#pragma omp parallel for reduction(+:IterSum)
		for (j = 0; j < n; j++)
			IterSum += A[i][j] * b[j];
		res[i] = IterSum;
	}
}
void omp_block_vec_mult(vector <vector <double>> A, vector<double> b,
	vector <double> &res, int n, int block_index)
{
	int block_row_size = n / THREADS_NUM * 2;
	int block_col_size = n / 2;
	int block_row_start = block_index / 2;
	int block_col_start = block_index % 2;

	for (int i = block_row_start * block_row_size; i < (block_row_start + 1) * block_row_size; i++)
	{
		for (int j = block_col_start * block_col_size; j < (block_col_start + 1) * block_col_size; j++)
		{
			res[i] += A[i][j] * b[j];
		}
	}
}

/* block matrix-matrix multiplication
   assume  n is even */
void omp_matrix_vector_block(vector <vector <double>> matrix, vector<double> vec, vector<double> &res, int n)
{
	#pragma omp parallel 
	{
		omp_block_vec_mult(matrix, vec, res, n, omp_get_thread_num());
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
void reinit_matrix(vector <vector <double>> &A, int n) {
	for (int i = 0; i < n; i++) 
	{
		for (int j = 0; j < n; j++) 
		{
			A[i][j] = 0;
		}
	}
}
void reinit_vector(vector <double> &vec, int n) 
{
	for (int i = 0; i < n; i++)	vec[i] = 0;
}

int main()
{
	const double n = 10; //size of matrixes

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
	show(R, n);
	cout << "seq calc time:" << dt << endl;
	

	//code optimize??
	reinit_matrix(R, n);
	start_t = omp_get_wtime();

	omp_stripped_mult(A, B, R, n);

	end_t = omp_get_wtime();
	dt = end_t - start_t;
	cout << "stripped calc time:" << dt << endl;
	show(R, n);
	//A*b
	//cout << "column vector - b: " << endl;
	//for (int i = 0; i < n; i++) cout << b[i] << " " << endl;

	reinit_matrix(R, n);
	start_t = omp_get_wtime();
	omp_matrix_matrix_block(A, B, R, n);
	end_t = omp_get_wtime();
	dt = end_t - start_t;
	cout << "block calc time:" << dt << endl;
	show(R, n);
	reinit_vector(res_vector, n);
	start_t = omp_get_wtime();
	mult(A, b, res_vector, n);
	end_t = omp_get_wtime();
	dt = end_t - start_t;

	cout << "A*b result" << endl;
	for (int i = 0; i < n; i++) cout << res_vector[i] << " " << endl;
	cout << "seq calc time:" << dt << endl;
	
	reinit_vector(res_vector, n);
	start_t = omp_get_wtime();
	omp_col_mult(A, b, res_vector, n);
	end_t = omp_get_wtime();
	dt = end_t - start_t;
	for (int i = 0; i < n; i++) cout << res_vector[i] << " " << endl;
	cout << "column calc time:" << dt<< endl;

	reinit_vector(res_vector, n);
	start_t = omp_get_wtime();
	omp_row_mult(A, b, res_vector, n);
	end_t = omp_get_wtime();
	dt = end_t - start_t;
	for (int i = 0; i < n; i++) cout << res_vector[i] << " " << endl;
	cout << "row calc time:" << dt  << endl;

	reinit_vector(res_vector, n);
	start_t = omp_get_wtime();
	omp_matrix_vector_block(A, b, res_vector, n);
	end_t = omp_get_wtime();
	dt = end_t - start_t;
	for (int i = 0; i < n; i++) cout << res_vector[i] << " " << endl;
	cout << "block calc time:" << dt << endl;

	return 0;
}