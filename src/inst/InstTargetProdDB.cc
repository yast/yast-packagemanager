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

  File:       InstTargetProdDB.cc

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose:

/-*/

#include <iostream>
#include <fstream>

#include <y2util/Y2SLog.h>
#include <y2util/PathInfo.h>
#include <y2util/stringutil.h>

#include <y2pm/InstTargetProdDB.h>
#include <y2pm/InstSrcDescr.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//	CLASS NAME : InstTargetProdDBPtr
//	CLASS NAME : constInstTargetProdDBPtr
///////////////////////////////////////////////////////////////////
IMPL_BASE_POINTER(InstTargetProdDB);

///////////////////////////////////////////////////////////////////

const Pathname InstTargetProdDB::_db_path( "/var/adm/YaST/ProdDB" );
const string   InstTargetProdDB::_db_stem( "prod_" );
const unsigned InstTargetProdDB::_db_nwidth( 8 );

///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstTargetProdDB::InstTargetProdDB
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
InstTargetProdDB::InstTargetProdDB()
    : _nextIdx( 0 )
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstTargetProdDB::~InstTargetProdDB
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
InstTargetProdDB::~InstTargetProdDB()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstTargetProdDB::db_file_name
//	METHOD TYPE : std::string
//
//	DESCRIPTION :
//
std::string InstTargetProdDB::db_file_name( unsigned num_r ) const
{
  return stringutil::form( "%s%0*d", _db_stem.c_str(), _db_nwidth, num_r );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstTargetProdDB::is_db_file_name
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool InstTargetProdDB::is_db_file_name( const std::string & fname_r ) const
{
  return(    fname_r.find( _db_stem ) == 0
	  && fname_r.find_first_not_of( "0123456789", _db_stem.size() ) == string::npos );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstTargetProdDB::db_file
//	METHOD TYPE : Pathname
//
//	DESCRIPTION :
//
Pathname InstTargetProdDB::db_file( const std::string & fname_r ) const
{
  return _db + fname_r;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstTargetProdDB::readDbFile
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError InstTargetProdDB::read_db_file( const string & fname_r,
					unsigned & idx_r, InstSrcDescrPtr & ndescr_r ) const
{
  idx_r    = 0;
  ndescr_r = 0;

  if ( ! is_db_file_name( fname_r ) ) {
    ERR << "Illegal entry '" << fname_r << "' in " << *this << endl;
    return Error::E_ProdDB_file_bad_magic;
  }

  ifstream istr( db_file( fname_r ).asString().c_str() );
  PMError err = InstSrcDescr::readStream( ndescr_r, istr );
  if ( err || !ndescr_r ) {
    ERR << "Error reading entry " << fname_r << " (" << err << ")" << endl;
    ndescr_r = 0;
    return Error::E_ProdDB_file_bad_magic;
  }

  idx_r = atoi( fname_r.substr( _db_stem.size() ).c_str() );
  if ( !idx_r ) {
    ERR << "Illegal entry index 0" << endl;
    ndescr_r = 0;
    return Error::E_ProdDB_file_bad_magic;
  }

  return Error::E_ok;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstTargetProdDB::open
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError InstTargetProdDB::open( const Pathname & system_root_r, const bool create_r )
{
  if ( isOpen() ) {
    WAR << *this << " " << Error::E_ProdDB_already_open << endl;
    return Error::E_ProdDB_already_open;
  }

  PathInfo db( system_root_r + _db_path );

  if ( !db.isDir() ) {

    if ( db.isExist() || !create_r ) {
      ERR << "Not a directory: " << db << endl;
      return Error::E_ProdDB_open_failed;
    }

    // should create one.
    int res = PathInfo::assert_dir( db.path() );
    if ( res ) {
      ERR << "Error create: " << db << " (errno " << res << ")" << endl;
      return Error::E_ProdDB_open_failed;
    }

  }

  // looks good
  const_cast<Pathname&>(_db) = db.path();

  // read directory
  list<string> retlist;
  int res = PathInfo::readdir( retlist, _db, false );
  if ( res ) {
    ERR << "Error reading content of " << _db << " (readdir " << res << ")" << endl;
    return Error::E_ProdDB_open_failed;
  }

  // get product files
  PMError err;
  for( list<string>::const_iterator it = retlist.begin(); !err && it != retlist.end(); ++it ) {

    unsigned idx = 0;
    InstSrcDescrPtr ndescr;
    err = read_db_file( *it, idx, ndescr );
    if ( err || !ndescr ) {
      err = Error::E_ProdDB_open_failed;
      break;
    }

    // put it into map
    map<unsigned,constInstSrcDescrPtr>::value_type nv( idx, ndescr );
    if ( ! _prodmap.insert( nv ).second ) {
      ERR << "Duplicate entry " << idx << " in " << *this << endl;
      err = Error::E_ProdDB_open_failed;
      break;
    }

  }

  // finalize
  if ( !err ) {
    for ( map<unsigned,constInstSrcDescrPtr>::const_iterator it = _prodmap.begin();
	  it != _prodmap.end(); ++ it ) {
      DBG << *this << ": [" << it->first << "] " << it->second << endl;
      _prodlist.push_front( it->second ); // reversed by id
    }
    _nextIdx = ( _prodmap.empty() ? 1 : _prodmap.rbegin()->first + 1 );
    MIL << *this << " " << _prodlist.size() << " products installed" << endl;
  } else {
    _prodmap.clear();
    _prodlist.clear();
    ERR << *this << " open failed" << endl;
  }

  return err;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstTargetProdDB::assert_open
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError InstTargetProdDB::assert_open() const
{
  if ( isOpen() )
    return Error::E_ok;

  ERR << *this << " not open" << endl;
  return Error::E_ProdDB_not_open;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstTargetProdDB::isInstalled
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool InstTargetProdDB::isInstalled( const constInstSrcDescrPtr & isd_r ) const
{
  if ( !isd_r )
    return false;

  for ( list<constInstSrcDescrPtr>::const_iterator it = _prodlist.begin(); it != _prodlist.end(); ++it ) {
    if ( InstSrcDescr::sameContentProduct( isd_r, *it ) )
      return true;
  }
  return false;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstTargetProdDB::install
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError InstTargetProdDB::install( const constInstSrcDescrPtr & isd_r )
{
  PMError err = assert_open();
  if ( err ) {
    return err;
  }

  if ( !isd_r ) {
    ERR << "Can't install NULL Product" << endl;
    return Error::E_ProdDB_install_failed;
  }

  if ( isInstalled( isd_r ) ) {
    ERR << "Already installed: " << isd_r << endl;
    return Error::E_ProdDB_install_failed;
  }

  // write isd_r, reread it, if Ok add to lists
  string nfilename( db_file_name( _nextIdx ) );
  PathInfo nfile( db_file( nfilename ) );
  if ( nfile.isExist() ) {
    INT << "New dbfile name '" << nfilename << "' already exists!" << endl;
    return Error::E_ProdDB_install_failed;
  }

  ofstream ostr( nfile.path().asString().c_str() );
  err = isd_r->writeStream( ostr );
  if ( err ) {
    ERR << "Error writing " << isd_r << endl;
    PathInfo::unlink( nfile.path() );
    return Error::E_ProdDB_install_failed;
  }
  ostr.close();

  unsigned idx = 0;
  InstSrcDescrPtr ndescr;
  err = read_db_file( nfilename, idx, ndescr );
  if ( err || !ndescr ) {
    ERR << "Error rereading from " << nfilename << endl;
    PathInfo::unlink( nfile.path() );
    return Error::E_ProdDB_install_failed;
  }

  map<unsigned,constInstSrcDescrPtr>::value_type nv( idx, ndescr );
  if ( ! _prodmap.insert( nv ).second ) {
    INT << "Duplicate entry " << idx << " in " << *this << endl;
    PathInfo::unlink( nfile.path() );
    return Error::E_ProdDB_install_failed;
  }
  DBG << *this << ":i[" << _nextIdx << "] " << ndescr << endl;
  _prodlist.push_front( ndescr ); // reversed by id
  ++_nextIdx;

  MIL << *this << " installed " << isd_r << endl;
  return Error::E_ok;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstTargetProdDB::remove
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError InstTargetProdDB::remove( const constInstSrcDescrPtr & isd_r )
{
  PMError err = assert_open();
  if ( err ) {
    return err;
  }

  if ( !isd_r ) {
    ERR << "Can't remove NULL Product" << endl;
    return Error::E_ProdDB_not_installed;
  }

  if ( !isInstalled( isd_r ) ) {
    ERR << "Is not installed: " << isd_r << endl;
    return Error::E_ProdDB_not_installed;
  }

  for ( map<unsigned,constInstSrcDescrPtr>::iterator it = _prodmap.begin();
	it != _prodmap.end(); ++ it ) {
    if ( InstSrcDescr::sameContentProduct( isd_r, it->second ) ) {
      // remove it from _prodlist
      DBG << "GOT [" << it->first << "] " << it->second << endl;
      unsigned lsze = _prodlist.size();
      _prodlist.remove( it->second );
      if ( _prodlist.size()+1 != lsze ) {
	INT << "SUSPICOIUS _prodlist remove: size " << lsze << " -> " << _prodlist.size() << endl;
      }
      // remove file from db
      string nfilename( db_file_name( it->first ) );
      PathInfo nfile( db_file( nfilename ) );
      if ( !nfile.isFile() ) {
	INT << "SUSPICOIUS _prodlist remove: " << nfilename << " does not exist in " << *this << endl;
      } else {
	PathInfo::unlink( nfile.path() );
      }
      // finaly from _prodmap
      _prodmap.erase( it );
      break;
    }
  }

  MIL << *this << " removed " << isd_r << endl;
  return Error::E_ok;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstTargetProdDB::dumpOn
//	METHOD TYPE : ostream &
//
//	DESCRIPTION :
//
ostream & InstTargetProdDB::dumpOn( ostream & str ) const
{
  return str <<  "ProductDB(" << _db << ")";
}

