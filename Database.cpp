using namespace std;

#include <iostream>
#include <sqlite3.h>
#include "Database.h"
#include <string>
#include <Vector>

char* error_message = nullptr;
sqlite3* db;
sqlite3_stmt* stmt;

//DatabaseClass::DatabaseClass() {}

void DatabaseClass::SetupDatabase() {
    std::cout << "Opened database successfully!";
    int exit_code = sqlite3_open("Graphs.db", &db);

    const char* sqlCommand = "CREATE TABLE IF NOT EXISTS Equation_Table ("
        "EquationID INTEGER PRIMARY KEY AUTOINCREMENT, "
        "EquationName TEXT NOT NULL, "
        "Equation TEXT NOT NULL);";
    exit_code = sqlite3_exec(db, sqlCommand, nullptr, nullptr, &error_message);

    if (exit_code != SQLITE_OK) {
        std::cerr << "SQL error: " << std::endl;
    }
    else {
        std::cout << "Table created successfully!" << std::endl;
    }

    sqlite3_close(db);
    return;
}

void DatabaseClass::InsertIntoDatabase(const char& EquationName, const char& Equation)
{
    sqlite3_exec(db, "COMMIT;", nullptr, nullptr, nullptr);

    if (sqlite3_open("Graphs.db", &db))
    {
        cout << "Can't open database";
    }
    const char* sqlCommand = "INSERT INTO Equation_Table (EquationName, Equation) VALUES (?, ?);";

    if (sqlite3_prepare_v2(db, sqlCommand, -1, &stmt, nullptr) != SQLITE_OK) {
        sqlite3_close(db);
        return;
    }

    sqlite3_bind_text(stmt, 1, &EquationName, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, &Equation, -1, SQLITE_STATIC);

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

vector<string> DatabaseClass::LastEquation()
{
    vector<string> temp;

    // Ensure the database is open
    if (!db) {
        if (sqlite3_open("Graphs.db", &db) != SQLITE_OK) {
            cerr << "Could not open database: " << sqlite3_errmsg(db) << endl;
            return temp;
        }
    }

    // Corrected query to get the last equation
    const char* selectSQL = "SELECT * FROM Equation_Table ORDER BY EquationID DESC LIMIT 100;";

    // Prepare statement
    if (sqlite3_prepare_v2(db, selectSQL, -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
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
