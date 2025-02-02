#include <vector>
#include "MathsUtils.h"

#ifndef ALGORITHMS_H
#define ALGORITHMS_H
using namespace std;

class Algorithms 
{
public:
	string toLower(const string& s);
	string replaceAll(const string& str, const string& from, const string& to);
	static float averageZ(const Utils::triangle& t);
	static float SinExpansion(double num, double precision = 15);
	static bool compareTriangles(const Utils::triangle& t1, const Utils::triangle& t2);
	static void merge(vector<Utils::triangle>& vec, int left, int mid, int right);
	static void parallelMergeSort(vector < Utils::triangle >& vec, int left, int right, int depth = 0);
};

#endif 
