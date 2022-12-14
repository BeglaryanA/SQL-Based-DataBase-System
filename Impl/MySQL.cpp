#include "../header/MySQL.h"
#include "../pars.h"
#include "../header/user.h"
#include "../header/task.h"
#include "../header/project.h"

// MySQL constructor
MySQL::MySQL(UserDB* user, TaskDB* task, ProjectDB* project) {
    // this->init(user, task, project);
    // For commands
    command["insert"] = Command::INSERT;
    command["select"] = Command::SELECT;
    command["update"] = Command::UPDATE;
    command["delete"] = Command::DELETE;
    command["add"] = Command::ADD;
    
    // For DB files
    DB["user.data"] = DBFile::USER;
    DB["project.data"] = DBFile::PROJECT;
    DB["task.data"] = DBFile::TASK;
    
    // For exact type DB
    DB_Obj[DBFile::USER] = /*new UserDB();*/ user;
    DB_Obj[DBFile::PROJECT] = /*new ProjectDB();*/ project;
    DB_Obj[DBFile::TASK] = /*new TaskDB();*/ task;

    // For function pointers
    func_ht[Command::INSERT] = &MySQL::insert;
    func_ht[Command::SELECT] = &MySQL::select;
    func_ht[Command::UPDATE] = &MySQL::update;
    func_ht[Command::DELETE] = &MySQL::delete_; 
    func_ht[Command::ADD] = &MySQL::add;
}

// Main function to execute input
void MySQL::execut(const std::string& rhs) {
    const std::string split = " ,";
    auto pars = parser(rhs, split);
    
    // Finding Data Base file name
    auto file_name = std::find_if(pars.begin(), pars.end(), [&](const std::string& rhs) { return (rhs.find('.') != std::string::npos);});
    (file_name == pars.end()) ? throw std::invalid_argument("No such file!") : (this->check_file(*file_name));
    
    // Finding command
    auto command_name = std::find_if(pars.begin(), (file_name), [&](std::string& rhs) { return (command.find(lowercase(rhs)) != command.end());});
    command_name = ((command_name == file_name) ? (throw std::invalid_argument("No such command!")) : (command_name));
    
    // Create vector for datas
    std::vector<std::string> vec_datas(file_name + 1, pars.end());
    
    // Calling fucntion for command execution
    (this->*func_ht[command[*command_name]])(DB_Obj[DB[*file_name]], vec_datas);
}

// Checking for file validity
void MySQL::check_file(const std::string& rhs) {
    if (!static_cast<bool>(DB[rhs])) {
        throw std::invalid_argument("No such file or dircetory!");
    }
}

//Connection funtion with DB files 
void MySQL::insert(Connect* connect_ptr, std::vector<std::string>& datas) {
    connect_ptr->insert(datas);
}

void MySQL::select(Connect* connect_ptr, std::vector<std::string>& datas) {
    connect_ptr->select(datas);
}

void MySQL::update(Connect* connect_ptr, std::vector<std::string>& datas) {
    connect_ptr->update(datas);
}

void MySQL::delete_(Connect* connect_ptr, std::vector<std::string>& datas) {
    connect_ptr->delete_(datas);
}

void MySQL::add(Connect* connect_ptr, std::vector<std::string>& datas) {
    connect_ptr->add(datas);
}

// MySQL destructor
MySQL::~MySQL() {
    // DB_Obj[DBFile::USER]->to_file();
    // DB_Obj[DBFile::PROJECT]->to_file();
    // DB_Obj[DBFile::TASK]->to_file();

    for (auto& it : DB_Obj) {
        delete it.second;
        it.second = nullptr;
    }    
}

//endif 