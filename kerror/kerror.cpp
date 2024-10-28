/*
 * K Error
 *
 * Copyright (C) 2024 KO Myung-Hun <komh78@gmail.com>
 *
 * This file is a part of K Error.
 *
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://www.wtfpl.net/ for more details.
 */

/** \file */

#include <iostream>

#include <vector>
#include <algorithm>

#include <cstdlib>
#include <cctype>

#define ERROR_ITEM( e, s ) { e, #e, s }

struct error_item
{
    int err;
    const std::string name;
    const std::string str;
};

#ifdef USE_OS2ERROR
#include "os2error.inc"
#else
#include "errno.inc"
#endif

static void print_item( const error_item& item )
{
    std::cout << item.name << "(" << item.err << "): " << item.str << "\n";
}

static int print_error( int err )
{
    bool found = false;

    for( auto item : error_list )
    {
        if( err == item.err )
        {
            print_item( item );

            found = true;
        }
    }

    return found;
}

static bool print_error( const std::string& name )
{
    bool found = false;

    for( auto item : error_list )
    {
        if( name.length() == item.name.length()
            && std::equal(name.begin(), name.end(), item.name.begin(),
                          [](auto a, auto b) {
                            return ::toupper( a ) == ::toupper( b );
                          }))
        {
            print_item( item );

            found = true;
        }
    }

    return found;
}

int main( int argc, char *argv[])
{
    std::string errname;
    bool found;

    if( argc < 2 )
    {
        std::cerr << "Specify an error number or an error name.\n";
        return 1;
    }

    errname = argv[ 1 ];

    try
    {
        found = print_error( std::stoi( errname ));
    }
    catch( ... )
    {
        found = print_error( errname );
    }

    if( !found )
    {
        std::cerr << "Error " << "`" << errname << "'" << " not found!!!\n";

        return 1;
    }

    return 0;
}
