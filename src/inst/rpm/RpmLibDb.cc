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

#include <cstdio>
extern "C" {
#include <fcntl.h>
#include <rpm/rpmmacro.h>
}

#include <cstdlib>

#include <iostream>
#include <map>

#include <y2util/Y2SLog.h>

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
//	METHOD NAME : RpmLibDb::const_header_set::operator[]
//	METHOD TYPE : constRpmLibHeaderPtr
//
//	DESCRIPTION :
//
constRpmLibHeaderPtr RpmLibDb::const_header_set::operator[]( const unsigned idx_r ) const
{
  if ( _dbptr && idx_r < size() ) {
    Header nh = ::rpmdbGetRecord( _dbptr, _idxSet[idx_r] );
    if ( nh )
      return new RpmLibHeader( nh );
    else
      ERR << "Bad record number " << _idxSet[idx_r] << " in rpmdb" << endl;
  }
  return 0;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibDb::const_iterator::setrec
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void RpmLibDb::const_iterator::setrec( int recnum_r )
{
  if ( recnum_r > 0 ) {
    _recnum  = recnum_r;
    Header nh = ::rpmdbGetRecord( _dbptr, _recnum );
    if ( nh )
      _hptr = new RpmLibHeader( nh );
    else {
      _hptr = 0;
      ERR << "Bad record number " << _recnum << " in rpmdb" << endl;
    }
  } else {
    _recnum = 0;
    _dbptr = 0;
    _hptr = 0;
  }
}

///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibDb::RpmLibDb
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
RpmLibDb::RpmLibDb( const Pathname & dbPath_r )
    : _dbPath( dbPath_r )
    , _db( 0 )
{
  if ( !_globalInitialized ) {
    globalInit();
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

  int rc = ::rpmReadConfigFiles( 0, 0 );
  if ( rc ) {
    ERR << "rpmReadConfigFiles returned: " << rc << endl;
    return Error::E_RpmLib_read_config;
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
    return Error::E_ok;
  }

  PMError err = globalInit();
  if ( err ) {
    ERR << *this << " globalInit returned: " << err << endl;
    return err;
  }

  ::addMacro(NULL, "_dbpath", NULL, _dbPath.asString().c_str(), RMIL_CMDLINE);

  //int rc = rpmdbOpenForTraversal( 0, &_db );
  int rc = ::rpmdbOpen( 0, &_db, O_RDONLY, 0644 );
  if ( rc ) {
    ERR << *this << " rpmdbOpen returned: " << rc << endl;
    dbClose();
    return Error::E_RpmLib_dbopen;
  }

  //MIL << *this << " dbOpen " << err << endl;
  return err;
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
    ::rpmdbClose( _db );
    _db = 0;
    //MIL << *this << " dbClose " << Error::E_ok << endl;
  }
  return Error::E_ok;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibDb::findByFile
//	METHOD TYPE : RpmLibDb::const_header_set
//
//	DESCRIPTION :
//
RpmLibDb::const_header_set RpmLibDb::findByFile( const std::string & which_r ) const
{
  if ( ! which_r[0] == '/' )
    return const_header_set();

  dbiIndexSet idxSet;
  switch ( ::rpmdbFindByFile( _db, which_r.c_str(), &idxSet ) ) {
  case 0:
    return const_header_set( _db, idxSet );

  case -1:
    WAR << "Error reading a database record" << endl;
  }
  return const_header_set();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibDb::findPackage
//	METHOD TYPE : constRpmLibHeaderPtr
//
//	DESCRIPTION :
//
constRpmLibHeaderPtr RpmLibDb::findPackage( const PkgName & name_r ) const
{
  dbiIndexSet idxSet;
  switch ( ::rpmdbFindPackage( _db, name_r->c_str(), &idxSet ) ) {
  case 0:
    {
      constRpmLibHeaderPtr h;
      const_header_set result( _db, idxSet );
      for ( unsigned i = 0; i < result.size(); ++i ) {
	if ( !h || h->tag_installtime() < result[i]->tag_installtime() ) {
	  h = result[i];
	}
      }
      return h;
    }

  case -1:
    WAR << "Error reading a database record" << endl;
  }
  return 0;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibDb::findPackage
//	METHOD TYPE : constRpmLibHeaderPtr
//
//	DESCRIPTION :
//
constRpmLibHeaderPtr RpmLibDb::findPackage( const PkgName & name_r, const PkgEdition & ed_r ) const
{
  dbiIndexSet idxSet;
  switch ( ::rpmdbFindPackage( _db, name_r->c_str(), &idxSet ) ) {
  case 0:
    {
      const_header_set result( _db, idxSet );
      for ( unsigned i = 0; i < result.size(); ++i ) {
	constRpmLibHeaderPtr h = result[i];
	if ( ed_r == h->tag_edition() )
	  return h;
      }
    }

  case -1:
    WAR << "Error reading a database record" << endl;
  }
  return 0;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibDb::findPackage
//	METHOD TYPE : constRpmLibHeaderPtr
//
//	DESCRIPTION :
//
constRpmLibHeaderPtr RpmLibDb::findPackage( const PkgNameEd & which_r ) const
{
  return findPackage( which_r.name, which_r.edition );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibDb::findPackage
//	METHOD TYPE : constRpmLibHeaderPtr
//
//	DESCRIPTION :
//
constRpmLibHeaderPtr RpmLibDb::findPackage( const constPMPackagePtr & which_r ) const
{
  if ( !which_r )
    return 0;
  return findPackage( which_r->name(), which_r->edition() );
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
