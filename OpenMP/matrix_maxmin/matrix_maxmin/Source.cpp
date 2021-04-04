#include "omp.h"
#include "time.h"
#include <iostream>
using namespace std;

int main() {
	//matrix : m rows, n cols
	const int MAX_RUND_NUM = 100;
	const int M = 5, N = 4;
	int matrix[M][N];
	srand(time(NULL));

	//initialisation
	for (int i = 0; i < M; i++)
		for (int j = 0; j < N; j++)
			matrix[i][j] = rand() % MAX_RUND_NUM;
	cout << "Genered matrix:";
	cout << "\n";
	for (int i = 0; i < M; i++) 
	{
		for (int j = 0; j < N; j++)
			cout << matrix[i][j]<<" ";
		cout << "\n";
	}
	
	int max_elem = -MAX_RUND_NUM;
	//finding max element among mins in row
	#pragma parallel for reduction(max: max_elem)
	for (int row = 0; row < M; row++)
	{
		int min_row_elem = matrix[row][0];
		
		#pragma parallel for reduction(min: min_row_elem)
			for (int col = 0; col < N; col++)
			{
				min_row_elem = min_row_elem < matrix[row][col] ? min_row_elem : matrix[row][col];
			}
		max_elem = min_row_elem > max_elem ? min_row_elem : max_elem;
	}

	printf("max: %d\n", max_elem);
}