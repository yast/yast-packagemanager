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

  File:       binHeader.cc

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose:

/-*/
#include "librpm.h"

#include <iostream>

#include <y2util/Y2SLog.h>

#include <y2pm/binHeader.h>

using namespace std;

#undef Y2LOG
#define Y2LOG "binHeader"

///////////////////////////////////////////////////////////////////
//      CLASS NAME : binHeaderPtr
//      CLASS NAME : constbinHeaderPtr
///////////////////////////////////////////////////////////////////
IMPL_BASE_POINTER(binHeader);

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : binHeader::intList
//
///////////////////////////////////////////////////////////////////

binHeader::intList::intList()
    : cnt( 0 ), val( 0 ), type( RPM_NULL_TYPE )
{}

unsigned binHeader::intList::set( void * val_r, tag cnt_r, tag type_r ) {
  val = val_r;
  cnt = val ? cnt_r : 0;
  type = type_r;
  return cnt;
}

int binHeader::intList::operator[]( const unsigned idx_r ) const {
  if ( idx_r < cnt ) {
    switch ( type ) {
    case RPM_CHAR_TYPE:
      return ((char*)val)[idx_r];
    case RPM_INT8_TYPE:
      return ((int_8*)val)[idx_r];
    case RPM_INT16_TYPE:
      return ((int_16*)val)[idx_r];
    case RPM_INT32_TYPE:
      return ((int_32*)val)[idx_r];
    }
  }
  return 0;
}

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : binHeader::stringList
//
///////////////////////////////////////////////////////////////////

void binHeader::stringList::clear() {
  if ( val )
    free( val );
  val = 0;
  cnt = 0;
}

binHeader::stringList::stringList()
    : cnt( 0 ), val( 0 )
{}

unsigned binHeader::stringList::set( char ** val_r, tag cnt_r ) {
  clear();
  val = val_r;
  cnt = val ? cnt_r : 0;
  return cnt;
}

std::string binHeader::stringList::operator[]( const unsigned idx_r ) const {
  return( idx_r < cnt ? val[idx_r] : "" );
}

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : binHeader
//
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : binHeader::binHeader
//	METHOD TYPE : Constructor
//
binHeader::binHeader( Header h_r )
    : _h( h_r )
{
  if ( _h ) {
    ::headerLink( _h );
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : binHeader::binHeader
//	METHOD TYPE : Constructor
//
binHeader::binHeader( binHeaderPtr & rhs )
{
  I__ << "INJECT from " << rhs;
  if ( ! (rhs && rhs->_h) ) {
    _h = 0;
  } else {
    _h = rhs->_h;  // ::headerLink already done in rhs
    rhs->_h = 0;
  }
  I__ << ": " << *this << "   (" << rhs << ")" << endl;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : binHeader::~binHeader
//	METHOD TYPE : Destructor
//
binHeader::~binHeader()
{
  if ( _h ) {
    ::headerFree( _h );
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : binHeader::assertHeader
//	METHOD TYPE : void
//
bool binHeader::assertHeader()
{
  if ( !_h ) {
    _h = ::headerNew();
    if ( !_h ) {
      INT << "OOPS: NULL HEADER created!" << endl;
      return false;
    }
  }
  return true;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : binHeader::has_tag
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool binHeader::has_tag( tag tag_r ) const
{
  return( !empty() && ::headerIsEntry( _h, tag_r ) );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : binHeader::int_list
//	METHOD TYPE : unsigned
//
//	DESCRIPTION :
//
unsigned binHeader::int_list( tag tag_r, intList & lst_r ) const
{
  if ( !empty() ) {
    int_32 type = 0;
    int_32 cnt  = 0;
    void * val  = 0;
    ::headerGetEntry( _h, tag_r, &type, &val, &cnt );

    if ( val ) {
      switch ( type ) {
      case RPM_NULL_TYPE:
	return lst_r.set( 0, 0, type );
      case RPM_CHAR_TYPE:
      case RPM_INT8_TYPE:
      case RPM_INT16_TYPE:
      case RPM_INT32_TYPE:
	return lst_r.set( val, cnt, type );

      case RPM_STRING_ARRAY_TYPE:
	free( val );
	// fall through
      default:
	INT << "RPM_TAG MISSMATCH: RPM_INT32_TYPE " << tag_r << " got type " << type << endl;
      }
    }
  }
  return lst_r.set( 0, 0, RPM_NULL_TYPE );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : binHeader::string_list
//	METHOD TYPE : unsigned
//
//	DESCRIPTION :
//
unsigned binHeader::string_list( tag tag_r, stringList & lst_r ) const
{
  if ( !empty() ) {
    int_32 type = 0;
    int_32 cnt  = 0;
    void * val  = 0;
    ::headerGetEntry( _h, tag_r, &type, &val, &cnt );

    if ( val ) {
      switch ( type ) {
      case RPM_NULL_TYPE:
	return lst_r.set( 0, 0 );
      case RPM_STRING_ARRAY_TYPE:
	return lst_r.set( (char**)val, cnt );

      default:
	INT << "RPM_TAG MISSMATCH: RPM_STRING_ARRAY_TYPE " << tag_r << " got type " << type << endl;
      }
    }
  }
  return lst_r.set( 0, 0 );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : binHeader::int_val
//	METHOD TYPE : int
//
//	DESCRIPTION :
//
int binHeader::int_val( tag tag_r ) const
{
  if ( !empty() ) {
    int_32 type = 0;
    int_32 cnt  = 0;
    void * val  = 0;
    ::headerGetEntry( _h, tag_r, &type, &val, &cnt );

    if ( val ) {
      switch ( type ) {
      case RPM_NULL_TYPE:
	return 0;
      case RPM_CHAR_TYPE:
	return *((char*)val);
      case RPM_INT8_TYPE:
	return *((int_8*)val);
      case RPM_INT16_TYPE:
	return *((int_16*)val);
      case RPM_INT32_TYPE:
	return *((int_32*)val);

      case RPM_STRING_ARRAY_TYPE:
	free( val );
	// fall through
      default:
	INT << "RPM_TAG MISSMATCH: RPM_INT32_TYPE " << tag_r << " got type " << type << endl;
      }
    }
  }
  return 0;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : binHeader::string_val
//	METHOD TYPE : std::string
//
//	DESCRIPTION :
//
std::string binHeader::string_val( tag tag_r ) const
{
  if ( !empty() ) {
    int_32 type = 0;
    int_32 cnt  = 0;
    void * val  = 0;
    ::headerGetEntry( _h, tag_r, &type, &val, &cnt );

    if ( val ) {
      switch ( type ) {
      case RPM_NULL_TYPE:
	return "";
      case RPM_STRING_TYPE:
	return (char*)val;

      case RPM_STRING_ARRAY_TYPE:
	free( val );
	// fall through
      default:
	INT << "RPM_TAG MISSMATCH: RPM_STRING_TYPE " << tag_r << " got type " << type << endl;
      }
    }
  }
  return "";
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : binHeader::stringList_val
//	METHOD TYPE : std::list<std::string>
//
//	DESCRIPTION :
//
std::list<std::string> binHeader::stringList_val( tag tag_r ) const
{
  std::list<std::string> ret;

  if ( !empty() ) {
    stringList lines;
    unsigned count = string_list( tag_r, lines );
    for ( unsigned i = 0; i < count; ++i ) {
      ret.push_back( lines[i] );
    }
  }
  return ret;
}

///////////////////////////////////////////////////////////////////
//
//
//      METHOD NAME : binHeader::dumpOn
//      METHOD TYPE : ostream &
//
//      DESCRIPTION :
//
ostream & binHeader::dumpOn( ostream & str ) const
{
  Rep::dumpOn( str );
  return str << '{' << (void*)_h << '}';
}
