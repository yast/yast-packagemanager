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

  File:       PMYouPatch.cc

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Defines the YouPatch object.

/-*/

#include <iostream>

#include <y2util/Y2SLog.h>
#include <y2util/FSize.h>

#include <y2pm/PMYouPatch.h>
#include <y2pm/PMYouPatchDataProvider.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMYouPatch
//
///////////////////////////////////////////////////////////////////

IMPL_DERIVED_POINTER( PMYouPatch, PMObject, PMSolvable );

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMYouPatch::PMYouPatch
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
PMYouPatch::PMYouPatch( const PkgName &    name_r,
			const PkgEdition & edition_r,
                        const PkgArch & arch_r,
			PMYouPatchDataProviderPtr dataProvider_r )
    : PMObject( name_r, edition_r, arch_r )
    , _kind( kind_invalid ), _updateOnlyInstalled( false )
    , _dataProvider( dataProvider_r )
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMYouPatch::~PMYouPatch
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
PMYouPatch::~PMYouPatch()
{
}

void
PMYouPatch::startRetrieval () const
{
    _dataProvider->startRetrieval ();
}

void
PMYouPatch::stopRetrieval () const
{
    _dataProvider->stopRetrieval ();
}

const std::string
PMYouPatch::summary() const { return _dataProvider->summary(); }
const std::list<std::string>
PMYouPatch::description() const { return _dataProvider->description(); }
const std::list<std::string>
PMYouPatch::insnotify() const { return _dataProvider->insnotify(); }
const std::list<std::string>
PMYouPatch::delnotify() const { return _dataProvider->delnotify(); }
const FSize
PMYouPatch::size() const { return _dataProvider->size(); }

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMYouPatch::dumpOn
//	METHOD TYPE : ostream &
//
//	DESCRIPTION :
//
ostream & PMYouPatch::dumpOn( ostream & str ) const
{
  PMObject::dumpOn( str );

  str << "Kind: " << kindLabel( _kind ) << endl;
  str << "ShortDescription: " << _shortDescription << endl;
  str << "LongDescription:" << endl << _longDescription << endl;
  str << "PreInformation:" << endl << _preInformation << endl;
  str << "PostInformation:" << endl << _postInformation << endl;
  str << "UpdateOnlyInstalled:" << ( _updateOnlyInstalled ? "true" : "false" ) << endl;
  str << "Prescript: " << _preScript << endl;
  str << "Postscript: " << _postScript << endl;

  return str;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMYouPatch::kindLabel
//	METHOD TYPE : ostream &
//
//	DESCRIPTION :
//
string PMYouPatch::kindLabel( Kind kind )
{
  switch ( kind ) {
    case kind_recommended:
      return "Recommended";
    case kind_security:
      return "Security";
    case kind_optional:
      return "Optional";
    case kind_document:
      return "Document";
    case kind_yast:
      return "YaST2";
    default:
      return "unknown";
   }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMYouPatch::kindLabel
//	METHOD TYPE : ostream &
//
//	DESCRIPTION :
//
void PMYouPatch::addPackage( const PMPackagePtr &pkg )
{
  _packages.push_back( pkg );
}
