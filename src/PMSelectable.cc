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

   File:       PMSelectable.cc

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/

#include <iostream>

#include <y2util/Y2SLog.h>

#include <y2pm/PMSelectable.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMSelectable
//
///////////////////////////////////////////////////////////////////

IMPL_HANDLES(PMSelectable);

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelectable::PMSelectable
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
PMSelectable::PMSelectable()
    : _manager( 0 )
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelectable::PMSelectable
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
PMSelectable::PMSelectable( const PkgName& name_r )
    : _manager( 0 )
    , _name( name_r )
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelectable::~PMSelectable
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
PMSelectable::~PMSelectable()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelectable::dumpOn
//	METHOD TYPE : ostream &
//
//	DESCRIPTION :
//
ostream & PMSelectable::dumpOn( ostream & str ) const
{
  str << _name << '[' << Rep::dumpOn( str )
    << "(inst:" << _installedObj << ")"
    << "(cand:" << _candidateObj << ")"
    << "(avai:" << _candidateList.size() << ")"
    << ']';
  return str;
}

