/*
 * FileBuffer
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

/** \file filbuffer.h */

#ifndef KFILECLIP_FILEBUFFER_H
#define KFILECLIP_FILEBUFFER_H

#include <string>

/**
 * File buffer class
 */
class FileBuffer
{
public:
    /**
     * Constructor
     *
     * \param[in] fileName A file name to read from
     */
    FileBuffer( const std::string& fileName = std::string())
        : _fileName( fileName ) { }

    bool read();

    /**
     * Get a buffer
     *
     * \return Buffer of a given file
     */
    const std::string& buffer() const { return _buffer; }

private:
    std::string _fileName;
    std::string _buffer;
};

#endif
