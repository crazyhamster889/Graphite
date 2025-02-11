using namespace std;

#include <iostream>
#include <sqlite3.h>
#include "Database.h"
#include <string>
#include <Vector>

char* error_message = nullptr;
sqlite3* db;
sqlite3_stmt* stmt;

void DatabaseClass::SetupDatabase() {
    std::cout << "Opened database successfully!";
    int exit_code = sqlite3_open("Graphs.db", &db);

    const char* createEquationTable = "CREATE TABLE IF NOT EXISTS Equation_Table ("
        "EquationID INTEGER PRIMARY KEY AUTOINCREMENT, "
        "UserID INTEGER, "
        "EquationName TEXT NOT NULL, "
        "Equation TEXT NOT NULL);";
    exit_code = sqlite3_exec(db, createEquationTable, nullptr, nullptr, &error_message);

    const char* createUserTable = "CREATE TABLE IF NOT EXISTS User_Table ("
        "UserID INTEGER PRIMARY KEY AUTOINCREMENT, "
        "Username TEXT NOT NULL, "
        "Password TEXT NOT NULL);";
    exit_code = sqlite3_exec(db, createUserTable, nullptr, nullptr, &error_message);

    const char* createCourseTable = "CREATE TABLE IF NOT EXISTS Course_Table ("
        "CourseID INTEGER PRIMARY KEY AUTOINCREMENT, "
        "CourseName TEXT NOT NULL, "
        "Subject TEXT NOT NULL);";
    exit_code = sqlite3_exec(db, createCourseTable, nullptr, nullptr, &error_message);

    const char* createClassTable = "CREATE TABLE IF NOT EXISTS Class_Table ("
        "ClassID INTEGER PRIMARY KEY AUTOINCREMENT, "
        "CourseID INTEGER, "
        "ClassCode TEXT NOT NULL); ";
    exit_code = sqlite3_exec(db, createCourseTable, nullptr, nullptr, &error_message);

    if (exit_code != SQLITE_OK) {
        std::cerr << "SQL error: " << std::endl;
    }
    else {
        std::cout << "Table created successfully!" << std::endl;
    }

    sqlite3_close(db);
    return;
}

void DatabaseClass::InsertIntoEquationTable(const char& EquationName, const char& Equation, const char ID)
{
    sqlite3_exec(db, "COMMIT;", nullptr, nullptr, nullptr);

    if (sqlite3_open("Graphs.db", &db))
    {
        cout << "Can't open database";
    }
    const char* sqlCommand = "INSERT INTO Equation_Table (EquationName, Equation, UserID) VALUES (?, ?, ?);";

    if (sqlite3_prepare_v2(db, sqlCommand, -1, &stmt, nullptr) != SQLITE_OK) {
        sqlite3_close(db);
        return;
    }

    sqlite3_bind_text(stmt, 1, &EquationName, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, &Equation, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, &ID, -1, SQLITE_STATIC);

    int result = sqlite3_step(stmt);
    if (result == SQLITE_DONE) {
        std::cout << "Data inserted successfully!" << std::endl;
    }
    else {
        std::cerr << "Error executing statement: " << sqlite3_errmsg(db) << std::endl;
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return;
}

int DatabaseClass::InsertIntoUserTable(const char& username, const char& password)
{
    if (sqlite3_open("Graphs.db", &db))
    {
        cout << "Can't open database";
    }

    sqlite3_exec(db, "COMMIT;", nullptr, nullptr, nullptr);

    const char* selectSQL = "SELECT * FROM User_Table WHERE Username = ? AND Password = ?";

    // Prepare statement
    if (sqlite3_prepare_v2(db, selectSQL, -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
        return -1;
    }

    sqlite3_bind_text(stmt, 1, &username, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, &password, -1, SQLITE_STATIC);


    if (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return id;
    }
    else
    {
        const char* sqlCommand = "INSERT INTO User_Table (Username, Password) VALUES (?, ?);";

        if (sqlite3_prepare_v2(db, sqlCommand, -1, &stmt, nullptr) != SQLITE_OK) {
            sqlite3_close(db);
            return -1;
        }

        sqlite3_bind_text(stmt, 1, &username, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, &password, -1, SQLITE_STATIC);

        int result = sqlite3_step(stmt);
        if (result == SQLITE_DONE) {
            std::cout << "Data inserted successfully!" << std::endl;
        }
        else {
            std::cerr << "Error executing statement: " << sqlite3_errmsg(db) << std::endl;
        }

        sqlite3_int64 ID = sqlite3_last_insert_rowid(db);

        sqlite3_finalize(stmt);
        sqlite3_close(db);

        return ID;
    }
    return -1;
}

vector<string> DatabaseClass::LastEquation(int userID)
{
    vector<string> temp;

    // Ensure the database is open
    if (sqlite3_open("Graphs.db", &db) != SQLITE_OK) {
        cerr << "Error opening database: " << sqlite3_errmsg(db) << endl;
        return temp;
    }

    cout << "Working here!!";

    // Corrected query to get the last equation
    const char* selectSQL = "SELECT * FROM Equation_Table WHERE UserID = ? ORDER BY EquationID DESC LIMIT 100;";

    int rc = sqlite3_prepare_v2(db, selectSQL, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        cerr << "SQL prepare error: " << sqlite3_errmsg(db) << endl;
        return temp;
    }

    if (!stmt) {
        cerr << "Statement is null before binding!" << endl;
        return temp;
    }

    rc = sqlite3_bind_int(stmt, 1, userID);
    if (rc != SQLITE_OK) {
        cerr << "SQL bind error: " << sqlite3_errmsg(db) << endl;
        return temp;
    }

    // Execute query
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const char* contents = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));

        if (contents) {
            string str(contents);
            cout << "Retrieved value: " << id << " -> " << str << endl;
            temp.push_back(str);
        }
    }

    // Finalize statement to avoid memory leaks
    if (stmt) {
        sqlite3_finalize(stmt);
        stmt = nullptr;
    }

    return temp;
}
