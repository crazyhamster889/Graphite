#include "Algorithms.h"
#include <future> 
#include <cmath> 
#include <thread>

using namespace std;
// Converts a string to lower case
string Algorithms::toLower(const string& input) {
	string lower = input;
	transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
	return lower;
}
// Replaces all text of a specific type
string Algorithms::replaceAll(const string& inputText, const string& from, const string& to) {
	if (from.empty()) return inputText;
	string result = inputText;
	// Converts search paramaters to lower to stop it being case sensititive
	string lowerResult = toLower(inputText);
	string lowerFrom = toLower(from);

	// replaces specified string with desired string, npos is really useful for identifying when a string wasn't found
	size_t startIndex = 0;
	while ((startIndex = lowerResult.find(lowerFrom, startIndex)) != string::npos) {
		result.replace(startIndex, lowerFrom.length(), to);
		startIndex += to.length();
		lowerResult = toLower(result);
	}
	return result;
}

// Custom hashing algorithm
string Algorithms::Hash(const string& input, int favouriteNumber)
{
	int salt = 362897 + favouriteNumber;
	int secretKey = sqrt(8741);
	int hashValue = salt;

	list<char> hashingOutput = list<char>();
	string hash = input;

	// Ensures the isn't zero length
	if (hash.length() <= 0)
		return "";

	// Ensures all hashes are the same length
	while (hash.length() < 32) {
		hash += input[input.length() % input.size()];
	}

	for (char character : hash)
	{
		hashValue = (hashValue << 5 + character) % 1559;
		// XOR with the key
		hashValue ^= secretKey;
		hashValue = (hashValue + salt) % 1559;
		// this allows the hash to output lowercase and uppercase letters 
		hashingOutput.push_back((hashValue % 52) < 26 ? 'A' + (hashValue % 26) : 'a' + (hashValue % 26));
	}

	string output(hashingOutput.begin(), hashingOutput.end());

	// If input is larger than 32 characters trim the result to 32
	if (output.length() > 32) {
		output = output.substr(0, 32);
	}

	return output;
}
// Maclaurin series expansion for Sin used to improve performance 
float Algorithms::SinExpansion(double number, double precision)
{
	float value = 0;
	// Ensures the number is within the range of 0 to 2pi, using modular arithmetic
	number = remainder(number + 3.14159f, 2 * 3.14159f);
	Utils mathsHelper;

	for (int n = 0; n < precision; n++) {
		// itterative formula for the Maclaurin series expansion of sin (((-1)^n)*x^(2n + 1))/(2n + 1)!
		value += pow(-1.0, n) * pow(number, 2 * n + 1) / mathsHelper.factorial(2 * n + 1);
	}
	return value;
}
// Maclaurin series expansion for Cos used to improve performance 
float Algorithms::CosExpansion(double number, double precision)
{
	float value = 0;

	// Ensures the number is within the range of 0 to 2pi, using modular arithmetic
	number = remainder(number + 3.14159f, 2 * 3.14159f);
	Utils mathsHelper;

	for (int n = 0; n < precision; n++) {
		// itterative formula for the Maclaurin series expansion of cos (((-1)^n)*x^(2n))/(2n)!
		value += pow(-1.0, n) * pow(number, 2 * n) / mathsHelper.factorial(2 * n);
	}
	return value;
}

// No Maclaurin series expansion for Tan because it's better to divide the output of the Sin expansion with the Cos expansion

// returns the average distance from the camera based on the 3 vertices in the triangle
float Algorithms::AverageZ(const Utils::triangle& triangle) {
	float value = (triangle.p[0].z + triangle.p[1].z + triangle.p[2].z) / 3.0f;
	if (value <= 0)
		return 0;

	return value;
}
// compares the depth of two given triangles
bool Algorithms::CompareTriangles(const Utils::triangle& t1, const Utils::triangle& t2) {
	return AverageZ(t1) > AverageZ(t2);
}
void Algorithms::Merge(vector<Utils::triangle>& triangles, int leftBound, int half, int rightBound) {
	// Get the sizes of the two lists
	int leftHalfSize = half - leftBound + 1;
	int rightHalfSize = rightBound - half;

	// Split the list into two sections based on the sizes defined above
	vector<Utils::triangle> leftHalf(triangles.begin() + leftBound, triangles.begin() + half + 1);
	vector<Utils::triangle> rightHalf(triangles.begin() + half + 1, triangles.begin() + rightBound + 1);

	int leftPointer = 0, rightPointer = 0, mergePointer = leftBound;
	// Checks whether both pointers have reached the end of their lists
	while (leftPointer < leftHalfSize && rightPointer < rightHalfSize) {
		// Compares the triangle depth, adds the triangle that's closer to the screen
		if (CompareTriangles(leftHalf[leftPointer], rightHalf[rightPointer])) {
			triangles[mergePointer] = leftHalf[leftPointer];
			leftPointer++;
		}
		else {
			triangles[mergePointer] = rightHalf[rightPointer];
			rightPointer++;
		}
		mergePointer++;
	}
	// Deals with the case where one of the lists is larger than the other
	// Adds the remaining triangles to the list
	while (leftPointer < leftHalfSize) {
		triangles[mergePointer] = leftHalf[leftPointer];
		leftPointer++;
		mergePointer++;
	}

	while (rightPointer < rightHalfSize) {
		triangles[mergePointer] = rightHalf[rightPointer];
		rightPointer++;
		mergePointer++;
	}
}
void Algorithms::ParallelMergeSort(vector < Utils::triangle >& triangles, int leftPointer, int rightPointer, int depth) {
	if (leftPointer < rightPointer) {
		int mid = leftPointer + (rightPointer - leftPointer) / 2;
		// Checks if the depth is less than the log base 2 of the number of threads, if so it will create a new thread
		// This improves the performance of the merge sort because it's running on multiple threads
		if (depth < log2(thread::hardware_concurrency())) {
			auto leftFuture = async(launch::async, &Algorithms::ParallelMergeSort, this, ref(triangles), leftPointer, mid, depth + 1);
			ParallelMergeSort(triangles, mid + 1, rightPointer, depth + 1);
			leftFuture.get(); 
		}
		else {
			// Else run on the same thread 
			ParallelMergeSort(triangles, leftPointer, mid, depth + 1);
			ParallelMergeSort(triangles, mid + 1, rightPointer, depth + 1);
		}
		// Else run on the same thread
		Merge(triangles, leftPointer, mid, rightPointer);
	}
}