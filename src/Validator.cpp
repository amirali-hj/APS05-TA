#include "Validator.hpp"
#include "Constants.hpp"
#include "DatabaseException.hpp"

using namespace std;

void Validator::checkTableDoesNotExist(const vector<Table *> &tables, const string &name)
{
    for (const auto &t : tables)
    {
        if (t->getName() == name)
            throw DatabaseException(Constants::ERR_TABLE_EXISTS);
    }
}

Table *Validator::findTableOrThrow(const vector<Table *> &tables, const string &name)
{
    for (auto t : tables)
    {
        if (t->getName() == name)
            return t;
    }
    throw DatabaseException(Constants::ERR_TABLE_NOT_EXIST_PREFIX + name + Constants::ERR_TABLE_NOT_EXIST_SUFFIX);
}

void Validator::checkDuplicateColumns(const vector<Column> &cols)
{
    for (size_t i = 0; i < cols.size(); ++i)
    {
        for (size_t j = i + 1; j < cols.size(); ++j)
        {
            if (cols[i].getName() == cols[j].getName())
                throw DatabaseException(Constants::ERR_DUPLICATE_COL);
        }
    }
}

void Validator::checkRequiredFields(const Table *table, const map<string, string> &inputData)
{
    for (const auto &col : table->getColumns())
    {
        if (col.isRequiredColumn())
        {
            if (inputData.find(col.getName()) == inputData.end())
            {
                throw DatabaseException(Constants::ERR_MISSING_VALUE);
            }
        }
    }
}

void Validator::checkDuplicateKey(const Table *table, const map<string, string> &inputData)
{
    if (!table->isEnhanced())
        return;

    int keyIdx = table->getKeyIndex();
    if (keyIdx == -1)
        return;

    string keyName = table->getColumns()[keyIdx].getName();
    if (inputData.find(keyName) == inputData.end())
        return;

    string newValue = inputData.at(keyName);
    for (const auto &row : table->getRows())
    {
        if (row[keyIdx] == newValue)
        {
            throw DatabaseException(Constants::ERR_DUPLICATE_VALUE);
        }
    }
}