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

  File:       F_Medianames.cc

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose:

/-*/

#include <iostream>

#include <Y2PM.h>
#include <y2pm/F_Medianames.h>

using namespace std;

///////////////////////////////////////////////////////////////////

const string F_Medianames::_noLabel; // empty string

///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : F_Medianames::F_Medianames
//	METHOD TYPE : Constructor
//
F_Medianames::F_Medianames()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : F_Medianames::~F_Medianames
//	METHOD TYPE : Destructor
//
F_Medianames::~F_Medianames()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : F_Medianames::label
//	METHOD TYPE : const std::string &
//
const std::string & F_Medianames::label( unsigned number_r ) const
{
  LabelMap::const_iterator n( _labels.find( number_r ) );
  if ( n != _labels.end() ) {
    const LangString & langString( n->second );
    // look for prefered locale and fallbacks
    PM::LocaleOrder langs( Y2PM::getLocaleFallback() );
    for ( PM::LocaleOrder::const_iterator lang = langs.begin(); lang != langs.end(); ++lang ) {
      LangString::const_iterator found = langString.find( *lang );
      if ( found != langString.end() ) {
	// gotcha
	return found->second;
      }
    }
    // look for empty locale
    LangString::const_iterator found = langString.find( LangCode() );
    if ( found != langString.end() ) {
      // gotcha
      return found->second;
    }
  }
  // found nothing at all
  return _noLabel;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : F_Medianames::read
//	METHOD TYPE : PMError
//
PMError F_Medianames::read( istream & stream_r )
{
  return PMError();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : F_Medianames::read
//	METHOD TYPE : PMError
//
PMError F_Medianames::read( const Pathname & file_r )
{
  return PMError();
}

/******************************************************************
**
**
**	FUNCTION NAME : operator<<
**	FUNCTION TYPE : ostream &
*/
ostream & operator<<( ostream & str, const F_Medianames & obj )
{
  return str << "F_Medianames(" << obj._labels.size() << ")";
}

