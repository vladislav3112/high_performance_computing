// C++ program for Merge Sort
#include <iostream> 
#include <algorithm> 
#include "time.h"
#include "mpi.h" 

const int MAXPROCS = 4; 
const int ARRAY_SIZE = 1000; 
const int ROOT = 0; 
const int LOCAL_ARR_SIZE = ARRAY_SIZE / MAXPROCS;

template <typename It, typename Compare = std::less<>> void merge_sort(It first, It last, Compare cmp = Compare{}) 
{
	const auto n = std::distance(first, last);
	if (n > 1) 
	{
		const auto middle = std::next(first, n / 2);

		merge_sort(first, middle, cmp); 
		merge_sort(middle, last, cmp);

		std::inplace_merge(first, middle, last);
	}
}

template <typename It, typename Compare = std::less<>> void mpi_merge(It first, It last, Compare cmp = Compare{}) 
{
	const auto n = std::distance(first, last);
	if (n > LOCAL_ARR_SIZE) 
	{
		const auto middle = std::next(first, n / 2);

		merge_sort(first, middle, cmp); merge_sort(middle, last, cmp);

		std::inplace_merge(first, middle, last);
	}
}

int* generated_array(int n) 
{
	int* res = new int[n]; srand(time(NULL));
	for (int i = 0; i < n; i++)		res[i] = rand() % 10000;
	return res;
}

void test(int*& array, int size) 
{
	for (int i = 0; i < size - 1; i++) if (array[i + 1] < array[i]) 
	{
		std::cout << "wrong order" << std::endl; 
		break;
	}
}


void test_mpi(int*& array, int size) {
	for (int i = 0; i < size - 1; i++)
		if (array[i + 1] < array[i]) {
		std::cout << "wrong order" << std::endl; 
		break;
	}
}

int main(int argc, char** argv) {
	// sequence part
	int* a = generated_array(ARRAY_SIZE); double t1 = clock();
	merge_sort(a, a + ARRAY_SIZE);
	double t2 = clock(); double t = t2 - t1;

	//MPI part
	int* array = generated_array(ARRAY_SIZE);
	int id, numprocs;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &id);

	int recv_buff[ARRAY_SIZE];

	//four stages: 1. scatter data 2. local merge sort 3. gather all data 4. inplace merge in root to finish

	t1 = MPI_Wtime();
	if (id == ROOT) MPI_Scatter(array, LOCAL_ARR_SIZE, MPI_INT, recv_buff, LOCAL_ARR_SIZE, MPI_INT, ROOT, MPI_COMM_WORLD);
	merge_sort(array, array + LOCAL_ARR_SIZE);
	MPI_Gather(array, LOCAL_ARR_SIZE, MPI_INT, recv_buff, LOCAL_ARR_SIZE, MPI_INT, ROOT, MPI_COMM_WORLD);
	if (id == ROOT) mpi_merge(array, array + ARRAY_SIZE);
	t2 = MPI_Wtime();

	MPI_Finalize();

	if (id == ROOT)
	{
		test_mpi(array, ARRAY_SIZE);
		std::cout << "array size: " << ARRAY_SIZE << std::endl; std::cout << "sequence work time: " << t / 1000 << std::endl;
		std::cout << "parallel work time: " << t2 - t1 << std::endl;
	}
	delete a; delete array; return 0;
}