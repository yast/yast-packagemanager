#include <iomanip>
#include <fstream>
#include <string>
#include <list>

#include <y2util/Y2SLog.h>
#include <y2util/Date.h>
#include <y2util/stringutil.h>
#include <y2util/ExternalProgram.h>

#include <Y2PM.h>
#include <y2pm/RpmDb.h>
#include <y2pm/RpmLibDb.h>

#include <y2pm/InstSrcManager.h>
#include <y2pm/InstSrc.h>
#include <y2pm/InstSrcDescr.h>
#include <y2pm/MediaAccess.h>
#include <y2pm/PMPackageManager.h>
#include <y2pm/PMSelectionManager.h>
#include <y2pm/InstTarget.h>
#include <y2pm/Timecount.h>
#include <y2pm/PMPackageImEx.h>


using namespace std;

#define TMGR Y2PM::instTarget()
#define PMGR Y2PM::packageManager()
#define SMGR Y2PM::selectionManager()
#define ISM  Y2PM::instSrcManager()

ostream & operator <<( ostream & str, const list<string> & t ) {
  stringutil::dumpOn( str, t, true );
  return str;
}

ostream & operator<<( ostream & str, const PkgSplitSet & obj ) {
  for ( PkgSplitSet::const_iterator it = obj.begin(); it != obj.end(); ++it ) {
    str << *it << endl;
  }
  return str;
}

void dataDump( ostream & str, constPMPackagePtr p ) {
  str << p << " ++++++++++++++++++++++++++++" << endl;
  str << "SUMMARY:       " << p->summary() << endl;
  str << "DESCRIPTION:   " << p->description() << endl;
  str << "INSNOTIFY:     " << p->insnotify() << endl;
  str << "DELNOTIFY:     " << p->delnotify() << endl;
  str << "SIZE:          " << p->size() << endl;
  str << "INSTSRCLABEL:  " << p->instSrcLabel() << endl;
  str << "INSTSRCVENDOR: " << p->instSrcVendor() << endl;
  str << "INSTSRCRANK:   " << p->instSrcRank() << endl;
  str << "BUILDTIME:     " << p->buildtime() << endl;
  str << "BUILDHOST:     " << p->buildhost() << endl;
  str << "INSTALLTIME:   " << p->installtime() << endl;
  str << "DISTRIBUTION:  " << p->distribution() << endl;
  str << "VENDOR:        " << p->vendor() << endl;
  str << "LICENSE:       " << p->license() << endl;
  str << "PACKAGER:      " << p->packager() << endl;
  str << "GROUP:         " << p->group() << endl;
  str << "CHANGELOG:     " << p->changelog() << endl;
  str << "URL:           " << p->url() << endl;
  str << "OS:            " << p->os() << endl;
  str << "PREIN:         " << p->prein() << endl;
  str << "POSTIN:        " << p->postin() << endl;
  str << "PREUN:         " << p->preun() << endl;
  str << "POSTUN:        " << p->postun() << endl;
  str << "SOURCELOC:     " << p->sourceloc() << endl;
  str << "SOURCESIZE:    " << p->sourcesize() << endl;
  str << "ARCHIVESIZE:   " << p->archivesize() << endl;
  str << "AUTHORS:       " << p->authors() << endl;
  str << "FILENAMES:     " << p->filenames() << endl;
  str << "RECOMMENDS:    " << p->recommends() << endl;
  str << "SUGGESTS:      " << p->suggests() << endl;
  str << "LOCATION:      " << p->location() << endl;
  str << "MEDIANR:       " << p->medianr() << endl;
  str << "KEYWORDS:      " << p->keywords() << endl;
  str << "INSTSOURCE:    " << p->source() << endl;
  str << "IS REMOTE:     " << p->isRemote() << endl;
  str << "PROVIDES:      " << p->provides().size() << endl;
  str << "REQUIRES:      " << p->requires().size() << endl;
  str << "CONFLICTS:     " << p->conflicts().size() << endl;
  str << "OBSOLETES:     " << p->obsoletes().size() << endl;
  str << "PREREQUIRES:   " << p->prerequires().size() << endl;
  str << "SPLITPROVIDES: " << p->splitprovides().size() << endl;
  str << "---------------" << endl;
}

void dummyDU()
{
  std::set<PkgDuMaster::MountPoint> mountpoints;
  mountpoints.insert( PkgDuMaster::MountPoint( "/",          FSize(4,FSize::K), FSize(1,FSize::G) ) );
  mountpoints.insert( PkgDuMaster::MountPoint( "/boot",      FSize(4,FSize::K), FSize(1,FSize::G) ) );
  mountpoints.insert( PkgDuMaster::MountPoint( "/bin",       FSize(4,FSize::K), FSize(1,FSize::G) ) );
  mountpoints.insert( PkgDuMaster::MountPoint( "/etc",       FSize(4,FSize::K), FSize(1,FSize::G) ) );
  mountpoints.insert( PkgDuMaster::MountPoint( "/lib",       FSize(4,FSize::K), FSize(1,FSize::G) ) );
  mountpoints.insert( PkgDuMaster::MountPoint( "/opt",       FSize(4,FSize::K), FSize(1,FSize::G) ) );
  mountpoints.insert( PkgDuMaster::MountPoint( "/sbin",      FSize(4,FSize::K), FSize(1,FSize::G) ) );
  mountpoints.insert( PkgDuMaster::MountPoint( "/usr",       FSize(4,FSize::K), FSize(1,FSize::G) ) );
  mountpoints.insert( PkgDuMaster::MountPoint( "/usr/local", FSize(4,FSize::K), FSize(1,FSize::G) ) );
  mountpoints.insert( PkgDuMaster::MountPoint( "/var",       FSize(4,FSize::K), FSize(1,FSize::G) ) );
  MIL << mountpoints;
  PMGR.setMountPoints( mountpoints );
}

InstSrcManager::ISrcId newSrc( const string & url_r ) {
  Url url( url_r );
  InstSrcManager::ISrcIdList idlist;
  Timecount _t( url_r.c_str() );
  PMError err = ISM.scanMedia( idlist, url_r );
  ( err ? ERR : MIL ) << "newSrc: " << idlist.size() << " (" << err << ")" << endl;
  return( idlist.size() ? *idlist.begin() : 0 );
}

ostream & dumpPkgWhatIf( ostream & str, bool all = false )
{
  str << "+++[dumpPkgWhatIf]+++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
  for ( PMManager::PMSelectableVec::const_iterator it = PMGR.begin(); it != PMGR.end(); ++it ) {
    if ( all || (*it)->to_modify() || (*it)->is_taboo() ) {
      (*it)->dumpStateOn( str ) << endl;
    }
  }
  str << "---[dumpPkgWhatIf]---------------------------------------------------" << endl;
  return str;
}

ostream & dumpSelWhatIf( ostream & str, bool all = false  )
{
  str << "+++[dumpSelWhatIf]+++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
  for ( PMManager::PMSelectableVec::const_iterator it = SMGR.begin(); it != SMGR.end(); ++it ) {
    if ( all || (*it)->to_modify() ) {
      (*it)->dumpStateOn( str ) << endl;
    }
  }
  str << "---[dumpSelWhatIf]---------------------------------------------------" << endl;
  return str;
}

/******************************************************************
 ******************************************************************/
struct pdata {
  unsigned at;
  bool     isSrc;
  Pathname pkgfile;
  pdata();
  pdata( const Pathname & citem_r, const int & isSource_r, const unsigned & at_r ) {
    pkgfile = citem_r;
    isSrc = isSource_r;
    at = at_r;
  }
  bool operator==( const pdata & rhs ) const {
    return( at == rhs.at );
  }
  bool operator<( const pdata & rhs ) const {
    return( at < rhs.at );
  }
  bool operator!=( const pdata & rhs ) const { return( ! operator==( rhs ) ); }
};

void cachetest( const Pathname cachefile_r )
{
  Timecount _t( cachefile_r.asString().c_str() );

  vector<pdata> data;

  PkgHeaderCache _cache( cachefile_r );
  if ( !_cache.openCache() ) {
    ERR << "Failed to open cache " << cachefile_r << endl;
    return;
  } else {
    MIL << "Opened cache " << cachefile_r << endl;
  }

  unsigned hpos;
  Pathname pkgfile;
  int      isSource;
  for ( constRpmLibHeaderPtr iter = _cache.getFirst( pkgfile, isSource, hpos );
	iter; iter = _cache.getNext( pkgfile, isSource, hpos ) ) {
    data.push_back( pdata( pkgfile, isSource, hpos ) );
    DBG << "At " << hpos << (isSource?" src ":" bin ") << iter << " for " << pkgfile << endl;
  }

  MIL << " Ok:" << _cache.cacheOk() << " Got:" << data.size() << endl;
  _cache.closeCache();
}

void DoTest();

/******************************************************************
**
**
**	FUNCTION NAME : main
**	FUNCTION TYPE : int
**
**	DESCRIPTION :
*/
int main()
{
  Y2Logging::setLogfileName("-");
  MIL << "START" << endl;
  if ( 1 ) {
  Y2PM::noAutoInstSrcManager();
  Timecount _t( "Launch InstTarget" );
  Y2PM::instTarget(true,"/");
  _t.start( "Launch PMPackageManager" );
  Y2PM::packageManager();
  _t.start( "Launch PMSelectionManager" );
  Y2PM::selectionManager();
  _t.start( "Launch InstSrcManager" );
  Y2PM::instSrcManager();
  _t.stop();
  INT << "Total Packages "   << PMGR.size() << endl;
  INT << "Total Selections " << SMGR.size() << endl;
  }

  InstSrcManager::ISrcId nid( newSrc( "dir:////tmp/PLAINRPM" ) );
  ISM.enableSource( nid );

  //DoTest();
  //SEC << "STOP" << endl;
  //INT << "===============================================" << endl;

  //cachetest( "/tmp/PLAIN/IS_PLAINcache" );
  //cachetest( "/tmp/PLAIN/IS_PLAINcache.gz" );

  SEC << "STOP" << endl;
  return 0;
}

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

extern "C" {
#include <rpm/rpmlib.h>
#include <rpm/rpmmacro.h>
#include <fcntl.h>
}

int XFtell( FD_t _fd ) {
  FILE * fp = (FILE *)fdGetFp(_fd);
  if ( fp ) {
    int rc = ftell( fp );
    return rc;
  }
  int fd = Fileno(_fd);
  if ( fd >= 0 ) {
    int rc = lseek( fd, 0, SEEK_CUR );
    return rc;
  }
  WAR << "no fd/fp" << -1 << endl;
  return -1;
}

void dread( FD_t fd ) {
  if ( fd == 0 || ::Ferror(fd) ) {
    ERR << "Can't open: " << ::Fstrerror(fd) << endl;
    return;
  }
  unsigned at = ::XFtell( fd );
  (at==0?DBG:ERR) << " at " << at << ": " << ::Ferror(fd) << " (" << ::Fstrerror(fd) << ")" << endl;

  char magic[1024];
  memset( magic, 0, 1024 );
  size_t got = ::Fread( magic, sizeof(char), 6, fd );
  (got==6?DBG:ERR) << " got " << got << ": " << ::Ferror(fd) << " (" << ::Fstrerror(fd) << ")" << endl;
  DBG << "  >>" << magic << "<<" << endl;

  at = ::Fseek( fd, 1, SEEK_CUR );
  at = ::XFtell( fd );
  (at==7?DBG:ERR) << " at " << at << ": " << ::Ferror(fd) << " (" << ::Fstrerror(fd) << ")" << endl;

  memset( magic, 0, 1024 );
  got = ::Fread( magic, sizeof(char), 4, fd );
  (got==3?DBG:ERR) << " got " << got << ": " << ::Ferror(fd) << " (" << ::Fstrerror(fd) << ")" << endl;
  DBG << "  >>" << magic << "<<" << endl;

  at = ::Fseek( fd, 1, SEEK_SET );
  at = ::XFtell( fd );
  (at==1?DBG:ERR) << " at " << at << ": " << ::Ferror(fd) << " (" << ::Fstrerror(fd) << ")" << endl;

  memset( magic, 0, 1024 );
  got = ::Fread( magic, sizeof(char), 2, fd );
  (got==2?DBG:ERR) << " got " << got << ": " << ::Ferror(fd) << " (" << ::Fstrerror(fd) << ")" << endl;
  DBG << "  >>" << magic << "<<" << endl;

  at = ::XFtell( fd );
  (at==3?DBG:ERR) << " at " << at << ": " << ::Ferror(fd) << " (" << ::Fstrerror(fd) << ")" << endl;
  INT << "STOP" << endl;
}

extern "C" {
#include <netinet/in.h>
struct entryInfo {
    int_32 tag;
    int_32 type;
    int_32 offset;              /* Offset from beginning of data segment,
                                   only defined on disk */
    int_32 count;
};
}

Header hdread( FD_t fd ) {
  if ( fd == 0 || ::Ferror(fd) ) {
    ERR << "Can't open: " << ::Fstrerror(fd) << endl;
    return 0;
  }
  unsigned at = ::XFtell( fd );
  (at==0?DBG:ERR) << " at " << at << ": " << ::Ferror(fd) << " (" << ::Fstrerror(fd) << ")" << endl;

  ::Fseek( fd, 64, SEEK_SET );
  at = ::XFtell( fd );
  (at==64?DBG:ERR) << " at " << at << ": " << ::Ferror(fd) << " (" << ::Fstrerror(fd) << ")" << endl;

  char sig[] = "xxxxxxxx";
  size_t got = ::Fread( sig, sizeof(char), 8, fd );
  if ( got != 8 ) {
    if ( got || ::Ferror(fd) ) {
      ERR << "Error reading entry (" << ::Fstrerror(fd) << ")" << endl;
    }
    return 0;
  }
  if ( sig[0] != '@' || sig[7] != '@' ) {
    ERR << "Invalid entry." << endl;
    return 0;
  }
  sig[7] = '\0';
  unsigned count = atoi( &sig[1] );

  char citem[count+1];
  if ( ::Fread( citem, sizeof(char), count, fd ) != count ) {
    ERR << "Error reading entry data (" << ::Fstrerror(fd) << ")" << endl;
    return 0;
  }
  citem[count] = '\0';

  at = ::XFtell( fd );
  (at==64+8+count?DBG:ERR) << " at " << at << ": " << ::Ferror(fd) << " (" << ::Fstrerror(fd) << ")" << endl;

  ////////////////////////////////////////////////////////////////////////////////
  static const int_32 header_magic = 0x01e8ad8e;
  bool magicp = true; // HEADER_MAGIC_YES
  int_32 block[4];
  int_32 il, dl;
  unsigned totalSize = 0;

  unsigned xxx;

  count = (magicp ? 4 : 2) * sizeof(int_32);
  if ( (xxx=::Fread( block, sizeof(char), count, fd )) != count ) {
    ERR << "Error reading header info " << xxx << " <-> " << count << " (" << ::Fstrerror(fd) << ")" << endl;
    return 0;
  }
  DBG << "Read header info " << xxx << " <-> " << count << " (" << ::Fstrerror(fd) << ")" << endl;

  count = 0;

  if ( magicp ) {
    if ( block[count] != header_magic ) {
      ERR << "Error bad header magic " << stringutil::hexstring( block[count] )
	<< " (" << stringutil::hexstring( header_magic ) << ")" << endl;
      return 0;
    }
    count += 2;
  }

  il = ntohl( block[count++] );
  dl = ntohl( block[count++] );

  totalSize = (2*sizeof(int_32)) + (il * sizeof(struct entryInfo)) + dl;
  if (totalSize > (32*1024*1024)) {
    ERR << "Error header ecxeeds 32Mb limit (" << totalSize << ")" << endl;
    return NULL;
  }
  DBG << "header size " << totalSize << endl;

  char * data = new char[totalSize];
  int_32 * p = (int_32 *)data;
  Header h = 0;

  *p++ = htonl(il);
  *p++ = htonl(dl);
  totalSize -= (2*sizeof(int_32));

  if ( ::Fread( (char *)p, sizeof(char), totalSize, fd ) != totalSize ) {
    ERR << "Error reading header data (" << ::Fstrerror(fd) << ")" << endl;
  } else {
    h = ::headerLoad( data );
    if ( !h ) {
      ERR << "Error loading header data" << endl;
    }
  }

  delete [] data;

  ////////////////////////////////////////////////////////////////////////////////
  (h==0?ERR:DBG) << " header " << h << endl;

  at = ::XFtell( fd );
  DBG << " at " << at << ": " << ::Ferror(fd) << " (" << ::Fstrerror(fd) << ")" << endl;

  INT << "STOP" << endl;
  return NULL;
}

void DoTest() {

  FD_t nfd = ::Fopen( "/tmp/PLAIN/IS_PLAINcache", "r" );
  INT << "START r /tmp/PLAIN/IS_PLAINcache" << endl;
  hdread( nfd );
  Fclose( nfd );

  FD_t gzbd = ::Fopen( "/tmp/PLAIN/IS_PLAINcache.gz", "r.fdio" );
  FD_t gzfd = ::Fdopen( gzbd, "r.gzdio"  );
  INT << "START r.gzdio /tmp/PLAIN/IS_PLAINcache.gz" << endl;
  hdread( gzfd );
  Fclose( gzbd );

#if 0
  FD_t nfd = ::Fopen( "test", "r" );
  INT << "START r" << endl;
  dread( nfd );
  Fclose( nfd );

  FD_t gzbd = ::Fopen( "test.gz", "r.fdio" );
  FD_t gzfd = ::Fdopen( gzbd, "r.gzdio"  );
  INT << "START r.gzdio" << endl;
  dread( gzfd );
  Fclose( gzbd );
#endif
}

