/*---------------------------------------------------------------------\
|                                                                      |
|                      __   __    ____ _____ ____                      |
|                      \ \ / /_ _/ ___|_   _|___ \                     |
|                       \ V / _` \___ \ | |   __) |                    |
|                        | | (_| |___) || |  / __/                     |
|                        |_|\__,_|____/ |_| |_____|                    |
|                                                                      |
|                               core system                            |
|                                                        (C) SuSE GmbH |
\----------------------------------------------------------------------/

  File:       librpm.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose:

/-*/
#ifndef librpm_h
#define librpm_h

extern "C" {
#include <rpm/rpmlib.h>
#include <rpm/rpmmacro.h>
#include <rpm/rpmdb.h>
#include <rpm/rpmts.h>
#include <fcntl.h>
}

///////////////////////////////////////////////////////////////////
//
// binHeaderCache::Cache::tell
//
///////////////////////////////////////////////////////////////////
extern "C" {
  typedef struct X_FDSTACK_s {
    FDIO_t	io;
    void *	fp;
    int		fdno;
  } XFDSTACK_t;

  struct X_FD_s {
    int		nrefs;
    int		flags;
    int		magic;
#define	XFDMAGIC	0x04463138
    int		nfps;
    XFDSTACK_t	fps[8];
  };
}

///////////////////////////////////////////////////////////////////
//
// binHeaderCache::Cache::readHeader
//
///////////////////////////////////////////////////////////////////
extern "C" {
#include <netinet/in.h>

  // from rpm: lib/header.c
  struct entryInfo {
    int_32 tag;
    int_32 type;
    int_32 offset;              /* Offset from beginning of data segment,
				   only defined on disk */
    int_32 count;
  };
}

#endif // librpm_h
