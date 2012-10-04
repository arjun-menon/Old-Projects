/*
 *  Axiom - an experimental programming language
 *
 *  Copyright (C) 2008-2012 Arjun G. Menon
 *  Released under the New BSD 3-Clause License
 *
 */

// datatypes prototypes
class dataType;
class dataType_null;
class dataType_num;
class dataType_str;
class dataType_pair;
class dataType_list;

class UserObjectClass
{
private:
    char dataTypeID;
    dataType *data;

public:
    char whatType() const;
    const dataType * get_data() const;

    UserObjectClass(const dataType_null &k_null);
    UserObjectClass(const dataType_num &k_num);
    UserObjectClass(const dataType_str &k_str);
    UserObjectClass(const dataType_pair &k_pair);
    UserObjectClass(const dataType_list &k_list);

    UserObjectClass(const UserObjectClass &copyFrom);
    UserObjectClass & operator=(const UserObjectClass &rhs);
    ~UserObjectClass();

    UserObjectClass fetchMember(const string &name) const;
    UserObjectClass callFunction(const UserObjectClass &arg) const;
    const string toString() const;

private:
    void copy(const UserObjectClass &obj);
    void clear();
};

class badUserObjectError : public fatalError
{
    public: badUserObjectError() : 
    fatalError("Badly constructed UserObject.") {}
};

UserObjectClass makeUserObject_fromString(const string &s);

