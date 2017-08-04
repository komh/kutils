/*
 * K Strip
 *
 * Copyright (C) 2017 KO Myung-Hun <komh@chollian.net>
 *
 * This file is a part of K Strip.
 *
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://www.wtfpl.net/ for more details.
 */

/** \file kstrip.cpp */

#include "lxheader.h"

#include <io.h>

#include <fstream>
#include <iostream>
#include <string>

/**
 * Strip debugging information
 *
 * \param[in] filename Filename to strip
 * \return 0 on success, 1 on error
 */
static int strip( const std::string& filename )
{
    std::cout << "Stripping debugging information of "
              << filename << "..." << std::endl;

    std::ifstream ifs;

    ifs.open( filename, ifs.in | ifs.binary );
    if( !ifs.good())
    {
        std::cerr << "Could not open!!!" << std::endl;

        return 1;
    }

    ifs.seekg( 0, ifs.end );
    auto filesize = ifs.tellg();

    ifs.close();

    LxHeader lxHdr( filename );

    if( !lxHdr.hasLx())
    {
        std::cerr << " Not a LX file!!!" << std::endl;

        return 1;
    }

    auto endOfDebug = lxHdr.debugInfo() + lxHdr.debugLen();

    // check if debugging information exists
    if( endOfDebug == 0 )
    {
        std::cerr << "No debugging information." << std::endl;

        return 1;
    }

    // check if debugging information exists at the end
    if( endOfDebug != filesize )
    {
        std::cerr << "No debugging information at the end."
                  << std::endl;

        return 1;
    }

    // truncate debugging information
    if( truncate( filename.c_str(), lxHdr.debugInfo()) == -1 )
    {
        std::cerr << "Could not strip debugging information!!!" << std::endl;

        return 1;
    }

    // update LX header
    lxHdr.setDebugInfo( 0 );
    lxHdr.setDebugLen( 0 );
    if( !lxHdr.write())
    {
        std::cerr << "Could not update LX header!!!" << std::endl;

        return 1;
    }

    std::cout << "Done." << std::endl;

    return 0;
}

int main( int argc, char *argv[])
{
    if( argc < 2 )
    {
        std::cerr << "Specify a LX filename!!!" << std::endl;

        return 1;
    }

    return strip( argv[ 1 ]);
}
