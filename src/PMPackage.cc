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

   File:       PMPackage.cc

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/

#include <y2util/Y2SLog.h>

#include <y2pm/PMPackage.h>
#include <y2pm/PMPackageDataProvider.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMPackage
//
///////////////////////////////////////////////////////////////////

IMPL_HANDLES(PMPackage);

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPackage::PMPackage
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
PMPackage::PMPackage( const PkgName &    name_r,
		      const PkgEdition & edition_r,
		      const PkgArch &    arch_r )
    : PMObject( name_r, edition_r )
    , _arch( arch_r )
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPackage::~PMPackage
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
PMPackage::~PMPackage()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPackage::Summary
//	METHOD TYPE : string
//
//	DESCRIPTION :
//
string PMPackage::Summary() const
{
  if ( !_dataProvider ) {
    ERR << "No DataProvider for " << *this << endl;
    return PMObject::Summary();
  }

  // TBD: get data from _dataProvider
  return string( "Faked summary for Package" ) + string( name() );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPackage::dumpOn
//	METHOD TYPE : ostream &
//
//	DESCRIPTION :
//
ostream & PMPackage::dumpOn( ostream & str ) const
{
  PMObject::dumpOn( str ) << '(' << _arch << ')';
  return str;
}

