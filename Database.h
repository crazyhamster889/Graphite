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
	bool OpenDatabase();
	bool ExecuteSQL(const char* sql);
	bool PrepareAndExecute(const char* statement, initializer_list<const char*> params);
	int PrepareAndExecuteSearch(const char* statement, initializer_list<const char*> params);
	vector<string> LastEquation(int userID);
};

#endif 
