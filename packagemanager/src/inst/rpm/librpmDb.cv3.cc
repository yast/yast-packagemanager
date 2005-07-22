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

  File:       librpmDb.cv3.cc

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Transfer rpmV3 database content into rpmV4 database.

/-*/
#include "librpm.h"

extern "C" {
#include <string.h>

#define FA_MAGIC      0x02050920

  struct faFileHeader{
    unsigned int magic;
    unsigned int firstFree;
  };

  struct faHeader {
    unsigned int size;
    unsigned int freeNext; /* offset of the next free block, 0 if none */
    unsigned int freePrev;
    unsigned int isFree;

    /* note that the u16's appear last for alignment/space reasons */
  };
}

static int fadFileSize;

static ssize_t Pread(FD_t fd, void * buf, size_t count, off_t offset) {
    if (Fseek(fd, offset, SEEK_SET) < 0)
        return -1;
    return Fread(buf, sizeof(char), count, fd);
}

static FD_t fadOpen(const char * path)
{
    struct faFileHeader newHdr;
    FD_t fd;
    struct stat stb;

    fd = Fopen(path, "r.fdio");
    if (!fd || Ferror(fd))
	return NULL;

    if (fstat(Fileno(fd), &stb)) {
	Fclose(fd);
        return NULL;
    }
    fadFileSize = stb.st_size;

    /* is this file brand new? */
    if (fadFileSize == 0) {
	Fclose(fd);
	return NULL;
    }
    if (Pread(fd, &newHdr, sizeof(newHdr), 0) != sizeof(newHdr)) {
	Fclose(fd);
	return NULL;
    }
    if (newHdr.magic != FA_MAGIC) {
	Fclose(fd);
	return NULL;
    }
    /*@-refcounttrans@*/ return fd /*@=refcounttrans@*/ ;
}

static int fadNextOffset(FD_t fd, unsigned int lastOffset)
{
    struct faHeader header;
    int offset;

    offset = (lastOffset)
	? (lastOffset - sizeof(header))
	: sizeof(struct faFileHeader);

    if (offset >= fadFileSize)
	return 0;

    if (Pread(fd, &header, sizeof(header), offset) != sizeof(header))
	return 0;

    if (!lastOffset && !header.isFree)
	return (offset + sizeof(header));

    do {
	offset += header.size;

	if (Pread(fd, &header, sizeof(header), offset) != sizeof(header))
	    return 0;

	if (!header.isFree) break;
    } while (offset < fadFileSize && header.isFree);

    if (offset < fadFileSize) {
	/* Sanity check this to make sure we're not going in loops */
	offset += sizeof(header);

	if (offset < 0 || (unsigned)offset <= lastOffset) return -1;

	return offset;
    } else
	return 0;
}

static int fadFirstOffset(FD_t fd)
{
    return fadNextOffset(fd, 0);
}

/*@-boundsread@*/
static int dncmp(const void * a, const void * b)
	/*@*/
{
    const char *const * first = (const char *const *)a;
    const char *const * second = (const char *const *)b;
    return strcmp(*first, *second);
}
/*@=boundsread@*/

/*@-bounds@*/
static void compressFilelist(Header h)
	/*@*/
{
    HGE_t hge = (HGE_t)headerGetEntryMinMemory;
    HAE_t hae = (HAE_t)headerAddEntry;
    HRE_t hre = (HRE_t)headerRemoveEntry;
    HFD_t hfd = headerFreeData;
    char ** fileNames;
    const char ** dirNames;
    const char ** baseNames;
    int_32 * dirIndexes;
    rpmTagType fnt;
    int count;
    int i, xx;
    int dirIndex = -1;

    /*
     * This assumes the file list is already sorted, and begins with a
     * single '/'. That assumption isn't critical, but it makes things go
     * a bit faster.
     */

    if (headerIsEntry(h, RPMTAG_DIRNAMES)) {
	xx = hre(h, RPMTAG_OLDFILENAMES);
	return;		/* Already converted. */
    }

    if (!hge(h, RPMTAG_OLDFILENAMES, &fnt, (void **) &fileNames, &count))
	return;		/* no file list */
    if (fileNames == NULL || count <= 0)
	return;

    dirNames = (const char **)alloca(sizeof(*dirNames) * count);	/* worst case */
    baseNames = (const char **)alloca(sizeof(*dirNames) * count);
    dirIndexes = (int_32 *)alloca(sizeof(*dirIndexes) * count);

    if (fileNames[0][0] != '/') {
	/* HACK. Source RPM, so just do things differently */
	dirIndex = 0;
	dirNames[dirIndex] = "";
	for (i = 0; i < count; i++) {
	    dirIndexes[i] = dirIndex;
	    baseNames[i] = fileNames[i];
	}
	goto exit;
    }

    /*@-branchstate@*/
    for (i = 0; i < count; i++) {
	const char ** needle;
	char savechar;
	char * baseName;
	int len;

	if (fileNames[i] == NULL)	/* XXX can't happen */
	    continue;
	baseName = strrchr(fileNames[i], '/') + 1;
	len = baseName - fileNames[i];
	needle = dirNames;
	savechar = *baseName;
	*baseName = '\0';
/*@-compdef@*/
	if (dirIndex < 0 ||
	    (needle = (const char **)bsearch(&fileNames[i], dirNames, dirIndex + 1, sizeof(dirNames[0]), dncmp)) == NULL) {
	    char *s = (char *)alloca(len + 1);
	    memcpy(s, fileNames[i], len + 1);
	    s[len] = '\0';
	    dirIndexes[i] = ++dirIndex;
	    dirNames[dirIndex] = s;
	} else
	    dirIndexes[i] = needle - dirNames;
/*@=compdef@*/

	*baseName = savechar;
	baseNames[i] = baseName;
    }
    /*@=branchstate@*/

exit:
    if (count > 0) {
	xx = hae(h, RPMTAG_DIRINDEXES, RPM_INT32_TYPE, dirIndexes, count);
	xx = hae(h, RPMTAG_BASENAMES, RPM_STRING_ARRAY_TYPE,
			baseNames, count);
	xx = hae(h, RPMTAG_DIRNAMES, RPM_STRING_ARRAY_TYPE,
			dirNames, dirIndex + 1);
    }

    fileNames = (char**)hfd(fileNames, fnt);

    xx = hre(h, RPMTAG_OLDFILENAMES);
}
/*@=bounds@*/

/*
 * Up to rpm 3.0.4, packages implicitly provided their own name-version-release.
 * Retrofit an explicit "Provides: name = epoch:version-release".
 */
void providePackageNVR(Header h)
{
    HGE_t hge = (HGE_t)headerGetEntryMinMemory;
    HFD_t hfd = headerFreeData;
    const char *name, *version, *release;
    int_32 * epoch;
    const char *pEVR;
    char *p;
    int_32 pFlags = RPMSENSE_EQUAL;
    const char ** provides = NULL;
    const char ** providesEVR = NULL;
    rpmTagType pnt, pvt;
    int_32 * provideFlags = NULL;
    int providesCount;
    int i, xx;
    int bingo = 1;

    /* Generate provides for this package name-version-release. */
    xx = headerNVR(h, &name, &version, &release);
    if (!(name && version && release))
        return;
    pEVR = p = (char *)alloca(21 + strlen(version) + 1 + strlen(release) + 1);
    *p = '\0';
    if (hge(h, RPMTAG_EPOCH, NULL, (void **) &epoch, NULL)) {
        sprintf(p, "%d:", *epoch);
        while (*p != '\0')
            p++;
    }
    (void) stpcpy( stpcpy( stpcpy(p, version) , "-") , release);

    /*
     * Rpm prior to 3.0.3 does not have versioned provides.
     * If no provides at all are available, we can just add.
     */
    if (!hge(h, RPMTAG_PROVIDENAME, &pnt, (void **) &provides, &providesCount))
        goto exit;

    /*
     * Otherwise, fill in entries on legacy packages.
     */
    if (!hge(h, RPMTAG_PROVIDEVERSION, &pvt, (void **) &providesEVR, NULL)) {
        for (i = 0; i < providesCount; i++) {
            char * vdummy = "";
            int_32 fdummy = RPMSENSE_ANY;
            xx = headerAddOrAppendEntry(h, RPMTAG_PROVIDEVERSION, RPM_STRING_ARRAY_TYPE,
                        &vdummy, 1);
            xx = headerAddOrAppendEntry(h, RPMTAG_PROVIDEFLAGS, RPM_INT32_TYPE,
                        &fdummy, 1);
        }
        goto exit;
    }

    xx = hge(h, RPMTAG_PROVIDEFLAGS, NULL, (void **) &provideFlags, NULL);

    /*@-nullderef@*/    /* LCL: providesEVR is not NULL */
    if (provides && providesEVR && provideFlags)
    for (i = 0; i < providesCount; i++) {
        if (!(provides[i] && providesEVR[i]))
            continue;
        if (!(provideFlags[i] == RPMSENSE_EQUAL &&
            !strcmp(name, provides[i]) && !strcmp(pEVR, providesEVR[i])))
            continue;
        bingo = 0;
        break;
    }
    /*@=nullderef@*/

exit:
    provides = (const char **)hfd(provides, pnt);
    providesEVR = (const char **)hfd(providesEVR, pvt);

    if (bingo) {
        xx = headerAddOrAppendEntry(h, RPMTAG_PROVIDENAME, RPM_STRING_ARRAY_TYPE,
                &name, 1);
        xx = headerAddOrAppendEntry(h, RPMTAG_PROVIDEFLAGS, RPM_INT32_TYPE,
                &pFlags, 1);
        xx = headerAddOrAppendEntry(h, RPMTAG_PROVIDEVERSION, RPM_STRING_ARRAY_TYPE,
                &pEVR, 1);
    }
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////

#include <iostream>

#include <y2util/Y2SLog.h>

#include <y2pm/Timecount.h>
#include <y2pm/librpmDb.h>
#include <y2pm/RpmDbCallbacks.h>

using namespace std;

#undef Y2LOG
#define Y2LOG "librpmDb"

/******************************************************************
**
**
**	FUNCTION NAME : internal_convertV3toV4
**	FUNCTION TYPE : int
*/
PMError internal_convertV3toV4( const Pathname & v3db_r, const constlibrpmDbPtr & v4db_r,
				RpmDbCallbacks::ConvertDbReport::Send & report )
{
  typedef librpmDb::Error Error;
  Timecount _t( "convert V3 to V4" );
  MIL << "Convert rpm3 database to rpm4" << endl;

  // Check arguments
  FD_t fd = fadOpen( v3db_r.asString().c_str() );
  if ( fd == 0 ) {
    Fclose( fd );
    ERR << "Can't open rpmV3 database for reading: " << v3db_r << endl;
    return Error::E_RpmDB_open_failed;
  }

  if ( ! v4db_r ) {
    Fclose( fd );
    INT << "NULL rpmV4 database passed as argument!" << endl;
    return Error::E_invalid_argument;
  }

  PMError err = v4db_r->error();
  if ( err ) {
    Fclose( fd );
    INT << "Can't access rpmV4 database " << v4db_r << endl;
    return err;
  }

  // open rpmV4 database for writing. v4db_r is ok so librpm should
  // be properly initialized.
  rpmdb db = 0;
  string rootstr( v4db_r->root().asString() );
  const char * root = ( rootstr == "/" ? NULL : rootstr.c_str() );

  int res = ::rpmdbOpen( root, &db, O_RDWR, 0644 );
  if ( res || ! db ) {
    err = Error::E_RpmDB_open_failed;
    if ( db ) {
      ::rpmdbClose( db );
    }
    Fclose( fd );
    ERR << "rpmdbOpen error(" << res << "): " << endl;
    return err;
  }

  // Check ammount of packages to process.
  int max = 0;
  for ( int offset = fadFirstOffset(fd); offset; offset = fadNextOffset(fd, offset) ) {
    ++max;
  }
  MIL << "Packages in rpmV3 database " << v3db_r << ": " << max << endl;

  ProgressData pd( max );
  unsigned failed      = 0;
  unsigned ignored     = 0;
  unsigned alreadyInV4 = 0;
  report->progress( pd, failed, ignored, alreadyInV4 );

  if ( !max ) {
    Fclose( fd );
    ::rpmdbClose( db );
    return err;
  }

  // Start conversion.
  CBSuggest proceed;
  for ( int offset = fadFirstOffset(fd); offset && proceed != CBSuggest::CANCEL;
	offset = fadNextOffset(fd, offset),
	report->progress( pd.incr(), failed, ignored, alreadyInV4 ) ) {

    // have to use lseek instead of Fseek because headerRead
    // uses low level IO
    if ( lseek( Fileno( fd ), (off_t)offset, SEEK_SET ) == -1 ) {
      proceed = report->dbReadError( offset );
      ostream * reportAs = &(ERR);
      if ( proceed == CBSuggest::SKIP ) {
	// ignore this error
	++ignored;
	reportAs = &(WAR << "IGNORED: ");
      } else {
	// PROCEED will fail after conversion; CANCEL immediately stop loop
	++failed;
      }
      (*reportAs) << "rpmV3 database entry: Can't seek to offset " << offset << " (errno " << errno << ")" << endl;
      continue;
    }
    Header h = headerRead(fd, HEADER_MAGIC_NO);
    if ( ! h ) {
      proceed = report->dbReadError( offset );
      ostream * reportAs = &(ERR);
      if ( proceed == CBSuggest::SKIP ) {
	// ignore this error
	++ignored;
	reportAs = &(WAR << "IGNORED: ");
      } else {
	// PROCEED will fail after conversion; CANCEL immediately stop loop
	++failed;
      }
      (*reportAs) << "rpmV3 database entry: No header at offset " << offset << endl;
      continue;
    }
    compressFilelist(h);
    providePackageNVR(h);
    const char *name = 0;
    const char *version = 0;
    const char *release = 0;
    headerNVR(h, &name, &version, &release);
    string nrv( string(name) + "-" +  version + "-" + release );
    rpmdbMatchIterator mi = rpmdbInitIterator(db, RPMTAG_NAME, name, 0);
    rpmdbSetIteratorRE(mi, RPMTAG_VERSION, RPMMIRE_DEFAULT, version);
    rpmdbSetIteratorRE(mi, RPMTAG_RELEASE, RPMMIRE_DEFAULT, release);
    if (rpmdbNextIterator(mi)) {
      report->dbInV4( nrv );
      WAR << "SKIP: rpmV3 database entry: " << nrv << " is already in rpmV4 database" << endl;
      rpmdbFreeIterator(mi);
      headerFree(h);
      ++alreadyInV4;
      continue;
    }
    rpmdbFreeIterator(mi);
    if (rpmdbAdd(db, -1, h, 0, 0)) {
      report->dbWriteError( nrv );
      proceed = CBSuggest::CANCEL; // immediately stop loop
      ++failed;
      ERR << "rpmV4 database error: could not add " << nrv << " to rpmV4 database" << endl;
      headerFree(h);
      continue;
    }
    headerFree(h);
  }

  Fclose(fd);
  ::rpmdbClose(db);

  if ( failed ) {
    ERR << "Convert rpm3 database to rpm4: Aborted after "
      << pd.val() << " package(s) and " << (failed+ignored) << " error(s)."
	<< endl;
    err = Error::E_RpmDB_convert_failed;
  } else {
    MIL << "Convert rpm3 database to rpm4: " << max << " package(s) processed";
    if ( alreadyInV4 ) {
      MIL << "; " << alreadyInV4 << " already present in rpmV4 database";
    }
    if ( ignored ) {
      MIL << "; IGNORED: " << ignored << " unconverted due to error";
    }
    MIL << endl;
  }

  return err;
}

/******************************************************************
**
**
**	FUNCTION NAME : convertV3toV4
**	FUNCTION TYPE : int
*/
PMError convertV3toV4( const Pathname & v3db_r, const constlibrpmDbPtr & v4db_r )
{
  // report
  RpmDbCallbacks::ConvertDbReport::Send report( RpmDbCallbacks::convertDbReport );
  report->start( v3db_r );

  PMError err = internal_convertV3toV4( v3db_r, v4db_r, report );

  report->stop( err );
  return err;
}
