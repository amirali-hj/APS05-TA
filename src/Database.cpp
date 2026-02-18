#include "Database.hpp"
#include "Validator.hpp"
#include "Constants.hpp"
#include "DatabaseException.hpp"
#include <algorithm>
#include <iostream>

using namespace std;

Database::~Database()
{
    for (auto t : tables)
        delete t;
}

const vector<Table *> &Database::getTables() const { return tables; }

void Database::createTable(string name, bool isEnhanced, const vector<Column> &cols)
{
    Table *newTable = isEnhanced ? (Table *)new EnhancedTable(name) : (Table *)new SimpleTable(name);
    for (const auto &col : cols)
        newTable->addColumn(col);
    tables.push_back(newTable);
}

void Database::dropTable(string name)
{
    auto it = find_if(tables.begin(), tables.end(), [&](Table *t)
                      { return t->getName() == name; });
    if (it != tables.end())
    {
        delete *it;
        tables.erase(it);
    }
}

void Database::insertInto(string tableName, const map<string, string> &dataMap)
{
    auto it = find_if(tables.begin(), tables.end(), [&](Table *t)
                      { return t->getName() == tableName; });
    Table *table = *it;

    vector<string> newRow;
    for (const auto &col : table->getColumns())
    {
        if (dataMap.find(col.getName()) != dataMap.end())
        {
            newRow.push_back(dataMap.at(col.getName()));
        }
        else
        {
            newRow.push_back("");
        }
    }
    table->insertRow(newRow);
}

void Database::updateTable(string tableName, string whereCol, string op, string whereVal, string setCol, string setVal)
{
    auto it = find_if(tables.begin(), tables.end(), [&](Table *t)
                      { return t->getName() == tableName; });
    (*it)->updateRecords(whereCol, op, whereVal, setCol, setVal);
}

void Database::printResult(const vector<vector<string>> results, const vector<string> &reqFields, Table *table)
{
    vector<int> reqIndices;
    for (const string &field : reqFields)
    {
        int idx = table->getColumnIndex(field);
        if (idx != -1)
            reqIndices.push_back(idx);
    }

    for (size_t i = 0; i < results.size(); ++i)
    {
        cout << (i + 1) << ". ";
        for (size_t j = 0; j < reqIndices.size(); ++j)
        {
            int colIdx = reqIndices[j];
            cout << results[i][colIdx];
            if (j < reqIndices.size() - 1)
            {
                cout << " : ";
            }
        }
        cout << endl;
    }
}

void Database::selectFromTable(string tableName, const vector<string> &reqFields, string whereCol, string op, string whereVal)
{
    auto it = find_if(tables.begin(), tables.end(), [&](Table *t)
                      { return t->getName() == tableName; });
    Table *table = *it;

    vector<vector<string>> results = table->selectRecords(whereCol, op, whereVal);

    if (results.empty())
    {
        throw DatabaseException(Constants::ERR_NO_MATCH);
    }
    printResult(results, reqFields, table);
}