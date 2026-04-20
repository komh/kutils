/*
 * KWatcomMapParser
 *
 * Copyright (C) 2026 KO Myung-Hun <komh78@gmail.com>
 *
 * This file is a part of K MapSym
 *
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://www.wtfpl.net/ for more details.
 */

/** @file */

#include "kwatcommapparser.h"

#include <iostream>
#include <string>
#include <string_view>
#include <filesystem>

/**
 * Check if @p sv starts with @p sub
 *
 * @param[in] sv    String to check
 * @param[in] sub   Sub-string to check
 * @return          true if @p sv starts with @p sub, otherwise false
 */
static inline bool startsWith( std::string_view sv, std::string_view sub )
{
    return sv.compare( 0, sub.length(), sub ) == 0;
}

/**
 * Print error
 *
 * @param[in] line  Line number
 * @return          false always
 */
static inline bool uel( std::string_view line )
{
    std::cerr << "Unexpected line: [" << line << "]\n";

    return false;
}

KWatcomMapParser::KWatcomMapParser( std::string_view fileName )
    : KMapParser( fileName )
{
}

KWatcomMapParser::~KWatcomMapParser()
{
}

bool KWatcomMapParser::parseLine( std::string_view line )
{
    if( line.empty())
        return true;

    size_t spos = std::string_view::npos;
    size_t pos = 0;
    std::vector< std::string_view > v;

    // split by space
    for( auto ch : line )
    {
        if( ch == ' ')
        {
            if( spos != std::string_view::npos )
            {
                v.push_back( line.substr( spos, pos - spos ));
                spos = std::string_view::npos;
            }
        }
        else if( spos == std::string_view::npos )
            spos = pos;

        ++pos;
    }

    if( spos != std::string_view::npos )
        v.push_back( line.substr( spos ));

    if( v.size() == 0 )
        return uel( line );

    if( v.front().compare("Group") == 0 )
        state( State::Groups );
    else if( v.front().compare("Segment") == 0 )
        state( State::Segments );
    else if( v.front().compare("Address") == 0 )
        state( State::PublicsByName );
    else if( v.front().compare("Symbol") == 0 )
        state( State::Imports );
    else if( line.find("Libraries Used") != std::string_view::npos )
        state( State::None );
    else if( line.find("Linker Statistics") != std::string_view::npos )
        state( State::None );
    else if( !( line[ 0 ] == ' ' || line[ 0 ] == '='
                || startsWith( line, "* = ") || startsWith( line, "+ = ")
                || startsWith( line, "Module: ")))
    {
        switch( state())
        {
            case State::Groups:
                if( v.size() != 3 )
                    return uel( line );

                groupCb({ std::string( v [ 1 ]), std::string( v[ 2 ]),
                          std::string( v[ 0 ])});
                break;

            case State::Segments:
            {
                if( v.size() != 5 )
                    return uel( line );

                int nBits = 0;  // unknown

                auto len = v[ 3 ].size();
                if( len == 13 )
                    nBits = 32;
                else if( len == 9 )
                    nBits = 16;

                if( nBits == 0 )
                    return uel( line );

                segmentCb({ std::string( v[ 3 ]), std::string( v[ 4 ]),
                            std::string( v[ 0 ]), std::string( v[ 1 ]),
                            nBits });
                break;
            }

            case State::PublicsByName:
            case State::PublicsByValue:
            {
                if( v.size() != 2 )
                    return uel( line );

                char ch = v[ 0 ].back();

                if( ch == '*' || ch == '+')
                  v[ 0 ].remove_suffix( 1 );

                publicCb({ std::string( v[ 0 ]), std::string( v[ 1 ])},
                         State::PublicsByName /* fake */);
                break;
            }

            case State::Imports:
                if( v.size() != 2 )
                    return uel( line );

                importCb({{}, std::string( v[ 0 ]), std::string( v[ 1 ]), {}});
                break;

            case State::None:
                if( startsWith( line, "Executable Image: "))
                    moduleCb( std::filesystem::path( line.substr( 18 )).stem()
                                .string());
                else if( startsWith( line, "Entry point address: "))
                    entryCb( v.back());
                // fall through

            default:
                // ignore
                break;
        }
    }

    return true;
}
