#include "Column.hpp"

using namespace std;

Column::Column(string n, string t, bool req) : name(n), type(t), isRequired(req) {}

string Column::getName() const { return name; }
string Column::getType() const { return type; }
bool Column::isRequiredColumn() const { return isRequired; }