/*
 * K ldd, a clone of ldd in UNIX world
 *
 * Copyright (C) 2017 KO Myung-Hun <komh@chollian.net>
 *
 * This file is a part of K ldd.
 *
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://www.wtfpl.net/ for more details.
 */

/** \file kldd.cpp */

#define INCL_DOS
#include <os2.h>

#include "lxheader.h"

#include <cctype>
#include <cstdlib>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>

static int m_maxDepth = 1;  ///< Max depth to check recursively

/**
 * Convert a string to upper case
 *
 * \param[in] s String to convert
 * \return String converted to upper case
 */
static std::string strUpr( const std::string& s )
{
    std::string us;

    for( const auto& c: s )
        us.push_back( toupper( c ));

    return us;
}

/**
 * Get name without extention from a path
 *
 * \param[in] path Path
 * \return Name in upper case if exists, otherwise empty string
 */
static std::string getName( const std::string& path )
{
    std::string name( path );

    auto dirPos = name.find_last_of(":/\\");

    if( dirPos != name.npos )
        name.erase( 0, dirPos + 1 );

    auto dotPos = name.find_last_of('.');
    if( dotPos != name.npos )
        name.erase( dotPos );

    return strUpr( name );
}

/**
 * Get extension including '.' from a path
 *
 * \param[in] path Path
 * \return Extension including '.' in upper case if exists,
 *         otherwise empty string
 */
static std::string getExt( const std::string& path )
{
    std::string ext( path );

    auto dirPos = ext.find_last_of(":/\\");

    if( dirPos != ext.npos )
        ext.erase( 0, dirPos + 1 );

    ext.erase( 0, ext.find_last_of('.'));

    return strUpr( ext );
}

/**
 * Check imported DLLs
 *
 * \param[in] filename Filename to check
 * \param[in] depth Current depth
 * \param[in] excludes Exclude list. DLLs in it are not checked.
 * \return 0 on success, 1 on error
 */
static int ldd( const std::string& filename, int depth,
                const std::vector< std::string > excludes )
{
    std::string filepath( filename );

    std::cout << filepath;

    char szPath[ CCHMAXPATH + 1 ];

    // first module
    if( depth == 0 )
    {
        // find .exe as well
        if( _path2( filepath.c_str(), ".exe", szPath, sizeof( szPath )) == 0 )
            filepath = szPath;

        std::cout << " => " << filepath;
    }

    std::cout << std::endl;

    // Check up to m_maxDepth depth, recursively. 0 means all dependecies.
    if( m_maxDepth != 0 && depth >= m_maxDepth )
        return 0;

    std::string preSpaces( depth * 2, ' ');

    LxHeader lxHdr( filepath );

    std::ifstream ifs;

    ifs.open( filepath, ifs.in | ifs.binary );
    if( !ifs.good())
    {
        std::cerr << preSpaces << "Could not open!!!" << std::endl;

        return 1;
    }

    if( !lxHdr.hasLx())
    {
        std::cerr << preSpaces << "Not a LX file!!!" << std::endl;

        return 1;
    }

    // seek to import module name table
    ifs.seekg( lxHdr.lxOffset() + lxHdr.impMod());
    if( !ifs.good())
    {
        std::cerr << preSpaces << "Could not seek!!!" << std::endl;

        return 1;
    }

    std::vector< std::string > mods;

    // find all the imported module names
    for( int i = lxHdr.impModCount(); i; --i )
    {
        std::string name;

        for( int len = ifs.get(); ifs.good() && len; --len )
            name.push_back( toupper( ifs.get()));

        mods.push_back( name );
    }

    ifs.close();

    ++depth;
    preSpaces += "  ";

    HMODULE hmod;
    std::vector< std::string > ex( excludes );

    // Exclude DLLs which were checked already. This prevents infinite
    // recursive calls caused by circular-dependencies.
    if( getExt( filepath ) == ".DLL")
        ex.push_back( getName( filepath ));

    int rc = 0;

    for( const auto &mod: mods )
    {
        std::cout << preSpaces << mod << " => ";

        if( DosLoadModule( szPath, sizeof( szPath ), mod.c_str(), &hmod ))
        {
            std::cerr << "Could not load due to " << szPath << std::endl;

            rc = 1;
        }
        else
        {
            DosQueryModuleName( hmod, sizeof( szPath ), szPath );

            // need to check ?
            if( std::find( ex.begin(), ex.end(), mod ) == ex.end())
                rc |= ldd( szPath, depth, ex ) != 0;
            else
                std::cout << szPath << std::endl;
        }
    }

    return rc;
}

int main( int argc, char *argv[])
{
    if( argc < 2 || argc > 3)
    {
        std::cerr << "Usage: " << argv[ 0 ] << " LX_filename [max depth]"
                  << std::endl;
        std::cerr << "Default max depth is 1." << std::endl;
        std::cerr << "If max depth is 0, then print all dependencies, "
                  << "recursively." << std::endl;

        return 1;
    }

    if( argc == 3 && isdigit( argv[ 2 ][ 0 ]))
        m_maxDepth = atoi( argv[ 2 ]);

    // DOSCALLS and KEE are not real DLLs. Exclude them at the beginning.
    return ldd( argv[ 1 ], 0, std::vector< std::string >{"DOSCALLS", "KEE"});
}
