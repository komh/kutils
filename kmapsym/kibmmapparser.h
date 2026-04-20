/*
 * KIbmMapParser
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

#ifndef KMAPSYM_KIBMMAPPARSER_H
#define KMAPSYM_KIBMMAPPARSER_H

#include "kmapparser.h"

#include <string_view>

/**
 * IBM .MAP file parser class
 */
class KIbmMapParser: public KMapParser
{
public:
    /**
     * Constructor
     *
     * @param[in] fileName  .MAP file name
     */
    KIbmMapParser( std::string_view fileName = {});

    /**
     * Destructor
     */
    virtual ~KIbmMapParser();

private:
    /**
     * Line parser
     *
     * @param[in] line  Line to parse
     * @return          true if success, otherwise false
     */
    bool parseLine( std::string_view line ) override;
};

#endif
