/*
 * K Replace Name
 *
 * Copyright (C) 2017 KO Myung-Hun <komh@chollian.net>
 *
 * This file is a part of K Replace Name.
 *
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://www.wtfpl.net/ for more details.
 */

/** \file krn.cpp */

#include "dirtraversal.h"

#include <cstdio>

#include <iostream>

/**
 * Convert '/' to '\'
 *
 * \param[in] name Name including '/'
 *
 * \return Converted name
 */
static std::string fnbackslashify( const std::string& name )
{
    std::string bname( name );

    for( auto& ch : bname )
    {
        if( ch == '/' )
            ch = '\\';
    }

    return bname;
}

int main( int argc, char *argv[])
{
    int i = 1;
    bool subdir = false;
    std::string filename;
    std::string substr;
    std::string replstr;

    // check /S option
    if( i < argc && argv[ i ][ 0 ] == '/'
        && ( argv[ i ][ 1 ] == 'S' || argv[ i ][ 1 ] == 's'))
    {
        subdir = true;

        ++i;
    }

    // check arguments sanity
    if( i + 3 != argc )
    {
        std::cerr << "Usage : KRN [/S] filename sub-string replace-string\n";

        return 1;
    }

    // get arguments
    filename = argv[ i++ ];
    substr = argv[ i++ ];
    replstr = argv[ i++ ];

    // split filename into dir and mask
    std::string::size_type pos = filename.find_last_of(":\\/");
    std::string dir( pos == filename.npos ? "." : filename.substr( 0, pos ));

    if( pos == filename.npos )
        pos = 0;
    else if( pos > filename.length())
        pos = filename.length();
    else
        ++pos;
    std::string mask( filename.substr( pos ));

    int count = 0;
    DirTraversal dt( dir, mask );

    // replace sub-string in filename with replace-string
    dt.traverse([&count, &substr, &replstr]
                ( const std::string& dir, const DirEntry& entry )
                {
                    std::string newName( entry.name());

                    std::string::size_type pos = 0;

                    // make the replaced name
                    while(( pos = newName.find( substr, pos ))
                            != newName.npos )
                    {
                        newName.erase( pos, substr.length());
                        newName.insert( pos, replstr );
                        pos += replstr.length();
                    }

                    // replaced ?
                    if( entry.name() != newName )
                    {
                        // rename actually
                        std::string bsdir( fnbackslashify( dir ));
                        std::string oldPath( bsdir + '\\' + entry.name());
                        std::string newPath( bsdir + "\\" + newName );

                        std::cout << oldPath << " -> " << newPath << "\n";

                        if( rename( oldPath.c_str(), newPath.c_str()) == -1 )
                            std::cerr << "Failed to rename!!!\n";
                        else
                            ++count;
                    }
                },
                subdir );

    std::cout << "\t" << count << " file(s) has been renamed.\n";

    return 0;
}
