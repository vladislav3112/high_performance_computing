#include <iostream>
#include <vector>
#include<time.h>
using namespace std;

const double n = 10; //SIZE


void slae_solution(vector <vector <double>> A, vector<double>b, vector<double>&x, int n) 
{
	double max;
	int k, index;
	const double eps = 0.00001; 
	k = 0;
	while (k < n)
	{
		// Поиск строки с максимальным a[i][k]
		max = abs(A[k][k]);
		index = k;
		for (int i = k + 1; i < n; i++)
		{
			if (abs(A[i][k]) > max)
			{
				max = abs(A[i][k]);
				index = i;
			}
		}
		// Перестановка строк
		if (max < eps)
		{
			cout << "Решение получить невозможно из-за нулевого столбца ";
			cout << index << " матрицы A" << endl;
		}
		for (int j = 0; j < n; j++)
		{
			double temp = A[k][j];
			A[k][j] = A[index][j];
			A[index][j] = temp;
		}
		double temp = b[k];
		b[k] = b[index];
		b[index] = temp;
		// Нормализация уравнений
		for (int i = k; i < n; i++)
		{
			double temp = A[i][k];
			if (abs(temp) < eps) continue;
			for (int j = 0; j < n; j++)
				A[i][j] = A[i][j] / temp;
			b[i] = b[i] / temp;
			if (i == k)  continue; // уравнение не вычитать само из себя
			for (int j = 0; j < n; j++)
				A[i][j] = A[i][j] - A[k][j];
			b[i] = b[i] - b[k];
		}
		k++;
	}
	// обратная подстановка
	for (k = n - 1; k >= 0; k--)
	{
		x[k] = b[k];
		for (int i = 0; i < k; i++)
			b[i] = b[i] - A[i][k] * x[k];
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
	vector <vector <double>> A(n);
	vector <double> b(n);
	vector <double> x(n);
	srand(time(NULL));
	for (int i = 0; i < n; i++)
	{
		b[i] = rand() % 100;
		for (int j = 0; j < n; j++)
		{
			A[i].push_back(rand() % 100);
		}
	}
	cout << "Fisrt matrix" << endl;
	show(A, n);

	cout << "b" << endl;
	for (int i = 0; i < n; i++) cout << "b[i] = " <<b[i]<< endl;
	slae_solution(A, b, x, n);
	cout << "\n";
	for (int i = 0; i < n; i++) cout << "x[i] = " << x[i] << endl;
	return 0;
}
