#ifndef DATABASEEXCEPTION_HPP
#define DATABASEEXCEPTION_HPP

#include <exception>
#include <string>

using namespace std;

class DatabaseException : public exception
{
    string message;

public:
    DatabaseException(const string &msg);
    const char *what() const noexcept override;
};

#endif