#include "Table.hpp"
#include "Constants.hpp"
#include <algorithm>
#include <stdexcept>

using namespace std;

Table::Table(string n) : name(n) {}

string Table::getName() const { return name; }
const vector<Column> &Table::getColumns() const { return columns; }
const vector<vector<string>> &Table::getRows() const { return rows; }

bool Table::checkCondition(const string &cellValue, const string &op, const string &condValue, const string &type) const
{
    if (type == Constants::TYPE_INT)
    {
        try
        {
            int val1 = stoi(cellValue);
            int val2 = stoi(condValue);
            if (op == Constants::OP_EQ)
                return val1 == val2;
            if (op == Constants::OP_LT)
                return val1 < val2;
            if (op == Constants::OP_GT)
                return val1 > val2;
            if (op == Constants::OP_NEQ)
                return val1 != val2;
        }
        catch (...)
        {
            return false;
        }
    }
    else
    {
        if (op == Constants::OP_EQ)
            return cellValue == condValue;
        if (op == Constants::OP_LT)
            return cellValue < condValue;
        if (op == Constants::OP_GT)
            return cellValue > condValue;
        if (op == Constants::OP_NEQ)
            return cellValue != condValue;
    }
    return false;
}

int Table::getColumnIndex(const string &colName) const
{
    for (size_t i = 0; i < columns.size(); ++i)
    {
        if (columns[i].getName() == colName)
            return i;
    }
    return -1;
}

void Table::addColumn(const Column &col) { columns.push_back(col); }
bool Table::isEnhanced() const { return false; }
int Table::getKeyIndex() const { return -1; }
void Table::insertRow(const vector<string> &row) { rows.push_back(row); }

void Table::updateRecords(string whereColName, string op, string whereValue, string setColName, string setValue)
{
    int whereIdx = getColumnIndex(whereColName);
    int setIdx = getColumnIndex(setColName);
    string colType = columns[whereIdx].getType();

    for (auto &row : rows)
    {
        if (checkCondition(row[whereIdx], op, whereValue, colType))
        {
            row[setIdx] = setValue;
        }
    }
}

vector<vector<string>> Table::selectRecords(string whereColName, string op, string whereValue)
{
    vector<vector<string>> matches;
    int whereIdx = getColumnIndex(whereColName);
    string colType = columns[whereIdx].getType();

    for (const auto &row : rows)
    {
        if (checkCondition(row[whereIdx], op, whereValue, colType))
        {
            matches.push_back(row);
        }
    }
    return matches;
}

SimpleTable::SimpleTable(string n) : Table(n) {}

EnhancedTable::EnhancedTable(string n) : Table(n), keyColumnIndex(-1) {}

bool EnhancedTable::isEnhanced() const { return true; }
int EnhancedTable::getKeyIndex() const { return keyColumnIndex; }

void EnhancedTable::addColumn(const Column &col)
{
    Table::addColumn(col);
    if (col.isRequiredColumn() && keyColumnIndex == -1)
    {
        keyColumnIndex = columns.size() - 1;
    }
}

void EnhancedTable::updateRecords(string whereColName, string op, string whereValue, string setColName, string setValue)
{
    int whereIdx = getColumnIndex(whereColName);
    int setIdx = getColumnIndex(setColName);
    string colType = columns[whereIdx].getType();

    for (auto &row : rows)
    {
        if (checkCondition(row[whereIdx], op, whereValue, colType))
        {
            row[setIdx] = setValue;
            if (whereIdx == keyColumnIndex)
                return;
        }
    }
}

vector<vector<string>> EnhancedTable::selectRecords(string whereColName, string op, string whereValue)
{
    vector<vector<string>> matches = Table::selectRecords(whereColName, op, whereValue);

    if (keyColumnIndex != -1 && !matches.empty())
    {
        int sortIdx = keyColumnIndex;
        string type = columns[sortIdx].getType();

        sort(matches.begin(), matches.end(), [sortIdx, type](const vector<string> &a, const vector<string> &b)
             {
            if (type == Constants::TYPE_INT) {
                return stoi(a[sortIdx]) < stoi(b[sortIdx]);
            } else {
                return a[sortIdx] < b[sortIdx];
            } });
    }
    return matches;
}