#pragma once
#include <fstream>
#include "connect.h"
#include "user.h"
#include "task.h"

struct Project {
    std::string name;
    std::vector<std::size_t> user;
    std::string description;
    std::vector<std::size_t> task;
    Date created;
    Date deadline;
    Project();
    Project(const Project&);
};


class ProjectDB : public Connect {
public:
    ProjectDB();
    ProjectDB(TaskDB* ts, UserDB* us);
    ~ProjectDB();
    void insert(std::vector<std::string>&);
    void select(std::vector<std::string>&);
    void delete_(std::vector<std::string>&);
    void update(std::vector<std::string>&);
    void add(std::vector<std::string>&);
    void to_file() override;
private:
    void init();
    void project_data_check(const Project*);
    bool isValidDeadline(const Date&);
    void set_all_true();
    void set_for_select(const Project&);
    bool for_select_unique(const Project&, Project* );
    void redefine();
private:
    // Stataic ID for every project
    static int ID;

    // To open project.data FILE
    std::fstream project_file;

    // For keywords
    std::map<std::string, Project_Keyword> keyword_ht;

    // Connect project ID with project FILE POSITION
    std::map<std::size_t, std::streampos> ID_pos;

    // Connect user ID with task FILE POSITION
    std::map<std::size_t, Project*> project_ID;

    std::map<std::string, bool> for_select;
private:
    bool flag_name;
    bool flag_description;
    bool flag_deadline;
    bool flag_task;
    bool flag_user;
    bool flag_created;
private:
    UserDB* user;
    TaskDB* task;
private:
    DataSave data;
};