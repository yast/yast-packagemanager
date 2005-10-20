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

  File:       PkgSplit.cc

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose:

/-*/

#include <iostream>

#include <y2util/Y2SLog.h>

#include <y2pm/PkgSplit.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PkgSplit::PkgSplit
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
PkgSplit::PkgSplit( const string & splitprovides_r, const bool quiet_r )
{
  if ( splitprovides_r.find_first_of( " \t\n" ) != string::npos ) {
    if ( !quiet_r )
      ERR << "Bad splitprovides (contains whitespace)'" << splitprovides_r << "'" << endl;
    return;
  }
  string::size_type sep = splitprovides_r.find( ":/" );
  if ( sep == string::npos || sep == 0 || sep == splitprovides_r.size()-2 ) {
    if ( !quiet_r )
      ERR << "Bad splitprovides (empty avlue)'" << splitprovides_r << "'" << endl;
    return;
  }
  _ipkg = PkgName( splitprovides_r.substr( 0, sep ) );
  _file = splitprovides_r.substr( sep+1 );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PkgSplit::PkgSplit
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
PkgSplit::PkgSplit( const PkgName & ipkg_r, const Pathname & file_r )
    : _ipkg( ipkg_r )
    , _file( file_r.absolutename() )
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PkgSplit::~PkgSplit
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
PkgSplit::~PkgSplit()
{
}

/******************************************************************
**
**
**	FUNCTION NAME : operator<<
**	FUNCTION TYPE : ostream &
**
**	DESCRIPTION :
*/
ostream & operator<<( ostream & str, const PkgSplit & obj )
{
  return str << obj._ipkg << ':' << obj._file;
}

