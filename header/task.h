#pragma once
#include <fstream>
#include <chrono>
#include "connect.h"
#include "../pars.h"
#include "user.h"


// Task
struct Task {
    std::string m_name;
    std::string user_email;
    Date created;
    Date deadline;
    Task();
    Task(const Task&);
};

class TaskDB : public Connect {
public:
    TaskDB();
    TaskDB(const TaskDB&) {}
    TaskDB(UserDB*);
    ~TaskDB();
    void insert(std::vector<std::string>&);
    void select(std::vector<std::string>&);
    void delete_(std::vector<std::string>&);
    void update(std::vector<std::string>&);
    void add(std::vector<std::string>&);
    void to_file() override;
    int ID_for_project(const std::string&);
    std::string return_name(const std::size_t&);
private:
    void init();
    void task_data_check(Task*);
    bool isValidDeadline(const Date&);
    void set_for_select(const Task&);
    void redefine();
    bool for_selection_unique(Task&, Task*);
private:
    void set_all_true();
private:
    // Static ID count
    static int ID;

    // To open task.data FILE
    std::fstream task_file;

    // Connect user ID with user FILE POSITION
    std::map<std::size_t, std::streampos> ID_pos;
    
    // Searching in inserted string keywords 
    std::map<std::string, Task_Keyword> keyword_ht; 
    
    // Connect user ID with task FILE POSITION
    std::map<std::size_t, Task*> task_ID; 

    std::map<std::string, bool> for_select;
private:
    bool flag_name;
    bool flag_user;
    bool flag_created;
    bool flag_deadline;
private:
    UserDB* user;
private:
    DataSave data;
};