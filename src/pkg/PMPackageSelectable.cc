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

   File:       PMPackageSelectable.cc

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/

#include <iostream>

#include <y2pm/PMPackageSelectable.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMPackageSelectable
//
///////////////////////////////////////////////////////////////////

IMPL_HANDLES(PMPackageSelectable);

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPackageSelectable::PMPackageSelectable
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
PMPackageSelectable::PMPackageSelectable()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPackageSelectable::~PMPackageSelectable
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
PMPackageSelectable::~PMPackageSelectable()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPackageSelectable::dumpOn
//	METHOD TYPE : ostream &
//
//	DESCRIPTION :
//
ostream & PMPackageSelectable::dumpOn( ostream & str ) const
{
  Rep::dumpOn( str );
  return str;
}

