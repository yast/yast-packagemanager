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

  Textdomain "packagemanager"

/-*/

#include <iostream>

#include <y2util/Y2SLog.h>
#include <y2util/FSize.h>
#include <y2util/stringutil.h>

#include <y2pm/PMYouPatch.h>
#include <y2pm/PMSelectable.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMYouPatch
//
///////////////////////////////////////////////////////////////////

IMPL_DERIVED_POINTER( PMYouPatch, PMSolvable );

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
                        const PkgArch & arch_r )
    : PMObject( name_r, edition_r, arch_r )
    , _kind( kind_invalid ), _updateOnlyInstalled( false ),
    _packagesInstalled( false ), _skipped( false )
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
      return _("Recommended");
    case kind_security:
      return _("Security");
    case kind_optional:
      return _("Optional");
    case kind_document:
      return _("Document");
    case kind_yast:
      return _("YaST2");
    default:
      return _("unknown");
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

void PMYouPatch::clearPackages()
{
  _packages.clear();
}

void PMYouPatch::setPackages( const std::list<PMPackagePtr> &p )
{
  _packages = p;
}

void PMYouPatch::setPatchSize( const FSize &size )
{
  _patchSize = size;
}

FSize PMYouPatch::patchSize() const
{
  return _patchSize;
}

string PMYouPatch::fullName() const
{
  string result = name();
  result += "-";
  result += edition().asString();
  return result;
}

list<string> PMYouPatch::description() const
{
  list<string> ret;
  vector<string> lines;
  stringutil::split( longDescription(), lines, "\n", true );
  vector<string>::const_iterator it;
  for( it = lines.begin(); it != lines.end(); ++it ) {
    ret.push_back( *it );
  }
  return ret;
}

list<string> PMYouPatch::insnotify() const
{
  if ( installable() ) {
    return PMObject::insnotify();
  }

  list<string> text;
  
  text.push_back( _("Installing this patch will have no effect, because it") );
  text.push_back( _("doesn't contain any updates to installed packages.") );

  return text;
}

bool PMYouPatch::installable() const
{
  return _packagesInstalled || !_updateOnlyInstalled ||
         !_preScript.empty() || !_postScript.empty() ||
         !_files.empty();
}

void PMYouPatch::setFiles( const std::list<PMYouFile> &files )
{
  _files = files;
}

void PMYouPatch::addFile( const PMYouFile &file )
{
  _files.push_back( file );
}

void PMYouPatch::setProduct( const PMYouProductPtr &p )
{
  _product = p;
}

PMYouProductPtr PMYouPatch::product() const
{
  return _product;
}

bool PMYouPatch::isSelected() const
{
  PMSelectablePtr selectable = getSelectable();
  if ( selectable && selectable->to_install() &&
       constPMYouPatchPtr( this ) == selectable->candidateObj() ) {
    return true;
  } else {
    return false;
  }
}
