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

  File:       PkgChangelog.cc

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Helper class providing changelog information.

/-*/

#include <iostream>

#include <y2util/stringutil.h>

#include <y2pm/PkgChangelog.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PkgChangelog::asStringList
//	METHOD TYPE : list<string>
//
//	DESCRIPTION :
//
list<string> PkgChangelog::asStringList() const
{
  list<string> ret;
  for ( const_iterator i = begin(); i != end(); ++i ) {
    ret.push_back( stringutil::form( "* %s - %s",
				     i->_date.form( "%a %b %d %Y" ).c_str(),
				     i->_name.c_str() ) );
    ret.push_back( string() );
    list<string> dummy( stringutil::splitToLines( i->_text ) );
    ret.splice( ret.end(), dummy );
    ret.push_back( string() );
  }
  return ret;
}

/******************************************************************
**
**
**	FUNCTION NAME : operator<<
**	FUNCTION TYPE : ostream &
**
**	DESCRIPTION :
*/
ostream & operator<<( ostream & str, const PkgChangelog & obj )
{
  return stringutil::dumpOn( str, obj.asStringList() );
}

