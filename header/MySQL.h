#pragma once
#include <fstream>
#include "enumaration.h"
#include "connect.h"
#include </home/andranik/Desktop/MySQL/header/user.h>
#include </home/andranik/Desktop/MySQL/header/task.h>
#include </home/andranik/Desktop/MySQL/header/project.h>


class MySQL {
public:
    MySQL();
    MySQL(UserDB*, TaskDB*, ProjectDB*);
    ~MySQL(); //Delete DB_obj->second TODO
public:
    void execut(const std::string&);
private:
    // void init(UserDB , TaskDB , ProjectDB );//
    void check_file(const std::string&);
    void insert(Connect*, std::vector<std::string>&);
    void select(Connect*, std::vector<std::string>&);
    void update(Connect*, std::vector<std::string>&);
    void delete_(Connect*, std::vector<std::string>&);
    void add(Connect*, std::vector<std::string>&);
private:
    using Func_Point = void (MySQL::*)(Connect*, std::vector<std::string>&);
    std::map<std::string, Command> command;
    std::map<std::string, DBFile> DB;
    std::map<DBFile, Connect*> DB_Obj;
    std::map<Command, Func_Point> func_ht;
    std::fstream file;
};