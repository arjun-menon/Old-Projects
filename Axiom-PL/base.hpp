/*
 *  Axiom - an experimental programming language
 *
 *  Copyright (C) 2008-2012 Arjun G. Menon
 *  Released under the New BSD 3-Clause License
 *
 */

// Classes
class userError : public exception
{
    // User Errors are due to the user's fault. 
private:
    const string error_description;
public:
    virtual const char* what() const throw() { return error_description.c_str(); }
    userError(string err) : error_description((string)("Error: ")+err) {}
    ~userError() throw() {}
};

class fatalError : public exception
{
    // Fatal Errors are due to the programmer's fault.
private:
    const string error_description;
public:
    virtual const char* what() const throw() { return error_description.c_str(); }
    fatalError(string err) : error_description((string)("Fatal Error: ")+err) {}
    ~fatalError() throw() {}
};

// Macros
#define UI unsigned int
#define UC unsigned char
#define ASSERT(X) if(!(X)) throw fatalError\
((string)("Assertion failure ( ")+(string)(#X)+(string)(" )"))

