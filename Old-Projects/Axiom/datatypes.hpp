/*
 *  Axiom - an experimental programming language
 *
 *  Copyright (C) 2008-2012 Arjun G. Menon
 *  Released under the New BSD 3-Clause License
 *
 */

class dataType
{
public:
    virtual UserObjectClass fetch_member(const string &name) = 0;
    virtual UserObjectClass call_function(const UserObjectClass &arg) = 0;
    virtual const string string_repr() = 0; // string representation
};

class dataType_null : public dataType
{
private:
    string functional;

public:
    dataType_null();
    dataType_null(const dataType_null &copy);
    dataType_null(const string &fn_name);
    ~dataType_null();
    const string string_repr();

    UserObjectClass fetch_member(const string &name);
    UserObjectClass call_function(const UserObjectClass &arg);
};

class dataType_num : public dataType
{
private:
    double num_val;
    string functional;

public:
    dataType_num(const double &k);
    dataType_num(const dataType_num &copy);
    dataType_num(const string &fn_name, const double &k);
    ~dataType_num();
    double get_num() const;
    const string string_repr();

    UserObjectClass fetch_member(const string &name);
    UserObjectClass call_function(const UserObjectClass &arg);

    static double convertUserObject_toNumber(const UserObjectClass &obj);
};

class dataType_str : public dataType
{
private:
    string str_val, functional;

public:
    dataType_str(const string &k);
    dataType_str(const dataType_str &copy);
    dataType_str(const string &fn_name, const string &k);
    ~dataType_str();
    string get_str() const;
    const string string_repr();

    UserObjectClass fetch_member(const string &name);
    UserObjectClass call_function(const UserObjectClass &arg);
};

class dataType_pair : public dataType
{
private:
    UserObjectClass first;
    UserObjectClass second;

public:
    dataType_pair(const UserObjectClass &a, const UserObjectClass &b);
    dataType_pair(const dataType_pair &copy);
    ~dataType_pair();
    const string string_repr();

    UserObjectClass fetch_member(const string &name);
    UserObjectClass call_function(const UserObjectClass &arg);
};

class dataType_list : public dataType
{
private:
    list<UserObjectClass> mylist;

public:
    dataType_list();
    ~dataType_list();
    const string string_repr();

    UserObjectClass fetch_member(const string &name);
    UserObjectClass call_function(const UserObjectClass &arg);

    void push_back(const UserObjectClass &obj);
};

#define DATATYPE_null 11
#define DATATYPE_num  22
#define DATATYPE_str  44
#define DATATYPE_pair 66
#define DATATYPE_list 88

class memberNotFoundException : public userError
{
    public : memberNotFoundException(string method_name, string obj_name="Object") : 
    userError(obj_name + " does not have a member called \""+method_name+"\".") {}; 
};

class functionNotFoundException : public fatalError
{
    public : functionNotFoundException(string name) : 
    fatalError("Non-existant function \""+name+"\" invoked.") {};
};

