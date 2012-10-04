/*
 *  Axiom - an experimental programming language
 *
 *  Copyright (C) 2008-2012 Arjun G. Menon
 *  Released under the New BSD 3-Clause License
 *
 */

#include "common.h"

// operators
typedef map<string, pair<string, double> > op_table;
op_table ops;

void init_ops()
{
    // ensure ["."] has highest precedence!
    ops["="]  = make_pair("add", 0.5);
    ops["+"]  = make_pair("add", 1);
    ops["-"]  = make_pair("sub", 1);
    ops["*"]  = make_pair("mul", 2);
    ops["/"]  = make_pair("div", 2);
    ops["%"]  = make_pair("mod", 2);
    ops["^"]  = make_pair("pow", 3);
    ops["++"] = make_pair("inc", 4);
    ops["."]  = make_pair(".", 5);
}

frag_chain::iterator findCriticalOperator(frag_chain &frags)
{
    frag_chain::iterator pos = frags.end();
    op_table::const_iterator op = ops.begin();
    
    // check if bops is empty
    if(op==ops.end())
        return pos;

    double max;
    // calculate max, whose value is greater than that
    // of the operator with the greatest mapped value.
    for(max = (op++)->second.second; op!=ops.end(); op++)
        if(op->second.second > max)
            max = op->second.second;
    max++;

    // Now find the operator with the *lowest* precedence:
    for(frag_chain::iterator i = frags.begin(); i!=frags.end(); i++) {
        if(i->size() == 1) { // op must be a unit fragment
            if((op = ops.find(i->front()))!=ops.end()) {
                if(op->second.second < max) {
                    pos = i;
                    max = op->second.second;
                }
            }
        }
    }

    return pos;
}

UserObjectClass eval(frag_chain frags); // prototype - used for recursion

UserObjectClass applyMemberFetch(UserObjectClass obj, frag_chain rhs)
{
    if(!rhs.empty())
    {
        string method_name = rhs.front().front();

        // validate method_name:
        if(rhs.front().size()==1) {
            if(!isValidIdentifier(method_name)) 
                throw userError("Invalid method (\"" + method_name + "\"). Method names must follow identifier syntax.");
        } else
            throw userError("You cannot have an expression (\"" + method_name + "\") as the method name.");

        if(rhs.size()==1)
        {
            if(rhs.front().size()==1)
            {
                if(isValidIdentifier(method_name))
                    return obj.fetchMember(method_name);
                else 
                    throw userError("Invalid method (\"" + method_name + "\"). Method names must follow identifier syntax.");
            }
            else
                throw userError("You cannot have an expression as the method name.");
        }
        else
        {
            rhs.pop_front();

            if(rhs.front().front()==".")
            {
                // conjugate method access experssion
                // ( eg: a.b.c, 2.neg.abs.neg )
                
                rhs.pop_front();
                // *** Recursion ***
                return applyMemberFetch(obj.fetchMember(method_name), rhs);
            }
            else
                throw userError(
"You cannot have a common expression as the method name. \
Unable to process \""+rhs.front().front()+"\" symbol. Symbol \
follwing method name in conjugate expression must be the method access operator. \
Please ensure that the method access operator has the highest precedence in order \
to avoid any conjugate expression operator conflicts.");
        }
    }
    else
        throw userError("Method name missing.");
}

UserObjectClass applyBOP(UserObjectClass obj, string op, frag_chain rhs)
{
    if(!ops.count(op))  // Operator Not Found
        throw fatalError("Operator \"" + op + "\" missing.");
    else
    {
        // while evaluating always go from left to right.
        
        if(rhs.size() >= 2)
        {
            frag_chain::const_iterator it = rhs.begin();
            it++; // points to the second fragment in rhs

            if(it->size()==1)
            {
                string k = it->front();

                // check if k is an operator
                if(ops.count(k))
                {
                    // check if k has same precedence as 'op'
                    if( ops[op].second == ops[k].second )
                    {
                        tok_chain first_rhs_elem = rhs.front();
                        rhs.pop_front(); // -> first_rhs_elem
                        rhs.pop_front(); // -> string k

                        frag_chain primary_rhs;
                        primary_rhs.push_front(first_rhs_elem);

                        return (obj.fetchMember(ops[op].first).callFunction(eval(primary_rhs)))
                            .fetchMember(ops[k].first).callFunction(eval(rhs));
                    }
                }
            }
        }

        return obj.fetchMember(ops[op].first).callFunction(eval(rhs));
    }
}

UserObjectClass applyOP(string op, frag_chain lhs, frag_chain rhs)
{
    if(op == ".") // member access operator
        return applyMemberFetch(eval(lhs), rhs);

    // Unary Operators
    else if(op=="++")
        return eval(lhs).fetchMember("inc");
    
    // Binary Operators
    else if(op=="=") {
        UserObjectClass first  = eval(lhs);
        UserObjectClass second = eval(rhs);
        return UserObjectClass(dataType_pair(first, second));
    }
    else
        return applyBOP(eval(lhs),op,rhs);
}

UserObjectClass eval(frag_chain frags)
{
    #ifdef SHOW_DEBUG_MESSAGES
    cout<<"Evaluating: "<<display_frags(frags, true)<<endl;
    #endif

    if(frags.size() < 1)
    {
        // frags is empty, so return null UserObj; this
        // happens either when there is a blank expr or ()
        return UserObjectClass(dataType_null()); // *** Base Case ***
    }
    else if(frags.size() == 1)
    {
        tok_chain toks = frags.front();

        if(toks.size() > 1)
        {
            // strip parentheses
            ASSERT(toks.front()=="(");
            ASSERT(toks.back()==")");
            toks.pop_front();
            toks.pop_back();

            // *** Recursive Call ***
            return eval(Fragmentize(toks));
        }
        else if(toks.size() == 1)
        {
            // we have reached a unit fragment *** Base Case ***
            //return UserObjectClass(toks.front());
            return makeUserObject_fromString(toks.front());
        }
        else
        {
            // empty token chain - fragmentation fault
            throw fragError();
        }
    }
    else // if(frags.size() > 1)
    {
        frag_chain::iterator pos;

        // search for binary operator
        if((pos = findCriticalOperator(frags)) != frags.end())
        {
            // found a binary operator
            frag_chain lhs_frags, rhs_frags;

            // splicing frags into lhs(lhs_frags), operator(frags) & rhs(rhs_frags)
            lhs_frags.splice(lhs_frags.begin(), frags, frags.begin(), pos);
            pos = frags.begin();
            rhs_frags.splice(rhs_frags.begin(), frags, ++pos, frags.end());

            // integrity check:
            ASSERT(frags.size()==1);
            if(frags.begin()->size() != 1)
                throw fragError();

            // now applying binary operation - *** Expect Recursion ***
            return applyOP(*(frags.begin()->begin()), lhs_frags, rhs_frags);
        }
        else
        {
            // construct list
            dataType_list k;

            for(pos = frags.begin(); pos != frags.end() ; pos++)
            {
                tok_chain &elem = *pos;
                k.push_back(eval(Fragmentize(elem)));
            }

            return UserObjectClass(k);
        }
    }
}

string exec(const string &str)
{
    try
    {
        tok_chain tokens = Tokenize(str);
        init_ops();
        return (eval(Fragmentize(tokens))).toString();
    }
    catch(exception &e) {
        return e.what();
    }
    catch(...) {
        return fatalError("Unhandled Exception.").what();
    }
}

