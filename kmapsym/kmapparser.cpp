/*
 * KMapParser
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

#include "kmapparser.h"

#include <algorithm>

#include <cctype>

KMapParser::KMapParser( std::string_view fileName )
    : _fileName( fileName )
    , _state( State::None )
{
}

KMapParser::~KMapParser()
{
    close();
}

bool KMapParser::open( std::string_view fileName )
{
    if( !fileName.empty())
        _fileName = fileName;

    if( _fileName.empty())
        return false;

    _ifs.open( std::string( _fileName ), std::ios::in );
    if( !_ifs )
        return false;

    return true;
}

bool KMapParser::close()
{
    _ifs.close();
    _ifs.clear();

    return true;
}

bool KMapParser::parse()
{
    std::string line;

    state( State::None );

    while( std::getline( _ifs, line ))
    {
        if( !parseLine( line ))
            return false;
    }

    if(( _publicsByName.empty() && _publicsByValue.empty())
       || ( !_publicsByName.empty() && !_publicsByValue.empty()))
        return true;

    // compare strings case-insensitively by converting to uppercase
    auto nameCmp = []( const Public& a, const Public& b )
    {
        std::string ua;
        std::string ub;

        ua.resize( a.name.size());
        ub.resize( b.name.size());

        std::transform( a.name.begin(), a.name.end(), ua.begin(),
                        ::toupper );
        std::transform( b.name.begin(), b.name.end(), ub.begin(),
                        ::toupper );

        auto cmp = ua.compare( ub );
        return cmp < 0 || ( cmp == 0 && a.name.compare( b.name ) < 0 );
    };

    if( _publicsByValue.empty())
    {
        // only _publicsByName was populated

        // watcom map, whose publics are not sorted by name neither by value

        // sort by name
        std::sort( _publicsByName.begin(), _publicsByName.end(), nameCmp );

        _publicsByValue = _publicsByName;

        // sort by value
        std::sort( _publicsByValue.begin(), _publicsByValue.end(),
                   [ &nameCmp ]( const Public& a, const Public& b )
        {
            auto cmp = a.addr.compare( b.addr );
            return cmp < 0 || ( cmp == 0 && nameCmp( a, b ));
        });
    }

    if( _publicsByName.empty())
    {
        // only _publicsByValue was populated

        // no real cases, yet

        _publicsByName = _publicsByValue;

        // sort by name
        std::sort( _publicsByName.begin(), _publicsByName.end(), nameCmp );
    }

    return true;
}

void KMapParser::moduleCb( std::string_view module )
{
    _moduleName = module;
}

void KMapParser::segmentCb( const Segment& segment )
{
    _segments.push_back( segment );
}

void KMapParser::groupCb( const Group& group)
{
    _groups.push_back( group );
}

void KMapParser::publicCb( const Public& pub, State st )
{
    switch( st )
    {
        case State::PublicsByName:
            _publicsByName.push_back( pub );
            break;

        case State::PublicsByValue:
            _publicsByValue.push_back( pub );
            break;

        default:
            // ignore
            break;
    }
}

void KMapParser::importCb( const Import& imp )
{
    _imports.push_back( imp );
}

void KMapParser::entryCb( const std::string_view entry )
{
    _entryPoint = entry;
}
