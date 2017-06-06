/*
 * Directory reader
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

/** \file dirreader.cpp */

#include "dirreader.h"

#include <fnmatch.h>

#include <string>

/**
 * Constructor
 *
 * \param[in] dir Directory to read. "." if empty
 * \param[in] mask Mask to filter. "*" if empty
 */
DirReader::DirReader( const std::string& dir, const std::string& mask )
     : _dir( dir ), _mask( mask ), _dirp( NULL )
{
    if( _dir.empty())
        _dir = ".";

    if( _mask.empty())
        _mask = "*";

    _dirp = opendir( _dir.c_str());
}

/**
 * Destructor
 */
DirReader::~DirReader()
{
    if( _dirp != NULL )
        closedir( _dirp );
}

/**
 * Determine if next entry is available
 *
 * \return True if available, otherwise false
 */
bool DirReader::hasNext()
{
    return _next.attr() != DirEntry::Invalid || readNext();
}

/**
 * Get a next entry
 *
 * \return Found entry if available. Otherwise invalid entry
 */
DirEntry DirReader::next()
{
    // if not ready, read first
    if( _next.attr() == DirEntry::Invalid )
        readNext();

    // save the found entry
    DirEntry result( _next );

    // read a next entry
    readNext();

    // return the found entry
    return result;
}

/**
 * Read next entry
 *
 * \return True if successful, otherwise false
 */
bool DirReader::readNext()
{
    // initial state is Invalid
    _next.setAttr( DirEntry::Invalid );

    if( _dirp == NULL )
        return false;

    struct dirent *d;

    // keep reading until finding matched entry or reaching to the end
    do
    {
        d = readdir( _dirp );
    } while( d != NULL
             && fnmatch( _mask.c_str(), d->d_name,
                         _FNM_OS2 | _FNM_IGNORECASE ) != 0 );

    // reached to end ?
    if( d == NULL )
        return false;

    // fill name and attributes of an entry
    _next.setName( d->d_name );

    int attr = 0;

    if ( d->d_attr & A_RONLY )
        attr |= DirEntry::ReadOnly;
    if ( d->d_attr & A_HIDDEN )
        attr |= DirEntry::Hidden;
    if ( d->d_attr & A_SYSTEM )
        attr |= DirEntry::System;
    if ( d->d_attr & A_LABEL )
        attr |= DirEntry::Label;
    if ( d->d_attr & A_DIR )
        attr |= DirEntry::Directory;
    if ( d->d_attr & A_ARCHIVE )
        attr |= DirEntry::Archive;

    _next.setAttr( attr );

    return true;
}
