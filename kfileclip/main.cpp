/*
 * K File Clip
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

/** \file main.cpp */

#include "filebuffer.h"
#include "clipboard.h"

int main(int argc, char *argv[])
{
    FileBuffer buffer( argc < 2 ? std::string() : argv[ 1 ]);
    buffer.read();

    Clipboard clipboard;
    clipboard.write( buffer.buffer());

    return 0;
}
