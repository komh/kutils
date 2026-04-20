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

#ifndef KMAPSYM_KMAPPARSER_H
#define KMAPSYM_KMAPPARSER_H

#include <fstream>

#include <string>
#include <string_view>
#include <vector>

/**
 * .MAP file parser base class
 */
class KMapParser
{
public:
    /**
     * Segment structure
     */
    struct Segment
    {
        std::string addr;       ///< address of the segment
        std::string length;     ///< length of the segment
        std::string name;       ///< name of the segment
        std::string className;  ///< class name of the segment
        int nBits;              ///< # of bits of the segment.
                                ///< 0: unknown, n: n-bits
    };

    /**
     * Group structure
     */
    struct Group
    {
        std::string addr;       ///< address of the group
        std::string length;     ///< length of the group, maybe empty
        std::string name;       ///< name of the group
    };

    /**
     * Public symbol structure
     */
    struct Public
    {
        std::string addr;       ///< address or value of the symbol
        std::string name;       ///< name of the symbol
    };

    /**
     * Import structure
     */
    struct Import
    {
        std::string addr;           ///< address of the import, maybe empty
        std::string name;           ///< name of the import
        std::string dllName;        ///< dll name of the import
        std::string dllOrdOrExp;    ///< ordinal of export entry of the import.
                                    ///< may be empty
    };

    /**
     * Constructor
     *
     * @param[in] fileName  .MAP file name
     */
    KMapParser( std::string_view fileName = {});

    /**
     * Destructor
     */
    virtual ~KMapParser();

    /**
     * Open a .MAP file
     *
     * @param[in] fileName  .MAP file name to open
     * @return              true if success, otherwise false
     */
    bool open( std::string_view fileName = {});

    /**
     * Close a .MAP file
     *
     * @return true if success, otherwise false
     */
    bool close();

    /**
     * Parse a .MAP file
     *
     * @return true if success, otherwise false
     */
    bool parse();

    /**
     * Get the module name
     *
     * @return module name
     */
    const std::string& moduleName() const { return _moduleName; }

    /**
     * Get the segment list
     */
    const std::vector< Segment >& segments() const { return _segments; }

    /**
     * Get the group list
     */
    const std::vector< Group >& groups() const { return _groups; }

    /**
     * Get the public symbols sorted by name
     */
    const std::vector< Public >& publicsByName() const
    {
        return _publicsByName;
    }

    /**
     * Get the public symbols sorted by address or value
     */
    const std::vector< Public >& publicsByValue() const
    {
        return _publicsByValue;
    }

    /**
     * Get the import list
     */
    const std::vector< Import >& imports() const { return _imports; }

    /**
     * Get the entry point
     */
    const std::string& entryPoint() const { return _entryPoint; }

protected:
    /**
     * Parser state
     */
    enum class State {
        None,               ///< Parsing non-state blocks
        Segments,           ///< Parsing segments block
        Groups,             ///< Parsing groups block
        PublicsByName,      ///< Parsing publics by name block
        PublicsByValue,     ///< Parsing publics by value block
        Imports             ///< Parsing imports block
    };

    /**
     * Get the .MAP file name
     */
    const std::string& fileName() const { return _fileName; }

    /**
     * Get the current parser state
     */
    State state() const { return _state; }

    /**
     * Set the current parser state
     */
    void state( State state ) { _state = state; }

    /**
     * Line parser
     *
     * @param[in] line  Line to parse
     */
    virtual bool parseLine( std::string_view line ) = 0;

    /**
     * Callback for module name
     *
     * @param[in] module    Module name
     */
    virtual void moduleCb( std::string_view module );

    /**
     * Callback for segment
     *
     * @param[in] segment   Segment info
     */
    virtual void segmentCb( const Segment& segment );

    /**
     * Callback for group
     *
     * @param[in] group     Group info
     */
    virtual void groupCb( const Group& group );

    /**
     * Callback for public symbol
     *
     * @param[in] pub   Public symbol info
     * @param[in] st    Parser state.
     *                  @ref State::PublicsByName or @ref State::PublicsByValue
     */
    virtual void publicCb( const Public& pub, State st );

    /**
     * Callback for import
     *
     * @param[in] imp   Import info
     */
    virtual void importCb( const Import& imp );

    /**
     * Callback for entry point
     *
     * @param[in] entry     Address of entry point. xxxx:yyyy or xxxx:yyyyyyyy
     */
    virtual void entryCb( std::string_view entry );

private:
    std::string _fileName;  ///< .MAP file name
    std::ifstream _ifs;     ///< file stream for reading
    State _state;           ///< parser state

    std::string _moduleName;                ///< module name
    std::vector< Segment > _segments;       ///< segment list
    std::vector< Group > _groups;           ///< group list
    std::vector< Public > _publicsByName;   ///< public list sorted by name
    std::vector< Public > _publicsByValue;  ///< public list sorted by address
                                            ///< or value
    std::vector< Import > _imports;         ///< import list
    std::string _entryPoint;                ///< entry point address
};

#endif
