/*
 * KSymWriter
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

#ifndef KMAPSYM_KSYMWRITER_H
#define KMAPSYM_KSYMWRITER_H

#include "kmapparser.h"

#include <fstream>
#include <string>
#include <string_view>
#include <vector>
#include <map>

#include <cstdint>

/**
 * .SYM writer class
 */
class KSymWriter
{
public:
    /**
     * Constructor
     *
     * @param[in] symFileName .SYM file name
     */
    KSymWriter( std::string_view symFileNamee = {});

    /**
     * Destructor
     */
    ~KSymWriter();

    /**
     * Open a .SYM file to write
     *
     * @param[in] symFileName   .SYM file name to open
     * @return                  true if succeeds, otherwise false
     */
    bool open( std::string_view symFileName = {});

    /**
     * Close a .SYM file
     *
     * @return true if succeeds, otherwise false
     */
    bool close();

    /**
     * Write to a .SYM file
     *
     * @return true if succeeds, otherwise false
     */
    bool write();

    /**
     * Set module name
     */
    void setModuleName( std::string_view moduleName )
    {
        _moduleName = moduleName;
    }

    /**
     * Set entry point
     *
     * @param[in] entryPoint    Entry point address
     * @return                  true if succeeds, otherwise false
     */
    bool setEntryPoint( std::string_view entryPoint );

    /**
     * Set the flag to omit alphabetical sorting of symbols
     */
    void setOmitAlphaSort( bool omitAlphaSort )
    {
        _omitAlphaSort = omitAlphaSort;
    }

    /**
     * Add group to a segment list
     *
     * @param[in] grp   Group to add
     * @return          true if succeeds, otherwise false
     */
    bool addGroup( const KMapParser::Group& grp )
    {
        return addSegGrp({ grp.addr, grp.length.empty() ? "0" : grp.length,
                           grp.name}, true );
    }


    /**
     * Add segment to a segment list
     *
     * @param[in] seg   Segment to add
     * @return          true if succeeds, otherwise false
     * @remark          Ignores segments with non-zero offset
     */
    bool addSegment( const KMapParser::Segment& seg )
    {
        return addSegGrp( seg, false );
    }

    /**
     * Add symbol to a symbol list
     *
     * @param[in] sym   Symbol to add
     * @return          true if succeeds, otherwise false
     */
    bool addSymbol( const KMapParser::Public & sym );

private:
    /**
     * Segment structure
     */
    struct Segment
    {
        std::string name;   ///< name of the segment
        uint32_t length;    ///< length of the segment
    };

    /**
     * Symbol structure
     */
    struct Symbol
    {
        uint32_t addr;      ///< address of the symbol
        std::string name;   ///< name of the symbol
    };

    static constexpr uint32_t SEG0 = 0; ///< segment number of constants

    std::string _symFileName;   ///< .SYM file name
    std::ofstream _ofs;         ///< file stream for writing

    std::string _moduleName;    ///< module name
    uint16_t _entrySegNum;      ///< segment number containing the entry point

    bool _omitAlphaSort = false;    ///< flag to omit alphabetical sorting

    std::map< size_t, Segment > _segments;  ///< segment list

    using Symbols = std::vector< Symbol >;
    using SegmentSymbolsMap = std::map< size_t, Symbols >;

    Symbols _consts;                ///< constant list
    SegmentSymbolsMap _segSymsMap;  ///< symbol list

    size_t _maxSymNameLen = 0;  ///< max length of symbol names

    /**
     * Add segment or group to a segment list
     *
     * @param[in] seg   Segment to add
     * @param[in] grp   Group indicator. true for group, false for segment
     * @return          true if succeeds, otherwise false
     */
    bool addSegGrp( const KMapParser::Segment& seg, bool grp );

    /**
     * Write binary data to .SYM file
     *
     * @param[in] data  Data to write
     * @param[in] n     Size of data to write
     * @return          true if succeeds, otherwise false
     */
    bool writeData( const void *data, size_t n )
    {
        return static_cast< bool >(
                    _ofs.write( reinterpret_cast< const char * >( data ), n ));
    }

    /**
     * Write 8-bit value to .SYM file
     *
     * @param[in] u8    8-bit value to write
     * @return          true if succeeds, otherwise false
     */
    bool write8( uint8_t u8 ) { return writeData( &u8, sizeof( u8 )); }

    /**
     * Write 16-bit value to .SYM file
     *
     * @param[in] u16   16-bit value to write
     * @return          true if succeeds, otherwise false
     */
    bool write16( uint16_t u16 ) { return writeData( &u16, sizeof( u16 )); }

    /**
     * Write 32-bit value to .SYM file
     *
     * @param[in] u32   32-bit value to write
     * @return          true if succeeds, otherwise false
     */
    bool write32( uint32_t u32 ) { return writeData( &u32, sizeof( u32 )); }

    /**
     * Write string to .SYM file
     *
     * @param[in] sv    String to write
     * @return          true if succeeds, otherwise false
     */
    bool writeStr( std::string_view sv )
    {
        return write8( sv.size()) && writeData( sv.data(), sv.size());
    }

    /**
     * Write a symbol list to .SYM file
     *
     * @param[in] segNum        Segment number
     * @param[in] symbols       Symbols to write
     * @param[in] firstOfs      Offset to write the first symbol
     * @return                  true if succeeds, otherwise false
     */
    bool writeSymbols( size_t segNum, const Symbols& symbols, size_t firstOfs );

    /**
     * Write paddings to .SYM file
     *
     * @param[in] used      Used bytes to calculate the padding size
     * @param[in] ch        Char for padding
     * @param[in] align     Alignment value for padding
     * @return              true if succeeds, otherwise false
     */
    bool writePadding( size_t used, char ch = '\0', size_t align = 16 )
    {
        std::string padding(( align - ( used % align )) % align, ch );

        return writeData( padding.data(), padding.size());
    }
};

#endif
