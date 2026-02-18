#ifndef COLUMN_HPP
#define COLUMN_HPP

#include <string>

using namespace std;

class Column
{
private:
    string name;
    string type;
    bool isRequired;

public:
    Column(string n, string t, bool req);
    string getName() const;
    string getType() const;
    bool isRequiredColumn() const;
};

#endif