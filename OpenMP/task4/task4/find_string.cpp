#include "omp.h"
#include <fstream>
#include <string>
#include <iostream>
#include <algorithm>

using namespace std;

static const int NUM_THREADS = 4;
const string FILE_NAME = "test.txt";
const string searched_str = "pretty well";


int bruteforce_find(const string &T, const string &P)
{
	int n = T.length();
	int m = P.length();

	for (int i = 0; i < n - m + 1; i++) {
		int j = 0;
		while (j < m && T[i + j] == P[j]) {
			++j;
		}
		if (j == m) {
			return i;
		}
	}

	return -1;
}
int omp_bruteforce_find(const string &T, const string &P)
{
	int return_number = -1;

	int n = T.length();
	int m = P.length();

	#pragma omp parallel for
	for (int i = 0; i < n - m + 1; i++) {
		int j = 0;
		while (j < m && T[i + j] == P[j]) {
			++j;
		}
		if (j == m) { 
			return_number = i;
			//break;
		}
	}

	return return_number;
}
int main() {
	ifstream myfile;
	myfile.open(FILE_NAME);
	unsigned int curLine = 0;
	string line;
	omp_set_num_threads(NUM_THREADS);
	int matches_counter = 0;

double t1 = omp_get_wtime();

while (getline(myfile, line)) 
	{ 
		curLine++;
		if (bruteforce_find(line, searched_str) != string::npos)
		{
			matches_counter++;
			//alternative:
			//cout<< "string: " << searched_str << " found on line: " << curLine << endl;
		}
	}

	int lines_num = curLine; //number needed for parallel loop
	double t2 = omp_get_wtime();
	
	double t = t2 - t1;

	int matches_counter_parallel = 0;
	myfile.clear();
	myfile.seekg(0);
	
	t1 = omp_get_wtime();

	int currLine = 0;


	while (getline(myfile, line))
	{
		curLine++;
		if (omp_bruteforce_find(line, searched_str) != string::npos)
		{
			matches_counter_parallel++;
			//alternative:
			//cout<< "string: " << searched_str << " found on line: " << curLine << endl;
		}
	}
	t2 = omp_get_wtime();
	cout << "sequence work time:" << t << endl;
	cout << "parallel work time: " << (t2 - t1) << endl;
	cout << "matches found: " << matches_counter << " " << matches_counter_parallel << endl;
	myfile.close();
	return 0;
}