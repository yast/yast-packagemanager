/*---------------------------------------------------------------------\
|                                                                      |
|                      __   __    ____ _____ ____                      |
|                      \ \ / /_ _/ ___|_   _|___ \                     |
|                       \ V / _` \___ \ | |   __) |                    |
|                        | | (_| |___) || |  / __/                     |
|                        |_|\__,_|____/ |_| |_____|                    |
|                                                                      |
|                               core system                            |
|                                                    (C) SuSE Linux AG |
\----------------------------------------------------------------------/

  File:       PkgPubkeyCache.cc

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Manage gpg-pubkeys (package singning keys)

/-*/

#include <iostream>

#include <y2util/Y2SLog.h>
#include <y2util/PathInfo.h>

#include <Y2PM.h>
#include <y2pm/InstTarget.h>
#include <y2pm/PkgPubkeyCache.h>

using namespace std;

#undef Y2LOG
#define Y2LOG "PkgPubkeyCache"

///////////////////////////////////////////////////////////////////

const string PkgPubkeyCache::_prefix( "gpg-pubkey-" );
const string PkgPubkeyCache::_ext( ".asc" );

///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PkgPubkeyCache::PkgPubkeyCache
//	METHOD TYPE : Constructor
//
PkgPubkeyCache::PkgPubkeyCache( const Pathname & cachedir_r )
{
  setCachedir( cachedir_r );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PkgPubkeyCache::~PkgPubkeyCache
//	METHOD TYPE : Destructor
//
PkgPubkeyCache::~PkgPubkeyCache()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PkgPubkeyCache::isKey
//	METHOD TYPE : bool
//
bool PkgPubkeyCache::isKey( const string & keyname_r )
{
  if ( keyname_r.find( _prefix ) != 0 )
    return false;

  if ( keyname_r.size() <= _ext.size()
       || keyname_r.rfind( _ext ) != (keyname_r.size() - _ext.size()) )
    return false;

  return true;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PkgPubkeyCache::keyEdition
//	METHOD TYPE : std::string
//
std::string PkgPubkeyCache::keyEdition( const std::string & keyname_r )
{
  if ( ! isKey( keyname_r ) )
    return "";

  string ret( keyname_r.substr( _prefix.size() ) );
  ret.erase( ret.size() - _ext.size() );
  return ret;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PkgPubkeyCache::hasKey
//	METHOD TYPE : bool
//
bool PkgPubkeyCache::hasKey( const std::string & keyname_r ) const
{
  if ( cacheDisabled() ) {
    WAR << "Cache is disabled!" << endl;
    return false;
  }

  string key( keyname_r );

  if ( key.find( _prefix ) != 0 ) {
    key = _prefix + key + _ext;
  }

  PathInfo keyfile( _cachedir + key );
  DBG << keyfile << endl;

  return keyfile.isFile();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PkgPubkeyCache::setCachedir
//	METHOD TYPE : PMError
//
PMError PkgPubkeyCache::setCachedir( const Pathname & cachedir_r )
{
  if ( cachedir_r.empty() ) {
    _cachedir = Pathname();
    MIL << "Cache disabled!" << endl;
    return Error::E_ok;
  }

  int res = PathInfo::assert_dir( cachedir_r );
  if ( res ) {
    _cachedir = Pathname();
    PMError ret( Error::E_cache_dir_create,
		 cachedir_r.asString() + " (" + strerror( res ) + ")" );
    ERR << "Unable to create cache: " << ret << endl;
    return ret;
  }

  _cachedir = cachedir_r;
  MIL << "Using cachedir " << _cachedir << endl;
  return Error::E_ok;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PkgPubkeyCache::storeKey
//	METHOD TYPE : PMError
//
PMError PkgPubkeyCache::storeKey( const Pathname & keyfile_r )
{
  if ( cacheDisabled() ) {
    WAR << "Cache is disabled!" << endl;
    return Error::E_bad_cache_dir;
  }

  if ( ! isKey( keyfile_r ) ) {
    WAR << "Not a vaild keyfile: " << keyfile_r << endl;
    return PMError( Error::E_error,
		    keyfile_r.asString() );
  }

  string keyname( keyfile_r.basename() );

  // Work arround wrong keyname created by autobuild
  if ( keyname == "gpg-pubkey-9c800aca-39eef481.asc" ) {
    PathInfo falsekey( keyfile_r );
    if ( falsekey.isFile() && falsekey.size() == 2173 ) {
      keyname = "gpg-pubkey-9c800aca-40d8063e.asc";
    }
  }

  if ( hasKey( keyname ) ) {
    DBG << "Have key " << keyfile_r << endl;
    return Error::E_ok;
  }

  int res = PathInfo::copy( keyfile_r, _cachedir+keyname );
  if ( res ) {
    PMError ret( Error::E_create_file,
		 keyname + " (" + strerror( res ) + ")" );
    ERR << "Unable to store key: " << ret << endl;
    return ret;
  }

  MIL << "Stored key " << keyname << endl;
  return Error::E_ok;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PkgPubkeyCache::sync
//	METHOD TYPE : PMError
//
PMError PkgPubkeyCache::sync()
{
  if ( cacheDisabled() ) {
    WAR << "Cache is disabled!" << endl;
    return Error::E_bad_cache_dir;
  }

  list<string> keys;
  int res = PathInfo::readdir( keys, _cachedir, /*dots*/false );
  if ( res ) {
    PMError ret( Error::E_bad_cache_dir,
		 _cachedir.asString() + " (" + strerror( res ) + ")" );
    ERR << "Error reading cache: " << ret << endl;
    return ret;
  }

  set<PkgEdition> installed( Y2PM::instTarget().pubkeys() );

  for ( list<string>::const_iterator it = keys.begin(); it != keys.end(); ++it ) {
    if ( ! isKey( *it ) )
      continue;

    if ( installed.find( PkgEdition( keyEdition( *it ) ) ) != installed.end() ) {
      DBG << "Known key: " << *it << endl;
    } else {
      Y2PM::instTarget().importPubkey( _cachedir + *it );
    }
  }

  return Error::E_ok;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PkgPubkeyCache::cacheCopyTo
//	METHOD TYPE : PMError
//
PMError PkgPubkeyCache::cacheCopyTo( const Pathname & newcachedir_r )
{
  if ( _cachedir == newcachedir_r ) {
    return Error::E_ok;
  }

  if ( cacheDisabled() ) {
    WAR << "Cache is disabled!" << endl;
    return Error::E_bad_cache_dir;
  }

  MIL << "Copy cache to " << newcachedir_r << endl;

  PkgPubkeyCache newcache( newcachedir_r );

  if ( newcache.cacheDisabled() ) {
    WAR << "No new cache dir set!" << endl;
    return Error::E_bad_cache_dir;
  }

  list<string> keys;
  int res = PathInfo::readdir( keys, _cachedir, /*dots*/false );
  if ( res ) {
    PMError ret( Error::E_bad_cache_dir,
		 _cachedir.asString() + " (" + strerror( res ) + ")" );
    ERR << "Error reading cache: " << ret << endl;
    return ret;
  }

  for ( list<string>::const_iterator it = keys.begin(); it != keys.end(); ++it ) {
    if ( ! isKey( *it ) )
      continue;

    newcache.storeKey( _cachedir + *it );
  }

  return Error::E_ok;
}

/******************************************************************
**
**
**	FUNCTION NAME : operator<<
**	FUNCTION TYPE : ostream &
*/
ostream & operator<<( ostream & str, const PkgPubkeyCache & obj )
{
  return str << "PkgPubkeyCache at " << obj.cachedir();
}

