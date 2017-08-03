/*
 * LxHeader
 *
 * Copyright (C) 2017 KO Myung-Hun <komh@chollian.net>
 *
 * This file is a part of KLxHdr.
 *
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://www.wtfpl.net/ for more details.
 */

/** \file lxheader.cpp */

#include "lxheader.h"

#include <exe386.h> // OS/2 LX header
#include <newexe.h> // DOS MZ header

#include <cstring>

#include <fstream>

/**
 * Cast const std::vector< char >& to const e32_exe&
 *
 * \param[in] v Variable to cast
 * \return Casted const e32_exe& variable
 */
static inline const e32_exe& e32( const std::vector< char >& v )
{
    return *reinterpret_cast< const e32_exe* >( v.data());
}

/**
 * LxHeader constructor
 *
 * \param[in] filename Filename to read LX header
 */
LxHeader::LxHeader( const std::string& filename )
    : _filename( filename ), _dosData( sizeof( exe_hdr )),
      _lxData( sizeof( e32_exe )), _dosStub( false ), _lx( false ),
      _lxOffset( -1 )
{
    if( !_filename.empty())
        setFilename( filename );
}

/**
 * LxHeader destructor
 */
LxHeader::~LxHeader()
{
}

/**
 * Read LX header of the given filename
 *
 * \param[in] filename Filename to read
 * \return true if read header, otherwise false
 */
bool LxHeader::setFilename( const std::string& filename )
{
    if( filename.empty())
        return false;

    // initialize member variables
    _filename = filename;
    _dosData.clear();
    _dosData.resize( sizeof( exe_hdr ));
    _lxData.clear();
    _lxData.resize( sizeof( e32_exe ));
    _dosStub = false;
    _lx = false;
    _lxOffset = -1;

    std::ifstream ifs;

    ifs.open( filename, ifs.in | ifs.binary );
    if( !ifs.is_open())
        return false;

    // check DOS stub header, first
    ifs.read( _dosData.data(), _dosData.size());
    if( !ifs.good())
        return false;

    long lxHdrPos = 0;

    if(( _dosData[ 0 ] | ( _dosData[ 1 ] << 8 )) == EMAGIC )
    {
        exe_hdr &dosHdr = *reinterpret_cast< exe_hdr* >( _dosData.data());

        _dosStub = dosHdr.e_lfarlc == 0x40;
        if( _dosStub )
            lxHdrPos = dosHdr.e_lfanew;
    }

    // read LX header
    _lxOffset = lxHdrPos;

    ifs.seekg( lxHdrPos );
    if( !ifs.good())
        return false;

    ifs.read( _lxData.data(), _lxData.size());
    if( !ifs.good())
        return false;

    // check if LX header really
    _lx = ( _lxData[ 0 ] | ( _lxData[ 1 ] << 8 )) == E32MAGIC;

    ifs.close();

    // All done. Return true regardless of the result of ifs.good()
    return true;
}

/**
 * Get signature of DOS stub header
 *
 * \return Signature of DOS stub header, that is, MZ
 */
std::string LxHeader::dosMagic() const
{
    return std::string{ _dosData[ 0 ], _dosData[ 1 ]};
}

/**
 * Get signature of LX header
 *
 * \return Signature of LX header, that is, LX
 */
std::string LxHeader::lxMagic() const
{
    return std::string{ _lxData[ 0 ], _lxData[ 1 ]};
}

/**
 * Get byte-ordering
 *
 * \return Byte-ordering
 */
unsigned LxHeader::byteOrder() const
{
    return e32( _lxData ).e32_border;
}

/**
 * Get word-ordering
 *
 * \return Word-ordering.
 */
unsigned LxHeader::wordOrder() const
{
    return e32( _lxData ).e32_worder;
}

/**
 * Get EXE format level
 *
 * \return EXE format level
 */
unsigned long LxHeader::level() const
{
    return e32( _lxData ).e32_level;
}

/**
 * Get CPU type
 *
 * \return CPU type
 */
unsigned LxHeader::cpu() const
{
    return e32( _lxData ).e32_cpu;
}

/**
 * Get OS type
 *
 * \return OS type
 */
unsigned LxHeader::os() const
{
    return e32( _lxData ).e32_os;
}

/**
 * Get module version
 *
 * \return Module version
 */
unsigned long LxHeader::modVer() const
{
    return e32( _lxData ).e32_ver;
}

/**
 * Get module flags
 *
 * \return Module flags
 */
unsigned long LxHeader::modFlags() const
{
    return e32( _lxData ).e32_mflags;
}

/**
 * Get module # pages
 *
 * \return module # pages
 */
unsigned long LxHeader::modPages() const
{
    return e32( _lxData ).e32_mpages;
}

/**
 * Get object # for instruction pointer
 *
 * \return Object # for instruction pointer
 */
unsigned long LxHeader::startObj() const
{
    return e32( _lxData ).e32_startobj;
}

/**
 * Get EIP
 *
 * \return EIP
 */
unsigned long LxHeader::eip() const
{
    return e32( _lxData ).e32_eip;
}

/**
 * Get object # for stack pointer
 *
 * \return Object # for stack pointer
 */
unsigned long LxHeader::stackObj() const
{
    return e32( _lxData ).e32_stackobj;
}

/**
 * Get ESP
 *
 * \return ESP
 */
unsigned long LxHeader::esp() const
{
    return e32( _lxData ).e32_esp;
}

/**
 * Get .EXE page size
 *
 * \return .EXE page size
 */
unsigned long LxHeader::pageSize() const
{
    return e32( _lxData ).e32_pagesize;
}

/**
 * Get page alignment shift in .EXE
 *
 * \return Page alignment shift in .EXE
 */
unsigned long LxHeader::pageShift() const
{
    return e32( _lxData ).e32_pageshift;
}

/**
 * Get fixup section size
 *
 * \return Fixup section size
 */
unsigned long LxHeader::fixupSize() const
{
    return e32( _lxData ).e32_fixupsize;
}

/**
 * Get fixup section checksum
 *
 * \return Fixup section checksum
 */
unsigned long LxHeader::fixupSum() const
{
    return e32( _lxData ).e32_fixupsum;
}

/**
 * Get loader section size
 *
 * \return Loader section size
 */
unsigned long LxHeader::ldrSize() const
{
    return e32( _lxData ).e32_ldrsize;
}

/**
 * Get loader section checksum
 *
 * \return Loader section checksum
 */
unsigned long LxHeader::ldrSum() const
{
    return e32( _lxData ).e32_ldrsum;
}

/**
 * Get object table offset
 *
 * \return Object table offset
 */
unsigned long LxHeader::objTable() const
{
    return e32( _lxData ).e32_objtab;
}

/**
 * Get number of objects in module
 *
 * \return Number of objects in module
 */
unsigned long LxHeader::objCount() const
{
    return e32( _lxData ).e32_objcnt;
}

/**
 * Get object page map offset
 *
 * \return Object page map offset
 */
unsigned long LxHeader::objMap() const
{
    return e32( _lxData ).e32_objmap;
}

/**
 * Get object iterated data map offset
 *
 * \return Object iterated data map offset
 */
unsigned long LxHeader::iterMap() const
{
    return e32( _lxData ).e32_itermap;
}

/**
 * Get offset of resource table
 *
 * \return Offset of resource table
 */
unsigned long LxHeader::rsrcTable() const
{
    return e32( _lxData ).e32_rsrctab;
}

/**
 * Get number of resource entries
 *
 * \return Number of resource entries
 */
unsigned long LxHeader::rsrcCount() const
{
    return e32( _lxData ).e32_rsrccnt;
}

/**
 * Get offset of resident name table
 *
 * \return Offset of resident name table
 */
unsigned long LxHeader::resTable() const
{
    return e32( _lxData ).e32_restab;
}

/**
 * Get offset of entry table
 *
 * \return Offset of entry table
 */
unsigned long LxHeader::entryTable() const
{
    return e32( _lxData ).e32_enttab;
}

/**
 * Get offset of module directive table
 *
 * \return Offset of module directive table
 */
unsigned long LxHeader::dirTable() const
{
    return e32( _lxData ).e32_dirtab;
}

/**
 * Get number of module directives
 *
 * \return Number of module directive
 */
unsigned long LxHeader::dirCount() const
{
    return e32( _lxData ).e32_dircnt;
}

/**
 * Get offset of fixup page table
 *
 * \return Offset of fixup page table
 */
unsigned long LxHeader::fixupPageTable() const
{
    return e32( _lxData ).e32_fpagetab;
}

/**
 * Get offset of fixup record table
 *
 * \return Offset of fixup record table
 */
unsigned long LxHeader::fixupRecTable() const
{
    return e32( _lxData ).e32_frectab;
}

/**
 * Get offset of import module name table
 *
 * \return Offset of import module name table
 */
unsigned long LxHeader::impMod() const
{
    return e32( _lxData ).e32_impmod;
}

/**
 * Get number of entries in import module name table
 *
 * \return Number of entries in import module name table
 */
unsigned long LxHeader::impModCount() const
{
    return e32( _lxData ).e32_impmodcnt;
}

/**
 * Get offset of import procedure name table
 *
 * \return Offset of import procedure name table
 */
unsigned long LxHeader::impProc() const
{
    return e32( _lxData ).e32_impproc;
}

/**
 * Get offset of per-page checksum table
 *
 * \return Offset of per-page checksum table
 */
unsigned long LxHeader::pageSum() const
{
    return e32( _lxData ).e32_pagesum;
}

/**
 * Get offset of enumerated data pages
 *
 * \return Offset of enumerated data pages
 */
unsigned long LxHeader::dataPage() const
{
    return e32( _lxData ).e32_datapage;
}

/**
 * Get number of preload pages
 *
 * \return Number of preload pages
 */
unsigned long LxHeader::preload() const
{
    return e32( _lxData ).e32_preload;
}

/**
 * Get offset of non-resident names table
 *
 * \return Offset of non-resident names table
 */
unsigned long LxHeader::nresTable() const
{
    return e32( _lxData ).e32_nrestab;
}

/**
 * Get size of non-resident name table
 *
 * \return Size of non-resident name table
 */
unsigned long LxHeader::nresTableSize() const
{
    return e32( _lxData ).e32_cbnrestab;
}

/**
 * Get non-resident name table checksum
 *
 * \return Non-resident name table checksum
 */
unsigned long LxHeader::nresSum() const
{
    return e32( _lxData ).e32_nressum;
}

/**
 * Get object # for automatic data object
 *
 * \return Object # for automatic data object
 */
unsigned long LxHeader::autoData() const
{
    return e32( _lxData ).e32_autodata;
}

/**
 * Get offset of the debugging information
 *
 * \return Offset of the debugging information
 */
unsigned long LxHeader::debugInfo() const
{
    return e32( _lxData ).e32_debuginfo;
}

/**
 * Get the length of the debugging information
 *
 * \return Length of the debugging information in bytes
 */
unsigned long LxHeader::debugLen() const
{
    return e32( _lxData ).e32_debuglen;
}

/**
 * Get number of instance pages in preload section of .EXE file
 *
 * \return Number of instace pages in preload section of .EXE file
 */
unsigned long LxHeader::instPreload() const
{
    return e32( _lxData ).e32_instpreload;
}

/**
 * Get number of instance pages in demand load section of .EXE file
 *
 * \return Number of instance pages in demand load section of .EXE file
 */
unsigned long LxHeader::instDemand() const
{
    return e32( _lxData ).e32_instdemand;
}

/**
 * Get size of heap
 *
 * \return Size of heap
 * \remark This field is for 16-bit apps
 */
unsigned long LxHeader::heapSize() const
{
    return e32( _lxData ).e32_heapsize;
}

/**
 * Get size of stack
 *
 * \return Size of stack
 */
unsigned long LxHeader::stackSize() const
{
    return e32( _lxData ).e32_stacksize;
}
