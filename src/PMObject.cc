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

   File:       PMObject.cc

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/

#include <y2pm/PMObject.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMObject
//
///////////////////////////////////////////////////////////////////

IMPL_HANDLES(PMObject);

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMObject::PMObject
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
PMObject::PMObject( const PkgName &    name_r,
		const PkgEdition & edition_r )
{
  _name    = name_r;
  _edition = edition_r;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMObject::~PMObject
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
PMObject::~PMObject()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMObject::dumpOn
//	METHOD TYPE : ostream &
//
//	DESCRIPTION :
//
ostream & PMObject::dumpOn( ostream & str ) const
{
  Rep::dumpOn( str ) << '(' << _name << ")(" << _edition.version() << ")(" << _edition.release() << ')';
  return str;
}

