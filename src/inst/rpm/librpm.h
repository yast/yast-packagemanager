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

//#define FAKELIBRPM

#ifdef FAKELIBRPM
#define H_HDRINLINE
#endif // FAKELIBRPM

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


///////////////////////////////////////////////////////////////////
#ifdef FAKELIBRPM
///////////////////////////////////////////////////////////////////

#define fpio ((FDIO_t)-1)

inline FD_t Fopen(/*@null@*/ const char * path, /*@null@*/ const char * fmode ) { return 0; }

inline int Fclose( /*@killref@*/ FD_t fd) { return -1; }

inline int Fseek(FD_t fd, _libio_off_t offset, int whence) { return -1; }

inline size_t Fwrite(const void * buf, size_t size, size_t nmemb, FD_t fd) { return 0; }

inline size_t Fread(/*@out@*/ void * buf, size_t size, size_t nmemb, FD_t fd) { return 0; }

inline int Ferror(/*@null@*/ FD_t fd) { return -1; }

inline const char * Fstrerror(/*@null@*/ FD_t fd) { return "FAKED LIBRPMDB"; }

///////////////////////////////////////////////////////////////////

inline void addMacro ( /*@null@*/ MacroContext mc, const char * n,
		       /*@null@*/ const char * o,
		       /*@null@*/ const char * b, int level ) {}

inline char * rpmExpand(/*@null@*/ const char * arg, ...) { return 0; }

///////////////////////////////////////////////////////////////////

inline int rpmReadConfigFiles( /*@null@*/ const char * file,
			       /*@null@*/ const char * target) { return -1; }

inline int rpmdbInit( /*@null@*/ const char * prefix, int perms ) { return -1; }

inline int rpmdbOpen (/*@null@*/ const char * prefix, /*@null@*/ /*@out@*/ rpmdb * dbp,
		       int mode, int perms) {
  if ( dbp ) { *dbp = 0; }
  return -1;
}

inline int rpmdbClose (/*@killref@*/ /*@only@*/ /*@null@*/ rpmdb db) { return -1; }

///////////////////////////////////////////////////////////////////

inline int rpmdbAppendIterator(/*@null@*/ rpmdbMatchIterator mi,
				/*@null@*/ const int * hdrNums, int nHdrNums) { return -1; }


inline rpmdbMatchIterator rpmdbInitIterator(/*@null@*/ rpmdb db, rpmTag rpmtag,
					     /*@null@*/ const void * keyp, size_t keylen) { return 0; }

inline rpmdbMatchIterator rpmdbFreeIterator(/*@only@*/ /*@null@*/rpmdbMatchIterator mi) { return 0; }

inline Header rpmdbNextIterator(/*@null@*/ rpmdbMatchIterator mi) { return 0; }

inline unsigned int rpmdbGetIteratorOffset(/*@null@*/ rpmdbMatchIterator mi) { return 0; }

inline int rpmdbGetIteratorCount(/*@null@*/ rpmdbMatchIterator mi) { return 0; }

///////////////////////////////////////////////////////////////////

inline rpmts rpmtsCreate() { return 0; }

inline rpmts rpmtsFree( rpmts ts ) { return 0; }

///////////////////////////////////////////////////////////////////

inline int rpmReadPackageFile(rpmts ts, FD_t fd, const char * fn, /*@null@*/ /*@out@*/ Header * hdrp) {
  if ( hdrp ) { *hdrp = 0; }
  return -1;
}
///////////////////////////////////////////////////////////////////

inline Header headerNew(void) { return 0; }

inline Header headerFree( /*@killref@*/ /*@null@*/ Header h) { return 0; }

inline Header headerLink(Header h) { return 0; }

inline Header headerUnlink(/*@killref@*/ /*@null@*/ Header h)  { return 0; }

inline Header headerLoad(/*@kept@*/ void * uh) { return 0; }

inline Header headerRead(FD_t fd, enum hMagic magicp) { return 0; }

inline int headerWrite(FD_t fd, /*@null@*/ Header h, enum hMagic magicp) { return 1; }

inline int headerGetEntry(Header h, int_32 tag,
		    /*@null@*/ /*@out@*/ hTYP_t type,
		    /*@null@*/ /*@out@*/ void ** p,
		    /*@null@*/ /*@out@*/ hCNT_t c) {
  if ( type ) { *type = 0; }
  if ( p ) { *p = 0; }
  if ( c ) { *c = 0; }
  return 0;
}

inline int headerIsEntry(/*@null@*/ Header h, int_32 tag) { return 0; }

///////////////////////////////////////////////////////////////////
#endif // FAKELIBRPM
///////////////////////////////////////////////////////////////////

#endif // librpm_h
