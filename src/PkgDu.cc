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

  File:       PkgDu.cc

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Helper classes to collect package disk usage info.

/-*/

#include <iostream>
#include <map>

#include <y2util/Y2SLog.h>
#include <y2util/stringutil.h>

#include <y2pm/PkgDu.h>
#include <y2pm/PMPackage.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PkgDu::Entry
//
///////////////////////////////////////////////////////////////////

/******************************************************************
**
**
**	FUNCTION NAME : operator<<
**	FUNCTION TYPE : std::ostream &
**
**	DESCRIPTION :
*/
std::ostream & operator<<( std::ostream & str, const PkgDu::Entry & obj )
{
  return str << obj._dirname << '\t' << obj._size << "; files " << obj._files ;
}

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PkgDu
//
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PkgDu::addFrom
//	METHOD TYPE : void
//
//	DESCRIPTION : Convert to PkgDu
//                    The list<string> of dudata provided by a package looks
//                    like this:
//
//                    /                       0 5156 0 444
//                    etc/                    0   13 0   5
//                    etc/WindowMaker/       13    0 5   0
//                    ...
//
//                    1st col: directory path
//                    2nd col: ammount of byte (in kB) stored in directory
//                    3rd col: ammount of byte (in kB) stored below directory
//                    4th col: number of files created in in directory
//                    5th col: number of files created below directory
//
//
// We're only interested in entries which actually contain files (i.e. 2nd col
// or 4th col are not zero)
//
void PkgDu::addFrom( const std::list<std::string> & dudata_r )
{
  for ( list<string>::const_iterator it = dudata_r.begin(); it != dudata_r.end(); ++it ) {

    string::size_type sep = it->rfind( '/' );
    if ( sep == string::npos ) {
      WAR << "Illegal DU entry: " << *it << endl;
      continue;
    }

    string dir( it->substr( 0, sep+1 ) );
    // add leading '/' if missing
    if ( dir[0] != '/' )
      dir.insert( 0, 1, '/' );

    vector<string> tdata;
    if ( stringutil::split( it->substr( sep+1 ), tdata ) == 4 ) {
      FSize    sze( atoi( tdata[0].c_str() ), FSize::K );
      unsigned num( atoi( tdata[2].c_str() ) );
      if ( sze || num ) {
	add( dir, sze, num );
      }
    } else {
      WAR << "Illegal DU entry: " << *it << endl;
    }
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PkgDu::extract
//	METHOD TYPE : PkgDu::Entry
//
//	DESCRIPTION :
//
PkgDu::Entry PkgDu::extract( const std::string & dirname_r )
{
  Entry ret( dirname_r );

  iterator fst = begin();
  for ( ; fst != end() && !fst->isBelow( ret ); ++fst )
    ; // seek 1st equal or below

  if ( fst != end() ) {
    iterator lst = fst;
    for ( ; lst != end() && lst->isBelow( ret ); ++lst ) {
      // collect while below
      ret += *lst;
    }
    // remove
    _dirs.erase( fst, lst );
  }

  return ret;
}

/******************************************************************
**
**
**	FUNCTION NAME : operator<<
**	FUNCTION TYPE : std::ostream &
**
**	DESCRIPTION :
*/
std::ostream & operator<<( std::ostream & str, const PkgDu & obj )
{
  str << "PkgDu {" << endl;
  for ( PkgDu::EntrySet::const_iterator it = obj._dirs.begin(); it != obj._dirs.end(); ++it ) {
    str << "   " << *it << endl;
  }
  return str << "}";
}

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PkgDuMaster::MountPoint
//
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PkgDuMaster::MountPoint::assignData
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool PkgDuMaster::MountPoint::assignData( const MountPoint & rhs ) const
{
  if (    _mountpoint != rhs._mountpoint
       || _blocksize  != rhs._blocksize )
    return false;

  _total    = rhs._total;
  _used     = rhs._used;
  _pkgusage = rhs._pkgusage;
  return true;
}

/******************************************************************
**
**
**	FUNCTION NAME : operator<<
**	FUNCTION TYPE : ostream &
**
**	DESCRIPTION :
*/
ostream & operator<<( ostream & str, const PkgDuMaster::MountPoint & obj )
{
  str
    << "| T:" << obj.total()            .form( FSize::K, 11 )
    << "| U:" << obj.initial_used()     .form( FSize::K, 11 )
    << "| A:" << obj.initial_available().form( FSize::K, 11 )
    << "| p:" << obj.pkg_diff()         .form( FSize::K, 11 )
    << "| u:" << obj.pkg_used()         .form( FSize::K, 11 )
    << "| " << obj._mountpoint;
  return str;
}

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PkgDuMaster
//
///////////////////////////////////////////////////////////////////

unsigned PkgDuMaster::_counter = 0;

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PkgDuMaster::PkgDuMaster
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
PkgDuMaster::PkgDuMaster()
{
  newcount();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PkgDuMaster::~PkgDuMaster
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
PkgDuMaster::~PkgDuMaster()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PkgDuMaster::addMaster
//	METHOD TYPE : void
//
//	DESCRIPTION : We know (and rely) PkgDuSlave does not pass NULL or
//                    an array size != _mountpoints.size()
//
void PkgDuMaster::add( FSize * data_r )
{
  for ( set<MountPoint>::iterator it = _mountpoints.begin(); it != _mountpoints.end(); ++it, ++data_r ) {
    it->_pkgusage += *data_r;
    _pkg_diff += *data_r;
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PkgDuMaster::sub
//	METHOD TYPE : void
//
//	DESCRIPTION : We know (and rely) PkgDuSlave does not pass NULL or
//                    an array size != _mountpoints.size()
//
void PkgDuMaster::sub( FSize * data_r )
{
  for ( set<MountPoint>::iterator it = _mountpoints.begin(); it != _mountpoints.end(); ++it, ++data_r ) {
    it->_pkgusage -= *data_r;
    _pkg_diff -= *data_r;
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PkgDuMaster::resetStats
//	METHOD TYPE : unsigned
//
//	DESCRIPTION :
//
unsigned PkgDuMaster::resetStats()
{
  for ( set<MountPoint>::iterator it = _mountpoints.begin(); it != _mountpoints.end(); ++it ) {
    it->_pkgusage = 0;
  }
  _pkg_diff = 0;
  return _mountpoints.size();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PkgDuMaster::newcount
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void PkgDuMaster::newcount()
{
  _count = ++_counter;
  MIL << *this;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PkgDuMaster::setMountPoints
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void PkgDuMaster::setMountPoints( const set<MountPoint> & mountpoints_r )
{
  if ( _mountpoints.size() == mountpoints_r.size() ) {
    // Common case is update of nonvital data (total, used, etc.)
    // So we start, and if different vital data (mountpoint/blocksize)
    // occur we abort and assign the whole set.
    for ( set<MountPoint>::iterator lit = _mountpoints.begin(), rit = mountpoints_r.begin();
	  lit != _mountpoints.end(); ++lit, ++rit ) {
      if ( ! lit->assignData( *rit ) ) {
	// different vital data
	_mountpoints = mountpoints_r;
	newcount();
	break;
      }
    }
  } else {
    _mountpoints = mountpoints_r;
    newcount();
  }

  _pkg_diff = 0;
  for ( set<MountPoint>::iterator it = _mountpoints.begin(); it != _mountpoints.end(); ++it ) {
    _pkg_diff += it->_pkgusage;
  }
}

/******************************************************************
**
**
**	FUNCTION NAME : operator<<
**	FUNCTION TYPE : ostream &
**
**	DESCRIPTION :
*/
ostream & operator<<( ostream & str, const set<PkgDuMaster::MountPoint> & obj )
{
  for ( set<PkgDuMaster::MountPoint>::const_iterator it = obj.begin(); it != obj.end(); ++it ) {
    str << *it << endl;
  }
  return str;
}

/******************************************************************
**
**
**	FUNCTION NAME : operator<<
**	FUNCTION TYPE : ostream &
**
**	DESCRIPTION :
*/
ostream & operator<<( ostream & str, const PkgDuMaster & obj )
{
  str << "--[" << obj._count <<  "]----------------------------" << endl;
  str << obj._mountpoints;
  str << "---------------------------------" << endl;
  return str;
}

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PkgDuSlave
//
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PkgDuSlave::PkgDuSlave
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
PkgDuSlave::PkgDuSlave()
    : _count( 0 )
    , _data( 0 )
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PkgDuSlave::~PkgDuSlave
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
PkgDuSlave::~PkgDuSlave()
{
  delete [] _data;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PkgDuSlave::sync
//	METHOD TYPE : bool
//
//	DESCRIPTION : Assert not to return true, on NULL _data or
//                    an array size != master_r._mountpoints.size().
//
//                    - Whenever the masters set of mountpoints changes,
//                    the masters sync_count() changes. If the _count
//                    remembered here does not match the masters sync_count
//                    _data must be rebuilt according to masters new set of
//                    mountpoints. And we remember the new sync_count.
//
//                    - If masters set of mountpoints is not empty and
//                    pkg provides dudata, we must set up _data as an
//                    array of FSize. Size of the array must be equal
//                    to the size of masters set of mountpoints. The
//                    FSize values placed in _data must correspond to
//                    the sequence in which the masters set of mountpoints
//                    is iterated:
//
//                    master_r.mountpoints().begin() ->  "/"    <-  _data[0]
//                                                   ->  "/usr" <-  _data[1]
//                                                   ->  "/var" <-  _data[2]
//
bool PkgDuSlave::sync( const PMPackage & pkg_r, PkgDuMaster & master_r ) const
{
  if ( _count != master_r.sync_count() ) {
    delete [] _data;
    _data  = 0;
    _count = master_r.sync_count();

    // now see if there are actual data to contribute
    if ( master_r.mountpoints().size() ) {
      PkgDu dudata;
      pkg_r.du( dudata );

      if ( dudata.size() ) {
	///////////////////////////////////////////////////////////////////
	// So we've got mountpoints and a list of du data. mountpoints
	// are alphabetically sorted. Thus we start at the end to get deeper
	// entries fist. We collect and remove their data from dudata.
	///////////////////////////////////////////////////////////////////

	_data    = new FSize[master_r.mountpoints().size()]; // initial values == 0
	unsigned _dataidx = master_r.mountpoints().size() - 1;

	for ( set<MountPoint>::const_reverse_iterator mp = master_r.mountpoints().rbegin();
	      mp != master_r.mountpoints().rend(); ++mp, --_dataidx ) {

	  PkgDu::Entry collected = dudata.extract( mp->_mountpoint );

	  if ( collected._size || collected._files ) {
	    _data[_dataidx] = collected._size + collected._files * mp->_blocksize / 2 ;
	  }
	}

      } else {
	W__ << "EMPTY DUDATA for " << pkg_r << endl;
      }
    }
  }
  return _data;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PkgDuSlave::add
//	METHOD TYPE : bool
//
//	DESCRIPTION : Assert not to pass NULL or
//                    an array size != master_r._mountpoints.size()
//
bool PkgDuSlave::add( const PMPackage & pkg_r, PkgDuMaster & master_r ) const
{
  if ( sync( pkg_r, master_r ) ) {
    master_r.add( _data );
    return true;
  }
  return false;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PkgDuSlave::sub
//	METHOD TYPE : bool
//
//	DESCRIPTION : Assert not to pass NULL or
//                    an array size != master_r._mountpoints.size()
//
bool PkgDuSlave::sub( const PMPackage & pkg_r, PkgDuMaster & master_r ) const
{
  if ( sync( pkg_r, master_r ) ) {
    master_r.sub( _data );
    return true;
  }
  return false;
}
