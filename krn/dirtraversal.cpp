/*
 * Directory traversal
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

/** \file dirtraversal.cpp */

#include "dirtraversal.h"

#include "dirreader.h"

#include <fnmatch.h>

#include <string>
#include <vector>

/**
 * Constructor
 *
 * \param[in] dir Directory to traverse. "." if empty
 * \param[in] mask Mask to filter. "*" if empty
 */
DirTraversal::DirTraversal( const std::string& dir, const std::string& mask )
    : _dir( dir ), _mask( mask )
{
    if( _dir.empty())
        _dir = ".";

    if( _mask.empty())
        _mask = "*";
}

/**
 * Traverse a directory
 *
 * \param[in] func Function called every entry
 * \param[in] subdir If true, traverse sub-directories, too
 */
void DirTraversal::traverse( const std::function<
                                void( const std::string &, const DirEntry& )>
                                    func,
                             bool subdir )
{
    traverse( _dir, func, subdir );
}

/**
 * Read all entries of the given directory, \a dir
 *
 * \param[in] dir Directory to read
 * \param[in] mask Mask to filter
 * \param[out] v Vector to store the entries
 */
static void readEntries( const std::string& dir, const std::string& mask,
                         std::vector< DirEntry > *v )
{
    DirReader dr( dir, mask );

    while( dr.hasNext())
    {
        DirEntry entry( dr.next());

        // exclude . and ..
        if( !entry.isDirectory()
            || ( entry.name() != "." && entry.name() != ".."))
            v->push_back( entry );
    }
}

/**
 * Traverse a directory
 *
 * This is a overloaded function which is used internally
 *
 * \param[in] dir Directory to traverse
 * \param[in] func Function called every entry
 * \param[in] subdir If true, traverse sub-directories, too
 */
void DirTraversal::traverse( const std::string dir,
                             const std::function<
                                void( const std::string&, const DirEntry& )>
                                    func,
                             bool subdir )
{
    std::vector< DirEntry > entries;

    // Read entries first, then call the callback function.
    // This prevents from infinite recursive calling
    readEntries( dir, _mask, &entries );

    for( const auto& entry : entries )
    {
        // call the callback function only with matched entry
        if( fnmatch( _mask.c_str(), entry.name().c_str(),
                     _FNM_OS2 | _FNM_IGNORECASE ) == 0 )
            func( dir, entry );
    }

    // sub-directories requested ?
    if( subdir )
    {
        // find sub-directories and traverse them
        entries.clear();

        readEntries( dir, "*", &entries );

        for( const auto& entry : entries )
        {
            if( entry.isDirectory())
                traverse( dir + "/" + entry.name(), func, subdir );
        }
    }
}

