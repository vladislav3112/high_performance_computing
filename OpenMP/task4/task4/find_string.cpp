#include "omp.h"
#include <fstream>
#include <string>
#include <iostream>
#include <algorithm>

using namespace std;

static const int NUM_THREADS = 4;
const string FILE_NAME = "test.txt";
const string searched_str = "but";


//overrided parallel find 

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
		if (line.find(searched_str, 0) != string::npos) 
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
	#pragma omp parallel for private(line)
	for (int curLine = 0; curLine < lines_num; curLine++) {
		getline(myfile, line);
		if (line.find(searched_str) != string::npos) matches_counter_parallel++;
		//alternative:
		//cout<< "string: " << searched_str << " found on line: " << curLine << endl;
	}
	
	t2 = omp_get_wtime();
	cout << "sequence work time:" << t << endl;
	cout << "parallel work time: " << (t2 - t1) << endl;
	cout << "matches found: " << matches_counter << " " << matches_counter_parallel << endl;
	myfile.close();
	return 0;
}