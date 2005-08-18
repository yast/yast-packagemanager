/*---------------------------------------------------------------------\
|                                                                      |
|                      __   __    ____ _____ ____                      |
|                      \ \ / /_ _/ ___|_   _|___ \                     |
|                       \ V / _` \___ \ | |   __) |                    |
|                        | | (_| |___) || |  / __/                     |
|                        |_|\__,_|____/ |_| |_____|                    |
|                                                                      |
|                               core system                            |
|                                         (C) SuSE Linux Products GmbH |
\----------------------------------------------------------------------/

  File:       YUMRepodata.cc

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose:

/-*/

#include <iostream>
#include <fstream>

#include <y2util/Y2SLog.h>

#include "YUMRepodata.h"
#include "y2pm/YUMParser.h"
#include "y2pm/MediaAccess.h"
#include "y2pm/InstSrcError.h"

using namespace std;

#undef Y2LOG
#define Y2LOG "Repodata"


///////////////////////////////////////////////////////////////////
namespace YUM
{ /////////////////////////////////////////////////////////////////

  ///////////////////////////////////////////////////////////////////
  //
  //	METHOD NAME : Repodata::Repodata
  //	METHOD TYPE : Constructor
  //
  Repodata::Repodata( const Pathname & repodataDir_r )
  : _repodataDir( repodataDir_r )
  {
    MIL << "Loading repodata from '" << repodataDir_r << "'" << endl;
    // If repomd.xml was found and successfully loaded, use
    // is sha1sum as ID.
    PathInfo repomd( _repodataDir + defaultRepomd_Xml() );
    if ( ! repomd.isFile() )
      {
        WAR << repomd << endl;
      }
    else
      {
        ifstream pIn( repomd.path().asString().c_str() );
        YUMRepomdParser iter( pIn, "" );
        for ( ; ! iter.atEnd(); ++iter )
          {
            DBG << **iter;
          }

        if ( iter.errorStatus() )
          ERR << iter.errorStatus()->msg() << " parsing " << repomd << endl;
        else
          _repodataId = PathInfo::sha1sum( repomd.path() );
      }
    MIL << repomd.path() << " has data ID '" << _repodataId << "'" << endl;
  }

  ///////////////////////////////////////////////////////////////////
  //
  //	METHOD NAME : Repodata::~Repodata
  //	METHOD TYPE : Destructor
  //
  Repodata::~Repodata()
  {
  }

  ///////////////////////////////////////////////////////////////////
  //
  //	METHOD NAME : Repodata::update
  //	METHOD TYPE : PMError
  //
  PMError
  Repodata::update( MediaAccessPtr media_r, const Pathname & repodataDir_r )
  {
    DBG << "START " << media_r << " " << repodataDir_r << endl;

    ///////////////////////////////////////////////////////////////////
    // Media check
    ///////////////////////////////////////////////////////////////////

    if ( ! ( media_r && media_r->isOpen() ) )
      {
        ERR << "Media not open: " << media_r << endl;
        return InstSrcError::E_no_media;
      }

    if ( ! media_r->isAttached() )
      {
        PMError ret = media_r->attach();
        if ( ret ) {
          ERR << "Failed to attach media: " << media_r << ": " << ret << endl;
          return InstSrcError::E_no_media;
        }
      }

    ///////////////////////////////////////////////////////////////////
    // Download
    ///////////////////////////////////////////////////////////////////

    Pathname rfile = repodataDir_r + defaultRepomd_Xml();
    MediaAccess::FileProvider lrepmod( media_r, rfile );
    if ( lrepmod.error() )
      {
        WAR << "Media can't provide '" << rfile << "' " << lrepmod.error() << endl;
        return PMError::E_error;
      }

    rfile = repodataDir_r + "primary.xml.gz";
    MediaAccess::FileProvider lprimary( media_r, rfile );
    if ( lprimary.error() )
      {
        WAR << "Media can't provide '" << rfile << "' " << lprimary.error() << endl;
        return PMError::E_error;
      }

    ///////////////////////////////////////////////////////////////////
    // Fill cache
    ///////////////////////////////////////////////////////////////////

    // XXX: FIXME: make it nondestructive
    PMError err;
    PathInfo::clean_dir( _repodataDir );

    int res = PathInfo::copy_file2dir( lrepmod(), _repodataDir )
            + PathInfo::copy_file2dir( lprimary(), _repodataDir );
    if ( res )
      {
        WAR << "Unable to load cache!" << endl;
        PathInfo::clean_dir( _repodataDir );
        err = PMError::E_error;
      }

    ///////////////////////////////////////////////////////////////////
    // Reload
    ///////////////////////////////////////////////////////////////////

    *this = Repodata( _repodataDir );
    // XXX: get errcode

    return err;
  }

  ///////////////////////////////////////////////////////////////////
  //
  //	METHOD NAME : Repodata::primaryFile
  //	METHOD TYPE : Pathname
  //
  Pathname
  Repodata::primaryFile() const
  {
    Pathname ret;
    if ( hasData() )
      {
        PathInfo retFile( _repodataDir + "primary.xml.gz" );
        if ( retFile.isFile() )
          {
            ret = retFile.path();
          }
      }
    return ret;
  }

  /******************************************************************
   **
   **
   **	FUNCTION NAME : operator<<
   **	FUNCTION TYPE : ostream &
  */
  ostream & operator<<( ostream & str, const Repodata & obj )
  {
    return str;
  }

  /////////////////////////////////////////////////////////////////
} // namespace YUM
///////////////////////////////////////////////////////////////////
