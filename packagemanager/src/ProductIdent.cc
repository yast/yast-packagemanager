/*---------------------------------------------------------------------\
|                                                                      |
|                      __   __    ____ _____ ____                      |
|                      \ \ / /_ _/ ___|_   _|___ \                     |
|                       \ V / _` \___ \ | |   __) |                    |
|                        | | (_| |___) || |  / __/                     |
|                        |_|\__,_|____/ |_| |_____|                    |
|                                                                      |
|                               core system                            |
|                                                    (C) SuSE Linux AG |
\----------------------------------------------------------------------/

  File:       ProductIdent.cc

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose:

/-*/

#include <iostream>

#include <y2util/stringutil.h>

#include <y2pm/ProductIdent.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : ProductIdent::_initCheck
//	METHOD TYPE : void
//
void ProductIdent::_initCheck()
{
  // It's illegal to have a name without version or vice versa!
  if ( (_product.name->empty()) != (_product.edition.asString() == PkgEdition().asString()) ) {
    _product = PkgNameEd( PkgName(), PkgEdition() );
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : ProductIdent::ProductIdent
//	METHOD TYPE : Constructor
//
ProductIdent::ProductIdent()
    : _product( PkgName(), PkgEdition() )
{
}
ProductIdent::ProductIdent( const PkgNameEd & ne )
    : _product( ne )
{
  _initCheck();
}
ProductIdent::ProductIdent( const PkgName & n, const PkgEdition & e )
    : _product( n, e )
{
  _initCheck();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : ProductIdent::~ProductIdent
//	METHOD TYPE : Destructor
//
ProductIdent::~ProductIdent()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : ProductIdent::isLine
//	METHOD TYPE : bool
//
bool ProductIdent::isLine( std::string name_r ) const
{
  // compare uppercase to catch stuff like 'SuSE - SUSE' or 'Linux - LINUX'
  name_r = stringutil::toUpper( name_r );
  string name( stringutil::toUpper( _product.name.asString() ) );

  if ( name == name_r ) {
    return true;
  }

  if ( ( name_r == "UNITEDLINUX" && name == "SUSE CORE" )
       ||
       ( name == "UNITEDLINUX" && name_r == "SUSE CORE" ) ) {
    return true;
  }

  return false;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : ProductIdent::obsoletes
//	METHOD TYPE : bool
//
bool ProductIdent::obsoletes( const ProductIdent & rhs ) const
{
  return( sameLine( rhs ) && ! sameVersion( rhs ) );
}

/******************************************************************
**
**
**	FUNCTION NAME : operator<<
**	FUNCTION TYPE : ostream &
*/
ostream & operator<<( ostream & str, const ProductIdent & obj )
{
  return str << obj._product;
}

