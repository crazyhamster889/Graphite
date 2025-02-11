#include <Vector>

#ifndef DATABASE_H
#define DATABASE_H
class DatabaseClass {
public:
	void SetupDatabase();
	void InsertIntoEquationTable(const char& EquationName, const char& Equation, const char ID);
	int InsertIntoUserTable(const char& Username, const char& Password);
	vector<string> LastEquation(int userID);
};

#endif 
