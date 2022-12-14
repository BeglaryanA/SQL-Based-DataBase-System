#include "../header/user.h"
#include <iostream>


std::size_t UserDB::ID = 0;

User::User(const User& rhs) {
    this->m_name = rhs.m_name;
    this->m_surname = rhs.m_surname;
    this->m_position = rhs.m_position;
    this->m_email = rhs.m_email;
    this->m_phone_number = rhs.m_phone_number;
    this->m_password = rhs.m_password;
}

void UserDB::init() {
    this->keyword_ht["name"] = User_Keyword::NAME;
    this->keyword_ht["surname"] = User_Keyword::SURNAME;
    this->keyword_ht["position"] = User_Keyword::POSITION;
    this->keyword_ht["email"] = User_Keyword::EMAIL;
    this->keyword_ht["phone_number"] = User_Keyword::PHONE_NUMBER;
    this->keyword_ht["where"] = User_Keyword::WHERE;
    this->keyword_ht["set"] = User_Keyword::SET;
    this->keyword_ht["all"] = User_Keyword::ALL;

    //map initialization for selection 
    this->for_select["name"] = false;
    this->for_select["surname"] = false;
    this->for_select["position"] = false;
    this->for_select["email"] = false;
    this->for_select["phone_number"] = false;
    
    //set all flag false
    this->flag_name = false;
    this->flag_surname = false;
    this->flag_email = false;
    this->flag_phone_number = false;
    this->flag_position = false;
}

// User constructor
UserDB::UserDB() {

    // Opening user.data file
    user_file.open("DB/user.data");
    if (!user_file.is_open()) {
        std::cout << "SETE"<< std::endl;
        throw std::runtime_error("Faild to open file!");
    }
    this->init();

    std::string line = "";
    std::string tmp = "";
    while (!user_file.eof()) {
        std::getline(user_file, line);
        if (line.empty()) { continue; }
        if ((line.find("ID") != std::string::npos)) {
            tmp += pars_data(line);
            if (std::find(deleted_ID.begin(), deleted_ID.end(), std::stoi(tmp)) != deleted_ID.end()) { continue; }
            ID_pos.insert(std::make_pair(std::stoi(tmp), user_file.tellg()));
            user_ID[std::stoi(tmp)] = new User;
        }
        tmp.clear();
        line.clear();
    }
    if (user_ID.empty()) { 
        user_file.close();
        if (user_file.is_open()) {
            throw std::runtime_error("Failed to close the file");
        }
        return; 
    }
    auto it_end = --user_ID.end();
    ID = it_end->first + 1;
    std::string object_data = "";
    for (auto& it : ID_pos) {

        // User name
        getline(user_file.seekg(it.second), object_data);
        user_ID[it.first]->m_name = pars_data(object_data);
        // User Surename
        getline(user_file, object_data);
        user_ID[it.first]->m_surname = pars_data(object_data);
        // User position
        getline(user_file, object_data);
        user_ID[it.first]->m_position = pars_data(object_data);
        // User email
        getline(user_file, object_data);
        user_ID[it.first]->m_email = pars_data(object_data);
        // User phone_number
        getline(user_file, object_data);
        user_ID[it.first]->m_phone_number = pars_data(object_data);
    }
    user_file.close();
    if (user_file.is_open()) {
        throw std::runtime_error("Failed to close the file");
    }
}





// Insert function implementation
void UserDB::insert(std::vector<std::string>& rhs) {
    std::string split = " =";
    std::string to_pars = to_string(rhs);
    auto parsed = parser(to_pars, split);
    std::size_t count_of_data = 0;
    User tmp;
    for (auto it = parsed.begin(); it != parsed.end(); ++it) {
        if (static_cast<bool>(keyword_ht[*it])) {
            tmp.m_name = (keyword_ht[*it] == User_Keyword::NAME) ? *(it + 1) : tmp.m_name;
            tmp.m_surname = (keyword_ht[*it] == User_Keyword::SURNAME) ? *(it + 1) : tmp.m_surname;
            tmp.m_position = (keyword_ht[*it] == User_Keyword::POSITION) ? *(it + 1) : tmp.m_position;
            tmp.m_email = (keyword_ht[*it] == User_Keyword::EMAIL) ? (*(it + 1)) : tmp.m_email;
            tmp.m_phone_number = (keyword_ht[*it] == User_Keyword::PHONE_NUMBER) ? *(it + 1) : tmp.m_phone_number;
            ++it;
        }
    }
    user_data_check(&tmp);

    user_ID[ID] = new User(tmp);

    // Opening user.data file to insert user
    ++ID;
    this->data.save(this);
}


// Delete function implementation
void UserDB::delete_(std::vector<std::string>& rhs) {

    // Polishing string
    std::string split = " =";
    std::string to_pars = to_string(rhs);
    auto parsed = parser(to_pars, split);
    
    // Finding keyword position
    auto where_it = std::find_if(parsed.begin(), parsed.end(),[&](const std::string& rhs) {return keyword_ht[rhs] == User_Keyword::WHERE; });
    
    if (where_it == parsed.end()) {
        throw std::invalid_argument("Syntax ERROR!");
    }

    ++where_it;
    std::size_t count_for_deletion = 0;
    std::vector<std::size_t> tmp_deletion;
    User tmp;
    while (where_it < parsed.end() - 1) {
        if (!static_cast<bool>(keyword_ht[*where_it])) { 
            throw std::invalid_argument("Invalid keyword!");
        }
        tmp.m_name = (keyword_ht[*where_it] == User_Keyword::NAME) ? *(where_it + 1) : tmp.m_name;
        tmp.m_surname = (keyword_ht[*where_it] == User_Keyword::SURNAME) ? *(where_it + 1) : tmp.m_surname;
        tmp.m_position = (keyword_ht[*where_it] == User_Keyword::POSITION) ? *(where_it + 1) : tmp.m_position;
        tmp.m_email = (keyword_ht[*where_it] == User_Keyword::EMAIL) ? (*(where_it + 1)) : tmp.m_email;//check TODO
        tmp.m_phone_number = (keyword_ht[*where_it] == User_Keyword::PHONE_NUMBER) ? *(where_it + 1) : tmp.m_phone_number;//check TODO
        where_it += 2;
    }
    
    this->set_for_select(tmp);    

    for (auto& it : user_ID) {
        if (for_selection_unique(tmp, it.second)) {
            std::cout << "name : " << it.second->m_name << std::endl;
            std::cout << "surname : " << it.second->m_surname << std::endl;
            std::cout << "email : " << it.second->m_email << std::endl;
            std::cout << "position : " << it.second->m_position << std::endl;
            std::cout << "phone_number : " << it.second->m_phone_number << std::endl;
            std::cout << std::endl;
            ++count_for_deletion;
            tmp_deletion.push_back(it.first);
        }
    }
    if (!count_for_deletion) {
        throw std::runtime_error("No such user to delete!");
    }

    std::cout <<  count_for_deletion << " users found to delete!" << std::endl;
    std::cout << "Are you shure ? (y/n) : ";
    char question;



    do {
        std::cin >> question;
        std::cin.ignore(1);
    } while ((question != 'y' && question != 'n'));
    if (question == 'n') { return; }

    deleted_ID.insert(deleted_ID.end(), tmp_deletion.begin(), tmp_deletion.end());
    for (auto it : deleted_ID) {
        if (user_ID.find(it) != user_ID.end()) {
            user_ID.erase(it);
        }
    }

    this->data.save(this);
}

// Select function implementation
void UserDB::select(std::vector<std::string>& rhs) {
    
    // Polishing string
    std::string spliter = " =";
    std::string to_pars = to_string(rhs);
    auto parsed = parser(to_pars, spliter);

    // Finding keyword positions
    auto all_it = std::find_if(parsed.begin(), parsed.end(),[&](const std::string& rhs) {return keyword_ht[rhs] == User_Keyword::ALL; });
    auto where_it = std::find_if(parsed.begin(), parsed.end(),[&](const std::string& rhs) {return keyword_ht[rhs] == User_Keyword::WHERE; });
    
    if (std::distance(all_it, where_it) != 1 && all_it != parsed.end()) {
        throw std::invalid_argument("Invalid syntax!");
    }
    // 
    for (auto it = parsed.begin(); it != where_it; ++it) {
        if (static_cast<bool>(keyword_ht[*it])) {
            this->flag_name = (keyword_ht[*it] == User_Keyword::NAME) ? true: flag_name;
            this->flag_surname = (keyword_ht[*it] == User_Keyword::SURNAME) ? true : flag_surname;
            this->flag_position = (keyword_ht[*it] == User_Keyword::POSITION) ? true: flag_position;
            this->flag_email = (keyword_ht[*it] == User_Keyword::EMAIL) ? true: flag_email;
            this->flag_phone_number = (keyword_ht[*it] == User_Keyword::PHONE_NUMBER) ? true: flag_phone_number;
        } 
        if (keyword_ht[*it] == User_Keyword::ALL) {
            this->set_all_true();
            break;
        }
    }


    ++where_it;
    User tmp;
    while (where_it < parsed.end() - 1) {
        if (!static_cast<bool>(keyword_ht[*where_it])) { 
            throw std::invalid_argument("Invalid keyword!");
        }
        tmp.m_name = (keyword_ht[*where_it] == User_Keyword::NAME) ? *(where_it + 1) : tmp.m_name;
        tmp.m_surname = (keyword_ht[*where_it] == User_Keyword::SURNAME) ? *(where_it + 1) : tmp.m_surname;
        tmp.m_position = (keyword_ht[*where_it] == User_Keyword::POSITION) ? *(where_it + 1) : tmp.m_position;
        tmp.m_email = (keyword_ht[*where_it] == User_Keyword::EMAIL) ? (*(where_it + 1)) : tmp.m_email;//check TODO
        tmp.m_phone_number = (keyword_ht[*where_it] == User_Keyword::PHONE_NUMBER) ? *(where_it + 1) : tmp.m_phone_number;//check TODO
        where_it += 2;
    }

    this->set_for_select(tmp);    

    for (auto& it : user_ID) {
        if (for_selection_unique(tmp, it.second)) {
            if (flag_name) {
                std::cout << "name : " << it.second->m_name << std::endl;
            }
            if (flag_surname) {
                std::cout << "surname : " << it.second->m_surname << std::endl;
            }
            if (flag_email) {
                std::cout << "email : " << it.second->m_email << std::endl;
            }
            if (flag_position) {
                std::cout << "position : " << it.second->m_position << std::endl;
            }
            if (flag_phone_number) {
                std::cout << "phone_number : " << it.second->m_phone_number << std::endl;
            }
        }
    }
    // Set all flags false
    this->redifne();
}


// Update function implementation
void UserDB::update(std::vector<std::string>& rhs) {
    std::string spliter = " =";
    std::string to_pars = to_string(rhs);
    auto parsed = parser(to_pars, spliter);

    auto where_it = std::find_if(parsed.begin(), parsed.end(), [&](const std::string& rhs) {return keyword_ht[rhs] == User_Keyword::WHERE; });
    
    if (where_it == parsed.end()) {
        throw std::invalid_argument("Syntax ERROR!");
    }
    
    auto set_it = std::find_if(parsed.begin(), parsed.end(), [&](const std::string& rhs) {return keyword_ht[rhs] == User_Keyword::SET; });

    if (std::distance(set_it, where_it) < 3) {
        throw std::invalid_argument("Syntax ERROR");
    }
    
    for (auto it = set_it; it != where_it; ++it) {
        if (static_cast<bool>(keyword_ht[*it])) {
            this->flag_name = (keyword_ht[*it] == User_Keyword::NAME) ? true: flag_name;
            this->flag_surname = (keyword_ht[*it] == User_Keyword::SURNAME) ? true : flag_surname;
            this->flag_position = (keyword_ht[*it] == User_Keyword::POSITION) ? true: flag_position;
            this->flag_email = (keyword_ht[*it] == User_Keyword::EMAIL) ? true: flag_email;
            this->flag_phone_number = (keyword_ht[*it] == User_Keyword::PHONE_NUMBER) ? true: flag_phone_number;
        }
    }
    User for_change;
    for (auto it = set_it + 1; it != where_it; ++it) {
        if (static_cast<bool>(keyword_ht[*it])) {
            for_change.m_name = (keyword_ht[*it] == User_Keyword::NAME) ? *(it + 1) : for_change.m_name;
            for_change.m_surname = (keyword_ht[*it] == User_Keyword::SURNAME) ? *(it + 1) : for_change.m_surname;
            for_change.m_position = (keyword_ht[*it] == User_Keyword::POSITION) ? *(it + 1) : for_change.m_position;
            for_change.m_email = (keyword_ht[*it] == User_Keyword::EMAIL) ? (*(it + 1)) : for_change.m_email;//check TODO
            for_change.m_phone_number = (keyword_ht[*it] == User_Keyword::PHONE_NUMBER) ? *(it + 1) : for_change.m_phone_number;//check TODO
        }
    }
    if (!for_change.m_email.empty()) {
        if (!isValidEmail(for_change.m_email)) { throw std::invalid_argument("Wrong email or email is used!");} 
    }
    if (!for_change.m_phone_number.empty()) {
        if (!isValidPhoneNumber(for_change.m_phone_number)) { throw std::invalid_argument("Wrong phone number or phone number is used!");} 
    }
    ++where_it;
    User tmp;
    while (where_it < parsed.end() - 1) {
        if (!static_cast<bool>(keyword_ht[*where_it])) { 
            throw std::invalid_argument("Invalid keyword!");
        }
        tmp.m_name = (keyword_ht[*where_it] == User_Keyword::NAME) ? *(where_it + 1) : tmp.m_name;
        tmp.m_surname = (keyword_ht[*where_it] == User_Keyword::SURNAME) ? *(where_it + 1) : tmp.m_surname;
        tmp.m_position = (keyword_ht[*where_it] == User_Keyword::POSITION) ? *(where_it + 1) : tmp.m_position;
        tmp.m_email = (keyword_ht[*where_it] == User_Keyword::EMAIL) ? (*(where_it + 1)) : tmp.m_email;//check TODO
        tmp.m_phone_number = (keyword_ht[*where_it] == User_Keyword::PHONE_NUMBER) ? *(where_it + 1) : tmp.m_phone_number;//check TODO
        where_it += 2;
    }
    this->set_for_select(tmp);    

    std::string str = "";
    for (auto& it : user_ID) {
        if (for_selection_unique(tmp, it.second)) {
            if (flag_name) {
                it.second->m_name = for_change.m_name;
            }
            if (flag_surname) {
                it.second->m_surname = for_change.m_surname;
            }
            if (flag_email) {
                it.second->m_email = for_change.m_email;
            }
            if (flag_position) {
                it.second->m_position = for_change.m_position;
            }
            if (flag_phone_number) {
                it.second->m_phone_number = for_change.m_phone_number;
            }
        }
    }

    this->redifne();

    this->data.save(this);
}

void UserDB::add(std::vector<std::string>&) {
    throw std::invalid_argument("Can't add to users!");
}

// Check for data validation
void UserDB::user_data_check(User* user)  {
    if (user->m_name.empty()) { 
        throw std::invalid_argument("Syntax ERROR"); 
    }
    if (user->m_surname.empty()) { 
        throw std::invalid_argument("Syntax ERROR!"); 
    }
    if (user->m_position.empty()) {
        user->m_position = "NIL";
    }
    if (!isValidEmail(user->m_email)) {
        throw std::invalid_argument("Wrong email!"); 
    }
    if (!isValidPhoneNumber(user->m_phone_number)) {
        throw std::invalid_argument("Wrong phone number!"); 
    }
}


// Check for email validity
bool UserDB::isValidEmail(std::string rhs) {
    if (rhs.empty()) { return false; }
    auto unique = std::find_if(user_ID.begin(), user_ID.end(), [&](std::pair<std::size_t, User*> for_check) { return for_check.second->m_email == rhs;});
    if (unique != user_ID.end()) { 
        throw std::invalid_argument("This email is used!");
    }
    std::size_t at_signe_ind = rhs.find('@');

    if (at_signe_ind == std::string::npos) { return false; }
    std::string to_check = rhs.substr(at_signe_ind, rhs.size() - at_signe_ind);
    
    for (int i = 0; i < at_signe_ind; ++i) {
        if (!((rhs[i] >= 'a' && rhs[i] <= 'z') || (rhs[i] >= 'A' && rhs[i] <= 'Z') || (rhs[i] == 46) || (rhs[i] == 95) || (rhs[i] >= '0' && rhs[i] <= '9'))) {
            return false;
        }
    } 
    if ((to_check == "@gmail.com") || (to_check == "@mail.ru")) {
        return true;
    }
    return false;
}


// Check for phone_number validity
bool UserDB::isValidPhoneNumber(const std::string& rhs) {
    if (rhs.empty()) { return false;}
    auto unique = std::find_if(user_ID.begin(), user_ID.end(), [&](std::pair<std::size_t, User*> for_check) {   return for_check.second->m_phone_number == rhs; } );

    if (unique != user_ID.end()) {
        throw std::invalid_argument("This phone number is used!");
    }
    
    for (auto& it : rhs) {
        if (!(it >= '0' && it <= '9')) {
            return false;
        }
    }
    return true;
}


bool UserDB::for_selection_unique(const User& lhs, User* rhs) {
    
    if (for_select["name"]) {
        if (lhs.m_name != rhs->m_name) { return false; }
    }
    if (for_select["surname"]) {
        if (lhs.m_surname != rhs->m_surname) { return false; }
    }
    if (for_select["position"]) {
        if (lhs.m_position != rhs->m_position) { return false; }
    }
    if (for_select["email"]) {
        if (lhs.m_email != rhs->m_email) { return false; }
    }
    if (for_select["phone_number"]) {
        if (lhs.m_phone_number != rhs->m_phone_number) { return false; }
    }
    return true;
}

void UserDB::set_all_true() {
    this->flag_name = true;
    this->flag_surname = true;
    this->flag_email = true;
    this->flag_phone_number = true;
    this->flag_position = true;
}

void UserDB::redifne() {
    this->flag_name = false;
    this->flag_surname = false;
    this->flag_email = false;
    this->flag_phone_number = false;
    this->flag_position = false;
}

void UserDB::set_for_select(const User& tmp) {
    this->for_select["name"] = tmp.m_name.empty() ? false : true;
    this->for_select["surname"] = tmp.m_surname.empty() ? false : true;
    this->for_select["position"] = tmp.m_position.empty() ? false : true;
    this->for_select["email"] = tmp.m_email.empty() ? false : true;
    this->for_select["phone_number"] = tmp.m_phone_number.empty() ? false : true;
}

//Dtor
void UserDB::to_file() {
    user_file.open("DB/user.data", std::ios::out);
    if (!user_file.is_open()) {
        throw std::invalid_argument("Unable to open DB!");
    }
    for (auto& it : user_ID) {
        user_file << std::endl <<"{" << std::endl;
        user_file << "  ID : "<< it.first << std::endl;
        user_file << "  name : "<< user_ID[it.first]->m_name << std::endl;
        user_file << "  surname : " << user_ID[it.first]->m_surname << std::endl;
        user_file << "  position : "<< user_ID[it.first]->m_position << std::endl;
        user_file << "  email : " << user_ID[it.first]->m_email  << std::endl;
        user_file << "  phone_number : "<< user_ID[it.first]->m_phone_number << std::endl;
        user_file << "}";
    }
    user_file.close();
    if (user_file.is_open()) {
        std::cout << "Failed to close the file" << std::endl;
    }
}


// UserDB destructor
UserDB::~UserDB() {

    // Deallocate user_ID users' variables
    for (auto& it : user_ID) {
        delete it.second;
        it.second = nullptr;
    }
}


bool UserDB::find_email(const std::string& rhs) {
    return std::find_if(this->user_ID.begin(), this->user_ID.end(), 
                        [&](auto args) -> bool {
                            return (args.second->m_email == rhs);
                        }) != this->user_ID.end();
}

int  UserDB::ID_for_project(const std::string& rhs) {
    auto user_id_project = std::find_if(this->user_ID.begin(), this->user_ID.end(), 
                        [&](auto args) {
                            return (args.second->m_email == rhs);
                        });
    if (user_id_project == user_ID.end()) {
        throw std::invalid_argument("No such user!");
    }

    return user_id_project->first;
}

std::string UserDB::return_email(const std::size_t& ind) {
    return (user_ID[ind]->m_email);
}