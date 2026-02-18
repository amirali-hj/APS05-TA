#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <exception>
#include <map>

using namespace std;

// ==========================================
// 1. Constants
// ==========================================
namespace Constants
{
    const string CMD_CREATE_TABLE = "create_table";
    const string CMD_CREATE_ENHANCED = "create_enhanced_table";
    const string CMD_DROP_TABLE = "drop_table";
    const string CMD_INSERT = "insert";
    const string CMD_UPDATE = "update";
    const string CMD_SELECT = "select";

    const string KW_INTO = "into";
    const string KW_WHERE = "where";
    const string KW_SET = "set";
    const string KW_FROM = "from";

    const string OP_EQ = "=";
    const string OP_LT = "<";
    const string OP_GT = ">";
    const string OP_NEQ = "!=";

    const string TYPE_INT = "int";
    const string TYPE_STRING = "string";
    const string CONSTRAINT_REQUIRED = "required";

    const string ERR_TABLE_EXISTS = "Error: A table with this name already exists";
    const string ERR_DUPLICATE_COL = "Error: Duplicate field name in table definition";
    const string ERR_TABLE_NOT_EXIST_PREFIX = "Error: Table ";
    const string ERR_TABLE_NOT_EXIST_SUFFIX = " does not exist";
    const string ERR_MISSING_VALUE = "Error: Missing value for required field";
    const string ERR_DUPLICATE_VALUE = "Error: Duplicate value for required field";
    const string ERR_NO_MATCH = "Error: No matching records found";

    const char DELIMITER_COLUMNS = ';';
    const char DELIMITER_FIELD_VAL = ':';

    const string MSG_SUCCESS_PREFIX = "Success: Table ";
    const string MSG_SUCCESS_SUFFIX = " created successfully";
    const string MSG_DELETE_SUCCESS_PREFIX = "Success: Table ";
    const string MSG_DELETE_SUCCESS_SUFFIX = " deleted successfully";
    const string MSG_INSERT_SUCCESS = "Success: Record inserted into table successfully";
    const string MSG_UPDATE_SUCCESS = "Success: Records updated in table";
}

// ==========================================
// 2. Exception & Entities
// ==========================================
class DatabaseException : public exception
{
    string message;

public:
    DatabaseException(const string &msg) : message(msg) {}
    const char *what() const noexcept override { return message.c_str(); }
};

class Column
{
private:
    string name;
    string type;
    bool isRequired;

public:
    Column(string n, string t, bool req) : name(n), type(t), isRequired(req) {}

    // Getters
    string getName() const { return name; }
    string getType() const { return type; }
    bool isRequiredColumn() const { return isRequired; }
};

// ==========================================
// 3. Table Hierarchy
// ==========================================
class Table
{
protected:
    string name;
    vector<Column> columns;
    vector<vector<string>> rows;

    bool checkCondition(const string &cellValue, const string &op, const string &condValue, const string &type) const
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

public:
    Table(string n) : name(n) {}
    virtual ~Table() = default;

    string getName() const { return name; }
    const vector<Column> &getColumns() const { return columns; }
    const vector<vector<string>> &getRows() const { return rows; }

    int getColumnIndex(const string &colName) const
    {
        for (int i = 0; i < columns.size(); ++i)
        {
            // استفاده از getter
            if (columns[i].getName() == colName)
                return i;
        }
        return -1;
    }

    virtual void addColumn(const Column &col) { columns.push_back(col); }
    virtual bool isEnhanced() const { return false; }
    virtual int getKeyIndex() const { return -1; }

    void insertRow(const vector<string> &row)
    {
        rows.push_back(row);
    }

    virtual void updateRecords(string whereColName, string op, string whereValue, string setColName, string setValue)
    {
        int whereIdx = getColumnIndex(whereColName);
        int setIdx = getColumnIndex(setColName);
        // استفاده از getter
        string colType = columns[whereIdx].getType();

        for (auto &row : rows)
        {
            if (checkCondition(row[whereIdx], op, whereValue, colType))
            {
                row[setIdx] = setValue;
            }
        }
    }

    virtual vector<vector<string>> selectRecords(string whereColName, string op, string whereValue)
    {
        vector<vector<string>> matches;
        int whereIdx = getColumnIndex(whereColName);
        // استفاده از getter
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
};

class SimpleTable : public Table
{
public:
    SimpleTable(string n) : Table(n) {}
};

class EnhancedTable : public Table
{
    int keyColumnIndex;

public:
    EnhancedTable(string n) : Table(n), keyColumnIndex(-1) {}

    bool isEnhanced() const override { return true; }
    int getKeyIndex() const override { return keyColumnIndex; }

    void addColumn(const Column &col) override
    {
        Table::addColumn(col);
        // استفاده از getter
        if (col.isRequiredColumn() && keyColumnIndex == -1)
        {
            keyColumnIndex = columns.size() - 1;
        }
    }

    void updateRecords(string whereColName, string op, string whereValue, string setColName, string setValue) override
    {
        int whereIdx = getColumnIndex(whereColName);
        int setIdx = getColumnIndex(setColName);
        // استفاده از getter
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

    vector<vector<string>> selectRecords(string whereColName, string op, string whereValue) override
    {
        vector<vector<string>> matches = Table::selectRecords(whereColName, op, whereValue);

        if (keyColumnIndex != -1 && !matches.empty())
        {
            int sortIdx = keyColumnIndex;
            // استفاده از getter
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
};

// ==========================================
// 4. Validator
// ==========================================
class Validator
{
public:
    static void checkTableDoesNotExist(const vector<Table *> &tables, const string &name)
    {
        for (const auto &t : tables)
        {
            if (t->getName() == name)
                throw DatabaseException(Constants::ERR_TABLE_EXISTS);
        }
    }

    static Table *findTableOrThrow(const vector<Table *> &tables, const string &name)
    {
        for (auto t : tables)
        {
            if (t->getName() == name)
                return t;
        }
        throw DatabaseException(Constants::ERR_TABLE_NOT_EXIST_PREFIX + name + Constants::ERR_TABLE_NOT_EXIST_SUFFIX);
    }

    static void checkDuplicateColumns(const vector<Column> &cols)
    {
        for (int i = 0; i < cols.size(); ++i)
        {
            for (int j = i + 1; j < cols.size(); ++j)
            {
                // استفاده از getter
                if (cols[i].getName() == cols[j].getName())
                    throw DatabaseException(Constants::ERR_DUPLICATE_COL);
            }
        }
    }

    static void checkRequiredFields(const Table *table, const map<string, string> &inputData)
    {
        for (const auto &col : table->getColumns())
        {
            // استفاده از getter
            if (col.isRequiredColumn())
            {
                if (inputData.find(col.getName()) == inputData.end())
                {
                    throw DatabaseException(Constants::ERR_MISSING_VALUE);
                }
            }
        }
    }

    static void checkDuplicateKey(const Table *table, const map<string, string> &inputData)
    {
        if (!table->isEnhanced())
            return;

        int keyIdx = table->getKeyIndex();
        if (keyIdx == -1)
            return;

        // استفاده از getter
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
};

// ==========================================
// 5. Database
// ==========================================
class Database
{
private:
    vector<Table *> tables;

public:
    ~Database()
    {
        for (auto t : tables)
            delete t;
    }

    const vector<Table *> &getTables() const { return tables; }

    void createTable(string name, bool isEnhanced, const vector<Column> &cols)
    {
        Table *newTable = isEnhanced ? (Table *)new EnhancedTable(name) : (Table *)new SimpleTable(name);
        for (const auto &col : cols)
            newTable->addColumn(col);
        tables.push_back(newTable);
    }

    void dropTable(string name)
    {
        auto it = find_if(tables.begin(), tables.end(), [&](Table *t)
                          { return t->getName() == name; });
        if (it != tables.end())
        {
            delete *it;
            tables.erase(it);
        }
    }

    void insertInto(string tableName, const map<string, string> &dataMap)
    {
        auto it = find_if(tables.begin(), tables.end(), [&](Table *t)
                          { return t->getName() == tableName; });
        Table *table = *it;

        vector<string> newRow;
        for (const auto &col : table->getColumns())
        {
            // استفاده از getter
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

    void updateTable(string tableName, string whereCol, string op, string whereVal, string setCol, string setVal)
    {
        auto it = find_if(tables.begin(), tables.end(), [&](Table *t)
                          { return t->getName() == tableName; });
        (*it)->updateRecords(whereCol, op, whereVal, setCol, setVal);
    }

    void selectFromTable(string tableName, const vector<string> &reqFields, string whereCol, string op, string whereVal)
    {
        auto it = find_if(tables.begin(), tables.end(), [&](Table *t)
                          { return t->getName() == tableName; });
        Table *table = *it;

        vector<vector<string>> results = table->selectRecords(whereCol, op, whereVal);

        if (results.empty())
        {
            throw DatabaseException(Constants::ERR_NO_MATCH);
        }

        vector<int> reqIndices;
        for (const string &field : reqFields)
        {
            int idx = table->getColumnIndex(field);
            if (idx != -1)
                reqIndices.push_back(idx);
        }

        for (int i = 0; i < results.size(); ++i)
        {
            cout << (i + 1) << ". ";
            for (int j = 0; j < reqIndices.size(); ++j)
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
};

// ==========================================
// 6. Command Handler
// ==========================================
class CommandHandler
{
private:
    Database db;

    string trim(const string &str)
    {
        const string w = " \t\r\n";
        int f = str.find_first_not_of(w);
        return (f == string::npos) ? "" : str.substr(f, str.find_last_not_of(w) - f + 1);
    }

    vector<Column> parseColumns(const string &line)
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

    map<string, string> parseDataFields(const string &line)
    {
        map<string, string> data;
        stringstream ss(line);
        string segment;
        while (getline(ss, segment, Constants::DELIMITER_COLUMNS))
        {
            segment = trim(segment);
            if (segment.empty())
                continue;
            int delimiterPos = segment.find(Constants::DELIMITER_FIELD_VAL);
            if (delimiterPos != string::npos)
            {
                string key = trim(segment.substr(0, delimiterPos));
                string val = trim(segment.substr(delimiterPos + 1));
                data[key] = val;
            }
        }
        return data;
    }

    vector<string> parseSelectFields(const string &fieldsStr)
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

    void handleCreate(stringstream &ss, bool isEnhanced)
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

    void handleDrop(stringstream &ss)
    {
        string name;
        ss >> name;
        Validator::findTableOrThrow(db.getTables(), name);
        db.dropTable(name);
        cout << Constants::MSG_DELETE_SUCCESS_PREFIX << name << Constants::MSG_DELETE_SUCCESS_SUFFIX << endl;
    }

    void handleInsert(stringstream &ss)
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

    void handleUpdate(stringstream &ss)
    {
        string tableName, whereKw, whereCol, op, whereVal, setKw, setCol, setVal;
        ss >> tableName >> whereKw >> whereCol >> op >> whereVal >> setKw >> setCol >> setVal;
        Validator::findTableOrThrow(db.getTables(), tableName);
        db.updateTable(tableName, whereCol, op, whereVal, setCol, setVal);
        cout << Constants::MSG_UPDATE_SUCCESS << endl;
    }

    void handleSelect(stringstream &ss)
    {
        string fieldsStr, fromKw, tableName, whereKw, whereCol, op, whereVal;
        ss >> fieldsStr >> fromKw >> tableName >> whereKw >> whereCol >> op >> whereVal;

        Validator::findTableOrThrow(db.getTables(), tableName);
        vector<string> reqFields = parseSelectFields(fieldsStr);
        db.selectFromTable(tableName, reqFields, whereCol, op, whereVal);
    }

public:
    void run()
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
};

int main()
{
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    CommandHandler handler;
    handler.run();
    return 0;
}