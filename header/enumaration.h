#pragma once
#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <algorithm>
#include <iterator>


enum class Command
{
    INSERT = 1,
    SELECT,
    DELETE,
    UPDATE,
    ADD,
};

enum class DBFile
{
    USER = 1,
    TASK,
    PROJECT
};

enum class User_Keyword 
{
    NAME = 1,
    SURNAME,
    POSITION,
    EMAIL,
    PHONE_NUMBER,
    WHERE,
    SET,
    ALL,
};

enum class Task_Keyword 
{
    NAME = 1,
    USER, 
    CREATED,
    DEADLINE,
    WHERE,
    SET,
    ALL,
};

enum class Project_Keyword 
{
    NAME = 1,
    USER,
    CREATED,
    DEADLINE,
    TASK,
    DESCRIPTION,
    WHERE,
    SET,
    ALL,
};