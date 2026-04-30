/*
 * KVerbose
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

#ifndef KMAPSYM_KVERBOSE_H
#define KMAPSYM_KVERBOSE_H

#include <iostream>

/**
 * KVerbose class
 */
class KVerbose
{
public:
    /**
     * Verbose level
     */
    enum class Level
    {
        Normal,     ///< normal
        Info,       ///< information
        Debug       ///< debug
    };

    /**
     * Singleton instance
     */
    static KVerbose& instance()
    {
        static KVerbose verbose;

        return verbose;
    }

    /**
     * Copy constructor
     */
     KVerbose( const KVerbose & ) = delete;

    /**
     * operator=
     */
     KVerbose& operator=( const KVerbose & ) = delete;

    /**
     * Getter of _level
     */
    Level level() const { return _level; }

    /**
     * Setter of _level
     */
    void level( Level lv ) { _level = lv; }

    /**
     * Print messages to std::cout
     */
    std::ostream& out()
    {
        if( _level >= Level::Normal )
            return std::cout;

        return _nullStream;
    }

    /**
     * Print messages to std::cerr
     */
    std::ostream& err()
    {
        if( _level >= Level::Normal )
            return std::cerr;

        return _nullStream;
    }

    /**
     * Print information messages to std::cout
     */
    std::ostream& info()
    {
        if( _level >= Level::Info )
            return std::cout;

        return _nullStream;
    }

    /**
     * Print debug messages to std::cout
     */
    std::ostream& debug()
    {
        if( _level >= Level::Debug )
            return std::cout;

        return _nullStream;
    }

private:
    /**
     * NullBuffer class to discard messages
     */
    struct NullBuffer: std::streambuf
    {
        /**
         * Discard all outputs
         */
        int overflow( int c ) override { return c; }
    };

    NullBuffer _nullBuffer;     ///< null buffer
    std::ostream _nullStream;   ///< null stream
    Level _level;               ///< Verbose level

    /**
     * Constructor
     */
    KVerbose( Level level = Level::Normal )
        : _nullStream( &_nullBuffer )
        , _level( level )
    {}
};

//extern KVerbose g_verbose;      ///< global instance for verbose

#endif
