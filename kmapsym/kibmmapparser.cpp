/*
 * KIbmMapParser
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

#include "kibmmapparser.h"

#include <iostream>

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

KIbmMapParser::KIbmMapParser( std::string_view fileName )
    : KMapParser( fileName )
{
}

KIbmMapParser::~KIbmMapParser()
{
}

bool KIbmMapParser::parseLine( std::string_view line )
{
    if( line.empty())
        return true;

    size_t spos = std::string::npos;
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
        return true;

    if( line[ 0 ] != ' ')
    {
        if( startsWith( line, "Program entry point at "))
            entryCb( v.back());

        return true;
    }
    else if( v.front().compare("Start") == 0 )
        state( State::Segments );
    else if( v.front().compare("Origin") == 0 )
        state( State::Groups );
    else if( v.back().compare("Name") == 0 )
        state( State::PublicsByName );
    else if( v.back().compare("Value") == 0 )
        state( State::PublicsByValue );
    else
    {
        switch( state())
        {
            case State::Segments:
            {
                if( v.size() < 4 || v.size() > 5)
                    return uel( line );

                // remove 'H' at the end
                if( v[ 1 ].back() == 'H')
                    v[ 1 ].remove_suffix( 1 );

                int nBits = 0;  // unknown

                if( v.size() > 4 )
                {
                    if( v[ 4 ].compare("32-bit") == 0 )
                        nBits = 32;
                    else  if( v[ 4 ].compare("16-bit") == 0 )
                        nBits = 16;
                }
                else if( v[ 0 ].size() == 13 )
                    nBits = 32;
                else if( v[ 0 ].size() == 9 )
                    nBits = 16;

                if( nBits == 0 )
                    return uel( line );

                segmentCb({ std::string( v[ 0 ]), std::string( v[ 1 ]),
                            std::string( v[ 2 ]), std::string( v[ 3 ]),
                            nBits });
                break;
            }

            case State::Groups:
                if( v.size() != 2 )
                    return uel( line );

                groupCb({ std::string( v[ 0 ]), {}, std::string( v[ 1 ])});
                break;

            case State::PublicsByName:
            case State::PublicsByValue:
            {
                std::string_view addr{ v[ 0 ]};
                std::string_view name;

                switch( v.size())
                {
                    case 2:
                        name = v[ 1 ];
                        break;

                    case 3:
                        if( v[ 1 ].compare("Abs") != 0 )
                            return uel( line );

                        name = v[ 2 ];
                        break;

                    case 4:
                    {
                        if( v[ 1 ].compare("Imp") != 0 )
                            return uel( line );

                        // remove '(' at the beginning
                        if( v[ 3 ].front() == '(')
                            v[ 3 ].remove_prefix( 1 );

                        // remove ')' at the end
                        if( v[ 3 ].back() == ')')
                            v[ 3 ].remove_suffix( 1 );

                        size_t dotPos = v[ 3 ].find_first_of('.');

                        importCb({ std::string( addr ),
                                   std::string( v[ 2 ]),
                                   std::string( v[ 3 ].substr( 0, dotPos )),
                                   std::string( v[ 3 ].substr( dotPos + 1 ))});

                        return true;
                    }

                    default:
                        return uel( line );
                }

                publicCb({ std::string( addr ), std::string( name )}, state());
                break;
            }

            case State::None:
                moduleCb( line.substr( 1 ));
                // fall through

            default:
                // ignore
                break;
        }
    }

    return true;
}
