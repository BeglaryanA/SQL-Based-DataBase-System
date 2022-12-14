#pragma once
#include <vector>
#include <string>
#include "enumaration.h"


struct Date {
    std::size_t day;
    std::size_t month;
    std::size_t year;
    Date();
    Date(const std::string&);
    Date(const Date&);
    bool is_empty() const;
};



class Connect {
public:
    Connect();
    virtual ~Connect();
    virtual void insert(std::vector<std::string>&) = 0;
    virtual void select(std::vector<std::string>&) = 0;
    virtual void delete_(std::vector<std::string>&) = 0;
    virtual void update(std::vector<std::string>&) = 0;
    virtual void add(std::vector<std::string>&) = 0;
    virtual void to_file() = 0;
};