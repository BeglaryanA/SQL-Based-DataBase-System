#pragma once
#include <iterator>
#include "connect.h"
#include "dataSave.h"
#include "../pars.h"
#include <fstream>

// User 
struct User
{
    std::string m_name;
    std::string m_surname;
    std::string m_position;
    std::string m_email;
    std::string m_password;
    std::string m_phone_number;
    User() = default;
    User(const User&);
};


class UserDB : public Connect {
public:
    UserDB();
    UserDB(const UserDB&) {}
    ~UserDB();
    void insert(std::vector<std::string>& rhs) override;
    void select(std::vector<std::string>&) override;
    void delete_(std::vector<std::string>&) override; 
    void update(std::vector<std::string>&) override; 
    void add(std::vector<std::string>&);
    bool find_email(const std::string&);
    int ID_for_project(const std::string&);
    void to_file() override;
    std::string return_email(const std::size_t&);
private:
    void init();
    void user_data_check(User*) ;
    bool for_selection_unique(const User&, User*);
    bool isValidEmail(std::string rhs);
    bool isValidPhoneNumber(const std::string& rhs);
    void set_all_true();
    void set_for_select(const User&);
    void redifne();
    // void init_for_flags();
private:
    // Static ID count
    static std::size_t ID;

    // Connect user ID with user DATA
    std::map<std::size_t, User*> user_ID;
    
    // Connect user ID with user FILE POSITION
    std::map<std::size_t, std::streampos> ID_pos;//
    
    // To open user.data FILE
    std::fstream user_file;
    
    // DELETED users ID
    std::vector<std::size_t> deleted_ID;

    std::map<std::string, User_Keyword> keyword_ht;
    
    std::map<std::string, bool> for_select;
private:
    DataSave data;
private:
    //flag for select and update
    bool flag_name;
    bool flag_surname;
    bool flag_email;
    bool flag_phone_number;
    bool flag_position;
};