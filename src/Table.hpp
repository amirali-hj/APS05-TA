#ifndef TABLE_HPP
#define TABLE_HPP

#include <string>
#include <vector>
#include "Column.hpp"

using namespace std;

class Table
{
protected:
    string name;
    vector<Column> columns;
    vector<vector<string>> rows;

    bool checkCondition(const string &cellValue, const string &op, const string &condValue, const string &type) const;

public:
    Table(string n);
    virtual ~Table() = default;

    string getName() const;
    const vector<Column> &getColumns() const;
    const vector<vector<string>> &getRows() const;

    int getColumnIndex(const string &colName) const;
    virtual void addColumn(const Column &col);
    virtual bool isEnhanced() const;
    virtual int getKeyIndex() const;
    void insertRow(const vector<string> &row);
    virtual void updateRecords(string whereColName, string op, string whereValue, string setColName, string setValue);
    virtual vector<vector<string>> selectRecords(string whereColName, string op, string whereValue);
};

class SimpleTable : public Table
{
public:
    SimpleTable(string n);
};

class EnhancedTable : public Table
{
    int keyColumnIndex;

public:
    EnhancedTable(string n);
    bool isEnhanced() const override;
    int getKeyIndex() const override;
    void addColumn(const Column &col) override;
    void updateRecords(string whereColName, string op, string whereValue, string setColName, string setValue) override;
    vector<vector<string>> selectRecords(string whereColName, string op, string whereValue) override;
};

#endif