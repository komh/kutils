/*
 * K MED Filter: Convert '/' in a file path to '\\', and use CRLF as EOL
 *
 * Copyright (C) 2026 KO Myung-Hun <komh78@gmail.com>
 *
 * This file is a part of KMedFilter
 *
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://www.wtfpl.net/ for more details.
 */

/** @file */

#include <iostream>
#include <string>
#include <algorithm>

int main()
{
    std::string line;

    while( std::getline( std::cin, line ))
    {
        auto pos = line.find_first_of(':');

        if( pos != std::string::npos)
        {
            // x:/path/to/file case ?
            if( pos == 1 )
                pos = line.find_first_of(':', pos + 1 );

            if( pos != std::string::npos )
                std::replace_if( line.begin(), line.begin() + pos,
                                 []( char ch ) { return ch == '/'; }, '\\');
        }

        std::cout << line << "\n";
    }

    return 0;
}
