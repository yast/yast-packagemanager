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

   File:       InstSrcManager.cc

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/

#include <iostream>

#include <y2util/Y2SLog.h>
#include <y2pm/InstSrcManager.h>
#include <y2pm/InstSrc.h>
#include <y2pm/PMPackageManager.h>

using namespace std;

///////////////////////////////////////////////////////////////////

#warning FIX TMPDIR( "/tmp/test_test" )
std::string InstSrcManager::_cache_root_dir( "/tmp/test_test" );


///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcManager::InstSrcManager
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
InstSrcManager::InstSrcManager()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcManager::~InstSrcManager
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
InstSrcManager::~InstSrcManager()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcManager::scanMedia
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError InstSrcManager::scanMedia( ISrcIdList & idlist_r, const Url & mediaurl_r )
{
  idlist_r.clear();
  PMError err;

  Pathname tmpDir( "/tmp/test_test" );

  MIL << "scanMedia " << mediaurl_r << endl;

  ///////////////////////////////////////////////////////////////////
  // prepare media
  ///////////////////////////////////////////////////////////////////
  MediaAccessPtr  media = new MediaAccess;
  if ( (err = _media->open( mediaurl_r )) ) {
    ERR << "Failed to open " << mediaurl_r << " " << err << endl;
    return err;
  }

  if ( (err = _media->attach ( )) ) {
    if ( err != MediaAccess::Error::E_ok) {
      ERR << "Failed to attach media: " << err << endl;
      return err;
    }
  }

  ///////////////////////////////////////////////////////////////////
  // look if there's a /media.1/products file
  ///////////////////////////////////////////////////////////////////
  list<Pathname,string> plist;

  if ( ! media->provideFile( "/media.1/products" ) ) {

  } else {

  }




  DBG << "scanMedia " << isrc_r << endl;

  isrc_r = 0;

  if ( !media_r )
    return E_NO_MEDIA;

  return E_Error;
}

PMError InstSrcManager::enableSource( InstSrcPtr & isrc_r )
{
  D__ << endl;
  if(isrc_r->Activate())
  {
    PMPackageManager::PM().addPackages( isrc_r->getPackages() );
    return E_OK;
  }
  return E_Error;
}

/******************************************************************
**
**
**	FUNCTION NAME : operator<<
**	FUNCTION TYPE : ostream &
**
**	DESCRIPTION :
*/
ostream & operator<<( ostream & str, const InstSrcManager & obj )
{
  return str;
}


