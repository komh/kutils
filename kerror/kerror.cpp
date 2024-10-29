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
#include <string>
#include <algorithm>

#include "errormessage.h"
#include "libcerrormessage.h"
#include "os2errormessage.h"

int main( int argc, char *argv[])
{
    if( argc < 2 )
    {
        std::cerr << "Specify an error number or an error name.\n";
        return 1;
    }

    std::string me( argv[ 0 ]);
    std::string name( argv[ 1 ]);

    ErrorMessage *em;

    if( me.find("os2err") == std::string::npos )
        em = new LibcErrorMessage();
    else
        em = new Os2ErrorMessage();

    try
    {
        em->find( std::stoi( name ));
    }
    catch( ... )
    {
        em->find( name );
    }

    auto result = em->result();
    delete em;

    if( result.empty())
    {
        std::cerr << "Error " << "`" << name << "'" << " not found!!!\n";

        return 1;
    }

    std::for_each( result.cbegin(), result.cend(),
                   []( const auto& item ) {
                        std::cout << item.name << "(" << item.error << "): "
                                  << item.message << "\n";
                   });

    return 0;
}
