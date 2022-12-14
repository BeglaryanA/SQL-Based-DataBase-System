#include </home/andranik/Desktop/MySQL/header/task.h>

int TaskDB::ID = 0;

bool operator!=(const Date& lhs, const Date& rhs);

std::ostream& operator<<(std::ostream& os, const Date& rhs);

Task::Task() 
: m_name{}
, user_email{}
{}

Task::Task(const Task& rhs) {
    this->m_name = rhs.m_name;
    this->deadline = rhs.deadline;
    this->user_email = rhs.user_email;
    this->created = rhs.created;
}

void TaskDB::init() {
    this->keyword_ht["name"] = Task_Keyword::NAME;
    this->keyword_ht["user"] = Task_Keyword::USER;
    this->keyword_ht["deadline"] = Task_Keyword::DEADLINE;
    this->keyword_ht["where"] = Task_Keyword::WHERE;
    this->keyword_ht["set"] = Task_Keyword::SET;
    this->keyword_ht["all"] = Task_Keyword::ALL;

    //map initialization for selection 
    this->for_select["name"] = false;
    this->for_select["user"] = false;
    this->for_select["deadline"] = false;
    //TODO

    this->flag_name = false;
    this->flag_created = false;
    this->flag_deadline = false;
    this->flag_user = false;
}

void TaskDB::set_all_true() {
    this->flag_name = true;
    this->flag_created = true;
    this->flag_deadline = true;
    this->flag_user = true;
}

TaskDB::TaskDB(UserDB* user_arg) 
: user{user_arg}
{
     // Opening task.data file
    task_file.open("DB/task.data");
    if (!task_file.is_open()) {
        throw std::invalid_argument("Fail to open file!");
    }
    this->init();

    std::string line = "";
    std::string tmp = "";

    while (!task_file.eof()) {
        std::getline(task_file, line);
        if (line.empty()) { continue; }
        if ((line.find("ID") != std::string::npos)) {
            tmp += pars_data(line);
            ID_pos.insert(std::make_pair(std::stoi(tmp), task_file.tellg()));
            
            task_ID[std::stoi(tmp)] = new Task;
        }
        tmp.clear();
        line.clear();
    }
    if (task_ID.empty()) { 
        task_file.close();
        if (task_file.is_open()) {
            throw std::runtime_error("Fail to close file!");
        } 
        return; 
    }
    auto it_end = --task_ID.end();
    ID = it_end->first + 1;
    std::string object_data = "";
    for (auto& it : ID_pos) {
        
        getline(task_file.seekg(it.second), object_data);
        task_ID[it.first]->m_name = pars_data(object_data);

        getline(task_file, object_data);
        task_ID[it.first]->user_email = pars_data(object_data);

        getline(task_file, object_data);
        task_ID[it.first]->created = pars_data(object_data);

        getline(task_file, object_data);
        task_ID[it.first]->deadline = pars_data(object_data);
    }

    task_file.close();
    if (task_file.is_open()) {
        throw std::runtime_error("Fail to close file!");
    }
}


void print(Task* rhs) {
    std::cout << "Name : " << rhs->m_name << std::endl; 
    std::cout << "Deadline : " << rhs->deadline.day << "/" << rhs->deadline.month << "/" << rhs->deadline.year << std::endl; 
    std::cout << "User : " << rhs->user_email << std::endl; 
    std::cout << "Current day  : " << (rhs->created.day) 
    << " Current month : " << (rhs->created.month)
    << " Current year : " << (rhs->created.year) << std::endl;
}

TaskDB::TaskDB() {}

// Insert function implementation
void TaskDB::insert(std::vector<std::string>& rhs) {
    typedef std::vector<std::string> vec_str;
    std::string split = "\"";
    std::string to_pars = to_string(rhs);
    auto parsed = parser(to_pars, split);
    std::string str = "";
    split = " =,";
    vec_str final_parsed;
    for (auto it = parsed.begin(); it != parsed.end(); ++it) {
        if (std::distance(parsed.begin(), it) == 1) { 
             final_parsed.push_back(*it);
             ++it;
        }
        auto tmp = parser(*it, split);
        for (int i = 0; i < tmp.size(); ++i) {
            if (tmp[i].empty()) { continue;}
            final_parsed.push_back(tmp[i]);
        }
    }
    Task tmp;

    for (auto it = final_parsed.begin(); it != final_parsed.end() - 1; ++it) {
        if (static_cast<bool>(this->keyword_ht[*it])) {
            tmp.m_name = (this->keyword_ht[*it] == Task_Keyword::NAME) ? *(it + 1) : tmp.m_name;
            tmp.user_email = (this->keyword_ht[*it] == Task_Keyword::USER) ? (user->find_email(*(it + 1)) ? *(it + 1) :  throw std::invalid_argument("No such user!") ): tmp.user_email; 
            tmp.deadline = (this->keyword_ht[*it]) == Task_Keyword::DEADLINE ? *(it + 1) : tmp.deadline;
        }
    }
    const std::chrono::time_point now(std::chrono::system_clock::now());
    std::chrono::year_month_day ymd(std::chrono::floor<std::chrono::days>(now));
    tmp.created.day = static_cast<unsigned>(ymd.day()); 
    tmp.created.month = static_cast<unsigned>(ymd.month()); 
    tmp.created.year = static_cast<int>(ymd.year()); 



    task_data_check(&tmp);
    task_ID[ID] = new Task(tmp);
    ++ID;

    this->data.save(this);

}


// Select function implementation
void TaskDB::select(std::vector<std::string>& rhs) {
    std::string spliter = " =";
    std::string to_pars = to_string(rhs);
    auto parsed = parser(to_pars, spliter);
    auto all_it = std::find_if(parsed.begin(), parsed.end(), [&] (const std::string& rhs) { return keyword_ht[rhs] == Task_Keyword::ALL; });
    auto where_it = std::find_if(parsed.begin(), parsed.end(), [&] (const std::string& rhs) { return keyword_ht[rhs] == Task_Keyword::WHERE;});
    
    if (where_it == parsed.end()) {
        throw std::invalid_argument("Syntax ERROR!");
    }

    if (std::distance(all_it, where_it) != 1 && all_it != parsed.end()) {
        throw std::invalid_argument("Invalid syntax!");
    }

    auto name_it = std::find_if(where_it, parsed.end(), [&] (const std::string& rhs) { return keyword_ht[rhs] == Task_Keyword::NAME;});
    Task tmp;
    if (name_it != parsed.end()) {
        auto pars_for_name = parser(to_pars, "\"");
        tmp.m_name = pars_for_name[1];
    }
    
    for (auto it = parsed.begin(); it != where_it; ++it) {
        if (static_cast<bool>(keyword_ht[*it])) {
            this->flag_name = (keyword_ht[*it] == Task_Keyword::NAME) ? true : flag_name;
            this->flag_user = (keyword_ht[*it] == Task_Keyword::USER) ? true : flag_user;
            this->flag_created = (keyword_ht[*it] == Task_Keyword::CREATED) ? true : flag_created;
            this->flag_deadline = (keyword_ht[*it] == Task_Keyword::DEADLINE) ? true : flag_deadline;
        }
        if (keyword_ht[*it] == Task_Keyword::ALL) {
            this->set_all_true();
            break;
        }
    }
    ++where_it;
    while (where_it < parsed.end() - 1) {
        if (!static_cast<bool>(keyword_ht[*where_it])) {
            throw std::invalid_argument("Invalid keyword!");
        }
        if (static_cast<bool>(keyword_ht[*where_it] == Task_Keyword::NAME)) {
            where_it += 2;
            while (where_it->find("\"") == std::string::npos) {
                ++where_it;
            }
            ++where_it;
        }
        tmp.user_email = (keyword_ht[*where_it] == Task_Keyword::USER) ? (*(where_it + 1)) : tmp.user_email;
        tmp.created = (keyword_ht[*where_it] == Task_Keyword::CREATED) ? *(where_it + 1) : tmp.created;
        tmp.deadline = (keyword_ht[*where_it] == Task_Keyword::DEADLINE) ? *(where_it + 1) : tmp.deadline;
        where_it += 2;

    }
    this->set_for_select(tmp);
    for (auto& it : task_ID) {
        if (for_selection_unique(tmp, it.second)) {
            if (flag_name) {
                std::cout << "name : " << it.second->m_name << std::endl;
            }
            if (flag_user) {
                std::cout << "email : " << /*user->find_email*/(it.second->user_email) << std::endl;
            }
            if (flag_created) {
                std::cout << "created : " << it.second->created << std::endl;
            }
            if (flag_deadline) {
                std::cout << "deadline : " << it.second->deadline << std::endl;
            }
            std::cout << std::endl;
        }
    }
    this->redefine();
}


// Delet function implementation
void TaskDB::delete_(std::vector<std::string>& rhs) {
    std::string spliter = " =";
    std::string to_pars = to_string(rhs);
    auto parsed = parser(to_pars, spliter);
    auto where_it = std::find_if(parsed.begin(), parsed.end(), [&] (const std::string& rhs) { return keyword_ht[rhs] == Task_Keyword::WHERE;});

    if (where_it == parsed.end()) {
        throw std::invalid_argument("Syntax ERROR!");
    }

    auto name_it = std::find_if(where_it, parsed.end(), [&] (const std::string& rhs) { return keyword_ht[rhs] == Task_Keyword::NAME;});

    Task tmp;
    if (name_it != parsed.end()) {
        auto pars_for_name = parser(to_pars, "\"");
        tmp.m_name = pars_for_name[1];
    }

    ++where_it;
    std::size_t count_for_deletion = 0;
    std::vector<std::size_t> tmp_deletion;
    
    while (where_it < parsed.end() - 1) {
        if (!static_cast<bool>(keyword_ht[*where_it])) {
            throw std::invalid_argument("Invalid Keyword!");    
        }
        if (static_cast<bool>(keyword_ht[*where_it] == Task_Keyword::NAME)) {
            where_it += 2;
            while (where_it->find("\"") == std::string::npos) {
                ++where_it;
            }
            ++where_it;
        }
        tmp.user_email = (keyword_ht[*where_it] == Task_Keyword::USER) ? (*(where_it + 1)) : tmp.user_email;
        tmp.created = (keyword_ht[*where_it] == Task_Keyword::CREATED) ? *(where_it + 1) : tmp.created;
        tmp.deadline = (keyword_ht[*where_it] == Task_Keyword::DEADLINE) ? *(where_it + 1) : tmp.deadline;
        where_it += 2;
    }
    this->set_for_select(tmp);

    for (auto& it : task_ID) {
        if (for_selection_unique(tmp, it.second)) {
            std::cout << " name : " << it.second->m_name << std::endl;
            std::cout << " user : " << it.second->user_email << std::endl;
            std::cout << " eadline : " << it.second->deadline << std::endl;
            ++count_for_deletion;
            tmp_deletion.push_back(it.first);
        }
    }
    if (!count_for_deletion) {
        throw std::runtime_error("No such task to delete!");
    }

    std::cout << count_for_deletion << " taks found to delete! " << std::endl;
    std::cout << "Are you shure ? (y/n) : ";
    char question;
    do {
        std::cin >> question;
        std::cin.ignore(1);
    } while (question != 'y' && question != 'n');
    if (question == 'n') { return; }

    for (auto& it : tmp_deletion) {
        if (task_ID.find(it) != task_ID.end()) {
            task_ID.erase(it);
        }
    }   
    this->data.save(this);
}


// Update function implementation
void TaskDB::update(std::vector<std::string>& rhs) {
    std::string spliter = " =";
    std::string to_pars = to_string(rhs);
    auto parsed = parser(to_pars, spliter);   
    
    // Finding "where" keyword 
    auto where_it = std::find_if(parsed.begin(), parsed.end(), [&] (const std::string& rhs) { return keyword_ht[rhs] == Task_Keyword::WHERE; });
    if (where_it == parsed.end()) {
        throw std::invalid_argument("Syntax ERROR!");
    }

    // Finding "set" keyword 
    auto set_it = std::find_if(parsed.begin(), parsed.end(), [&] (const std::string& rhs) { return keyword_ht[rhs] == Task_Keyword::SET; });

    // Thrwo if "set" is after "where"
    if (std::distance(set_it, where_it) < 3) {
        throw std::invalid_argument("Syntax ERROR!");
    }

    // Finding name if it is between "set" and "where" 
    auto between = std::find_if(set_it, where_it, [&] (const std::string& rhs) { return keyword_ht[rhs] == Task_Keyword::NAME; });
    Task for_change;


    // Initializing task object with name if it is between "set" and "where"
    if (between != where_it) {
        auto pars_for_change_name = parser(to_pars, "\"");
        for_change.m_name = pars_for_change_name[1];
        this->flag_name = true;
    }

    // Initialing flags true 
    for (auto it = set_it; it != where_it; ++it) {
        if (static_cast<bool>(keyword_ht[*it])) {
            this->flag_user = (keyword_ht[*it] == Task_Keyword::USER) ? true : flag_user;
            this->flag_deadline = (keyword_ht[*it] == Task_Keyword::DEADLINE) ? true: flag_deadline;
        }
    }

    // Initializing task object with values
    for (auto it = set_it + 1; it != where_it; ++it) {
        if (keyword_ht[*it] == Task_Keyword::NAME) {
            it += 2;
            while (it->find("\"") == std::string::npos) {
                ++it;
            }
            if (++it == where_it) { break; }
        }
        if (static_cast<bool>(keyword_ht[*it])) {
            if (keyword_ht[*it] == Task_Keyword::CREATED) { throw std::invalid_argument("Syntax ERROR!"); }
            for_change.deadline = (keyword_ht[*it] == Task_Keyword::DEADLINE) ? *(it + 1) : for_change.deadline;
            for_change.user_email = (keyword_ht[*it] == Task_Keyword::USER) ? *(it + 1) : for_change.user_email;
        }
    }


    // Finding name if it is befor "where" keyword
    Task tmp;
    auto after_where = std::find_if(where_it, parsed.end(), [&] (const std::string& rhs) { return keyword_ht[rhs] == Task_Keyword::NAME; });
    if (after_where != parsed.end()) {
        std::vector<std::string> tmp_vec(where_it + 1, parsed.end());
        std::string to_pars_after_where = to_string(tmp_vec);
        auto vec = parser(to_pars_after_where, "\"");
        if (std::find_if(task_ID.begin(), task_ID.end(), [&] (auto args) {
            return args.second->m_name == vec[1];
        }) == task_ID.end()) {
            throw std::invalid_argument("No such task!");
        }
        tmp.m_name = vec[1];
    }

    // Initializng temprorary object for comparing 
    // objects in DB
    for (auto it = where_it; it != parsed.end(); ++it) {
        if (keyword_ht[*it] == Task_Keyword::NAME) {
            it += 2;
            while (it->find("\"") == std::string::npos) {
                ++it;
            }
            if (++it == parsed.end()) { break; }
        }
        tmp.user_email = (keyword_ht[*it] == Task_Keyword::USER) ? *(it + 1) : tmp.user_email;
        tmp.deadline = (keyword_ht[*it] == Task_Keyword::DEADLINE) ? *(it + 1) : tmp.deadline;   
    }
    
    // Selectiong flags for comparing objects
    // tmp and DB
    this->set_for_select(tmp);
    
    // Comparing and changing object 
    // field values
    for (auto& it : task_ID) {
        if (for_selection_unique(tmp, it.second)) {
            if (flag_name) {
                it.second->m_name = for_change.m_name;
            }
            if (flag_user) {
                // Throwing if no such user in DB
                if (!user->find_email(for_change.user_email)) { throw std::invalid_argument("No user found!"); }
                it.second->user_email = for_change.user_email;
            }
            if (flag_deadline) {
                it.second->deadline = for_change.deadline;
            }
        }
    }
    // Redifidng all flags false for future changes
    this->redefine();
    this->data.save(this);
}

void TaskDB::add(std::vector<std::string>&)  {
    throw std::invalid_argument("Can't add to task!");
}


// Set all flags false
void TaskDB::redefine() {
    this->flag_name = false;
    this->flag_created = false;
    this->flag_deadline = false;
    this->flag_user = false;
}

void TaskDB::set_for_select(const Task& rhs) {
    this->for_select["name"] = rhs.m_name.empty() ? false : true;
    this->for_select["user"] = rhs.user_email.empty() ? false : true;
    this->for_select["created"] = rhs.created.is_empty() ? false : true;
    this->for_select["deadline"] = rhs.deadline.is_empty() ? false : true; 
}

bool TaskDB::for_selection_unique( Task& lhs,  Task* rhs) {
    if (for_select["name"]) {
        if (lhs.m_name != rhs->m_name) { return false; } 
    }
    if (for_select["user"]) {
        if (lhs.user_email != rhs->user_email) { return false; } 
    }
    if (for_select["created"]) {
        if (lhs.created != rhs->created) { return false; } 
    }
    if (for_select["deadline"]) {
        if (lhs.deadline != rhs->deadline) { return false; }
    }
    return true;
}

// Put inserted data to file
void TaskDB::to_file() {
    task_file.open("DB/task.data", std::ios::out);
    if (!task_file.is_open()) {
        throw std::invalid_argument("Unable to open DB!");
    }

    for (auto& it : task_ID) {
        task_file << std::endl <<"{" << std::endl;
        task_file << "  ID : "<< it.first << std::endl;
        task_file << "  name : "<< task_ID[it.first]->m_name << std::endl;
        task_file << "  user : " << task_ID[it.first]->user_email << std::endl;
        task_file << "  created : "<< static_cast<unsigned>(task_ID[it.first]->created.day) 
                            << "/" << static_cast<unsigned>(task_ID[it.first]->created.month) 
                            << "/" << static_cast<int>(task_ID[it.first]->created.year) << std::endl;
        task_file << "  Deadline : " << task_ID[it.first]->deadline.day << "/" 
                            << task_ID[it.first]->deadline.month << "/"
                            << task_ID[it.first]->deadline.year << std::endl;
        task_file << "}";
    }
    task_file.close();
    if (task_file.is_open()) {
        std::cout << "Failed to close the file" << std::endl;
    }
}


// Check for deadline validity
bool TaskDB::isValidDeadline(const Date& rhs) { 
    if (rhs.day > 31) { throw std::invalid_argument("Wrong date");}
    if (rhs.month > 12) { throw std::invalid_argument("Wrong date");}
    return true;
}


void TaskDB::task_data_check(Task* rhs) {
    if (rhs->m_name.empty()) {
        throw std::invalid_argument("Syntax ERROR");
    }
    if (isValidDeadline(rhs->deadline)) {
        if (rhs->deadline.year < (rhs->created.year)) {
            throw std::invalid_argument("Invalid date");
        }
        if (rhs->deadline.year == (rhs->created.year)) {
            if (rhs->deadline.month < (rhs->created.month)) {
                throw std::invalid_argument("Invalid date");
            }
        }
        if (rhs->deadline.month == (rhs->created.month)) {
            if (rhs->deadline.day < (rhs->created.day)) {
                throw std::invalid_argument("Invalid date");
            }
        }
    }

}


int TaskDB::ID_for_project(const std::string& rhs) {
    for (auto& it : this->task_ID) {
        if (it.second->m_name == rhs) {
            return it.first;   
        }
    }
    throw std::invalid_argument("No such task!");
}


// TaskDB destructor
TaskDB::~TaskDB() {

    // Deallocate task_ID tasks' variables
    for (auto& it : task_ID) {
        delete it.second;
        it.second = nullptr;
    }
}

std::string TaskDB::return_name(const std::size_t& ind) {
    return this->task_ID[ind]->m_name;
}


