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
#include <y2pm/PMSelection.h>
#include <y2pm/ULSelectionParser.h>

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
    : _sellistDirty( true )
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
PMError InstTargetSelDB::open( const Pathname & system_root_r )
{
  Pathname dbpath( system_root_r + _db_path );

  if ( isOpen() ) {
    if ( _db == dbpath ) {
      return Error::E_ok;
    } else {
      ERR << *this << " " << Error::E_SelDB_already_open << endl;
      return Error::E_SelDB_already_open;
    }
  }

  PathInfo db( dbpath );

  if ( !db.isDir() ) {

    if ( db.isExist() ) {
      ERR << "Not a directory: " << db << endl;
      return Error::E_SelDB_open_failed;
    }

    // create one.
    int res = PathInfo::assert_dir( db.path() );
    if ( res ) {
      ERR << "Error create: " << db << " (errno " << res << ")" << endl;
      return Error::E_SelDB_open_failed;
    }

  }

  // looks good
  const_cast<Pathname&>(_db) = db.path();
  PMError err = rescan();

  MIL << *this << " " << _sellist.size() << " selections installed" << endl;
  return err;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstTargetSelDB::rescan
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError InstTargetSelDB::rescan() const
{
  PMError err = assert_open();
  if ( err ) {
    return err;
  }

  _sellist.clear();

  list<string> content;
  PathInfo::readdir( content, _db, /*dots*/false );
  for ( list<string>::iterator it = content.begin(); it != content.end(); ++it ) {
    string::size_type ext = it->find( ".sel" );
    if ( ext != string::npos && ext == it->size()-4 ) {

      ULSelectionParser parser( 0 );
      PMSelectionPtr    nsel;
      err = parser.fromPath( db_file( *it ), nsel );

      if ( err ) {
	WAR << "Error parsing selection " << *it << " (" << err << ")" << endl;
      } else if ( !nsel ) {
	WAR << "Selection without data " << *it << " (" << err << ")" << endl;
      } else {
	_sellist.push_back( nsel );
	DBG << "Parse found " << nsel << endl;
      }

    } else {
      WAR << "Skip non selection file: " << *it << endl;
    }
  }

  _sellistDirty = false;
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

#warning should test file magic and selection name
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

  if ( bname.size() > 4 &&  bname.rfind( ".sel" ) == bname.size() - 4 ) {
    bname.erase( bname.size() - 4 );
    string::size_type sep = bname.rfind( "." );
    if ( sep != string::npos ) {
      sep = bname.rfind( "-" );
      if ( sep != string::npos ) {
	bname.erase( sep );
      }
      sep = bname.rfind( "-" );
      if ( sep != string::npos ) {
	bname.erase( sep );
      }
    }
    bname += ".sel";
  }

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

  //int res = PathInfo::copy_file2dir( selfile_r, _db );
  int res = PathInfo::copy( selfile_r, db_file( selfile_r ) );
  if ( res ) {
    ERR << "Failed install " << selfile_r << " (copy_file2dir " << res << ')' << endl;
    return Error::E_SelDB_install_failed;
  }

  MIL << *this << " installed " << selfile_r << endl;
  _sellistDirty = true;
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
  _sellistDirty = true;
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
  return str <<  "SelectionDB(" << _db << ")";
}
