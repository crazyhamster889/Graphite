#include <vector>
#include "MathsUtils.h"

#ifndef ALGORITHMS_H
#define ALGORITHMS_H
using namespace std;

class Algorithms 
{
private:
    void Merge(vector<Utils::triangle>& vec, int left, int mid, int right);
    float AverageZ(const Utils::triangle& t);
    bool CompareTriangles(const Utils::triangle& t1, const Utils::triangle& t2);

public:
	string toLower(const string& s);
	string Hash(const string& input, int favouriteNumber);
	string replaceAll(const string& str, const string& from, const string& to);
    float SinExpansion(double num, double precision = 15);
    float CosExpansion(double num, double precision = 15);
    void ParallelMergeSort(vector < Utils::triangle >& vec, int left, int right, int depth = 0);
};

#endif 
