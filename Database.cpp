using namespace std;

#include <iostream>
#include <sqlite3.h>
#include "Database.h"
#include <string>

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

    cout << "HIIIII";

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

string DatabaseClass::LastEquation()
{
    int current_id = -1;

    sqlite3_exec(db, "COMMIT;", nullptr, nullptr, nullptr);
    sqlite3_open("Graphs.db", &db);

    const char* id_Query = "SELECT MAX(EquationID) FROM Equation_Table";

    if (sqlite3_prepare_v2(db, id_Query, -1, &stmt, nullptr) != SQLITE_OK) {
        sqlite3_close(db);
        return 0;
    }

    sqlite3_step(stmt);

    current_id = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);


    if (current_id == -1) {

        cout << "database empty";
        sqlite3_close(db);

        return 0;
    }

    const char* selectSQL = "SELECT * FROM Equation_Table WHERE EquationID < ? ORDER BY EquationID DESC LIMIT 1";


    if (sqlite3_prepare_v2(db, selectSQL, -1, &stmt, nullptr) != SQLITE_OK) {
        sqlite3_close(db);
        return 0;
    }
    sqlite3_bind_int(stmt, 1, current_id);

    string output;


    if (sqlite3_step(stmt) == SQLITE_ROW) {

        //cout << "This is the id" << current_id;

        //sqlite3_step(stmt);
        int id = sqlite3_column_int(stmt, 0);

        const char* contents = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        std::string str(contents);
        std::cout << "Retrieved value: " << id << std::endl;

        output = str;
    }
    sqlite3_finalize(stmt);

    sqlite3_close(db);
    
    return output;
}
