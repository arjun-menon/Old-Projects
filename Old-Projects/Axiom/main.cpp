/*
 *  Axiom - an experimental programming language
 *
 *  Copyright (C) 2008-2012 Arjun G. Menon
 *  Released under the New BSD 3-Clause License
 *
 */

#include <iostream>
#include <string>

int main()
{
    std::cout<<"\nAXIOM-REPL: "<<std::endl;

    // exec function prototype:
    std::string exec(const std::string &str);

    // to hold user input
    std::string input = "";

    while(true)
    {
        std::cout<<"\n> ";
        getline(std::cin, input);

        if(input=="quit")
            break;
        else
            std::cout<<exec(input);
    }

    return 0;
}

