
#ifndef DATABASE_H
#define DATABASE_H
class DatabaseClass {
public:
	void SetupDatabase();
	void InsertIntoDatabase(const char& database, const char& contents);
	string LastEquation();
};

#endif 
