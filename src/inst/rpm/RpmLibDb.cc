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

  File:       RpmLibDb.cc

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Wraps acces to rpmdb via librpm.

/-*/
extern "C" {
#include <sys/types.h>
#include <sys/stat.h>
}

#include "RpmLib.h"

#include <iostream>
#include <fstream>
#include <map>

#include <y2util/Y2SLog.h>
#include <y2util/stringutil.h>
#include <y2util/PathInfo.h>

#include <y2pm/RpmLibDb.h>
#include <y2pm/RpmLibHeader.h>
#include <y2pm/PMPackage.h>

using namespace std;

#undef Y2LOG
#define Y2LOG "RpmLib"

///////////////////////////////////////////////////////////////////

bool RpmLibDb::_globalInitialized = false;

///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibDb::RpmLibDb
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
RpmLibDb::RpmLibDb( const Pathname & dbPath_r, const bool no_open_r )
    : _dbPath( dbPath_r )
    , _dbOpenError( Error::E_RpmLib_db_not_open )
{
  if ( !_globalInitialized ) {
    globalInit();
  }
  if ( ! no_open_r ) {
    dbOpen();
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibDb::~RpmLibDb
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
RpmLibDb::~RpmLibDb()
{
  dbClose();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibDb::globalInit
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError RpmLibDb::globalInit()
{
  if ( _globalInitialized ) {
    return Error::E_ok;
  }

  int rc = ::rpmReadConfigFiles( NULL, NULL );
  if ( rc ) {
    ERR << "rpmReadConfigFiles returned: " << rc << endl;
    return Error::E_RpmLib_read_config_failed;
  }

  _globalInitialized = true;

  MIL << "globalInit " << Error::E_ok << endl;
  return Error::E_ok;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibDb::dbOpen
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError RpmLibDb::dbOpen()
{
  if ( _db ) {
    return _dbOpenError; // is Error::E_ok
  }

  _dbOpenError = globalInit();
  if ( _dbOpenError ) {
    ERR << *this << " globalInit returned: " << _dbOpenError << endl;
    return _dbOpenError;
  }

#warning Check whether to split root from _dbPath
  ::addMacro(NULL, "_dbpath", NULL, _dbPath.asString().c_str(), RMIL_CMDLINE);

  int rc = 0;
  _db = librpmDb::access( rc );
  if ( rc ) {
    ERR << *this << " rpmdbOpen returned: " << rc << endl;
    _dbOpenError = Error::E_RpmLib_dbopen_failed; // set it before calling dbClose
    dbClose();
    return _dbOpenError;
  }

  return _dbOpenError; // is Error::E_ok
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibDb::dbClose
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError RpmLibDb::dbClose()
{
  if ( _db ) {
    _db = 0;
    if ( !_dbOpenError )
      _dbOpenError = Error::E_RpmLib_db_not_open;
  }
  return Error::E_ok;
}

/******************************************************************
**
**
**	FUNCTION NAME : operator<<
**	FUNCTION TYPE : ostream &
**
**	DESCRIPTION :
*/
ostream & operator<<( ostream & str, const RpmLibDb & obj )
{
  str << "RpmDb(" << obj._dbPath << '|';
  if ( obj._db )
    str << "open";
  else
    str << (obj._globalInitialized ? "closed" : "noinit" );

  return str << ')';
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibDb::hasFile
//	METHOD TYPE : bool
//
bool RpmLibDb::hasFile( const std::string & file_r ) const
{
  librpmDb::db_const_iterator it( _db );
  return it.findByFile( file_r );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibDb::hasProvides
//	METHOD TYPE : bool
//
bool RpmLibDb::hasProvides( const std::string & tag_r ) const
{
  librpmDb::db_const_iterator it( _db );
  return it.findByProvides( tag_r );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibDb::hasRequiredBy
//	METHOD TYPE : bool
//
bool RpmLibDb::hasRequiredBy( const std::string & tag_r ) const
{
  librpmDb::db_const_iterator it( _db );
  return it.findByRequiredBy( tag_r );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibDb::hasConflicts
//	METHOD TYPE : bool
//
bool RpmLibDb::hasConflicts( const std::string & tag_r ) const
{
  librpmDb::db_const_iterator it( _db );
  return it.findByConflicts( tag_r );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibDb::hasPackage
//	METHOD TYPE : bool
//
bool RpmLibDb::hasPackage( const PkgName & name_r ) const
{
  librpmDb::db_const_iterator it( _db );
  return it.findPackage( name_r );
}

#warning CLEANUP findPackage

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibDb::findPackage
//	METHOD TYPE : constRpmLibHeaderPtr
//
constRpmLibHeaderPtr RpmLibDb::findPackage( const PkgName & name_r ) const
{
  librpmDb::db_const_iterator it( _db );
  it.findPackage( name_r );
  return *it;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibDb::findPackage
//	METHOD TYPE : constRpmLibHeaderPtr
//
constRpmLibHeaderPtr RpmLibDb::findPackage( const PkgName & name_r, const PkgEdition & ed_r ) const
{
  librpmDb::db_const_iterator it( _db );
  it.findPackage( name_r, ed_r );
  return *it;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibDb::findPackage
//	METHOD TYPE : constRpmLibHeaderPtr
//
constRpmLibHeaderPtr RpmLibDb::findPackage( const PkgNameEd & which_r ) const
{
  librpmDb::db_const_iterator it( _db );
  it.findPackage( which_r );
  return *it;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibDb::findPackage
//	METHOD TYPE : constRpmLibHeaderPtr
//
constRpmLibHeaderPtr RpmLibDb::findPackage( const constPMPackagePtr & which_r ) const
{
  librpmDb::db_const_iterator it( _db );
  it.findPackage( which_r );
  return *it;
}


#warning CHECK IT

static int rpmReadPackageHeader(FD_t fd, Header * hdr, int * isSource, int * major, int * minor ) {
  INT << "Illegal use of old api: " << __FUNCTION__ << endl;
  * hdr = 0;
  * isSource = 0;
  * major = 0;
  * minor = 0;
  return -1;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibDb::getData
//	METHOD TYPE : constRpmLibHeaderPtr
//
constRpmLibHeaderPtr RpmLibDb::getData( const Pathname & path )
{
  PathInfo file( path );
  if ( ! file.isFile() ) {
    ERR << "Not a file: " << file << endl;
    return 0;
  }

  FD_t fd = ::Fopen( file.asString().c_str(), "r" );
  if ( fd == 0 || ::Ferror(fd) ) {
    ERR << "Can't open file for reading: " << file << " (" << ::Fstrerror(fd) << ")" << endl;
    if ( fd )
      ::Fclose( fd );
    return 0;
  }

  Header nh    = 0;
  int isSource = 0;
  int major    = 0;
  int minor    = 0;

  int res = ::rpmReadPackageHeader( fd, &nh, &isSource, &major, &minor );
  ::Fclose( fd );
  if ( res || !nh ) {
    ERR << "Error reading: " << file << (res==1?" (bad magic)":"") << endl;
    return 0;
  }

  constRpmLibHeaderPtr h( new RpmLibHeader( nh, isSource ) );
  MIL << major << "." << minor << "-" << (isSource?"src ":"bin ") << h << " from " << path << endl;

  return h;
}

