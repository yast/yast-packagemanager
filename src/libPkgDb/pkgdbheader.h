/*
 * This file has been copied from rpm 3.0 sources (GPL)
 */

/* RPM - Copyright (C) 1995 Red Hat Software
 *
 * header.h - routines for managing rpm tagged structures
 */

/* WARNING: 1 means success, 0 means failure (yes, this is backwards) */

#ifndef _PkgDb_H_HEADER
#define _PkgDb_H_HEADER
#include <stdio.h>
// #include <zlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int PFD_t;
#define pfdOpen			open
#define pfdClose		close
#define pfdLseek		lseek
#define pfdRead			read
#define pfdWrite		write
#define pfdValid(fd)	((fd) >= 0)
#define pfbFileno(fd)	(fd)

#if defined(__alpha__) || defined(__alpha)
typedef long int int_64;
typedef int int_32;
typedef short int int_16;
typedef char int_8;

typedef unsigned int uint_32;
typedef unsigned short uint_16;

#else

typedef long long int int_64;
typedef int int_32;
typedef short int int_16;
typedef char int_8;

typedef unsigned int uint_32;
typedef unsigned short uint_16;
#endif

typedef struct headerToken *PkgDb_Header;
typedef struct headerIteratorS *PkgDb_HeaderIterator;

/* read and write a header from a file */
PkgDb_Header PkgDb_headerRead(PFD_t fd, int magicp);
void PkgDb_headerWrite(PFD_t fd, PkgDb_Header h, int magicp);
unsigned int PkgDb_headerSizeof(PkgDb_Header h, int magicp);

#define HEADER_MAGIC_NO   0
#define HEADER_MAGIC_YES  1

/* load and unload a header from a chunk of memory */
PkgDb_Header PkgDb_headerLoad(void *p);
void *PkgDb_headerUnload(PkgDb_Header h);

PkgDb_Header PkgDb_headerNew(void);
void PkgDb_headerFree(/*@only@*/ PkgDb_Header h);

/* Duplicate tags are okay, but only defined for iteration (with the
   exceptions noted below). While you are allowed to add i18n string
   arrays through this function, you probably don't mean to. See
   headerAddI18NString() instead */
int PkgDb_headerAddEntry(PkgDb_Header h, int_32 tag, int_32 type, void *p, int_32 c);
/* if there are multiple entries with this tag, the first one gets replaced */
int PkgDb_headerModifyEntry(PkgDb_Header h, int_32 tag, int_32 type, void *p, int_32 c);

/* Appends item p to entry w/ tag and type as passed. Won't work on
   RPM_STRING_TYPE. Any pointers from headerGetEntry() for this entry
   are invalid after this call has been made! */
int PkgDb_headerAppendEntry(PkgDb_Header h, int_32 tag, int_32 type, void * p, int_32 c);
int PkgDb_headerAddOrAppendEntry(PkgDb_Header h, int_32 tag, int_32 type,
			   void * p, int_32 c);

/* Will never return RPM_I18NSTRING_TYPE! RPM_STRING_TYPE elements w/
   RPM_I18NSTRING_TYPE equivalent enreies are translated (if HEADER_I18NTABLE
   entry is present). */
int PkgDb_headerGetEntry(PkgDb_Header h, int_32 tag, int_32 *type, /*@out@*/void **p, int_32 *c);

int PkgDb_headerIsEntry(PkgDb_Header h, int_32 tag);
/* removes all entries of type tag from the header, returns 1 if none were
   found */
int PkgDb_headerRemoveEntry(PkgDb_Header h, int_32 tag);

PkgDb_HeaderIterator PkgDb_headerInitIterator(PkgDb_Header h);
int PkgDb_headerNextIterator(PkgDb_HeaderIterator iter,
		       int_32 *tag, int_32 *type, void **p, int_32 *c);
void PkgDb_headerFreeIterator(/*@only@*/ PkgDb_HeaderIterator iter);

PkgDb_Header PkgDb_headerCopy(PkgDb_Header h);
void PkgDb_headerSort(PkgDb_Header h);
PkgDb_Header PkgDb_headerLink(PkgDb_Header h);

/* Entry Types */

#define	RPM_MIN_TYPE		0
#define RPM_NULL_TYPE		0
#define RPM_CHAR_TYPE		1
#define RPM_INT8_TYPE		2
#define RPM_INT16_TYPE		3
#define RPM_INT32_TYPE		4
/* #define RPM_INT64_TYPE	5   ---- These aren't supported (yet) */
#define RPM_STRING_TYPE		6
#define RPM_BIN_TYPE		7
#define RPM_STRING_ARRAY_TYPE	8
#define RPM_I18NSTRING_TYPE	9
#define	RPM_MAX_TYPE		9

/* Tags -- general use tags should start at 1000 (RPM's tag space starts
   there) */

#define HEADER_I18NTABLE	100



int PkgDb_rpmReadPackageInfo(PFD_t fd, PkgDb_Header * signatures, PkgDb_Header * hdr);
int PkgDb_rpmReadPackageHeader(PFD_t fd, PkgDb_Header * hdr, int * isSource,
							   int * major, int * minor);
   /* 0 = success */
   /* 1 = bad magic */
   /* 2 = error */
	
#define	RPMSENSE_ANY		0
#define	RPMSENSE_SERIAL		(1 << 0)
#define	RPMSENSE_LESS		(1 << 1)
#define	RPMSENSE_GREATER	(1 << 2)
#define	RPMSENSE_EQUAL		(1 << 3)
#define	RPMSENSE_PROVIDES	(1 << 4) /* only used internally by builds */
#define	RPMSENSE_CONFLICTS	(1 << 5) /* only used internally by builds */
#define	RPMSENSE_PREREQ		(1 << 6)
#define	RPMSENSE_OBSOLETES	(1 << 7) /* only used internally by builds */
#define	RPMSENSE_SENSEMASK	15       /* Mask to get senses, ie serial, */
                                          /* less, greater, equal.          */


#ifdef __cplusplus
}
#endif

#endif	/* _PkgDb_H_HEADER */

// Local Variables:
// tab-width: 
// End:
