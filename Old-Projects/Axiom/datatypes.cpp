/*
 *  Axiom - an experimental programming language
 *
 *  Copyright (C) 2008-2012 Arjun G. Menon
 *  Released under the New BSD 3-Clause License
 *
 */

#include "common.h"

/***************.
| Datatype Null |
\***************/

// Trivial Functions:
// ------------------

dataType_null::dataType_null() : functional("")
{
    #ifdef SHOW_DEBUG_MESSAGES
    cout<<"Constructing DataType null ...\n";
    #endif
}

dataType_null::dataType_null(const dataType_null &copy) : functional(copy.functional)
{
    #ifdef SHOW_DEBUG_MESSAGES
    cout<<"Copying DataType null ...\n";
    #endif
}

dataType_null::dataType_null(const string &fn_name) : functional(fn_name)
{
    #ifdef SHOW_DEBUG_MESSAGES
    cout<<"Constructing DataType *function* null ...\n";
    #endif
}

dataType_null::~dataType_null()
{
    #ifdef SHOW_DEBUG_MESSAGES
    cout<<"Destroying DataType null ...\n";
    #endif
}

const string dataType_null::string_repr()
{
    return "(null)";
}

// Important Fucntions:
// ====================

UserObjectClass dataType_null::fetch_member(const string &name)
{
    ASSERT(isValidIdentifier(name));

    // procedural-type members:
    if(name=="num")
        return UserObjectClass(dataType_num(0));
    else if(name=="str")
        return UserObjectClass(dataType_str(""));

    // functional-type memebers:
    else if(name=="sub")
        return UserObjectClass(dataType_null("sub"));

    else
        return UserObjectClass(dataType_null(name));
}

UserObjectClass dataType_null::call_function(const UserObjectClass &arg)
{
    if(functional.empty())
        throw userError("You cannot perform a function call on null.");
    else
    {
        if(functional=="sub")
            return arg.fetchMember("neg");
        else if(arg.whatType()!=DATATYPE_null)
            return arg.fetchMember(functional).callFunction(dataType_null());
        else
            throw functionNotFoundException(functional);
    }
}

/*****************.
| Datatype Number |
\*****************/

// Trivial Functions:
// ------------------

dataType_num::dataType_num(const double &k) : num_val(k), functional("")
{
    #ifdef SHOW_DEBUG_MESSAGES
    cout<<"Constructing DataType num ...\n";
    #endif
}

dataType_num::dataType_num(const dataType_num &copy) : num_val(copy.num_val), functional(copy.functional) 
{
    #ifdef SHOW_DEBUG_MESSAGES
    cout<<"Copying DataType num ...\n";
    #endif
}

dataType_num::dataType_num(const string &fn_name, const double &k) : functional(fn_name), num_val(k)
{
    #ifdef SHOW_DEBUG_MESSAGES
    cout<<"Constructing DataType *function* num ...\n";
    #endif
}

dataType_num::~dataType_num()
{
    #ifdef SHOW_DEBUG_MESSAGES
    cout<<"Destroying DataType num ...\n";
    #endif
}

double dataType_num::get_num() const
{
    return num_val;
}

const string dataType_num::string_repr()
{
    return cons(num_val);
}

// Important Fucntions:
// ====================

UserObjectClass dataType_num::fetch_member(const string &name)
{
    ASSERT(isValidIdentifier(name));

    // method-type members:
    // ~~~~~~~~~~~~~~~~~~~~
    if(name=="inc") {
        dataType_num init_value(num_val);
        num_val++;
        return UserObjectClass(init_value);
    }
    // Rounding, absolute value and negation functions:
    else if(name=="neg")
        return UserObjectClass(dataType_num(-(num_val)));
    else if(name=="abs")
        return UserObjectClass(dataType_num(fabs(num_val)));
    else if(name=="ceil")
        return UserObjectClass(dataType_num(ceil(num_val)));
    else if(name=="floor")
        return UserObjectClass(dataType_num(floor(num_val)));
    // Trigonometric and logarithmic functions:
    else if(name=="sin")
        return UserObjectClass(dataType_num(sin(num_val)));
    else if(name=="cos")
        return UserObjectClass(dataType_num(cos(num_val)));
    else if(name=="tan")
        return UserObjectClass(dataType_num(tan(num_val)));
    else if(name=="asin") {
        if( num_val<-1 || num_val>1 ) // out of range
            throw userError("Invalid parameter for sin sine function. Must be in the interval [-1,+1].");
        return UserObjectClass(dataType_num(asin(num_val)));
    }
    else if(name=="acos") {
        if( num_val<-1 || num_val>1 ) // out of range
            throw userError("Invalid parameter for arc cosine function. Must be in the interval [-1,+1].");
        return UserObjectClass(dataType_num(acos(num_val)));
    }
    else if(name=="atan")
        return UserObjectClass(dataType_num(atan(num_val)));
    else if(name=="sinh")
        return UserObjectClass(dataType_num(sinh(num_val)));
    else if(name=="cosh")
        return UserObjectClass(dataType_num(cosh(num_val)));
    else if(name=="tanh")
        return UserObjectClass(dataType_num(tanh(num_val)));
    // Exponential and logarithmic functions:
    else if(name=="exp")
        return UserObjectClass(dataType_num(exp(num_val)));
    else if(name=="log")
        return UserObjectClass(dataType_num(log(num_val)));
    else if(name=="log10")
        return UserObjectClass(dataType_num(log10(num_val)));
    else if(name=="modf"||name=="split") {
        dataType_list k;
        double intpart = 0;
        double fractpart = modf(num_val, &intpart);
        k.push_back(dataType_num(intpart));
        k.push_back(dataType_num(fractpart));
        return UserObjectClass(k);
    }
    else if(name=="frexp") {
        dataType_list k;
        int exp = 0;
        double result = frexp(num_val, &exp);
        k.push_back(dataType_num(result));
        k.push_back(dataType_num((double)(exp)));
        return UserObjectClass(k);
    }

    // functional-type memebers:
    // ~~~~~~~~~~~~~~~~~~~~~~~~~
    #define EIF_RFN(S) else if(name==S) return UserObjectClass(dataType_num(S, num_val))
    EIF_RFN("add");
    EIF_RFN("sub");
    EIF_RFN("mul");
    EIF_RFN("div");
    EIF_RFN("mod");
    EIF_RFN("pow");
    #undef IF_RFN

    else
        throw memberNotFoundException(name, "Number");
}

UserObjectClass dataType_num::call_function(const UserObjectClass &arg)
{
    if(functional.empty())
        throw userError("You cannot perform a function call on a number.");
    else
    {
        if(functional=="add")
            return UserObjectClass(dataType_num( num_val + convertUserObject_toNumber(arg) ));
        else if(functional=="sub")
            return UserObjectClass(dataType_num( num_val - convertUserObject_toNumber(arg) ));
        else if(functional=="mul")
            return UserObjectClass(dataType_num( num_val * convertUserObject_toNumber(arg) ));
        else if(functional=="div")
            return UserObjectClass(dataType_num( num_val / convertUserObject_toNumber(arg) ));
        else if(functional=="mod")
            return UserObjectClass(dataType_num( fmod(num_val, convertUserObject_toNumber(arg)) ));
        else if(functional=="pow")
            return UserObjectClass(dataType_num( pow(num_val, convertUserObject_toNumber(arg)) ));
        else
            throw functionNotFoundException(functional);
    }
}

double dataType_num::convertUserObject_toNumber(const UserObjectClass &obj)
{
    switch(obj.whatType())
    {
    case DATATYPE_num:
        {
            const dataType_num *ptr = 0;
            ASSERT(( ptr=dynamic_cast<const dataType_num *>(obj.get_data()) )!=0);

            return ptr->get_num();
        }
        break;

    case DATATYPE_str:
        {
            const dataType_str *ptr = 0;
            ASSERT(( ptr=dynamic_cast<const dataType_str *>(obj.get_data()) )!=0);

            return atof((ptr->get_str()).c_str());
        }
        break;

    case DATATYPE_null:
        return 0;

    default:
        throw userError("Type mismatch");
    }
}

/*****************.
| Datatype String |
\*****************/

// Trivial Functions:
// ------------------

dataType_str::dataType_str(const string &k) : str_val(k), functional("")
{
    #ifdef SHOW_DEBUG_MESSAGES
    cout<<"Constructing DataType str ...\n";
    #endif
}

dataType_str::dataType_str(const dataType_str &copy) : str_val(copy.str_val), functional(copy.functional)
{
    #ifdef SHOW_DEBUG_MESSAGES
    cout<<"Copying DataType str ...\n";
    #endif
}

dataType_str::dataType_str(const string &fn_name, const string &k) : functional(fn_name), str_val(k)
{
    #ifdef SHOW_DEBUG_MESSAGES
    cout<<"Constructing DataType *function* str ...\n";
    #endif
}

dataType_str::~dataType_str()
{
    #ifdef SHOW_DEBUG_MESSAGES
    cout<<"Destroying DataType str ...\n";
    #endif
}

string dataType_str::get_str() const
{
    return str_val;
}

const string dataType_str::string_repr()
{
    return str_val;
}

// Important Fucntions:
// ====================

UserObjectClass dataType_str::fetch_member(const string &name)
{
    ASSERT(isValidIdentifier(name));

    if(name=="len")
        return UserObjectClass(dataType_num(str_val.length()));
    else if(name=="add")
        return UserObjectClass(dataType_str("add", str_val));
    else
        throw memberNotFoundException(name, "String");
}

UserObjectClass dataType_str::call_function(const UserObjectClass &arg)
{
    if(functional.empty())
        throw userError("You cannot perform a function call on a number.");
    else
    {
        if(functional=="add")
            return UserObjectClass(dataType_str( str_val + arg.toString() ));
        else
            throw functionNotFoundException(functional);
    }
}

/***************.
| Datatype Pair |
\***************/

// Trivial Functions:
// ------------------

dataType_pair::dataType_pair(const UserObjectClass &a, const UserObjectClass &b) : first(a), second(b)
{
    #ifdef SHOW_DEBUG_MESSAGES
    cout<<"Constructing DataType pair ...\n";
    #endif
}

dataType_pair::dataType_pair(const dataType_pair &copy) : first(copy.first), second(copy.second)
{
    #ifdef SHOW_DEBUG_MESSAGES
    cout<<"Copying DataType pair ...\n";
    #endif
}

dataType_pair::~dataType_pair()
{
    #ifdef SHOW_DEBUG_MESSAGES
    cout<<"Destroying DataType pair ...\n";
    #endif
}

const string dataType_pair::string_repr()
{
    return "( " + first.toString() + " => " + second.toString() + " )";
}

// Important Fucntions:
// ====================

UserObjectClass dataType_pair::fetch_member(const string &name)
{
    throw("The pair object has no members.");
}

UserObjectClass dataType_pair::call_function(const UserObjectClass &arg)
{
    throw("The pair cannot be called as a function");
}

/***************.
| Datatype List |
\***************/

// Trivial Functions:
// ------------------

dataType_list::dataType_list()
{
    #ifdef SHOW_DEBUG_MESSAGES
    cout<<"Constructing DataType list ...\n";
    #endif
}

dataType_list::~dataType_list()
{
    #ifdef SHOW_DEBUG_MESSAGES
    cout<<"Destroying DataType list ...\n";
    #endif
}

const string dataType_list::string_repr()
{
    string repr = "(";
    bool comma = false;

    for(list<UserObjectClass>::const_iterator i = mylist.begin(); i != mylist.end(); i++)
    {
        if(comma)
            repr += ", ";
        UserObjectClass k = *i;
        repr += k.toString();
        comma = true;
    }

    return repr + ")";
}

// Important Fucntions:
// ====================

UserObjectClass dataType_list::fetch_member(const string &name)
{
    throw("This list object has no members.");
}

UserObjectClass dataType_list::call_function(const UserObjectClass &arg)
{
    throw("This list cannot be called as a function");
}

void dataType_list::push_back(const UserObjectClass &obj)
{
    mylist.push_back(obj);
}

