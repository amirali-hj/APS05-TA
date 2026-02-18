#include "DatabaseException.hpp"

using namespace std;

DatabaseException::DatabaseException(const string &msg) : message(msg) {}

const char *DatabaseException::what() const noexcept
{
    return message.c_str();
}