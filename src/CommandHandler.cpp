#include "CommandHandler.hpp"
#include "Constants.hpp"
#include "Validator.hpp"
#include "DatabaseException.hpp"
#include <iostream>

using namespace std;

string CommandHandler::trim(const string &str)
{
    const string w = " \t\r\n";
    size_t f = str.find_first_not_of(w);
    return (f == string::npos) ? "" : str.substr(f, str.find_last_not_of(w) - f + 1);
}

vector<Column> CommandHandler::parseColumns(const string &line)
{
    vector<Column> cols;
    stringstream ss(line);
    string segment;
    while (getline(ss, segment, Constants::DELIMITER_COLUMNS))
    {
        segment = trim(segment);
        if (segment.empty())
            continue;
        stringstream segSs(segment);
        string n, t, c;
        segSs >> n >> t >> c;
        cols.emplace_back(n, t, c == Constants::CONSTRAINT_REQUIRED);
    }
    return cols;
}

map<string, string> CommandHandler::parseDataFields(const string &line)
{
    map<string, string> data;
    stringstream ss(line);
    string segment;
    while (getline(ss, segment, Constants::DELIMITER_COLUMNS))
    {
        segment = trim(segment);
        if (segment.empty())
            continue;
        size_t delimiterPos = segment.find(Constants::DELIMITER_FIELD_VAL);
        if (delimiterPos != string::npos)
        {
            string key = trim(segment.substr(0, delimiterPos));
            string val = trim(segment.substr(delimiterPos + 1));
            data[key] = val;
        }
    }
    return data;
}

vector<string> CommandHandler::parseSelectFields(const string &fieldsStr)
{
    vector<string> fields;
    stringstream ss(fieldsStr);
    string segment;
    while (getline(ss, segment, Constants::DELIMITER_COLUMNS))
    {
        segment = trim(segment);
        if (!segment.empty())
            fields.push_back(segment);
    }
    return fields;
}

void CommandHandler::handleCreate(stringstream &ss, bool isEnhanced)
{
    string name, colLine;
    ss >> name;
    getline(ss, colLine);

    vector<Column> cols = parseColumns(colLine);
    Validator::checkTableDoesNotExist(db.getTables(), name);
    Validator::checkDuplicateColumns(cols);

    db.createTable(name, isEnhanced, cols);
    cout << Constants::MSG_SUCCESS_PREFIX << name << Constants::MSG_SUCCESS_SUFFIX << endl;
}

void CommandHandler::handleDrop(stringstream &ss)
{
    string name;
    ss >> name;
    Validator::findTableOrThrow(db.getTables(), name);
    db.dropTable(name);
    cout << Constants::MSG_DELETE_SUCCESS_PREFIX << name << Constants::MSG_DELETE_SUCCESS_SUFFIX << endl;
}

void CommandHandler::handleInsert(stringstream &ss)
{
    string intoKw, tableName, dataLine;
    ss >> intoKw >> tableName;
    getline(ss, dataLine);

    map<string, string> inputData = parseDataFields(dataLine);
    Table *table = Validator::findTableOrThrow(db.getTables(), tableName);

    Validator::checkRequiredFields(table, inputData);
    Validator::checkDuplicateKey(table, inputData);

    db.insertInto(tableName, inputData);
    cout << Constants::MSG_INSERT_SUCCESS << endl;
}

void CommandHandler::handleUpdate(stringstream &ss)
{
    string tableName, whereKw, whereCol, op, whereVal, setKw, setCol, setVal;
    ss >> tableName >> whereKw >> whereCol >> op >> whereVal >> setKw >> setCol >> setVal;
    Validator::findTableOrThrow(db.getTables(), tableName);
    db.updateTable(tableName, whereCol, op, whereVal, setCol, setVal);
    cout << Constants::MSG_UPDATE_SUCCESS << endl;
}

void CommandHandler::handleSelect(stringstream &ss)
{
    string fieldsStr, fromKw, tableName, whereKw, whereCol, op, whereVal;
    ss >> fieldsStr >> fromKw >> tableName >> whereKw >> whereCol >> op >> whereVal;

    Validator::findTableOrThrow(db.getTables(), tableName);
    vector<string> reqFields = parseSelectFields(fieldsStr);
    db.selectFromTable(tableName, reqFields, whereCol, op, whereVal);
}

void CommandHandler::run()
{
    string line;
    while (getline(cin, line))
    {
        if (trim(line).empty())
            continue;
        stringstream ss(line);
        string cmd;
        ss >> cmd;

        try
        {
            if (cmd == Constants::CMD_CREATE_TABLE)
                handleCreate(ss, false);
            else if (cmd == Constants::CMD_CREATE_ENHANCED)
                handleCreate(ss, true);
            else if (cmd == Constants::CMD_DROP_TABLE)
                handleDrop(ss);
            else if (cmd == Constants::CMD_INSERT)
                handleInsert(ss);
            else if (cmd == Constants::CMD_UPDATE)
                handleUpdate(ss);
            else if (cmd == Constants::CMD_SELECT)
                handleSelect(ss);
        }
        catch (const DatabaseException &e)
        {
            cout << e.what() << endl;
        }
    }
}