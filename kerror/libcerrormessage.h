/*
 * LibcErrorMessage
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

#ifndef KERROR_LIBCERRORMESSAGE_H
#define KERROR_LIBCERRORMESSAGE_H

#include "errormessage.h"

/**
 * LibcErrorMessage class
 */
class LibcErrorMessage : public ErrorMessage
{
public:
    /**
     * Constructor
     */
    LibcErrorMessage() {};

    /**
     * Destructor
     */
    virtual ~LibcErrorMessage() {};

protected:
    /**
     * Get an LIBC error list
     *
     * \return LIBC error list
     */
    virtual const error_list& errors() const override;
};

#endif
