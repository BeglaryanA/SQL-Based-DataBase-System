#include <iostream>
#include </home/andranik/Desktop/MySQL/header/MySQL.h>


int main() {
    std::string str;
    UserDB* user = new UserDB();
    TaskDB* task = new TaskDB(user);
    ProjectDB* project = new ProjectDB(task, user);
    MySQL SQL(user, task, project);
    while (true) {
        try {
            std::cout << "Enter command : ";
            std::getline(std::cin, str);
            if (str == "exit") { 
                break;
            }
            SQL.execut(str);
        } catch(std::exception& e) {
            std::cerr << e.what() << std::endl;
            continue;
        }
        std::cout << std::endl;
    }
    return 0;
}