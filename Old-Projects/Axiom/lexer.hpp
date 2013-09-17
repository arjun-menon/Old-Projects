/*
 *  Axiom - an experimental programming language
 *
 *  Copyright (C) 2008-2012 Arjun G. Menon
 *  Released under the New BSD 3-Clause License
 *
 */

// Typedefs
typedef deque<string> tok_chain; // NOTE !!! tok_chain = fragment
typedef list<tok_chain> frag_chain;

// Function prototypes
tok_chain Tokenize(const string &str);
frag_chain Fragmentize(deque<string> &toks);
void trim(string &str, const string to_remove = " \t");
string display_toks(const tok_chain &, bool linear);
string display_frags(const frag_chain &, bool linear);
bool isValidIdentifier(string s);

class fragError : public fatalError
{
    public: fragError() : 
    fatalError("Fragmentation fault") {}
};

// Template to construct a string consisting of generic x
template <typename T> string cons(const T &x)
{
    stringstream s;
    s << x;
    return s.str();
}

