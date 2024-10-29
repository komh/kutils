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

#include <cerrno>

#include "errormessage.h"
#include "libcerrormessage.h"


#ifndef EZERO
#define EZERO   0
#endif

// EMX specific
#ifndef ENEWVER
#define ENEWVER 90
#endif

// EMX specific
#ifndef EBADVER
#define EBADVER 91
#endif

static error_list libc_errors = {
    ERROR_ITEM( EZERO, "Error 0" ),
    ERROR_ITEM( EPERM, "Operation not permitted" ),
    ERROR_ITEM( ENOENT, "No such file or directory" ),
    ERROR_ITEM( ESRCH, "No such process" ),
    ERROR_ITEM( EINTR, "Interrupted system call" ),
    ERROR_ITEM( EIO, "Input/output error" ),
    ERROR_ITEM( ENXIO, "Device not configured" ),
    ERROR_ITEM( E2BIG, "Argument list too long" ),
    ERROR_ITEM( ENOEXEC, "Exec format error" ),
    ERROR_ITEM( EBADF, "Bad file descriptor" ),
    ERROR_ITEM( ECHILD, "No child processes" ),
    ERROR_ITEM( EDEADLK, "Resource deadlock avoided" ),
    ERROR_ITEM( ENOMEM, "Cannot allocate memory" ),
    ERROR_ITEM( EACCES, "Permission denied" ),
    ERROR_ITEM( EFAULT, "Bad address" ),
    ERROR_ITEM( ENOTBLK, "Block device required" ),
    ERROR_ITEM( EBUSY, "Device busy" ),
    ERROR_ITEM( EEXIST, "File exists" ),
    ERROR_ITEM( EXDEV, "Cross-device link" ),
    ERROR_ITEM( ENODEV, "Operation not supported by device" ),
    ERROR_ITEM( ENOTDIR, "Not a directory" ),
    ERROR_ITEM( EISDIR, "Is a directory" ),
    ERROR_ITEM( EINVAL, "Invalid argument" ),
    ERROR_ITEM( ENFILE, "Too many open files in system" ),
    ERROR_ITEM( EMFILE, "Too many open files" ),
    ERROR_ITEM( ENOTTY, "Inappropriate ioctl for device" ),
    ERROR_ITEM( ETXTBSY, "Text file busy" ),
    ERROR_ITEM( EFBIG, "File too large" ),
    ERROR_ITEM( ENOSPC, "No space left on device" ),
    ERROR_ITEM( ESPIPE, "Illegal seek" ),
    ERROR_ITEM( EROFS, "Read-only filesystem" ),
    ERROR_ITEM( EMLINK, "Too many links" ),
    ERROR_ITEM( EPIPE, "Broken pipe" ),
    ERROR_ITEM( EDOM, "Numerical argument out of domain" ),
    ERROR_ITEM( ERANGE, "Result too large" ),
    ERROR_ITEM( EAGAIN, "Resource temporarily unavailable" ),
    ERROR_ITEM( EWOULDBLOCK, "Operation would block" ),
    ERROR_ITEM( EINPROGRESS, "Operation now in progress" ),
    ERROR_ITEM( EALREADY, "Operation already in progress" ),
    ERROR_ITEM( ENOTSOCK, "Socket operation on non-socket" ),
    ERROR_ITEM( EDESTADDRREQ, "Destination address required" ),
    ERROR_ITEM( EMSGSIZE, "Message too long" ),
    ERROR_ITEM( EPROTOTYPE, "Protocol wrong type for socket" ),
    ERROR_ITEM( ENOPROTOOPT, "Protocol not available" ),
    ERROR_ITEM( EPROTONOSUPPORT, "Protocol not supported" ),
    ERROR_ITEM( ESOCKTNOSUPPORT, "Socket type not supported" ),
    ERROR_ITEM( EOPNOTSUPP, "Operation not supported" ),
    ERROR_ITEM( ENOTSUP, "Operation not supported" ),
    ERROR_ITEM( EPFNOSUPPORT, "Protocol family not supported" ),
    ERROR_ITEM( EAFNOSUPPORT, "Address family not supported by protocol family" ),
    ERROR_ITEM( EADDRINUSE, "Address already in use" ),
    ERROR_ITEM( EADDRNOTAVAIL, "Can't assign requested address" ),
    ERROR_ITEM( ENETDOWN, "Network is down" ),
    ERROR_ITEM( ENETUNREACH, "Network is unreachable" ),
    ERROR_ITEM( ENETRESET, "Network dropped connection on reset" ),
    ERROR_ITEM( ECONNABORTED, "Software caused connection abort" ),
    ERROR_ITEM( ECONNRESET, "Connection reset by peer" ),
    ERROR_ITEM( ENOBUFS, "No buffer space available" ),
    ERROR_ITEM( EISCONN, "Socket is already connected" ),
    ERROR_ITEM( ENOTCONN, "Socket is not connected" ),
    ERROR_ITEM( ESHUTDOWN, "Can't send after socket shutdown" ),
    ERROR_ITEM( ETOOMANYREFS, "Too many references: can't splice" ),
    ERROR_ITEM( ETIMEDOUT, "Operation timed out" ),
    ERROR_ITEM( ECONNREFUSED, "Connection refused" ),
    ERROR_ITEM( ELOOP, "Too many levels of symbolic links" ),
    ERROR_ITEM( ENAMETOOLONG, "File name too long" ),
    ERROR_ITEM( EHOSTDOWN, "Host is down" ),
    ERROR_ITEM( EHOSTUNREACH, "No route to host" ),
    ERROR_ITEM( ENOTEMPTY, "Directory not empty" ),
    ERROR_ITEM( EPROCLIM, "Too many processes" ),
    ERROR_ITEM( EUSERS, "Too many users" ),
    ERROR_ITEM( EDQUOT, "Disc quota exceeded" ),
    ERROR_ITEM( ESTALE, "Stale NFS file handle" ),
    ERROR_ITEM( EREMOTE, "Too many levels of remote in path" ),
    ERROR_ITEM( EBADRPC, "RPC struct is bad" ),
    ERROR_ITEM( ERPCMISMATCH, "RPC version wrong" ),
    ERROR_ITEM( EPROGUNAVAIL, "RPC prog. not avail" ),
    ERROR_ITEM( EPROGMISMATCH, "Program version wrong" ),
    ERROR_ITEM( EPROCUNAVAIL, "Bad procedure for program" ),
    ERROR_ITEM( ENOLCK, "No locks available" ),
    ERROR_ITEM( ENOSYS, "Function not implemented" ),
    ERROR_ITEM( EFTYPE, "Inappropriate file type or format" ),
    ERROR_ITEM( EAUTH, "Authentication error" ),
    ERROR_ITEM( ENEEDAUTH, "Need authenticator" ),
    ERROR_ITEM( EIDRM, "Identifier removed" ),
    ERROR_ITEM( ENOMSG, "No message of desired type" ),
    ERROR_ITEM( EOVERFLOW, "Value too large to be stored in data type" ),
    ERROR_ITEM( ECANCELED, "Operation canceled" ),
    ERROR_ITEM( EILSEQ, "Illegal byte sequence" ),
    ERROR_ITEM( ENOATTR, "Attribute not found" ),
    ERROR_ITEM( EDOOFUS, "Programming error" ),
    ERROR_ITEM( ELAST, "Must be equal largest errno" ),
    // EMX specific
    ERROR_ITEM( ENEWVER, "mixing with a new version of a struct" ),
    ERROR_ITEM( EBADVER, "bad version number" ),
};

const error_list& LibcErrorMessage::errors() const
{
    return libc_errors;
}
