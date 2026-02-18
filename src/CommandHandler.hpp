#ifndef COMMANDHANDLER_HPP
#define COMMANDHANDLER_HPP

#include "Database.hpp"
#include <string>
#include <vector>
#include <map>
#include <sstream>

using namespace std;

class CommandHandler
{
private:
    Database db;

    string trim(const string &str);
    vector<Column> parseColumns(const string &line);
    map<string, string> parseDataFields(const string &line);
    vector<string> parseSelectFields(const string &fieldsStr);

    void handleCreate(stringstream &ss, bool isEnhanced);
    void handleDrop(stringstream &ss);
    void handleInsert(stringstream &ss);
    void handleUpdate(stringstream &ss);
    void handleSelect(stringstream &ss);

public:
    void run();
};

#endif