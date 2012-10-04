/*
 *  Axiom - an experimental programming language
 *
 *  Copyright (C) 2008-2012 Arjun G. Menon
 *  Released under the New BSD 3-Clause License
 *
 */

#include "common.h"

/*****************.
| UserObjectClass |
\*****************/

char UserObjectClass::whatType() const
{
    return dataTypeID;
}

const dataType * UserObjectClass::get_data() const
{
    return data;
}

UserObjectClass::UserObjectClass(const dataType_null &k_null)
{
    #ifdef SHOW_DEBUG_MESSAGES
    cout<<"Constructing: UserObject - null ...\n";
    #endif
    dataTypeID = DATATYPE_null;
    data = new dataType_null(k_null);
}

UserObjectClass::UserObjectClass(const dataType_num &k_num)
{
    #ifdef SHOW_DEBUG_MESSAGES
    cout<<"Constructing: UserObject - num ...\n";
    #endif
    dataTypeID = DATATYPE_num;
    data = new dataType_num(k_num);
}

UserObjectClass::UserObjectClass(const dataType_str &k_str)
{
    #ifdef SHOW_DEBUG_MESSAGES
    cout<<"Constructing: UserObject - str ...\n";
    #endif
    dataTypeID = DATATYPE_str;
    data = new dataType_str(k_str);
}

UserObjectClass::UserObjectClass(const dataType_pair &k_pair)
{
    #ifdef SHOW_DEBUG_MESSAGES
    cout<<"Constructing: UserObject - list ...\n";
    #endif
    dataTypeID = DATATYPE_pair;
    data = new dataType_pair(k_pair);
}

UserObjectClass::UserObjectClass(const dataType_list &k_list)
{
    #ifdef SHOW_DEBUG_MESSAGES
    cout<<"Constructing: UserObject - list ...\n";
    #endif
    dataTypeID = DATATYPE_list;
    data = new dataType_list(k_list);
}

UserObjectClass::UserObjectClass(const UserObjectClass &copyFrom)
{
    copy(copyFrom);
}

UserObjectClass & UserObjectClass::operator=(const UserObjectClass &rhs)
{
    #ifdef SHOW_DEBUG_MESSAGES
    cout<<"Assignment operator: UserObject ...\n";
    #endif
    clear();
    copy(rhs);
    return *this;
}

UserObjectClass::~UserObjectClass()
{
    clear();
}

// Important Fucntions:
// --------------------

UserObjectClass UserObjectClass::fetchMember(const string &name) const
{
    if(data != NULL)
        return data->fetch_member(name);
    else
        throw badUserObjectError();
}

UserObjectClass UserObjectClass::callFunction(const UserObjectClass &arg) const
{
    if(data != NULL)
        return data->call_function(arg);
    else
        throw badUserObjectError();
}

const string UserObjectClass::toString() const
{
    if(data != NULL)
        return data->string_repr();
    else
        throw badUserObjectError();
}

void UserObjectClass::copy(const UserObjectClass &obj)
{
    #ifdef SHOW_DEBUG_MESSAGES
    cout<<"Copying: UserObject ...\n";
    #endif

    dataTypeID = obj.dataTypeID;

    if(obj.data != NULL)
    {
        switch(obj.dataTypeID)
        {
        case DATATYPE_null:
            {
                dataType_null *null_dt_ptr = 0;
                ASSERT(( null_dt_ptr=dynamic_cast<dataType_null *>(obj.data) )!=0);
                data = new dataType_null(*null_dt_ptr);
            }
            break;
        case DATATYPE_num:
            {
                dataType_num *num_dt_ptr = 0;
                ASSERT(( num_dt_ptr=dynamic_cast<dataType_num *>(obj.data) )!=0);
                data = new dataType_num(*num_dt_ptr);
            }
            break;
        case DATATYPE_str:
            {
                dataType_str *str_dt_ptr = 0;
                ASSERT(( str_dt_ptr=dynamic_cast<dataType_str *>(obj.data) )!=0);
                data = new dataType_str(*str_dt_ptr);
            }
            break;
        case DATATYPE_pair:
            {
                dataType_pair *str_dt_pair = 0;
                ASSERT(( str_dt_pair=dynamic_cast<dataType_pair *>(obj.data) )!=0);
                data = new dataType_pair(*str_dt_pair);
            }
            break;
        case DATATYPE_list:
            {
                dataType_list *str_dt_list = 0;
                ASSERT(( str_dt_list=dynamic_cast<dataType_list *>(obj.data) )!=0);
                data = new dataType_list(*str_dt_list);
            }
            break;
        default:
            throw badUserObjectError();
        }
    }
    else throw badUserObjectError();
}

void UserObjectClass::clear()
{
    #ifdef SHOW_DEBUG_MESSAGES
    cout<<"Destroying: UserObject ...\n";
    #endif

    if(data != NULL)
    {
        switch(dataTypeID)
        {
        case DATATYPE_null:
            {
                dataType_null *null_dt_ptr = 0;
                ASSERT(( null_dt_ptr=dynamic_cast<dataType_null *>(data) )!=0);
                delete null_dt_ptr;
            }
            break;
        case DATATYPE_num:
            {
                dataType_num *num_dt_ptr = 0;
                ASSERT(( num_dt_ptr=dynamic_cast<dataType_num *>(data) )!=0);
                delete num_dt_ptr;
            }
            break;
        case DATATYPE_str:
            {
                dataType_str *str_dt_ptr = 0;
                ASSERT(( str_dt_ptr=dynamic_cast<dataType_str *>(data) )!=0);
                delete str_dt_ptr;
            }
            break;
        case DATATYPE_pair:
            {
                dataType_pair *str_dt_pair = 0;
                ASSERT(( str_dt_pair=dynamic_cast<dataType_pair *>(data) )!=0);
                delete str_dt_pair;
            }
            break;
        case DATATYPE_list:
            {
                dataType_list *str_dt_list = 0;
                ASSERT(( str_dt_list=dynamic_cast<dataType_list *>(data) )!=0);
                delete str_dt_list;
            }
            break;
        default:
            throw badUserObjectError();
        }
        dataTypeID = 0;
        data = NULL;
    }
    else throw badUserObjectError();
}

/******************************.
| UserObject Service Functions |
\******************************/

UserObjectClass makeUserObject_fromString(const string &s)
{
    if(!s.empty())
    {
        #define isNumeric(C) ( (C>='0') && (C<='9') )
        if(isNumeric( *(s.c_str()) ))
            return UserObjectClass(dataType_num(atof(s.c_str())));
        #undef isNumeric

        else if(s.at(0)=='\"')
        {
            if(s.size()>=2) {
                if( (s.at(0)=='\"') && s.at(s.size()-1)=='\"' ) {
                    return UserObjectClass(dataType_str(s.substr(1, s.size()-2)));
                }
            }
            return UserObjectClass(dataType_str(""));
        }
        else // not a number, not a string
        {
            throw userError("Unable to find symbol \""+s+"\" in reference list.");
            // must be a vairable
        }
    }
    return UserObjectClass(dataType_null());
}

