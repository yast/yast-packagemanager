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

  File:       PMCallbacks.cc

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Provides some types frequently used in callback settings.

/-*/

#include <iostream>

#include <y2util/Y2SLog.h>
#include <y2util/stringutil.h>

#include <y2pm/PMCallbacks.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : CBSuggest
//
///////////////////////////////////////////////////////////////////

static inline bool isPrefix( const string & reqStr_r, const string & optStr_r, const string & prfx_r )
{
  string::size_type r = reqStr_r.size();
  string::size_type o = optStr_r.size();
  string::size_type p = prfx_r.size();

  if ( p < r || p > r+o )
    return false; // prefix shorter than requires part or longer than string

  if ( r && prfx_r.substr( 0, r ) != reqStr_r )
    return false; // does not match required part

  p -= r; // size of optional Part to compare

  if ( p && prfx_r.substr( r ) != optStr_r.substr( 0, p ) )
    return false; // does not prefix optional part

  return true;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : CBSuggest::CBSuggest
//	METHOD TYPE : Constructor
//
CBSuggest::CBSuggest( const string & str_r )
    : action( PROCEED )
{
  if ( str_r.empty() )
    return; // PROCEED

  string parse( stringutil::toUpper( str_r ) );

  switch ( *parse.c_str() ) {
#define SETIF(R,O) if ( isPrefix( #R, #O, parse ) ) { action = R##O; return; }
  case 'C':
    SETIF( C,ANCEL );
    break;
  case 'P':
    SETIF( P,ROCEED );
    break;
  case 'R':
    SETIF( R,ETRY );
    break;
  case 'S':
    SETIF( S,KIP );
    break;
#undef SETIF
  }
  // if switch didn't return it's sonmething unknown.
  _INT("CBSuggest") << "Unknown CBSuggest value '" << str_r << "' -> using 'PROCEED'" << endl;
}

/******************************************************************
**
**
**	FUNCTION NAME : operator<<
**	FUNCTION TYPE : std::ostream &
*/
std::ostream & operator<<( std::ostream & str, const CBSuggest::Action & obj )
{
  switch ( obj ) {
#define ENUMOUT(S) case CBSuggest::S: return str << #S; break
    ENUMOUT( CANCEL );
    ENUMOUT( PROCEED );
    ENUMOUT( RETRY );
    ENUMOUT( SKIP );
#undef ENUMOUT
  }
  return str << "CBSuggest(UNKNOWN)";
}
