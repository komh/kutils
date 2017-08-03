/*
 * KLxHdr
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

/** \file klxhdr.cpp */

#include "lxheader.h"

#define SMP         // for E32NOTMPSAFE
#include <exe386.h> // OS/2 LX header

#include <iostream>
#include <string>

/**
 * Print message and value in HEX or DEC
 *
 * \param[in] msg Message to print
 * \param[in] val Value to print
 * \param[in] hex Radix flag. true for HEX, false for DEC
 */
static inline void printVal( const std::string& msg, unsigned long val,
                             bool hex )
{
    std::cout << msg;
    if( hex )
        std::cout << std::hex << "0x";
    else
        std::cout << std::dec;

    std::cout << val << std::endl;
}

/**
 * Print message and value in HEX
 *
 * \param[in] msg Message to print
 * \param[in] val Value to print
 */
static inline void printHex( const std::string& msg, unsigned long val )
{
    printVal( msg, val, true );
}

/**
 * Print message and value in DEC
 *
 * \param[in] msg Message to print
 * \param[in] val Value to print
 */
static inline void printDec( const std::string& msg, unsigned long val )
{
    printVal( msg, val, false );
}

/**
 * Print message and value in Yes or No
 *
 * \param[in] msg Message to print
 * \param[in] fl Flags to test
 * \param[in] mask Bits to mask
 * \param[in] bits Bits to test. If not specified, \a bits is set to \a mask.
 */
static inline void printBool( const std::string& msg, unsigned long fl,
                              unsigned long mask, unsigned long bits = ~0UL )
{
    if( bits == ~0UL )
        bits = mask;

    std::cout << msg;
    std::cout << (( fl & mask ) == bits  ? "Yes" : "No" ) << std::endl;
}

/**
 * Dump LX header
 *
 * \param[in] filename Filename to dump
 */
static void dump( const std::string& filename )
{
    LxHeader lxHdr( filename );

    if( lxHdr.hasDosStub())
    {
        std::cout << "----- DOS Stub Header -----" << std::endl;
        std::cout << "Signature: " << lxHdr.dosMagic() << std::endl;

        printHex("Offset to LX: ", lxHdr.lxOffset());

        std::cout << std::endl;
    }

    if( lxHdr.hasLx())
    {
        std::cout << "----- LX Header -----" << std::endl;
        std::cout << "Signature: " << lxHdr.lxMagic() << std::endl;

        std::cout << "Byte ordering: " << lxHdr.byteOrder() << " (";
        switch( lxHdr.byteOrder())
        {
            case E32LEBO: std::cout << "Little endian"; break;
            case E32BEBO: std::cout << "Big endian"; break;
            default: std::cout << "Unknown endian"; break;
        }
        std::cout << ")" << std::endl;

        std::cout << "Word ordering: " << lxHdr.wordOrder() << " (";
        switch( lxHdr.wordOrder())
        {
            case E32LEWO: std::cout << "Little endian"; break;
            case E32BEWO: std::cout << "Big endian"; break;
            default: std::cout << "Unkonwn endian"; break;
        }
        std::cout << ")" << std::endl;

        std::cout << "Format level: " << lxHdr.level() << " (";
        std::cout << ( lxHdr.level() == E32LEVEL
                       ? "32-bit EXE format level" : "Unknown format level");
        std::cout << ")" << std::endl;

        std::cout << "CPU type: " << lxHdr.cpu() << " (";
        switch( lxHdr.cpu())
        {
            case E32CPU286: std::cout << "Intel 80286+"; break;
            case E32CPU386: std::cout << "Intel 80386+"; break;
            case E32CPU486: std::cout << "Intel 80486+"; break;
            default: std::cout << "Unknown CPU type"; break;
        }
        std::cout << ")" << std::endl;

        std::cout << "OS type: " << lxHdr.os() << " (";
        switch( lxHdr.os())
        {
            case 0x01: std::cout << "OS/2"; break;
            case 0x02: std::cout << "Windows"; break;
            case 0x03: std::cout << "DOS 4.x"; break;
            case 0x04: std::cout << "Windows 386"; break;
            case 0x05: std::cout << "IBM Microkernel Personality Neutral ";
                       break;
            case 0x00:
            default: std::cout << "Unkonwn OS"; break;
        }
        std::cout << ")" << std::endl;

        printDec("Module version: ", lxHdr.modVer());

        unsigned long fl = lxHdr.modFlags();
        printHex("Module flags: ", fl );
        printBool("\tLibrary module: ", fl, E32NOTP );
        printBool("\tModule not loadable: ", fl, E32NOLOAD );

        printHex("\tApplication type flags: ", fl & E32APPMASK );
        printBool("\t\tUse PM Windowing API: ", fl, E32APPMASK, E32PMAPI );
        printBool("\t\tCompatible with PM Windowing: ",
                  fl, E32APPMASK, E32PMW );
        printBool("\t\tIncompatible with PM Windowing: ",
                  fl, E32APPMASK, E32NOPMW );

        printBool("\tNO external fixups in .EXE: ", fl, E32NOEXTFIX );
        printBool("\tNO internal fixups in .EXE: ", fl, E32NOINTFIX );
        printBool("\tSystem DLL, internal fixups discarded: ",
                  fl, E32SYSDLL );
        printBool("\tPer-process library initialization: ", fl, E32LIBINIT );
        printBool("\tPer-process library termination: ", fl, E32LIBTERM );

        printHex("\tModule type flags: ", fl & E32MODMASK );
        printBool("\t\tProtected memory library module: ", fl, E32PROTDLL );
        printBool("\t\t.EXE module: ", fl, E32MODMASK, E32MODEXE );
        printBool("\t\t.DLL module: ", fl, E32MODMASK, E32MODDLL );
        printBool("\t\tDevice driver: ", fl, E32DEVICE );
        printBool("\t\t\tPhysical device driver: ",
                  fl, E32MODMASK, E32MODPDEV );
        printBool("\t\t\tVirtual device driver: ",
                  fl, E32MODMASK, E32MODVDEV );

        printBool("\tProcess is multi-processor unsafe: ", fl, E32NOTMPSAFE );

        printDec("Physical number of pages in module: ", lxHdr.modPages());
        printDec("Object number to which the entry adress is relative: ",
                 lxHdr.startObj());
        printHex("Entry address of module: ", lxHdr.eip());
        printDec("Object number to which the ESP is relative: ",
                 lxHdr.stackObj());
        printHex("Starting stack address of module: ", lxHdr.esp());
        printDec("Page size: ", lxHdr.pageSize());
        printDec("The shift left bits for page offsets: ", lxHdr.pageShift());
        printDec("Total size of the fixup info in bytes: ", lxHdr.fixupSize());
        printHex("Checksum for fixup info: ", lxHdr.fixupSum());
        printDec("Size of memory resident tables: ", lxHdr.ldrSize());
        printHex("Checksum for loader section: ", lxHdr.ldrSum());
        printHex("Object table offset: ", lxHdr.objTable());
        printDec("Object table count: ", lxHdr.objCount());
        printHex("Object page map offset: ", lxHdr.objMap());
        printHex("Object iterated data map offset: ", lxHdr.iterMap());
        printHex("Resource table offset: ", lxHdr.rsrcTable());
        printDec("Number of entries in resource table: ", lxHdr.rsrcCount());
        printHex("Resident name table offset: ", lxHdr.resTable());
        printHex("Entry table offset: ", lxHdr.entryTable());
        printHex("Module format directives table offset: ", lxHdr.dirTable());
        printDec("Number of module format directives in the table: ",
                 lxHdr.dirCount());
        printHex("Fixup page table offset: ", lxHdr.fixupPageTable());
        printHex("Fixup record table offset: ", lxHdr.fixupRecTable());
        printHex("Import module name table offset: ", lxHdr.impMod());
        printDec("Number of entries in the import module name table: ",
                 lxHdr.impModCount());
        printHex("Import procedure name table offset: ", lxHdr.impProc());
        printHex("Per-page checksum table offset: ", lxHdr.pageSum());
        printHex("Data pages offset: ", lxHdr.dataPage());
        printDec("Number of preload pages for this module: ", lxHdr.preload());
        printHex("Non-resident name table offset: ", lxHdr.nresTable());
        printDec("Size of non-resident name table: ", lxHdr.nresTableSize());
        printHex("Non-resident name table checksum: ", lxHdr.nresSum());
        printDec("Audo data segment object number: ", lxHdr.autoData());
        printHex("Debug info offset: ", lxHdr.debugInfo());
        printDec("Debuginfo length: ", lxHdr.debugLen());
        printDec("Instance pages in preload section: ", lxHdr.instPreload());
        printDec("Instance pages in demand section: ", lxHdr.instDemand());
        printDec("Heap size: ", lxHdr.heapSize());
        printDec("Stack size: ", lxHdr.stackSize());
    }
    else
    {
        std::cout << "----- Non-LX Header -----" << std::endl;
        std::cout << "Signature: [" << lxHdr.lxMagic() << "]" << std::endl;
    }

    std::cout << std::endl;
}

int main( int argc, char *argv[])
{
    if( argc < 2 )
    {
        std::cerr << "Specify a LX filename!!!" << std::endl;

        return 1;
    }

    dump( argv[ 1 ]);

    return 0;
}
