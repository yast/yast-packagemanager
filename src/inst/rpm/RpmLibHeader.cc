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

  File:       RpmLibHeader.cc

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose:

/-*/

extern "C" {
#include <rpm/rpmlib.h>
}

#include <iostream>
#include <map>
#include <set>

#include <y2util/Y2SLog.h>
#include <y2util/PathInfo.h>

#include <y2pm/RpmLibHeader.h>
#include <y2pm/PkgDu.h>

using namespace std;

#undef Y2LOG
#define Y2LOG "RpmLibHeader"

///////////////////////////////////////////////////////////////////
//	CLASS NAME : RpmLibHeaderPtr
//	CLASS NAME : constRpmLibHeaderPtr
///////////////////////////////////////////////////////////////////
IMPL_DERIVED_POINTER(RpmLibHeader,binHeader,binHeader);

///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibHeader::RpmLibHeader
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
RpmLibHeader::RpmLibHeader( Header h_r, bool isSrc )
    : binHeader( h_r )
    , _isSrc( isSrc )
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibHeader::RpmLibHeader
//	METHOD TYPE : Constructor
//
RpmLibHeader::RpmLibHeader( binHeaderPtr & rhs, bool isSrc )
    : binHeader( rhs )
    , _isSrc( isSrc )
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibHeader::~RpmLibHeader
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
RpmLibHeader::~RpmLibHeader()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibHeader::readPackage
//	METHOD TYPE : constRpmLibHeaderPtr
//
#warning OLD RPMAPI CHECK IT
static int rpmReadPackageHeader(FD_t fd, Header * hdr, int * isSource, int * major, int * minor ) {
  INT << "Illegal use of old api: " << __FUNCTION__ << endl;
  * hdr = 0;
  * isSource = 0;
  * major = 0;
  * minor = 0;
  return -1;
}
constRpmLibHeaderPtr RpmLibHeader::readPackage( const Pathname & path_r )
{
  PathInfo file( path_r );
  if ( ! file.isFile() ) {
    ERR << "Not a file: " << file << endl;
    return (RpmLibHeader*)0;
  }

  FD_t fd = ::Fopen( file.asString().c_str(), "r" );
  if ( fd == 0 || ::Ferror(fd) ) {
    ERR << "Can't open file for reading: " << file << " (" << ::Fstrerror(fd) << ")" << endl;
    if ( fd )
      ::Fclose( fd );
    return (RpmLibHeader*)0;
  }

  Header nh    = 0;
  int isSource = 0;
  int major    = 0;
  int minor    = 0;

  int res = ::rpmReadPackageHeader( fd, &nh, &isSource, &major, &minor );
  ::Fclose( fd );
  if ( res || !nh ) {
    ERR << "Error reading: " << file << (res==1?" (bad magic)":"") << endl;
    return (RpmLibHeader*)0;
  }

  constRpmLibHeaderPtr h( new RpmLibHeader( nh, isSource ) );
  MIL << major << "." << minor << "-" << (isSource?"src ":"bin ") << h << " from " << path_r << endl;
  return h;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibHeader::dumpOn
//	METHOD TYPE : ostream &
//
//	DESCRIPTION :
//
ostream & RpmLibHeader::dumpOn( ostream & str ) const
{
  binHeader::dumpOn( str );
  return binHeader::dumpOn( str ) << '{' << tag_name() << "-" << tag_edition() << ( _isSrc ? ".src}" : "}");
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibHeader::tag_name
//	METHOD TYPE : PkgName
//
//	DESCRIPTION :
//
PkgName RpmLibHeader::tag_name() const
{
  return PkgName( string_val( RPMTAG_NAME ) );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibHeader::tag_edition
//	METHOD TYPE : PkgEdition
//
//	DESCRIPTION :
//
PkgEdition RpmLibHeader::tag_edition() const
{
  return PkgEdition( int_val   ( RPMTAG_EPOCH ),
		     string_val( RPMTAG_VERSION ),
		     string_val( RPMTAG_RELEASE ),
		     int_val   ( RPMTAG_BUILDTIME ) );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibHeader::tag_arch
//	METHOD TYPE : PkgArch
//
//	DESCRIPTION :
//
PkgArch RpmLibHeader::tag_arch() const
{
  return PkgArch( string_val( RPMTAG_ARCH ) );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibHeader::tag_installtime
//	METHOD TYPE : Date
//
//	DESCRIPTION :
//
Date RpmLibHeader::tag_installtime() const
{
  return int_val( RPMTAG_INSTALLTIME );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibHeader::tag_buildtime
//	METHOD TYPE : Date
//
//	DESCRIPTION :
//
Date RpmLibHeader::tag_buildtime() const
{
  return int_val( RPMTAG_BUILDTIME );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibHeader::PkgRelList_val
//	METHOD TYPE : unsigned
//
//	DESCRIPTION :
//
PMSolvable::PkgRelList_type RpmLibHeader::PkgRelList_val( tag tag_r, FileDeps::FileNames * freq_r ) const
{
  PMSolvable::PkgRelList_type ret;

  int_32  kindFlags   = 0;
  int_32  kindVersion = 0;

  switch ( tag_r ) {
  case RPMTAG_REQUIRENAME:
    kindFlags   = RPMTAG_REQUIREFLAGS;
    kindVersion = RPMTAG_REQUIREVERSION;
    break;
  case RPMTAG_PROVIDENAME:
    kindFlags   = RPMTAG_PROVIDEFLAGS;
    kindVersion = RPMTAG_PROVIDEVERSION;
    break;
  case RPMTAG_OBSOLETENAME:
    kindFlags   = RPMTAG_OBSOLETEFLAGS;
    kindVersion = RPMTAG_OBSOLETEVERSION;
    break;
  case RPMTAG_CONFLICTNAME:
    kindFlags   = RPMTAG_CONFLICTFLAGS;
    kindVersion = RPMTAG_CONFLICTVERSION;
    break;
  default:
    INT << "Illegal RPMTAG_dependencyNAME " << tag_r << endl;
    return ret;
    break;
  }

  stringList names;
  unsigned count = string_list( tag_r, names );
  if ( !count )
    return ret;

  intList  flags;
  int_list( kindFlags, flags );

  stringList versions;
  string_list( kindVersion, versions );

  PkgName self( string_val( RPMTAG_NAME ) );

  for ( unsigned i = 0; i < count; ++i ) {

    PkgName n( names[i] );

    if ( n == self ) {
      //_I__("DEPCHECK") << self << " has dependency on it self" << endl;
      continue;
    }

    rel_op op = NONE;
    int_32 f  = flags[i];
    string v  = versions[i];

    if ( n.asString()[0] == '/' ) {
      if ( freq_r ) {
	freq_r->insert( n );
      }
    } else {
      if ( v.size() ) {
	switch ( f & RPMSENSE_SENSEMASK ) {
	case RPMSENSE_LESS:
	  op = LT;
	  break;
	case RPMSENSE_LESS|RPMSENSE_EQUAL:
	  op = LE;
	  break;
	case RPMSENSE_GREATER:
	  op = GT;
	  break;
	case RPMSENSE_GREATER|RPMSENSE_EQUAL:
	  op = GE;
	  break;
	case RPMSENSE_EQUAL:
	  op = EQ;
	  break;
	}
      }
    }

    if ( op == NONE ) {
      ret.push_back( PkgRelation( PkgName( n ) ) );
    } else {
      ret.push_back( PkgRelation( PkgName( n ), op, PkgEdition::fromString( v ) ) );
    }

    if ( f & RPMSENSE_PREREQ ) {
      ret.back().setPreReq( true );
    }
  }

  return ret;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibHeader::tag_provides
//	METHOD TYPE : PMSolvable::PkgRelList_type
//
//	DESCRIPTION :
//
PMSolvable::PkgRelList_type RpmLibHeader::tag_provides( FileDeps::FileNames * freq_r ) const
{
  return PkgRelList_val( RPMTAG_PROVIDENAME, freq_r );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibHeader::tag_requires
//	METHOD TYPE : PMSolvable::PkgRelList_type
//
//	DESCRIPTION :
//
PMSolvable::PkgRelList_type RpmLibHeader::tag_requires( FileDeps::FileNames * freq_r ) const
{
  return PkgRelList_val( RPMTAG_REQUIRENAME, freq_r );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibHeader::tag_conflicts
//	METHOD TYPE : PMSolvable::PkgRelList_type
//
//	DESCRIPTION :
//
PMSolvable::PkgRelList_type RpmLibHeader::tag_conflicts( FileDeps::FileNames * freq_r ) const
{
  return PkgRelList_val( RPMTAG_CONFLICTNAME, freq_r );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibHeader::tag_obsoletes
//	METHOD TYPE : PMSolvable::PkgRelList_type
//
//	DESCRIPTION :
//
PMSolvable::PkgRelList_type RpmLibHeader::tag_obsoletes( FileDeps::FileNames * freq_r ) const
{
  return PkgRelList_val( RPMTAG_OBSOLETENAME, freq_r );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibHeader::tag_size
//	METHOD TYPE : FSize
//
//	DESCRIPTION :
//
FSize RpmLibHeader::tag_size() const
{
  return int_val( RPMTAG_SIZE );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibHeader::tag_archivesize
//	METHOD TYPE : FSize
//
//	DESCRIPTION :
//
FSize RpmLibHeader::tag_archivesize() const
{
  return int_val( RPMTAG_ARCHIVESIZE );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibHeader::tag_summary
//	METHOD TYPE : std::string
//
//	DESCRIPTION :
//
std::string RpmLibHeader::tag_summary() const
{
  return string_val( RPMTAG_SUMMARY );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibHeader::tag_description
//	METHOD TYPE : std::string
//
//	DESCRIPTION :
//
std::string RpmLibHeader::tag_description() const
{
  return string_val( RPMTAG_DESCRIPTION );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibHeader::tag_group
//	METHOD TYPE : std::string
//
//	DESCRIPTION :
//
std::string RpmLibHeader::tag_group() const
{
  return string_val( RPMTAG_GROUP );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibHeader::tag_vendor
//	METHOD TYPE : std::string
//
//	DESCRIPTION :
//
std::string RpmLibHeader::tag_vendor() const
{
  return string_val( RPMTAG_VENDOR );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibHeader::tag_distribution
//	METHOD TYPE : std::string
//
//	DESCRIPTION :
//
std::string RpmLibHeader::tag_distribution() const
{
  return string_val( RPMTAG_DISTRIBUTION );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibHeader::tag_license
//	METHOD TYPE : std::string
//
//	DESCRIPTION :
//
std::string RpmLibHeader::tag_license() const
{
  return string_val( RPMTAG_LICENSE );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibHeader::tag_buildhost
//	METHOD TYPE : std::string
//
//	DESCRIPTION :
//
std::string RpmLibHeader::tag_buildhost() const
{
  return string_val( RPMTAG_BUILDHOST );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibHeader::tag_packager
//	METHOD TYPE : std::string
//
//	DESCRIPTION :
//
std::string RpmLibHeader::tag_packager() const
{
  return string_val( RPMTAG_PACKAGER );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibHeader::tag_url
//	METHOD TYPE : std::string
//
//	DESCRIPTION :
//
std::string RpmLibHeader::tag_url() const
{
  return string_val( RPMTAG_URL );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibHeader::tag_os
//	METHOD TYPE : std::string
//
//	DESCRIPTION :
//
std::string RpmLibHeader::tag_os() const
{
  return string_val( RPMTAG_OS );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibHeader::tag_prein
//	METHOD TYPE : std::string
//
//	DESCRIPTION :
//
std::string RpmLibHeader::tag_prein() const
{
  return string_val( RPMTAG_PREIN );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibHeader::tag_postin
//	METHOD TYPE : std::string
//
//	DESCRIPTION :
//
std::string RpmLibHeader::tag_postin() const
{
  return string_val( RPMTAG_POSTIN );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibHeader::tag_preun
//	METHOD TYPE : std::string
//
//	DESCRIPTION :
//
std::string RpmLibHeader::tag_preun() const
{
  return string_val( RPMTAG_PREUN );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibHeader::tag_postun
//	METHOD TYPE : std::string
//
//	DESCRIPTION :
//
std::string RpmLibHeader::tag_postun() const
{
  return string_val( RPMTAG_POSTUN );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibHeader::tag_sourcerpm
//	METHOD TYPE : std::string
//
//	DESCRIPTION :
//
std::string RpmLibHeader::tag_sourcerpm() const
{
  return string_val( RPMTAG_SOURCERPM );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibHeader::tag_filenames
//	METHOD TYPE : std::list<std::string>
//
//	DESCRIPTION :
//
std::list<std::string> RpmLibHeader::tag_filenames() const
{
  std::list<std::string> ret;

  stringList basenames;
  if ( string_list( RPMTAG_BASENAMES, basenames ) ) {
    stringList dirnames;
    string_list( RPMTAG_DIRNAMES, dirnames );
    intList  dirindexes;
    int_list( RPMTAG_DIRINDEXES, dirindexes );
    for ( unsigned i = 0; i < basenames.size(); ++ i ) {
      ret.push_back( dirnames[dirindexes[i]] + basenames[i] );
    }
  }

  return ret;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibHeader::tag_changelog
//	METHOD TYPE : PkgChangelog
//
//	DESCRIPTION :
//
PkgChangelog RpmLibHeader::tag_changelog() const
{
  PkgChangelog ret;

  intList times;
  if ( int_list( RPMTAG_CHANGELOGTIME, times ) ) {
    stringList names;
    string_list( RPMTAG_CHANGELOGNAME, names );
    stringList texts;
    string_list( RPMTAG_CHANGELOGTEXT, texts );
    for ( unsigned i = 0; i < times.size(); ++ i ) {
      ret.push_back( PkgChangelog::Entry( times[i], names[i], texts[i] ) );
    }
  }

  return ret;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibHeader::tag_du
//	METHOD TYPE : PkgDu &
//
//	DESCRIPTION :
//
PkgDu & RpmLibHeader::tag_du( PkgDu & dudata_r ) const
{
  dudata_r.clear();
  stringList basenames;
  if ( string_list( RPMTAG_BASENAMES, basenames ) ) {
    stringList dirnames;
    string_list( RPMTAG_DIRNAMES, dirnames );
    intList dirindexes;
    int_list( RPMTAG_DIRINDEXES, dirindexes );

    intList filedevices;
    int_list( RPMTAG_FILEDEVICES, filedevices );
    intList fileinodes;
    int_list( RPMTAG_FILEINODES, fileinodes );
    intList filesizes;
    int_list( RPMTAG_FILESIZES, filesizes );
    intList filemodes;
    int_list( RPMTAG_FILEMODES, filemodes );

    ///////////////////////////////////////////////////////////////////
    // Create and collect Entries by index. devino_cache is used to
    // filter out hardliks ( different name but same device and inode ).
    ///////////////////////////////////////////////////////////////////
    PathInfo::devino_cache trace;
    vector<PkgDu::Entry> entries;
    entries.resize( dirnames.size() );
    for ( unsigned i = 0; i < dirnames.size(); ++i ) {
      entries[i] = dirnames[i];
    }

    for ( unsigned i = 0; i < basenames.size(); ++ i ) {
      PathInfo::stat_mode mode( filemodes[i] );
      if ( mode.isFile() ) {
	if ( trace.insert( filedevices[i], fileinodes[i] ) ) {
	  // Count full 1K blocks
	  entries[dirindexes[i]]._size += FSize( filesizes[i] ).fullBlock();
	  ++(entries[dirindexes[i]]._files);
	}
	// else: hardlink; already counted this device/inode
      }
    }

    ///////////////////////////////////////////////////////////////////
    // Crreate and collect by index Entries. DevInoTrace is used to
    // filter out hardliks ( different name but same device and inode ).
    ///////////////////////////////////////////////////////////////////
    for ( unsigned i = 0; i < entries.size(); ++i ) {
      if ( entries[i]._size ) {
	dudata_r.add( entries[i] );
      }
    }
  }
  return dudata_r;
}

