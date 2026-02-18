#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <vector>
#include <string>
#include <map>
#include "Table.hpp"

using namespace std;

class Database
{
private:
    vector<Table *> tables;

public:
    ~Database();
    const vector<Table *> &getTables() const;
    void createTable(string name, bool isEnhanced, const vector<Column> &cols);
    void dropTable(string name);
    void insertInto(string tableName, const map<string, string> &dataMap);
    void updateTable(string tableName, string whereCol, string op, string whereVal, string setCol, string setVal);
    void printResult(const vector<vector<string>> results, const vector<string> &reqFields, Table *table);
    void selectFromTable(string tableName, const vector<string> &reqFields, string whereCol, string op, string whereVal);
};

#endif