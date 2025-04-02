
#pragma once

#include <vector>

class DatabaseClass {
public:
	void InsertIntoEquationTable(const char& EquationDescription, const char& Equation, const char& ID);
	void InsertIntoCourseTable(const char& courseName, const char& subject);
	void InsertIntoClassTable(const char& classCode, const char& courseName);
	int InsertIntoUserTable(const char& username, const char& password, const char& className, int favouriteNumber);
	vector<string> FetchRecentEquations(int userID);
	void SetupDatabase();
private:
	bool ExecuteSQL(const char* sql);
	bool OpenDatabase();
	bool PrepareAndExecute(const char* statement, initializer_list<const char*> params);
	int PrepareAndExecuteSearch(const char* statement, initializer_list<const char*> params);
};