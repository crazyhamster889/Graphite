#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

using namespace std;
#include <iostream>
#include <sqlite3.h>
#include "Database.h"
#include "Algorithms.h"
#include <string>
#include <Vector>

char* errorMessage = nullptr;
sqlite3* database;
sqlite3_stmt* stmt;

// Opens the database
bool DatabaseClass::OpenDatabase() {
    if (sqlite3_open("Graphs.db", &database) != SQLITE_OK) {
        // throws an error if the database can't be opened
        cerr << "Can't open database: " << sqlite3_errmsg(database) << endl;
        return false;
    }
    else
    {
        cout << "Successfully opened database" << endl;
    }
    return true;
}

// Execute SQL
bool DatabaseClass::ExecuteSQL(const char* sql) {
    if (sqlite3_exec(database, sql, nullptr, nullptr, &errorMessage) != SQLITE_OK) {
        cerr << "SQL error: " << errorMessage << endl;
        sqlite3_free(errorMessage);
        return false;
    }
    return true;
}

// Prepare and execute SQL for inserting data
bool DatabaseClass::PrepareAndExecute(const char* statement, initializer_list<const char*> params) {
	// prepare the statement
    if (sqlite3_prepare_v2(database, statement, -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "SQL prepare error: " << sqlite3_errmsg(database) << endl;
        return false;
    }

    // binds all the variables to the statement
    int i = 1;
    for (const char* param : params) 
        sqlite3_bind_text(stmt, i++, param, -1, SQLITE_STATIC);

	// if the statement is successful, return false, otherwise return true
    bool success = (sqlite3_step(stmt) == SQLITE_OK) ? false : true;

    sqlite3_finalize(stmt);
    return success;
}

// Prepare and execute SQL for searching data
int DatabaseClass::PrepareAndExecuteSearch(const char* statement, initializer_list<const char*> params) {
    // prepare the statement
    if (sqlite3_prepare_v2(database, statement, -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "SQL prepare error: " << sqlite3_errmsg(database) << endl;
        return false;
    }

	// binds all the variables to the statement
    int i = 1;
    for (const char* param : params) 
        sqlite3_bind_text(stmt, i++, param, -1, SQLITE_STATIC);

    int output = -1;
	// if the search returns something then return the output
    if (sqlite3_step(stmt) == SQLITE_ROW) 
        output = sqlite3_column_int(stmt, 0);
   
    sqlite3_finalize(stmt);
    return output;
}

void DatabaseClass::SetupDatabase() {
    int exit_code = sqlite3_open("Graphs.db", &database);
	// Create tables if it doesn't exist
    const char* createStatements[] = {
        "CREATE TABLE IF NOT EXISTS Equation_Table ("
        "EquationID INTEGER PRIMARY KEY AUTOINCREMENT, "
        "UserID INTEGER, EquationDescription TEXT NOT NULL, Equation TEXT NOT NULL UNIQUE);",

        "CREATE TABLE IF NOT EXISTS User_Table ("
        "UserID INTEGER PRIMARY KEY AUTOINCREMENT, "
        "ClassCode TEXT NOT NULL, Username TEXT NOT NULL, Password TEXT NOT NULL);",

        "CREATE TABLE IF NOT EXISTS Course_Table ("
        "CourseID INTEGER AUTOINCREMENT, "
        "CourseName TEXT PRIMARY KEY NOT NULL UNIQUE, Subject TEXT NOT NULL);",

        "CREATE TABLE IF NOT EXISTS Class_Table ("
        "ClassID INTEGER PRIMARY KEY AUTOINCREMENT"
        "ClassCode TEXT NOT NULL, CourseName TEXT NOT NULL UNIQUE);"
    };

	// Iterates through the createStatements array and executes the SQL
    for (const char* statement : createStatements) {
        ExecuteSQL(statement);
    }

    sqlite3_close(database);
    return;
}

// Insert into the equation table
void DatabaseClass::InsertIntoEquationTable(const char& EquationDescription, const char& Equation, const char& ID)
{
    if (!OpenDatabase())
        return;

    const char* sqlCommand = "INSERT INTO Equation_Table (EquationDescription, Equation, UserID) VALUES (?, ?, ?);";
    PrepareAndExecute(sqlCommand, { &EquationDescription, &Equation, &ID});
    sqlite3_close(database);
    return;
}
// Insert into the course table
void DatabaseClass::InsertIntoCourseTable(const char& courseName, const char& subject)
{
    if (!OpenDatabase()) return;

    const char* sqlCommand = "INSERT INTO Course_Table (CourseName, Subject) VALUES (?, ?);";
    PrepareAndExecute(sqlCommand, { &courseName, &subject});
    sqlite3_close(database);

    MessageBox(NULL, L"Successfully created course", L"Information", MB_OK);
    return;
}

// Insert into the class table
void DatabaseClass::InsertIntoClassTable(const char& classCode, const char& courseName)
{
    if (!OpenDatabase()) return;

    const char* sqlCommand = "INSERT INTO Class_Table (ClassCode, CourseName) VALUES (?, ?);";
    PrepareAndExecute(sqlCommand, { &classCode, &courseName });
    sqlite3_close(database);

    MessageBox(NULL, L"Successfully created class", L"Information", MB_OK);
    return;
}

// Insert into the User table
int DatabaseClass::InsertIntoUserTable(const char& username, const char& password, const char& className, int favouriteNumber)
{
    if (!OpenDatabase()) return -1;

    Algorithms algorithms;
    string strClassName = &className;
    string hashedUsername = algorithms.Hash(&username, favouriteNumber);
    string hashedPassword = algorithms.Hash(&password, favouriteNumber);

	// Using joins to join the Class table and the Course table when for data in the User class
    const char* selectSQL =  "SELECT * FROM User_Table "
    "INNER JOIN Class_Table ON User_Table.ClassCode = Class_Table.ClassCode "
    "INNER JOIN Course_Table ON Class_Table.CourseName = Course_Table.CourseName "
    "WHERE User_Table.Username = ? AND User_Table.Password = ? AND Class_Table.ClassCode = ?;";
    cout << strClassName << endl;

    int id = PrepareAndExecuteSearch(selectSQL, { hashedUsername.data(), hashedPassword.data(), strClassName.data()});
    // returns true if the search failed
    if (id != -1) 
    {
        MessageBox(NULL, L"Successfully found user", L"Information", MB_OK);
        return id;
    }

	// If the user didn't exist then insert the user into the User table
    const char* sqlCommand = "INSERT INTO User_Table (ClassCode, Username, Password) VALUES (?, ?, ?);";

    if (!PrepareAndExecute(sqlCommand, { &className, hashedUsername.data(), hashedPassword.data() })) {
        sqlite3_close(database);
        return -1;
    }

    MessageBox(NULL, L"Successfully created user", L"Information", MB_OK);

    // Get inserted user ID
    sqlite3_int64 newID = sqlite3_last_insert_rowid(database);
    sqlite3_close(database);

    return newID;
}

// Fetches all recent equations
vector<string> DatabaseClass::FetchRecentEquations(int userID)
{
    vector<string> foundEquations;

    // Ensure the database is open
    if (!OpenDatabase()) return foundEquations;

    // Corrected query to get the last equation
    const char* selectSQL = "SELECT * FROM Equation_Table WHERE UserID = ? ORDER BY EquationID DESC LIMIT 100;";

	// Prepare the statement
    if (sqlite3_prepare_v2(database, selectSQL, -1, &stmt, nullptr)) {
        cerr << "SQL prepare error: " << sqlite3_errmsg(database) << endl;
        return foundEquations;
    }

	// each user has their own "Inventory" of equations, so we need to bind the user ID to the statement when searching for equations
    sqlite3_bind_int(stmt, 1, userID);

	// Fetch the equations
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const char* contents = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));

        string str(contents);
        foundEquations.push_back(str);
    }

	// Finalise the statement, to prevent memory leaks
    if (stmt) {
        sqlite3_finalize(stmt);
        stmt = nullptr;
    }

    return foundEquations;
}