#include "Algorithms.h"
#include <future> 
#include <cmath> 
#include <thread>

using namespace std;

string Algorithms::toLower(const string& s) {
	string lower = s;
	transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
	return lower;
}

string Algorithms::replaceAll(const string& str, const string& from, const string& to) {
	if (from.empty()) return str;
	string result = str;
	string lowerResult = toLower(str);
	string lowerFrom = toLower(from);

	size_t start_pos = 0;
	while ((start_pos = lowerResult.find(lowerFrom, start_pos)) != string::npos) {
		result.replace(start_pos, lowerFrom.length(), to);
		start_pos += to.length();
		lowerResult = toLower(result);
	}
	return result;
}

float Algorithms::SinExpansion(double num, double precision)
{
	float value = 0;

	num = remainder(num + 3.14159f, 2 * 3.14159f);

	for (int n = 0; n < precision; n++) {
		value += pow(-1.0, n) * pow(num, 2 * n + 1) / Utils::factorial(2 * n + 1);
	}
	return value;
}
float Algorithms::CosExpansion(double num, double precision)
{
	float value = 0;

	num = remainder(num + 3.14159f, 2 * 3.14159f);

	for (int n = 0; n < precision; n++) {
		value += pow(-1.0, n) * pow(num, 2 * n) / Utils::factorial(2 * n);
	}
	return value;
}
float Algorithms::averageZ(const Utils::triangle& t) {
	return (t.p[0].z + t.p[1].z + t.p[2].z) / 3.0f;
}

bool Algorithms::compareTriangles(const Utils::triangle& t1, const Utils::triangle& t2) {
	return averageZ(t1) > averageZ(t2);
}

void Algorithms::merge(vector<Utils::triangle>& vec, int left, int mid, int right) {
	int n1 = mid - left + 1;
	int n2 = right - mid;

	vector<Utils::triangle> leftHalf(vec.begin() + left, vec.begin() + mid + 1);
	vector<Utils::triangle> rightHalf(vec.begin() + mid + 1, vec.begin() + right + 1);

	int i = 0, j = 0, k = left;
	while (i < n1 && j < n2) {
		if (compareTriangles(leftHalf[i], rightHalf[j])) {
			vec[k] = leftHalf[i];
			i++;
		}
		else {
			vec[k] = rightHalf[j];
			j++;
		}
		k++;
	}

	while (i < n1) {
		vec[k] = leftHalf[i];
		i++;
		k++;
	}

	while (j < n2) {
		vec[k] = rightHalf[j];
		j++;
		k++;
	}
}
void Algorithms::parallelMergeSort(vector < Utils::triangle >& vec, int left, int right, int depth) {
	if (left < right) {
		int mid = left + (right - left) / 2;

		if (depth < log2(thread::hardware_concurrency())) {
			auto leftFuture = async(launch::async, &Algorithms::parallelMergeSort, ref(vec), left, mid, depth + 1);
			parallelMergeSort(vec, mid + 1, right, depth + 1);
			leftFuture.get(); 
		}
		else {
			parallelMergeSort(vec, left, mid, depth + 1);
			parallelMergeSort(vec, mid + 1, right, depth + 1);
		}

		merge(vec, left, mid, right);
	}
}