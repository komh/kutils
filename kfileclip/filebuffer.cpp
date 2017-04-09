/*
 * FileBuffer
 *
 * Copyright (C) 2017 KO Myung-Hun <komh@chollian.net>
 *
 * This file is a part of K File Clip.
 *
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://www.wtfpl.net/ for more details.
 */

/** \file filebuffer.cpp */

#include "filebuffer.h"

#include <io.h>

#include <fstream>
#include <iostream>
#include <string>

/**
 * Read all of a given file into a buffer
 */
bool FileBuffer::read()
{
    std::ifstream ifs;

    _buffer.clear();
    // a file was given ?
    if( !_fileName.empty())
    {
        // open it
        ifs.open( _fileName );

        // allocate buffer as many as file size
        ifs.seekg( 0, ifs.end );
        _buffer.reserve( ifs.tellg());
        ifs.seekg( 0, ifs.beg );
    }

    // read from stdin unless file name was given, or from a given file
    std::istream& is = _fileName.empty() ? std::cin : ifs;

    std::string s;

    // read all of a file
    char ch = is.get();
    while( is.good())
    {
        // convert LF to CR+LF
        if( ch == '\n')
            _buffer.push_back('\r');

        _buffer.push_back( ch );

        // write a character only if not redirected
        if( !isatty( 0 ))
            std::cout << ch;

        ch = is.get();
    }

    bool ok = is.good();

    // close a stream if a file
    if( !_fileName.empty())
    {
        ifs.close();

        ok = ok && ifs.good();
    }

    return ok;
}
