#include "omp.h"
#include <iostream>
#include "time.h"
using namespace std;

const int SIZE = 4000;

void odd_even_sort(int* array, int N)
{
	bool isSorted = false;
	while (!isSorted)
	{
		isSorted = true;
		for (int j = 0; j < N - 1; j += 2) //even
			if (array[j] > array[j + 1])
			{
				swap(array[j], array[j + 1]);
				isSorted = false;
			}
		for (int j = 1; j < N - 2; j += 2)      //odd
			if (array[j] > array[j + 1])
			{
				swap(array[j], array[j + 1]);
				isSorted = false;
			}
	}
}

void omp_odd_even_sort(int* array, int N)
{
	bool isSorted = false;
	while (!isSorted)
	{
		isSorted = true;
#pragma omp parallel for
		for (int j = 0; j < N - 1; j += 2) //even
			if (array[j] > array[j + 1])
			{
				swap(array[j], array[j + 1]);
				isSorted = false;
			}
#pragma omp parallel for
		for (int j = 1; j < N - 2; j += 2)      //odd
			if (array[j] > array[j + 1])
			{
				swap(array[j], array[j + 1]);
				isSorted = false;
			}
	}
}
int* generated_array(int n)
{
	int* res = new int[n];
	srand(time(NULL));
	for (int i = 0; i < n; i++)
	{
		res[i] = rand() % 10000;
	}
	return res;
}
void show(int* array, int n)
{
	cout << "array: \n";
	for (int i = 0; i < n; i++) cout << array[i] << " ";
	cout << "\n";
}
int main()
{
	int* a = generated_array(SIZE);
	//show(a, SIZE);
	double t1 = omp_get_wtime();
	odd_even_sort(a, SIZE);
	double t2 = omp_get_wtime();
	show(a, SIZE);
	cout << "sequence sort, time elapsed: " << t2 - t1 << endl;
	cout << "\n";

	a = generated_array(SIZE);
	//show(a, SIZE);
	omp_set_num_threads(4);

	t1 = omp_get_wtime();
	omp_odd_even_sort(a, SIZE);
	t2 = omp_get_wtime();
	//show(a, SIZE);
	cout << "parallel sort, time elapsed: " << t2 - t1 << endl;

	return 0;
}