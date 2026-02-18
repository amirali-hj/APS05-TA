#ifndef VALIDATOR_HPP
#define VALIDATOR_HPP

#include <vector>
#include <string>
#include <map>
#include "Table.hpp"

using namespace std;

class Validator
{
public:
    static void checkTableDoesNotExist(const vector<Table *> &tables, const string &name);
    static Table *findTableOrThrow(const vector<Table *> &tables, const string &name);
    static void checkDuplicateColumns(const vector<Column> &cols);
    static void checkRequiredFields(const Table *table, const map<string, string> &inputData);
    static void checkDuplicateKey(const Table *table, const map<string, string> &inputData);
};

#endif