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

#include "ksymwriter.h"

#include <iostream>
#include <charconv>
#include <filesystem>
#include <algorithm>

#ifdef DEBUG
#include <iomanip>
#endif

#pragma pack( push, 1 )

/******************************************************************************

.SYM file structure

     +-------------------+-------------------+
     | PART              | TYPE              |
     +-------------------+-------------------+
     | HEADER            | SymHeader         |
     +-------------------+-------------------+
     | CONSTANTS         | SymbolInfo        |
     +-------------------+-------------------+
     | OFFSET TABLE OF   | OffsetTable       |
     | CONSTANTS         |                   |
     |                   |                   |
     +---- PARAGRAPH(16 bytes) ALIGNMENT ----+
     |                   |                   |
     | SEGMENT HEADER    | SegmentInfo       |
     +-------------------+-------------------+
     | SEGMENT SYMBOLS   | SymbolInfo        |
     +-------------------+-------------------+
     | OFFSET TABLE OF   | OffsetTable       |
     | SYMBOLS           |                   |
     |                   |                   |
     +---- PARAGRAPH(16 bytes) ALIGNMENT ----+
     |                   |                   |
     |                 REPEAT                |
     |                   |                   |
     +---- PARAGRAPH(16 bytes) ALIGNMENT ----+
     |                   |                   |
     + LINE INFO         | Not used          |
     |                   |                   |
     +---- PARAGRAPH(16 bytes) ALIGNMENT ----+
     |                   |                   |
     | MARK OF END       | MSB 0x00 0x00 LSB |
     +-------------------+-------------------+
     | GENERATOR VERSION | MSB 0x05 0x01 LSB |
     +-------------------+-------------------+

* SymbolInfo
    type addr;
        type is uint16_t if all the constant values and the symbol addresses
                            are 16-bit (addrType == 2)
        type is uint32_t if any of the constant values and the symbol addresses
                            is 32-bit (addrType == 3)
        addr is a value if constant
        addr is an address if segment

    uint8_t nameLen;
        length of constant/symbol name

    uint8_t name[ nameLen ];
        constant/symbol name

* OffsetTable
    uint16_t offsetsByValue[ nSyms ];
        sorted by value in constants or address in segments

    uint16_t offsetsByName[ nSyms ];
        sorted by name converted to lowercase
        optional

        offset is to symbols from .SYM file header if constants
                             from segment header if segment symbols

******************************************************************************/

/**
 * Address type
 */
enum class AddrType : uint16_t {
    Bit16 = 2,  ///< 16-bit address
    Bit32 = 3   ///< 32-bit address
};

/**
 * Header of .SYM file
 */
struct SymHeader
{
    uint16_t fileSizePara;  ///< 00: .SYM file size in para
                            ///<     except mark of end and generator version
    AddrType addrType;      ///< 02: 2 for 16-bit, 3 for 32-bit
    uint16_t entrySegNum;   ///< 04: segment number containing the entry point
    uint16_t nConsts;       ///< 06: # of constants
    uint16_t headerSize;    ///< 08: size of header + constants in bytes
    uint16_t nSegs;         ///< 10: # of segments except constants
    uint16_t firstSegPara;  ///< 12: offset to the first segment in para from
                            ///<     the beginning of .SYM file
    uint8_t  maxSymNameLen; ///< 14: max. length of symbol names in .SYM file
    // followed by:
    // uint8_t  nameLen;            ///< 15: module name length
    // uint8_t  name[ nameLen ];    ///< 16: module name
};

/**
 * Segment info of .SYM file
 */
struct SegmentInfo
{
    uint16_t nextSegPara;   ///< 0: offset to next segment in para from the
                            ///<    beginning of .SYM file
    uint16_t nSyms;         ///< 2: number of symbols in this block
    uint16_t segSize;       ///< 4: size of segment header + symbols in bytes
    uint16_t segNum;        ///< 6: segment number
    uint16_t r8;            ///< 8: reserved
    uint16_t ra;            ///< 10: reserved
    uint16_t rc;            ///< 12: reserved
    AddrType addrType;      ///< 14: 2 for 16-bit, 3 for 32-bit
    uint16_t r10;           ///< 16: reserved
    uint16_t u12;           ///< 18: unknown, usually 0xFF00
    // followed by:
    // uint8_t nameLen;         ///< 20: segment name length
    // uint8_t name[ nameLen ]; ///< 21: segment name
};

#pragma pack( pop )

/**
 * Convert the length of the segment to AddrType
 *
 * @param[in] len   Length of the segment
 * @return          @c AddrType::Bit32 if @p len > 0xFFFF
 * @return          @c AddrType::Bit16 otherwise
 */
static inline AddrType l2a( size_t len )
{
    return len > 0xFFFF ? AddrType::Bit32 : AddrType::Bit16;
}

KSymWriter::KSymWriter( std::string_view symFileName )
    : _symFileName( symFileName )
{

}

KSymWriter::~KSymWriter()
{
    close();
}

bool KSymWriter::open( std::string_view symFileName )
{
    if( !symFileName.empty())
        _symFileName = symFileName;

    if( _symFileName.empty())
        return false;

    _ofs.open( std::string( _symFileName ), std::ios::out | std::ios::binary );
    if( !_ofs )
        return false;

    return true;
}

bool KSymWriter::close()
{
    _ofs.close();
    _ofs.clear();

    return true;
}

bool KSymWriter::write()
{
    if( _moduleName.empty())
        _moduleName = std::filesystem::path( _symFileName ).stem().string();

    // remove segments without any symbols
    for( auto it = _segSymsMap.begin(); it != _segSymsMap.end(); ++it )
    {
        if( it->second.empty())
            it = _segSymsMap.erase( it );
    }

    if( _segSymsMap.size() == 0 )
    {
        std::cerr << "No symbols found!!!\n";

        return false;
    }

#ifdef DEBUG
    std::cout << "Module: " << _moduleName << "\n";
    std::cout << "\n";

    for( const auto& segSyms: _segSymsMap )
    {
        std::cout << "Segment: " << _segments[ segSyms.first ].name << "\n";

        for( const auto& sym: segSyms.second )
        {
            std::cout << std::setfill('0') << std::setw( 4 )
                      << segSyms.first << ":"
                      << std::setw( 8 ) << std::hex << sym.addr << " "
                      << sym.name << "\n";
        }
        std::cout << "\n";
    }

    std::cout << "Segment containing entry point: "
              << _entrySegNum << "\n";
#endif

    // calculate the offset of the first symbol
    auto calcFirstSymOfs = []( const auto& t, std::string_view name )
    {
        return sizeof( t ) + sizeof( uint8_t ) + name.size();
    };

    // calculate the size of the symbol table
    auto calcSymSize = []( AddrType addrType,
                                 const Symbols& symbols )
    {
        size_t symSize = 0;

        for( const auto& sym: symbols )
        {
            symSize += ( addrType == AddrType::Bit32 ?
                         sizeof( uint32_t ) : sizeof( uint16_t )) +
                       sizeof( uint8_t ) + sym.name.size();
        }

        return symSize;
    };

    // calculate the size of the symbol offset table
    auto calcSymOfsSize = [ this ]( const Symbols& symbols )
    {
        /* for the table sorted by addr and by name */
        return symbols.size() * sizeof( uint16_t ) *
               ( this->_omitAlphaSort ? 1 : 2 );
    };

    // convert bytes to paragraphs
    auto b2p = []( size_t bytes )
    {
        return ( bytes + 15 ) / 16;
    };

    SymHeader header{};

    header.addrType = l2a( _consts.empty() ? 0 : _segments[ SEG0 ].length );
    header.entrySegNum = _entrySegNum;
    header.nConsts = _consts.size();
    header.headerSize = calcFirstSymOfs( header, _moduleName ) +
                        calcSymSize( header.addrType, _consts );
    header.nSegs = _segSymsMap.size();

    auto nextSeg = b2p( header.headerSize + calcSymOfsSize( _consts ));
    header.firstSegPara = nextSeg;

    header.maxSymNameLen = _maxSymNameLen;

    std::map< int, SegmentInfo > segs;

    for( const auto& segSyms: _segSymsMap )
    {
        SegmentInfo seg{};

        auto addrType = l2a( _segments[ segSyms.first ].length );

        seg.nSyms = segSyms.second.size();
        seg.segSize = calcFirstSymOfs( seg, _segments[ segSyms.first ].name ) +
                      calcSymSize( addrType, segSyms.second );
        seg.segNum = segSyms.first;
        seg.addrType = addrType;
        seg.u12 = 0xFF00;

        nextSeg += b2p( seg.segSize + calcSymOfsSize( segSyms.second ));
        seg.nextSegPara = nextSeg;

        segs[ seg.segNum ] = seg ;
    }

    // mark the last segment
    segs.rbegin()->second.nextSegPara = 0;

    header.fileSizePara = nextSeg;

    // write header
    writeData( &header, sizeof( header ));
    writeStr( _moduleName );

    // write constants
    writeSymbols( SEG0, _consts, calcFirstSymOfs( header, _moduleName ));

    // write padding
    writePadding( header.headerSize + calcSymOfsSize( _consts ));

    // write segments
    for( const auto& segSyms: _segSymsMap )
    {
        auto seg = segs[ segSyms.first ];
        auto segName = _segments[ segSyms.first ].name;

        // write segment
        writeData( &seg, sizeof( seg ));
        writeStr( segName );

        // write symbols
        writeSymbols( segSyms.first, segSyms.second,
                      calcFirstSymOfs( seg, segName ));

        // write padding
        writePadding( seg.segSize + calcSymOfsSize( segSyms.second ));
    }

    // write the mark of end
    write16( 0x0000 );

    // write .SYM file generator version info
    write16( 0x0501 );

    return true;
}

bool KSymWriter::setEntryPoint( std::string_view entryPoint )
{
    uint32_t segNum;

    // take segment number only
    auto res = std::from_chars( entryPoint.data(),
                                entryPoint.data() + entryPoint.size(),
                                segNum, 16 );
    if( !( res.ec == std::errc() && *res.ptr == ':'))
        return false;

    _entrySegNum = segNum;

    return true;
}

bool KSymWriter::addGroup( const KMapParser::Group& grp )
{
    return addSegment({ grp.addr, grp.length.empty() ? "0" : grp.length,
                        grp.name});
}

bool KSymWriter::addSegment( const KMapParser::Segment& seg  )
{
    uint32_t segNum, segOfs;

    auto res = std::from_chars( seg.addr.data(),
                                seg.addr.data() + seg.addr.size(), segNum, 16 );

    if( !( res.ec == std::errc() && *res.ptr == ':'))
        return false;

    // ignore 0000:xxxxxxxx
    if( segNum == SEG0 )
        return true;

    res = std::from_chars( res.ptr + 1, seg.addr.data() + seg.addr.size(),
                           segOfs, 16 );
    if( !( res.ec == std::errc() && *res.ptr == '\0'))
        return false;

    uint32_t segLen;

    res = std::from_chars( seg.length.data(),
                           seg.length.data() + seg.length.size(), segLen, 16 );
    if( !( res.ec == std::errc() && *res.ptr == '\0'))
        return false;

    auto it = _segments.find( segNum );

    if( it == _segments.end())
    {
        _segments[ segNum ] = { seg.name, segOfs + segLen };
        _segSymsMap[ segNum ] = {};
    }
    else if( it->second.length < segOfs + segLen )
        it->second.length = segOfs + segLen;

    return true;
}

bool KSymWriter::addSymbol( const KMapParser::Public& sym )
{
    uint32_t segNum, ofs;

    auto res = std::from_chars( sym.addr.data(),
                                sym.addr.data() + sym.addr.size(), segNum, 16 );
    if( !( res.ec == std::errc() && *res.ptr == ':'))
        return false;

    res = std::from_chars( res.ptr + 1, sym.addr.data() + sym.addr.size(),
                           ofs, 16 );
    if( !( res.ec == std::errc() && *res.ptr == '\0'))
        return false;

    // update maximum length of the symbol names
    auto len = sym.name.size();
    if( _maxSymNameLen < len)
        _maxSymNameLen = len;

    // constants ?
    if( segNum == SEG0 )
    {
        auto it = _segments.find( SEG0 );

        if( it == _segments.end())
            _segments[ SEG0 ] = {"CONSTANT", 0 };

        _consts.push_back({ ofs, sym.name });

        if( _segments[ SEG0 ].length < ofs )
            _segments[ SEG0 ].length = ofs;

        return true;
    }

    // no registered segments ?
    if( _segSymsMap.find( segNum ) == _segSymsMap.end())
        return false;

    _segSymsMap[ segNum ].push_back({ ofs, sym.name });

    return true;
}

bool KSymWriter::writeSymbols( size_t segNum, const Symbols& symbols,
                               size_t firstSymOfs )
{
    if( symbols.empty())
        return true;

    auto addrType = l2a( _segments[ segNum ].length );

    Symbols syms{ symbols };

    using SymOfsPair = std::pair< size_t, std::string >;
    std::vector< SymOfsPair > symOfsTbl;

    // write symbols and build symbol offset table sorted by address
    auto symOfs = firstSymOfs;

    for( const auto& sym: syms )
    {
        symOfsTbl.push_back( std::make_pair( symOfs, sym.name ));

        if( addrType == AddrType::Bit32 )
        {
            write32( sym.addr );
            symOfs += sizeof( uint32_t );
        }
        else
        {
            write16( sym.addr );
            symOfs += sizeof( uint16_t );
        }

        writeStr( sym.name );
        symOfs += sizeof( uint8_t ) + sym.name.size();
    }

    // write symbol offset table sorted by value
    for( const auto& sym: symOfsTbl )
        write16( sym.first );

    if( _omitAlphaSort )
        return true;

    // compare strings case-insensitively by converting to lowercase
    auto nameCmp = []( const auto& a, const auto& b )
    {
        std::string_view aname;
        std::string_view bname;
        std::string la;
        std::string lb;

        using T = std::decay_t< decltype( a )>;

        if constexpr( std::is_same_v< T, Symbol >)
        {
            aname = a.name;
            bname = b.name;
        }
        else
        {
            aname = a.second;
            bname = b.second;
        }

        la.resize( aname.size());
        lb.resize( bname.size());

        std::transform( aname.begin(), aname.end(), la.begin(), ::tolower );
        std::transform( bname.begin(), bname.end(), lb.begin(), ::tolower );

        auto cmp = la.compare( lb );
        return cmp < 0 || ( cmp == 0 && aname.compare( bname ) < 0 );
    };

    // sort symbol offset table by name
    std::sort( symOfsTbl.begin(), symOfsTbl.end(), nameCmp );

    // write symbol offset table sorted by name
    for( const auto& sym: symOfsTbl )
        write16( sym.first );

    return true;
}
