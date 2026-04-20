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
     * Address type
     */
    enum class AddrType : uint16_t {
        None,       ///< Unknown
        Bit16 = 2,  ///< 16-bit address
        Bit32 = 3   ///< 32-bit address
    };

    /**
     * Constructor
     *
     * @param[in] symFileName .SYM file name
     * @param[in] addrType    Address type
     */
    KSymWriter( std::string_view symFileNamee = {},
                AddrType addrType = AddrType::Bit32 );

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
     * Add group to a segment list
     *
     * @param[in] grp   Group to add
     * @return          true if succeeds, otherwise false
     */
    bool addGroup( const KMapParser::Group& grp );

    /**
     * Add segment to a segment list
     *
     * @param[in] seg   Segment to add
     * @return          true if succeeds, otherwise false
     * @remark          Ignores segments with non-zero offset
     */
    bool addSegment( const KMapParser::Segment& seg );

    /**
     * Add group to a group list
     *
     * @param[in] grp   Group to add
     * @return          true if succeeds, otherwise false
     */
    bool addSymbol( const KMapParser::Public & sym );

private:
    /**
     * Symbol structure
     */
    struct Symbol
    {
        uint32_t addr;      ///< address of the symbol
        std::string name;   ///< name of the symbol
    };

    std::string _symFileName;   ///< .SYM file name
    AddrType _addrType;         ///< address type
    std::ofstream _ofs;         ///< file stream for writing

    std::string _moduleName;    ///< module name
    uint16_t _entrySegNum;      ///< segment number containing the entry point

    std::vector< Symbol > _consts;                          ///< constant list
    std::map< uint32_t, std::string > _segments;            ///< segment list
    std::map< uint32_t, std::vector< Symbol >> _symbols;    ///< symbol list

    size_t _maxSymNameLen = 0;  ///< max length of symbol names

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
     * @param[in] symbols   Symbol list to write
     * @param[in] firstOfs  Offset to write the first symbol
     * @return              true if succeeds, otherwise false
     */
    bool writeSymbols( const std::vector< Symbol >& symbols, size_t firstOfs );

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
