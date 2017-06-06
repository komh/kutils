/*
 * K Line Calculator
 *
 * Copyright (C) 2017 KO Myung-Hun <komh@chollian.net>
 *
 * This file is a part of K Line Calculator.
 *
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://www.wtfpl.net/ for more details.
 */

/** \file klinecalc.cpp */

#include "numcalc.h"

#include <string>
#include <iostream>

/**
 * Calculate expression
 *
 * \param[in] expr Expression
 * \return 0 on success, 1 on error
 */
static int calc( const std::string& expr )
{
    NumCalc nc( expr );

    if( nc.error())
    {
        std::cerr << nc.errorStr() << "\n";

        return 1;
    }

    std::cout << expr << " = " << nc.result() << "\n";

    return 0;
}

/**
 * Calculate line by line
 */
static void lineCalc()
{
    std::string expr;

    for(;;)
    {
        std::cout << "> ";
        std::getline( std::cin, expr );

        if( expr.compare("quit") == 0 || expr.compare("exit") == 0 )
            break;

        calc( expr );
    }
}

int main( int argc, char *argv[])
{
    if( argc < 2 )
    {
        lineCalc();

        return 0;
    }

    std::string expr;

    for( int i = 1; i < argc; ++i )
    {
        if( i > 1 )
            expr += " ";

        expr += argv[ i ];
    }

    return calc( expr );
}
