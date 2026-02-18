#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <string>

using namespace std;

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

#endif