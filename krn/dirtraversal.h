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

/** \file dirtraversal.h */

#ifndef KRN_DIRTRAVERSAL_H
#define KRN_DIRTRAVERSAL_H

#include "direntry.h"

#include <functional>
#include <string>

/**
 * Directory Traversal
 */
class DirTraversal
{
public:
    /**
     * Constructor
     *
     * \param[in] dir Directory to traverse. "." if empty
     * \param[in] mask Mask to filter. "*" if empty
     */
    DirTraversal( const std::string& dir = std::string(),
                  const std::string& mask = std::string());

    /**
     * Traverse a directory
     *
     * \param[in] func Function called every entry
     * \param[in] subdir If true, traverse sub-directories, too
     */
    void traverse( const std::function< void( const std::string&,
                                              const DirEntry& )> func,
                   bool subdir = false );

private:
    /**
     * Traverse a directory
     *
     * This is a overloaded function which is used internally
     *
     * \param[in] dir Directory to traverse
     * \param[in] func Function called every entry
     * \param[in] subdir If true, traverse sub-directories, too
     */
    void traverse( const std::string dir,
                   const std::function< void( const std::string&,
                                              const DirEntry& )> func,
                   bool subdir );

    std::string _dir;   ///< Directory to traverse
    std::string _mask;  ///< Mask to filter
};

#endif
