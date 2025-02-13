#include <Vector>

#ifndef DATABASE_H
#define DATABASE_H
class DatabaseClass {
public:
	void SetupDatabase();
	void InsertIntoEquationTable(const char& EquationName, const char& Equation, const char ID);
	void InsertIntoCourseTable(const char& courseName, const char& subject);
	void InsertIntoClassTable(const char& classCode, const char& courseName);
	int InsertIntoUserTable(const char& username, const char& password, const char& className);
	vector<string> LastEquation(int userID);
};

#endif 
