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

  File:       PkgIdent.cc

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose:

/-*/

#include <iostream>

#include <y2util/stringutil.h>

#include <y2pm/PkgIdent.h>
#include <y2pm/PMSolvable.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PkgIdent::PkgIdent
//	METHOD TYPE : Constructor
//
PkgIdent::PkgIdent( constPMSolvablePtr slv_r )
{
  if ( ! slv_r )
    return;
  _name    = slv_r->name();
  _edition = slv_r->edition();
  _arch    = slv_r->arch();
}

/******************************************************************
**
**
**	FUNCTION NAME : operator<<
**	FUNCTION TYPE : ostream &
*/
ostream & operator<<( ostream & str, const PkgIdent & obj )
{
  return str << obj.nameEdArch();
}

