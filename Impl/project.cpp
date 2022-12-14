#include </home/andranik/Desktop/MySQL/header/project.h>

int ProjectDB::ID = 0;

bool operator!=(const Date& lhs, const Date& rhs);
std::ostream& operator<<(std::ostream& os, const Date& rhs);


Project::Project()
: name{}
, description{}
, deadline{}
{
}

Project::Project(const Project& rhs) {
    this->name = rhs.name;
    this->user = rhs.user;
    this->description = rhs.description;
    this->task = rhs.task;
    this->created = rhs.created;
    this->deadline = rhs.deadline;
}

ProjectDB::ProjectDB(TaskDB* ts, UserDB* us) 
: user{us}
, task{ts}
{
    project_file.open("DB/project.data");
    if (!project_file.is_open()) {
        throw std::runtime_error("Faild to open file!");
    }
    this->init();
    std::string line = "";
    std::string tmp = "";
    while (!project_file.eof()) {
        std::getline(project_file, line);
        if (line.empty()) { continue; }
        if ((line.find("ID") != std::string::npos)) {
            tmp += pars_data(line);

            ID_pos.insert(std::make_pair(std::stoi(tmp), project_file.tellg()));

            project_ID[std::stoi(tmp)] = new Project;
        }
        line.clear();
        tmp.clear();
    }
    if (project_ID.empty()) {
        project_file.close();
        if (project_file.is_open()) {
            throw std::runtime_error("Fail to close file!");
        }
        return;
    }
    
    auto it_end = --project_ID.end();
    ID = it_end->first + 1;
    std::string object_data = "";
    for (auto& it : ID_pos) {

        std::getline(project_file.seekg(it.second), object_data);
        project_ID[it.first]->name = pars_data(object_data);

        std::getline(project_file, object_data);
        auto user_id_vec = parser(pars_data(object_data), "|");
        
        for (auto& it_in_user : user_id_vec) {
            if (it_in_user.empty()) {
                continue;
            }
            project_ID[it.first]->user.push_back(std::stoi(it_in_user));
        }

        std::getline(project_file, object_data);
        project_ID[it.first]->created = pars_data(object_data);

        std::getline(project_file, object_data);
        project_ID[it.first]->deadline = pars_data(object_data);

        std::getline(project_file, object_data);
        auto task_id_vec = parser(pars_data(object_data), "|");

        for (auto& it_in_task : task_id_vec) {
            if (it_in_task.empty()) {
                continue;
            }
            project_ID[it.first]->task.push_back(std::stoi(it_in_task));
        }

        std::getline(project_file, object_data);
        project_ID[it.first]->description = pars_data(object_data);
    }

    project_file.close();
    if (project_file.is_open()) {
        throw std::runtime_error("Faild to close file!");
    }
}

void ProjectDB::init() {
    // Keywords
    keyword_ht["name"] = Project_Keyword::NAME;
    keyword_ht["description"] = Project_Keyword::DESCRIPTION;
    keyword_ht["deadline"] = Project_Keyword::DEADLINE;
    keyword_ht["user"] = Project_Keyword::USER;
    keyword_ht["task"] = Project_Keyword::TASK;
    keyword_ht["created"] = Project_Keyword::CREATED;
    keyword_ht["where"] = Project_Keyword::WHERE;
    keyword_ht["all"] = Project_Keyword::ALL;
    keyword_ht["set"] = Project_Keyword::SET;


    // for selection
    this->for_select["name"] = false;
    this->for_select["description"] = false;
    this->for_select["user"] = false;
    this->for_select["task"] = false;
    this->for_select["deadline"] = false;

    // Flags
    this->flag_name = false;
    this->flag_description = false;
    this->flag_deadline = false;
    this->flag_task = false;
    this->flag_user = false;
    this->flag_created = false;
}



void ProjectDB::insert(std::vector<std::string>& rhs) {
    std::string to_pars = to_string(rhs);
    auto tmp_pars = parser(to_pars, "=, ");
    //
    auto task_it = std::find_if(tmp_pars.begin(), tmp_pars.end(), [&] (const std::string& rhs) {
        return keyword_ht[rhs] == Project_Keyword::TASK;
    });

    Project tmp;
    if (task_it != tmp_pars.end()) {
        auto task_names_it = task_it + 1;
        while (!static_cast<bool>(keyword_ht[*task_names_it])) {
            if (task_names_it == tmp_pars.end() - 2) { break;}
            std::string str = *(task_names_it + 1);
            if (str[0] == '\"') {
                while (str[str.size() - 1] != '\"') {
                    str = *(task_names_it + 1);
                    ++task_names_it;
                }
                continue;
            }
            if (static_cast<bool>(keyword_ht[*task_names_it])) {
                break;
            }
            ++task_names_it;

        }
        std::vector<std::string> vec (task_it + 1, task_names_it);

        std::string for_task = to_string(vec);
        auto task_parsed = parser(for_task, "\"");

        for (auto& it : task_parsed) {
            if (it == " ") {
                continue;
            }
            tmp.task.push_back(task->ID_for_project(it));    
        }
    }
    auto parsed = parser(to_pars, "\"");
    
    for (auto it = parsed.begin(); it < parsed.end(); it += 2) {
        auto tmp_parsed = parser(*it, "= ");
        for (auto tmp_it = tmp_parsed.begin(); tmp_it < tmp_parsed.end(); ++tmp_it) {
            
            if (keyword_ht[*(tmp_it)] == Project_Keyword::USER) {
                ++tmp_it;
                while ((tmp_it < tmp_parsed.end() - 1)) {
                    if (static_cast<bool>(keyword_ht[*tmp_it])) {
                        break;

                    }
                    tmp.user.push_back(user->ID_for_project(*tmp_it));
                    ++tmp_it;
                }
            }
            tmp.name = (keyword_ht[*tmp_it] == Project_Keyword::NAME) ? *(it + 1) : tmp.name;
            tmp.description = (keyword_ht[*tmp_it] == Project_Keyword::DESCRIPTION) ? *(it + 1) : tmp.description;
            tmp.deadline = (keyword_ht[*tmp_it] == Project_Keyword::DEADLINE) ? *(tmp_it + 1) : tmp.deadline;
        }
    }
    const std::chrono::time_point now(std::chrono::system_clock::now());
    std::chrono::year_month_day ymd(std::chrono::floor<std::chrono::days>(now));
    tmp.created.day = static_cast<unsigned>(ymd.day()); 
    tmp.created.month = static_cast<unsigned>(ymd.month()); 
    tmp.created.year = static_cast<int>(ymd.year());

    this->project_data_check(&tmp);
    project_ID[ID] = new Project(tmp);
    ++ID;
    this->data.save(this);
}


void ProjectDB::select(std::vector<std::string>& rhs) {
    std::string to_pars = to_string(rhs);
    auto parsed = parser(to_pars, "= ");

    auto where_it = std::find_if(parsed.begin(), parsed.end(), [&] (const std::string& rhs) { return keyword_ht[rhs] == Project_Keyword::WHERE; });
    if (where_it == parsed.end()) {
        throw std::invalid_argument("Syntax ERROR!");
    }

    auto all_it = std::find_if(parsed.begin(), parsed.end(), [&] (const std::string& rhs) { return keyword_ht[rhs] == Project_Keyword::ALL; });
    
    if (std::distance(all_it, where_it) != 1 && all_it != parsed.end()) {
        throw std::invalid_argument("Syntax ERROR!");
    }

    for (auto it = parsed.begin(); it != where_it; ++it) {
        if (static_cast<bool>(keyword_ht[*it])) {
            this->flag_name = (keyword_ht[*it] == Project_Keyword::NAME) ? true : flag_name;
            this->flag_deadline = (keyword_ht[*it] == Project_Keyword::DEADLINE) ? true : flag_deadline;
            this->flag_description = (keyword_ht[*it] == Project_Keyword::DESCRIPTION) ? true : flag_description;
            this->flag_user = (keyword_ht[*it] == Project_Keyword::USER) ? true : flag_user;
            this->flag_task = (keyword_ht[*it] == Project_Keyword::TASK) ? true : flag_task;

        }
        if (keyword_ht[*it] == Project_Keyword::ALL) {
            this->set_all_true();
            break;
        }
    }
    Project tmp;
    std::vector<std::string> vec(where_it + 1, parsed.end());
    std::string after_where = to_string(vec);
    auto after_where_vec = parser(after_where, "\"");
    for (auto it = after_where_vec.begin(); it < after_where_vec.end(); it += 2) {
        auto tmp_parsed = parser(*it, " =,");
        for (auto tmp_it = tmp_parsed.begin(); tmp_it != tmp_parsed.end(); ++tmp_it) {
            if (keyword_ht[*tmp_it] == Project_Keyword::USER) {
                ++tmp_it;
                while (tmp_it != tmp_parsed.end() - 1) {
                    if (static_cast<bool>(keyword_ht[*tmp_it])) { break; }
                    tmp.user.push_back(user->ID_for_project(*tmp_it));
                    ++tmp_it;
                }
            }
            tmp.name = (keyword_ht[*tmp_it] == Project_Keyword::NAME) ? *(it + 1) : tmp.name;
            tmp.description = (keyword_ht[*tmp_it] == Project_Keyword::DESCRIPTION) ? *(it + 1) : tmp.description;
            tmp.deadline = (keyword_ht[*tmp_it] == Project_Keyword::DEADLINE) ? *(tmp_it + 1) : tmp.deadline;
        }
    }

    this->set_for_select(tmp);
    for (auto& it : project_ID) {
        if (for_select_unique(tmp, it.second)) {
            if (flag_name) {
                std::cout << " name : " << it.second->name << std::endl;
            }
            if (flag_user) {
                std::cout << " user : ";
                for (auto& iter : it.second->user) {
                    std::cout << user->return_email(iter) << "  ";
                }
                std::cout << std::endl;
            }
            if (flag_created) {
                std::cout << " created : " << it.second->created << std::endl;
            }
            if (flag_deadline) {
                std::cout << "deadline : " << it.second->deadline << std::endl;
            }
            if (flag_task) {
                std::cout << " tasks : ";
                for (auto& iter : it.second->task) {
                    std::cout << task->return_name(iter) << "  ";
                }
                std::cout << std::endl;
            }
            if (flag_description) {
                std::cout << " description : " << it.second->description << std::endl;
            }
            std::cout << std::endl;
        }
    }
    this->redefine();
}

void ProjectDB::delete_(std::vector<std::string>& rhs) {

    std::string to_pars = to_string(rhs);
    auto parsed = parser(to_pars, " =");
    auto where_it = std::find_if(parsed.begin(), parsed.end(), [&] (const std::string& rhs) { return keyword_ht[rhs] == Project_Keyword::WHERE; });
    if (where_it == parsed.end()) {
        throw std::invalid_argument("Syntax ERROR!");
    }
    auto for_initializing = parser(to_pars, "\"");
    Project tmp;
    for (auto it = for_initializing.begin(); it < for_initializing.end(); it += 2) {
        auto tmp_parsed = parser(*it, " =,");
        for (auto tmp_it = tmp_parsed.begin(); tmp_it != tmp_parsed.end(); ++tmp_it) {
            if (keyword_ht[*tmp_it] == Project_Keyword::USER) {
                ++tmp_it;
                while (tmp_it != tmp_parsed.end() - 1) {
                    if (static_cast<bool>(keyword_ht[*tmp_it])) { break; }
                    tmp.user.push_back(user->ID_for_project(*tmp_it));
                    ++tmp_it;
                }
            }
            tmp.name = (keyword_ht[*tmp_it] == Project_Keyword::NAME) ? *(it + 1) : tmp.name;
            tmp.description = (keyword_ht[*tmp_it] == Project_Keyword::DESCRIPTION) ? *(it + 1) : tmp.description;
            tmp.deadline = (keyword_ht[*tmp_it] == Project_Keyword::DEADLINE) ? *(tmp_it + 1) : tmp.deadline;
        }
    }

    std::vector<std::size_t> id_for_deletion;
    int count_of_deletion = 0;
    this->set_for_select(tmp);
    for (auto& it : project_ID) {
        if (for_select_unique(tmp, it.second)) {
            std::cout << " name : " << it.second->name << std::endl;
            std::cout << " user : ";
            for (auto iter : it.second->user) {
                std::cout << user->return_email(iter) << "  ";
            }
            std::cout << std::endl;
            std::cout << " created : " << it.second->created << std::endl;
            std::cout << " deadline : " << it.second->deadline << std::endl;
            std::cout << " task : ";
            for (auto iter : it.second->task) {
                std::cout << task->return_name(iter) << " ";
            }
            std::cout << std::endl;
            std::cout << " description : " << it.second->description << std::endl;
            ++count_of_deletion;
            id_for_deletion.push_back(it.first);
        }
    }
    if (!count_of_deletion) {
        throw std::runtime_error("No such project to delete!");
    }
    std::cout << count_of_deletion << " project found to delete! " << std::endl;
    std::cout << "Are you shure ? (y/n) : ";
    char question;
    do {
        std::cin >> question;
        std::cin.ignore(1);
    } while (question != 'y' && question != 'n');

    if (question == 'n') { return; }

    for (auto& it : id_for_deletion) {
        if (project_ID.find(it) != project_ID.end()) {
            project_ID.erase(it);
        }
    }
    this->data.save(this);
}


void ProjectDB::update(std::vector<std::string>& rhs) {
    std::string to_pars = to_string(rhs);
    auto parsed = parser(to_pars, "= ");

    auto where_it = std::find_if(parsed.begin(), parsed.end(), [&] (const std::string& rhs) { return keyword_ht[rhs] == Project_Keyword::WHERE; });
    if (where_it == parsed.end()) { 
        throw std::invalid_argument("Syntax ERROR!");
    }
    
    auto set_it = std::find_if(parsed.begin(), parsed.end(), [&] (const std::string& rhs) { return keyword_ht[rhs] == Project_Keyword::SET; });
    if (set_it == parsed.end()) {
        throw std::invalid_argument("Syntax ERROR!");
    }
    if (std::distance(set_it, where_it) < 3) {
        throw std::invalid_argument("Syntax ERROR!");
    }
    Project for_change;
    std::vector<std::string> for_change_vec(set_it + 1, where_it);
    std::string for_change_str = to_string(for_change_vec);
    auto for_change_parsed = parser(for_change_str, "\"");
    
    for (auto it_out = for_change_parsed.begin(); it_out < for_change_parsed.end(); it_out += 2) {
        auto tmp_parsed = parser(*it_out, "=, ");
        for (auto it = tmp_parsed.begin(); it < tmp_parsed.end(); ++it) {
            if (keyword_ht[*it] == Project_Keyword::USER) {
                this->flag_user = true;
            }
            if (keyword_ht[*it] == Project_Keyword::CREATED) { throw std::invalid_argument("Syntax ERROR!"); }
            this->flag_name = (keyword_ht[*it] == Project_Keyword::NAME) ? true : flag_name; 
            this->flag_description = (keyword_ht[*it] == Project_Keyword::DESCRIPTION) ? true : flag_description; 
            this->flag_deadline = (keyword_ht[*it] == Project_Keyword::DEADLINE) ? true : flag_deadline; 

        }
    }

    for (auto it_out = for_change_parsed.begin(); it_out < for_change_parsed.end(); it_out += 2) {
        auto tmp_parsed = parser(*it_out, "=, ");
        for (auto it = tmp_parsed.begin(); it < tmp_parsed.end(); ++it) {
            if (keyword_ht[*it] == Project_Keyword::USER) {    
                ++it;       
                while (it != tmp_parsed.end() - 1) {
                    if (static_cast<bool>(keyword_ht[*it])) { break; }
                    for_change.user.push_back(user->ID_for_project(*(it + 1)));
                    ++it;
                }
            }
            for_change.deadline = (keyword_ht[*it] == Project_Keyword::DEADLINE) ? *(it + 1) : for_change.deadline; 
            for_change.name = (keyword_ht[*it] == Project_Keyword::NAME) ? *(it_out + 1) : for_change.name;
            for_change.description = (keyword_ht[*it] == Project_Keyword::DESCRIPTION) ? *(it_out + 1) : for_change.description; 
        }
    }
    std::vector<std::string> after_where(where_it + 1, parsed.end());
    std::string after_where_str = to_string(after_where);
    auto after_where_vec = parser(after_where_str, "\"");
    Project tmp;
    for (auto it_out = after_where_vec.begin(); it_out < after_where_vec.end(); it_out += 2) {
        auto tmp_parsed = parser(*it_out, "= ,");
        for (auto it = tmp_parsed.begin(); it != tmp_parsed.end(); ++it) {
            tmp.deadline = (keyword_ht[*it] == Project_Keyword::DEADLINE) ? *(it + 1) : tmp.deadline; 
            tmp.name = (keyword_ht[*it] == Project_Keyword::NAME) ? *(it_out + 1) : tmp.name;
            tmp.description = (keyword_ht[*it] == Project_Keyword::DESCRIPTION) ? *(it_out + 1) : tmp.description; 
        }
    }
    this->set_for_select(tmp);

    for (auto& it : project_ID) {
        if (for_select_unique(tmp, it.second)) {
            if (flag_name) {
                it.second->name = for_change.name;
            }
            if (flag_description) {
                it.second->description = for_change.description;
            }
            if (flag_user) {
                it.second->user = for_change.user;
            }
            if (flag_deadline) {
                it.second->deadline = for_change.deadline;
            }
        }
    }
    this->redefine();
}   


void ProjectDB::add(std::vector<std::string>& rhs) {
    std::string to_pars = to_string(rhs);
    auto parsed = parser(to_pars, " =,");

    auto where_it = std::find_if(parsed.begin(), parsed.end(), [&] (const std::string& rhs) { return keyword_ht[rhs] == Project_Keyword::WHERE; });
    if (where_it == parsed.end()) {
        throw std::invalid_argument("Syntax ERROR!");
    }

    auto task_it = std::find_if(parsed.begin(), parsed.end(), [&] (const std::string& rhs) { return keyword_ht[rhs] == Project_Keyword::TASK; });
    std::vector<std::size_t> for_task;
    if (task_it != parsed.end()) {
        this->flag_task = true;
        std::vector<std::string> task_vec(task_it, where_it - 1);
        std::string task_str = to_string(task_vec);
        auto task_parsed = parser(task_str, "\"");
        std::copy(task_parsed.begin(), task_parsed.end(), std::ostream_iterator<std::string>(std::cout, "|"));
        for (auto it = task_parsed.begin() + 1; it < task_parsed.end() - 1; ++it) {
            if (*it == " ") { continue; }
            for_task.push_back(this->task->ID_for_project(*it));
        }
    }

    auto user_it = std::find_if(parsed.begin(), parsed.end(), [&] (const std::string& rhs) { return keyword_ht[rhs] == Project_Keyword::USER; });
    std::vector<std::size_t> for_user;

    if ((task_it == parsed.end()) && (user_it == parsed.end())) {
        throw std::invalid_argument("Syntax ERROR!");
    }

    if (user_it != parsed.end()) {
        for_user.push_back(this->user->ID_for_project(*(user_it + 1)));
        this->flag_user = true;
    }
    std::vector<std::string> vec_tmp(where_it + 1, parsed.end());

    std::string after_where = to_string(vec_tmp);

    auto pars_for_check = parser(after_where, "\"");
    Project tmp;
    for (auto it_out = pars_for_check.begin(); it_out < pars_for_check.end() - 1; it_out += 2) {
        auto tmp_parsed = parser(*it_out, " =,");
        for (auto it = tmp_parsed.begin(); it != tmp_parsed.end(); ++it) {
            if (keyword_ht[*it] == Project_Keyword::NAME) {
                this->flag_name = true;
            }
            if (keyword_ht[*it] == Project_Keyword::DESCRIPTION) {
                this->flag_description = true;
            }
            tmp.name = (keyword_ht[*it] == Project_Keyword::NAME) ? *(it_out + 1) : tmp.name;
            tmp.description = (keyword_ht[*it] == Project_Keyword::DESCRIPTION) ? *(it_out + 1) : tmp.description;
        }
    }
    this->set_for_select(tmp);

    for (auto& it : project_ID) {
        if (for_select_unique(tmp, it.second)) {
            if (flag_task) {
                std::copy(for_task.begin(), for_task.end(), std::back_inserter(it.second->task));
            }
            if (flag_user) {
                std::copy(for_user.begin(), for_user.end(), std::back_inserter(it.second->user));
            }
        }
    }
    this->redefine();
    this->data.save(this);
}


void ProjectDB::redefine() {
    this->flag_name = false;
    this->flag_description = false;
    this->flag_deadline = false;
    this->flag_task = false;
    this->flag_user = false;
    this->flag_created = false;
}


bool ProjectDB::for_select_unique(const Project& lhs, Project* rhs) {
    if (for_select["name"]) {
        if (lhs.name != rhs->name) { return false; }
    }
    if (for_select["description"]) {
        if (lhs.description != rhs->description) { return false; }
    }
    if (for_select["user"]) {
        if (lhs.user != rhs->user) { return false; }
    }
    if (for_select["deadline"]) {
        if (lhs.deadline != rhs->deadline) { return false; }
    }
    return true;
}


void ProjectDB::set_for_select(const Project& rhs) {
    this->for_select["name"] = rhs.name.empty() ? false : true;
    this->for_select["description"] = rhs.description.empty()  ? false : true;
    this->for_select["user"] =  rhs.user.empty() ? false : true;
    this->for_select["task"] = rhs.task.empty() ? false : true;
    this->for_select["deadline"] = rhs.deadline.is_empty() ? false : true;
}

void ProjectDB::set_all_true() {
    this->flag_name = true;
    this->flag_description = true;
    this->flag_deadline = true;
    this->flag_task = true;
    this->flag_user = true;
    this->flag_created = true;
}

void ProjectDB::project_data_check(const Project* rhs) {
    if (rhs->name.empty()) {
        throw std::invalid_argument("Syntax ERROR!");
    }
    if (rhs->description.empty()) {
        throw std::invalid_argument("Syntax ERROR!");
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

// Check validation of deadline
bool ProjectDB::isValidDeadline(const Date& rhs) {
    if (rhs.day > 31) { throw std::invalid_argument("Wrong date");}
    if (rhs.month > 12) { throw std::invalid_argument("Wrong date");}
    return true;
}


void ProjectDB::to_file() {
    project_file.open("DB/project.data", std::ios::out);
    if (!project_file.is_open()) {
        throw std::runtime_error("Unable to open DB!");
    }
    
    for (auto& it_to_file : project_ID) {
        project_file << std::endl << "{" << std::endl;
        project_file << "  ID : " << it_to_file.first << std::endl;
        project_file << "  name : " << project_ID[it_to_file.first]->name << std::endl;
        project_file << "  users : ";
        for (auto& it : project_ID[it_to_file.first]->user) {
            project_file << (std::to_string(it)) << "|"; 
        }
        project_file << std::endl;
        project_file << "  created : " << project_ID[it_to_file.first]->created << std::endl;
        project_file << "  deadline : " << project_ID[it_to_file.first]->deadline << std::endl;
        project_file << "  tasks : ";
        for (auto& it : project_ID[it_to_file.first]->task) {
            project_file << (std::to_string(it)) << "|";
        }
        project_file << std::endl;
        project_file << "  description : " << project_ID[it_to_file.first]->description << std::endl;
        project_file << "}"; 
    }
    
    project_file.close();
    if (project_file.is_open()) {
        throw std::runtime_error("Faild to close file!");
    }
}

ProjectDB::~ProjectDB() {
    for (auto& it : project_ID) {
        delete it.second;
        it.second = nullptr;
    }
}