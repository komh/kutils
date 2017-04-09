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

/** \file clipboard.h */

#ifndef KFILECLIP_CLIPBOARD_H
#define KFILECLIP_CLIPBOARD_H

#include <string>

/**
 * Clipboard class
 */
class Clipboard
{
public:
    Clipboard();
    ~Clipboard();

    bool write( const std::string& buffer );

private:
    bool _opened;   ///< open indicator of clipboard
};

#endif
