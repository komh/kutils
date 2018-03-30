/*
 * Clipboard
 *
 * Copyright (C) 2017 KO Myung-Hun <komh@chollian.net>
 *
 * This file is a part of K File Clip.
 *
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://www.wtfpl.net/ for more details.
 */

/** \file clipboard.cpp */

#define INCL_DOS
#define INCL_WINCLIPBOARD
#include <os2.h>

#include "clipboard.h"

#include <cstring>

#include <string>

/**
 * Constructor
 */
Clipboard::Clipboard() : _opened( false )
{
    PPIB ppib;

    DosGetInfoBlocks( NULL, &ppib );

    // Workaround for VirtualBox. Without morphing to PM, clipboard is not
    // shared between host and guest.
    if( ppib->pib_ultype == 3 )
        ppib->pib_ultype = 4;

    // open clipboard
    _opened = WinOpenClipbrd( 0 );
}

/**
 * Destructor
 */
Clipboard::~Clipboard()
{
    // close clipboard if opened
    if( _opened )
        WinCloseClipbrd( 0 );
}

/**
 * Write buffer to clipboard
 *
 * \param[in] buffer Buffer to write to clipboard
 * \return true on success, otherwise false
 */
bool Clipboard::write( const std::string& buffer )
{
    PVOID pBuffer;
    ULONG ulFlag = PAG_COMMIT | fPERM | fSHARE;
     // buffer size including null-terminator
    std::string::size_type bufSize = buffer.length() + 1;

    // Allocate shared memory for clipboard. Clipboard does not support
    // high-memory
    if( DosAllocSharedMem( &pBuffer, NULL, bufSize, ulFlag ))
        return false;

    // copy buffer to shared memory
    memcpy( pBuffer, buffer.c_str(), bufSize );

    // clear and set clipboard
    if(  !WinEmptyClipbrd( 0 )
         || !WinSetClipbrdData( 0, ( ULONG )pBuffer, CF_TEXT, CFI_POINTER ))
    {
        // if failed, free buffer
        DosFreeMem( pBuffer );

        return false;
    }

    return true;
}
