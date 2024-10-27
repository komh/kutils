/*
 * PMApp
 *
 * Copyright (C) 2024 KO Myung-Hun <komh78@gmail.com>
 *
 * This file is a part of K File Clip.
 *
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://www.wtfpl.net/ for more details.
 */

/** \file pmapp.cpp */

#define INCL_DOS
#define INCL_WIN
#include <os2.h>

#include "pmapp.h"

/**
 * Constructor
 */
PMApp::PMApp() : _hab( 0 ), _hmq( 0 )
{
    PPIB ppib;

    DosGetInfoBlocks( NULL, &ppib );

    // morph to PM
    if( ppib->pib_ultype == 2 )
        ppib->pib_ultype = 3;

    _hab = WinInitialize( 0 );
    _hmq = WinCreateMsgQueue( _hab, 0 );
}

/**
 * Destructor
 */
PMApp::~PMApp()
{
    if( !_hmq )
        WinDestroyMsgQueue( _hmq );

    if( !_hab )
        WinTerminate( _hab );
}
