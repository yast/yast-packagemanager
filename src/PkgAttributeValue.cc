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

  File:       PkgAttributeValue.cc

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose:

/-*/

#include <iostream>

#include <y2pm/PkgAttributeValue.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PkgAttributeValue::PkgAttributeValue
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
PkgAttributeValue::PkgAttributeValue()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PkgAttributeValue::PkgAttributeValue
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
PkgAttributeValue::PkgAttributeValue( const std::string & val_r )
{
  push_back( val_r );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PkgAttributeValue::PkgAttributeValue
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
PkgAttributeValue::PkgAttributeValue( const std::list<string> & val_r )
    :  std::list<string>( val_r )
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PkgAttributeValue::~PkgAttributeValue
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
PkgAttributeValue::~PkgAttributeValue()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PkgAttributeValue::defValue
//	METHOD TYPE : string
//
//	DESCRIPTION :
//
string PkgAttributeValue::firstLine( const string & default_r ) const
{
  if ( empty() )
    return default_r;
  return front();
}

/******************************************************************
**
**
**	FUNCTION NAME : operator<<
**	FUNCTION TYPE : ostream &
**
**	DESCRIPTION :
*/
ostream & operator<<( ostream & str, const PkgAttributeValue & obj )
{
  return str << "AttributeValue(lines:" << obj.size()
    << " first:" << obj.firstLine() << ")";
}

