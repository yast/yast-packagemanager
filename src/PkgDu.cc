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

std::ostream & operator<<( std::ostream & str, const std::set<PkgDuMaster::MountPoint> & obj ) {
  for ( set<PkgDuMaster::MountPoint>::const_iterator it = obj.begin(); it != obj.end(); ++it ) {
    str << stringutil::form( "%-35s (%4lld): %10lld",
			      it->_mountpoint.c_str(),
			      (long long)it->_blocksize,
			      (long long)it->_pkgusage ) << endl;
  }
  return str;
}
///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PkgDuMaster::setMountPoints
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void PkgDuMaster::setMountPoints( const std::set<MountPoint> & mountpoints_r )
{
  SEC << "NEW:" << endl << mountpoints_r;
  SEC << "OLD:" << endl << _mountpoints;
  //SEC << "CMP:" << (mountpoints_r == _mountpoints) << endl;
}

/******************************************************************
**
**
**	FUNCTION NAME : operator<<
**	FUNCTION TYPE : std::ostream &
**
**	DESCRIPTION :
*/
std::ostream & operator<<( std::ostream & str, const PkgDuMaster & obj )
{
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
