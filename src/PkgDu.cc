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

  Purpose:

/-*/

#include <iostream>

#include <y2util/Y2SLog.h>
#include <y2util/stringutil.h>

#include <y2pm/PkgDu.h>

using namespace std;

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
    << " T:" << obj._total   .form( FSize::K, 10 )
    << " U:" << obj._used    .form( FSize::K, 10 )
    << " P:" << obj._pkgusage.form( FSize::K, 10 )
    << " " << obj._mountpoint;
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
//	METHOD NAME : PkgDu::addMaster
//	METHOD TYPE : void
//
//	DESCRIPTION : We know (and rely) PkgDu does not pass NULL or
//                    an array size != _mountpoints.size()
//
void PkgDuMaster::add( FSize * data_r )
{
  for ( set<MountPoint>::iterator it = _mountpoints.begin(); it != _mountpoints.end(); ++it, ++data_r ) {
    SEC << it->_mountpoint << ":\t" << it->_pkgusage << " + " << *data_r << " = ";
    it->_pkgusage += *data_r;
    SEC << it->_pkgusage << endl;
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PkgDuMaster::sub
//	METHOD TYPE : void
//
//	DESCRIPTION : We know (and rely) PkgDu does not pass NULL or
//                    an array size != _mountpoints.size()
//
void PkgDuMaster::sub( FSize * data_r )
{
  for ( set<MountPoint>::iterator it = _mountpoints.begin(); it != _mountpoints.end(); ++it, ++data_r ) {
    SEC << it->_mountpoint << ":\t" << it->_pkgusage << " - " << *data_r << " = ";
    it->_pkgusage -= *data_r;
    SEC << it->_pkgusage << endl;
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
  return _mountpoints.size();
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
  SEC << "OLD:" << *this;
  SEC << "SET:" << endl << mountpoints_r;

  if ( _mountpoints.size() == mountpoints_r.size() ) {
    // Common case is update of nonvital data (total, used, etc.)
    // So we start, and if different vital data (mountpoint/blocksize)
    // occur we abort and assign the whole set.
    for ( set<MountPoint>::iterator lit = _mountpoints.begin(), rit = mountpoints_r.begin();
	  lit != _mountpoints.end(); ++lit, ++rit ) {
      if ( ! lit->assignData( *rit ) ) {
	// different vital data
	SEC << "new set (hard assign)" << endl;
	_mountpoints = mountpoints_r;
	newcount();
	break; //return;
      }
    }
  } else {
    SEC << "new set" << endl;
    _mountpoints = mountpoints_r;
    newcount();
  }
  SEC << "NEW:" << *this;
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
//	CLASS NAME : PkgDu
//
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PkgDu::PkgDu
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
PkgDu::PkgDu()
    : _count( 0 )
    , _data( 0 )
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PkgDu::~PkgDu
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
PkgDu::~PkgDu()
{
  delete [] _data;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PkgDu::sync
//	METHOD TYPE : bool
//
//	DESCRIPTION : Assert not to return true, on NULL _data or
//                    an array size != master_r._mountpoints.size()
//
bool PkgDu::sync( const PMPackage & pkg_r, PkgDuMaster & master_r ) const
{
  if ( _count != master_r.sync_count() ) {
    delete [] _data;
    _data = 0;
  }
  return _data;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PkgDu::add
//	METHOD TYPE : bool
//
//	DESCRIPTION : Assert not to pass NULL or
//                    an array size != master_r._mountpoints.size()
//
bool PkgDu::add( const PMPackage & pkg_r, PkgDuMaster & master_r ) const
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
//	METHOD NAME : PkgDu::sub
//	METHOD TYPE : bool
//
//	DESCRIPTION : Assert not to pass NULL or
//                    an array size != master_r._mountpoints.size()
//
bool PkgDu::sub( const PMPackage & pkg_r, PkgDuMaster & master_r ) const
{
  if ( sync( pkg_r, master_r ) ) {
    master_r.sub( _data );
    return true;
  }
  return false;
}
