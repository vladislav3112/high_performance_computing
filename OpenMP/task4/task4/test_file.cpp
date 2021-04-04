#include <omp.h>
#include <fstream>
#include <string>


static const int num_threads = 8;


// read text file as single string
std::string read_file(const char* file)
{
	std::ifstream fin(file);
	std::string text, line;
	if (fin.is_open())
	{
		while (getline(fin, line))
		{
			text += line;
		}
	}
	else {
		throw "failed to open";
	}
	fin.close();
	return std::move(text);
}


int count_substr(const std::string& str, const std::string& substr, double& dt)
{
	size_t pos = -1;
	int res = 0;

	double t1 = omp_get_wtime();
	while (true)
	{
		pos = str.find(substr, pos + 1);
		if (pos == std::string::npos)
		{
			break;
		}
		++res;
	}
	double t2 = omp_get_wtime();
	dt = (t2 - t1) * 1000.0;
	return res;
}


int local_count(const std::string& str, const std::string& substr, int thread)
{
	size_t str_size = str.size();
	size_t pos = thread * str_size / num_threads - 1;
	size_t bound = (thread + 1) * str_size / num_threads;
	int res = 0;
	while (true)
	{
		pos = str.find(substr, pos + 1);
		if (pos < bound)
		{
			++res;
		}
		else {
			break;
		}
	}
	return res;
}


int count_substr_omp(const std::string& str, const std::string& substr, double& dt)
{
	int res_local[num_threads];
	int res = 0;
	omp_set_num_threads(num_threads);
	double t1 = omp_get_wtime();
#pragma omp parallel shared(res_local, str, substr)
	{
		int i = omp_get_thread_num();
		res_local[i] = local_count(str, substr, i);
	}
	for (int i = 0; i < num_threads; ++i)
	{
		res += res_local[i];
	}
	double t2 = omp_get_wtime();
	dt = (t2 - t1) * 1000.0;
	return res;
}


int main1(int argc, char* argv[])
{

	std::string text;
	try
	{
		text = read_file("test.txt");
	}
	catch (const char* msg)
	{
		printf("%s %s\n", msg, "al");
		return 0;
	}
	std::string templ = "mother";
	double t, t_omp;

	int cnt = count_substr(text, templ, t);
	int cnt_omp = count_substr_omp(text, templ, t_omp);
	printf("count: %d\n", cnt);
	printf("count omp: %d\n", cnt_omp);
	printf("count time: %lf\n", t);
	printf("count time omp: %lf\n", t_omp);
	printf("number of symbols in file: %d\n", text.size());

	return 0;
}