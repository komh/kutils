/*
 * ErrorMessage
 *
 * Copyright (C) 2024 KO Myung-Hun <komh78@gmail.com>
 *
 * This file is a part of K Error.
 *
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://www.wtfpl.net/ for more details.
 */

/** \file */

#ifndef KERROR_ERRORMESSAGE_H
#define KERROR_ERRORMESSAGE_H

#include <string>
#include <vector>
#include <algorithm>

#define ERROR_ITEM( e, s ) { e, #e, s }

struct error_item
{
    int error;
    const std::string name;
    const std::string message;
};

typedef std::vector< error_item > error_list;

/**
 * ErrorMessage class
 */
class ErrorMessage
{
public:
    /**
     * Constructor
     */
    ErrorMessage() {}

    /**
     * Destructor
     */
    virtual ~ErrorMessage() {}

    /**
     * Find an error with an error code \a error.
     *
     * \param[in] error error code to find
     */
    void find( int error )
    {
        _result.clear();

        std::for_each( errors().cbegin(), errors().cend(),
                       [ this, error ]( const auto& item ) {
                        if( error == item.error )
                            _result.push_back( item );
                       });
    }

    /**
     * Find an error with an error name \a name
     *
     * \param[in] name error name to find
     */
    void find( const std::string& name )
    {
        _result.clear();

        std::for_each( errors().cbegin(), errors().cend(),
                       [ this, name ]( const auto& item ) {
                        if( name.length() == item.name.length()
                            && std::equal( name.cbegin(), name.cend(),
                                           item.name.cbegin(),
                                           []( const auto& a,
                                               const auto& b ) {
                                                return ::toupper( a )
                                                       == ::toupper( b );
                                           }))
                            _result.push_back( item );
                    });
    }

    /**
     * Get the found result
     *
     * \return The found result
     */
    const error_list& result() const { return _result; }

protected:
    /**
     * Get an error list
     *
     * \return Supported error list
     */
    virtual const error_list& errors() const = 0;

private:
    error_list _result; ///< Where to store the found result
};

#endif
