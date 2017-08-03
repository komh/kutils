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

/** \file lxheader.h */

#ifndef KLXHDR_LXHEADER_H
#define KLXHDR_LXHEADER_H

#include <string>
#include <vector>

/**
 * class for LX header
 */
class LxHeader
{
public:
    LxHeader( const std::string& filename = std::string());
    ~LxHeader();

    bool setFilename( const std::string& filename );

    std::string dosMagic() const;

    /**
     * Check if having DOS stub header
     *
     * \return true if having DOS stub, otherwise false
     */
    bool hasDosStub() const { return _dosStub; };

    /**
     * Check if having LX header
     *
     * \return true if having LX header, otherwise false
     */
    bool hasLx() const { return _lx; }

    /**
     * Get offset of LX header
     *
     * \return Offset of LX header
     */
    long lxOffset() const { return _lxOffset; }

    std::string lxMagic() const;
    unsigned byteOrder() const;
    unsigned wordOrder() const;
    unsigned long level() const;
    unsigned cpu() const;
    unsigned os() const;
    unsigned long modVer() const;
    unsigned long modFlags() const;
    unsigned long modPages() const;
    unsigned long startObj() const;
    unsigned long eip() const;
    unsigned long stackObj() const;
    unsigned long esp() const;
    unsigned long pageSize() const;
    unsigned long pageShift() const;
    unsigned long fixupSize() const;
    unsigned long fixupSum() const;
    unsigned long ldrSize() const;
    unsigned long ldrSum() const;
    unsigned long objTable() const;
    unsigned long objCount() const;
    unsigned long objMap() const;
    unsigned long iterMap() const;
    unsigned long rsrcTable() const;
    unsigned long rsrcCount() const;
    unsigned long resTable() const;
    unsigned long entryTable() const;
    unsigned long dirTable() const;
    unsigned long dirCount() const;
    unsigned long fixupPageTable() const;
    unsigned long fixupRecTable() const;
    unsigned long impMod() const;
    unsigned long impModCount() const;
    unsigned long impProc() const;
    unsigned long pageSum() const;
    unsigned long dataPage() const;
    unsigned long preload() const;
    unsigned long nresTable() const;
    unsigned long nresTableSize() const;
    unsigned long nresSum() const;
    unsigned long autoData() const;
    unsigned long debugInfo() const;
    unsigned long debugLen() const;
    unsigned long instPreload() const;
    unsigned long instDemand() const;
    unsigned long heapSize() const;
    unsigned long stackSize() const;

private:
    std::string _filename;          ///< Filename to read LX header
    std::vector< char > _dosData;   ///< Buffer for DOS stub header
    std::vector< char > _lxData;    ///< Buffer for LX header
    bool _dosStub;                  ///< Indicator for DOS stub header
    bool _lx;                       ///< Indicator for LX header
    long _lxOffset;                 ///< Offset of LX header
};

#endif
