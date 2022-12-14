#include </home/andranik/Desktop/MySQL/header/connect.h>
#include </home/andranik/Desktop/MySQL/pars.h>

Connect::Connect() {}

Connect::~Connect() {}

std::ostream& operator<<(std::ostream& os, const Date& rhs) {
    os << rhs.day << "/" << rhs.month << "/" << rhs.year;
    return os;
}

bool operator!=(const Date& lhs, const Date& rhs) {
    return (lhs.day != rhs.day) || 
                (lhs.month != rhs.month) ||
                    (lhs.year != rhs.year);
}

Date::Date() 
: day{}
, month{}
, year{}
{}

Date::Date(const std::string& rhs) {
    std::string spliter = {"/ "};
    auto parsed = parser(rhs, spliter);
    if (parsed.size() != 3) { throw std::invalid_argument("Wrong deadline input!"); }
    this->day = std::stoi(parsed[0]);
    this->month = std::stoi(parsed[1]);
    this->year = std::stoi(parsed[2]);
}

Date::Date(const Date& rhs) {
    this->day = rhs.day;
    this->month = rhs.month;
    this->year = rhs.year;
}

bool Date::is_empty() const {
    return (this->day == 0) && (this->month == 0) && (this->year == 0); 
}