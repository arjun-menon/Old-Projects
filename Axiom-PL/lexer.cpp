/*
 *  Axiom - an experimental programming language
 *
 *  Copyright (C) 2008-2012 Arjun G. Menon
 *  Released under the New BSD 3-Clause License
 *
 */

#include "common.h"

frag_chain Fragmentize(tok_chain &toks)
{
    frag_chain frags;

    while(!toks.empty())
    {
        tok_chain some_toks;

        // fetches, pops & pushes the front-most token in 'tok_chain':
        string s = toks.front();
        toks.pop_front();
        some_toks.push_back(s);

        if(s=="(")
        {
            // balance is the number of yet un-closed parentheses
            int balance = 1;

            while(!toks.empty())
            {
                // fetches, pops & pushes the front-most token in 'tok_chain': (repeated code)
                s = toks.front();
                toks.pop_front();
                some_toks.push_back(s);

                if(s=="(")
                    balance++;
                else if(s==")")
                    balance--;

                if(!balance)
                    break;
            }

            // balance = 0 means the expression is balanced
            if(balance)
                throw userError("Missing closing parenthesis.");
        }
        else if(s==")")
            throw userError("Missing opening parenthesis.");

        // push fragment
        if(!some_toks.empty())
            frags.push_back(some_toks);
    }

    return frags;
}

inline void push_token(string &s, tok_chain &l)
{
    trim(s);
    if(s.length())
        l.push_back(s);
    s = "";
}

inline void fixChar(char &c) // called once only
{
    /* conditionally converts characters */

    // tab => space
    if(c=='\t')
        c = ' ';
    // ' => "
    else if(c=='\'')
        c = '\"';
    // newline => ;
    else if(c=='\n')
        c = ';';
}

tok_chain Tokenize(const string &str)
{
    // UNDERSCORES (_) IN VAR. NAMES !!!
    // use string instead of *sp, call at() to access elements

    const char *sp = str.c_str();
    tok_chain tokens;
    string temp;
    bool inString = false;

    for(char currentChar = *sp, prevChar = '\0', prevChar2 = '\0', 
        nextChar = '\0' ; currentChar ; currentChar = *++sp )
    {
        /*
        Set nextChar
        remember, if *sp = '\0' then we would never entered 
        this loop, so dereferencing (sp+1) is okay.
        similarly, in the last iteration nextChar will be equal to '\0' and 
        there is no further iteration, so we will never read beyond sp.
        */
        nextChar = *(sp+1);

        #define isPrintableASCII(C) ( ((C>=32)&&(C<=126)) || (C=='\t')  )
        #define isBindingSymbol(C) ( C=='(' || C==')' || C=='{' || C=='}' )
        #define isNumeric(C) ( (C>='0') && (C<='9') )
        #define isAlphaNumeric(C) ( isNumeric(C) || (C>='A'&&C<='Z') || (C>='a'&&C<='z') )

        if(isPrintableASCII(currentChar))
        {
            fixChar(currentChar); /* hopefully this job can be given to the 
            function that has to construct sp, removes coments e.t.c. */

            if(inString)
            {
                if(currentChar=='\"' && prevChar!='\\' // below for "\\"
                ||(currentChar=='\"' && prevChar=='\\' && prevChar2=='\\'))
                {
                    // string literal termination
                    inString = false;
                    temp += currentChar;
                    push_token(temp, tokens);
                }
                else
                    temp += currentChar;
            }
            else
            {
                if(currentChar=='\"')
                {
                    push_token(temp, tokens);
                    inString = true;
                    temp += currentChar;
                }
                else if(currentChar==' ')
                {
                    push_token(temp, tokens);
                }
                else if(isBindingSymbol(currentChar))
                {
                    push_token(temp, tokens);
                    temp = currentChar;
                    push_token(temp, tokens);
                }
                else
                {
                    if(!
                        // The following conditions denote characters that may appear together in
                        // a token. If the condition fails, the token in hand so-far is pushed.
                        (
                            // contiguous alphanumeric characters (abc, ab5d, 5e6) and
                            // contigous non-A.N. characters (%, &&, +++, ==, -, e.t.c)
                            ( isAlphaNumeric(prevChar) == isAlphaNumeric(currentChar) )
                            // this test is for real number literals (example: 2.34)
                            || (isNumeric(prevChar) && currentChar=='.' && isNumeric(nextChar))
                            || (isNumeric(prevChar2) && prevChar=='.' && isNumeric(currentChar))
                        )
                    )
                        push_token(temp, tokens);

                    temp += currentChar;
                }
            }

            prevChar2 = prevChar;
            prevChar = currentChar;
        }

        #undef isBindingSymbol
        #undef isAlphaNumeric
        #undef isPrintableASCII

        else
            throw userError("Non-printable ASCII character(" + cons((UC)(currentChar)) 
                + ":" + cons((UI)((UC)(currentChar))) + ") detected.");
    }
    push_token(temp, tokens);

    if(inString)
        throw userError("Unterminated string");

    return tokens;
}

void trim(string &str, const string to_remove)
{  
    // trim leading & trailing 'to_remove'
    size_t startpos = str.find_first_not_of(to_remove);
    size_t endpos = str.find_last_not_of(to_remove);

    // if empty or all 'to_remove' return an empty string otherwise trim
    str = (( string::npos == startpos ) || ( string::npos == endpos)) ? "" : 
        str.substr( startpos, endpos-startpos+1 );
}

string display_toks(const tok_chain &k, bool linear)
{
    stringstream sout;
    if(!linear)
        sout<<endl<<k.size()<<" tokens:"<<endl;
    for(tok_chain::const_iterator i = k.begin(); i!=k.end(); i++)
    {
        sout<<*i;
        
        if(!linear)
            sout<<endl;
    }
    return sout.str();
}

string display_frags(const frag_chain &c, bool linear)
{
    stringstream sout;
    int counter = 0;
    
    if(!linear)
        sout<<endl<<c.size()<<" fragments:"<<endl;
    
    for(frag_chain::const_iterator i = c.begin(); i!=c.end(); i++)
    {
        if(!linear)
            sout<<++counter<<": "<<endl;
        
        for(tok_chain::const_iterator j = i->begin(); j!=i->end(); j++)
        {
            if(linear)
                sout<<*j;
            else
                sout<<"\t"<<*j<<endl;
        }
        if(!linear)
            sout<<endl;
    }
    return sout.str();
}

bool isValid_id(const char *str)
{
    // checks the syntactical validity
    // of a proposed identifier.

    if(*str>='0'&&*str<='9')
        return false;

    for(;*str!='\0';str++)
        if((*str<'A'||*str>'Z')
        && (*str<'a'||*str>'z')
        && (*str<'0'||*str>'9')
        && (*str!='_'))
            return false;

    return true;
}

bool isValidIdentifier(string s)
{
    return isValid_id(s.c_str());
}

