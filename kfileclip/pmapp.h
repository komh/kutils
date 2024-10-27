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

/** \file pmapp.h */

#ifndef KFILECLIP_PMAPP_H
#define KFILECLIP_PMAPP_H

#define INCL_WIN
#include <os2.h>

/**
 * PMApp class
 */
class PMApp
{
public:
    PMApp();
    ~PMApp();

    /**
     * Get a handle of an anchor block
     *
     * \return anchor block handle
     */
    HAB hab() { return _hab; }

    /**
     * Get a handle of a message queue
     *
     * \return message queue handle
     */
    HMQ hmq() { return _hmq; }

private:
    HAB _hab;       ///< anchor block handle
    HMQ _hmq;       ///< message queue handle
};

#endif
