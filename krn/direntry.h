/*
 * Directory entry
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

/** \file direntry.h */

#ifndef KRN_DIRENTRY_H
#define KRN_DIRENTRY_H

#include <string>

/**
 * Directory entry
 */
class DirEntry
{
public:
    /**
     * Attributes of entry
     */
    enum
    {
        ReadOnly    = 0x0001,   ///< Read only
        Hidden      = 0x0002,   ///< Hidden
        System      = 0x0004,   ///< System
        Label       = 0x0008,   ///< Volume label
        Directory   = 0x0010,   ///< Directory
        Archive     = 0x0020,   ///< Archive
        Invalid     = 0xFFFF    ///< Invalid
    };

    /**
     * Constructor
     */
    DirEntry() : _attr( Invalid ) {}

    /**
     * Copy constructor
     */
    DirEntry( const DirEntry & ) = default;

    /**
     * Check if directory
     *
     * \return True if directory, otherwise false
     */
    bool isDirectory() const { return ( _attr & Directory ) == Directory; }

    /**
     * Get a name of an entry
     *
     * \return Name of an entry
     */
    std::string name() const { return _name; }

    /**
     * Set a name of an entry
     *
     * \param[in] name Name of an entry
     */
    void setName( const std::string& name ) { _name = name; }

    /**
     * Get attributes of an entry
     *
     * \return Attributes of an entry
     */
    int attr() const { return _attr; }

    /**
     * Set attributes of an entry
     *
     * \param[in] attr Attributes of an entry
     */
    void setAttr( int attr ) { _attr = attr; }

private:
    std::string _name;  ///< Name of an entry
    int _attr;          ///< Attributes of an entry
};

#endif
