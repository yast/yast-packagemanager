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

  File:       InstTargetSelDB.cc

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose:

/-*/

#include <iostream>

#include <y2util/Y2SLog.h>
#include <y2util/PathInfo.h>

#include <y2pm/InstTargetSelDB.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//	CLASS NAME : InstTargetSelDBPtr
//	CLASS NAME : constInstTargetSelDBPtr
///////////////////////////////////////////////////////////////////
IMPL_BASE_POINTER(InstTargetSelDB);

///////////////////////////////////////////////////////////////////

const Pathname InstTargetSelDB::_db_path( "/var/adm/YaST/SelDB" );

///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstTargetSelDB::InstTargetSelDB
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
InstTargetSelDB::InstTargetSelDB()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstTargetSelDB::~InstTargetSelDB
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
InstTargetSelDB::~InstTargetSelDB()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstTargetSelDB::open
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError InstTargetSelDB::open( const Pathname & system_root_r, const bool create_r )
{
  if ( isOpen() ) {
    ERR << *this << " " << Error::E_SelDB_already_open << endl;
    return Error::E_SelDB_already_open;
  }

  PathInfo db( system_root_r + _db_path );

  if ( !db.isDir() ) {

    if ( db.isExist() || !create_r ) {
      ERR << "Not a directory: " << db << endl;
      return Error::E_SelDB_open_failed;
    }

    // should create one.
    int res = PathInfo::assert_dir( db.path(), 0700 );
    if ( res ) {
      ERR << "Error create: " << db << " (errno " << res << ")" << endl;
      return Error::E_SelDB_open_failed;
    }

  }

  // looks good
  const_cast<Pathname&>(_db) = db.path();

  MIL << *this << endl;
  return Error::E_ok;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstTargetSelDB::assert_open
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError InstTargetSelDB::assert_open() const
{
  if ( isOpen() )
    return Error::E_ok;

  ERR << *this << " not open" << endl;
  return Error::E_SelDB_not_open;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstTargetSelDB::check_file
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError InstTargetSelDB::check_file( const Pathname & selfile_r ) const
{
  PathInfo selfile( selfile_r );

  if ( !selfile.isFile() ) {
    return Error::E_SelDB_file_not_found;
  }

#warning should test file magic
  string base = selfile_r.basename();
  if ( base.size() <= 4 || base.rfind( ".sel" ) != base.size() - 4 )
    return Error::E_SelDB_file_bad_magic;

  return Error::E_ok;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstTargetSelDB::db_file
//	METHOD TYPE : Pathname
//
//	DESCRIPTION :
//
Pathname InstTargetSelDB::db_file( const Pathname & selfile_r ) const
{
  if ( !isOpen() || selfile_r.empty() )
    return Pathname();

  string bname = selfile_r.basename();
  if ( bname.empty() )
    return Pathname();

  return _db + bname;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstTargetSelDB::isInstalled
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool InstTargetSelDB::isInstalled( const Pathname & selfile_r ) const
{
  PathInfo dbfile( db_file( selfile_r ) );
  return dbfile.isFile();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstTargetSelDB::install
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError InstTargetSelDB::install( const Pathname & selfile_r )
{
  PMError err = assert_open();
  if ( err ) {
    return err;
  }

  err = check_file( selfile_r );
  if ( err ) {
    ERR << "Won't install " << selfile_r << " " << err << endl;
    return err;
  }

  int res = PathInfo::copy_file2dir( selfile_r, _db );
  if ( res ) {
    ERR << "Failed install " << selfile_r << " (copy_file2dir " << res << ')' << endl;
    return Error::E_SelDB_install_failed;
  }

  MIL << *this << " installed " << selfile_r << endl;
  return Error::E_ok;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstTargetSelDB::remove
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError InstTargetSelDB::remove( const Pathname & selfile_r )
{
  PMError err = assert_open();
  if ( err ) {
    return err;
  }

  PathInfo dbfile( db_file( selfile_r ) );
  if ( !dbfile.isFile() ) {
    ERR << dbfile.path().basename() << " is not installed" << endl;
    return Error::E_SelDB_not_installed;
  }

  PathInfo::unlink( dbfile.path() );
  MIL << *this << " removed " << dbfile.path().basename() << endl;
  return Error::E_ok;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstTargetSelDB::dumpOn
//	METHOD TYPE : ostream &
//
//	DESCRIPTION :
//
ostream & InstTargetSelDB::dumpOn( ostream & str ) const
{
  return str <<  "SelectionDB(" << _db << ")" << endl;
}
