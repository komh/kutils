/*
 * Os2ErrorMessage
 *
 * Copyright (C) 2024 KO Myung-Hun <komh78@gmail.com>
 *
 * This file is a part of K Error.
 *
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://www.wtfpl.net/ for more details.
 */

/** \file */

#ifndef KERROR_OS2ERRORMESSAGE_H
#define KERROR_OS2ERRORMESSAGE_H

#include "errormessage.h"

/**
 * Os2ErrorMessage class
 */
class Os2ErrorMessage : public ErrorMessage
{
public:
    /**
     * Constructor
     */
    Os2ErrorMessage() {};

    /**
     * Destructor
     */
    virtual ~Os2ErrorMessage() {};

protected:
    /**
     * Get an OS2 error list
     *
     * \return OS2 error list
     */
    virtual const error_list& errors() const override;
};

#endif
