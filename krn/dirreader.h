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

/** \file dirreader.h */

#ifndef KRN_DIRREADER_H
#define KRN_DIRREADER_H

#include "direntry.h"

#include <sys/types.h>
#include <dirent.h>

#include <string>

/**
 * Directory reader
 */
class DirReader
{
public:
    /**
     * Constructor
     *
     * \param[in] dir Directory to read. "." if empty
     * \param[in] mask Mask to filter. "*" if empty
     */
    DirReader( const std::string& dir = std::string(),
               const std::string& mask = std::string());
    /**
     * Destructor
     */
    ~DirReader();

    /**
     * Determine if next entry is available
     *
     * \return True if available, otherwise false
     */
    bool hasNext();

    /**
     * Get a next entry
     *
     * \return Found entry if available. Otherwise invalid entry
     */
    DirEntry next();

private:
    std::string _dir;   ///< Directory to read
    std::string _mask;  ///< Mask to filter
    DIR *_dirp;         ///< Directory handle

    DirEntry _next;     ///< Next entry

    /**
     * Read next entry
     *
     * \return True if successful, otherwise false
     */
    bool readNext();
};

#endif
