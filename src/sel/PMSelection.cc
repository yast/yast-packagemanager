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

  File:       PMSelection.cc

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Defines the software Selection object.

/-*/

#include <iostream>

#include <y2util/Y2SLog.h>

#include <y2pm/PMSelection.h>
#include <y2pm/PMSelectionDataProvider.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMSelection
//
///////////////////////////////////////////////////////////////////

IMPL_DERIVED_POINTER( PMSelection, PMObject, PMSolvable );

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelection::PMSelection
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
PMSelection::PMSelection( const PkgName &    name_r,
			  const PkgEdition & edition_r,
			  const PkgArch &    arch_r,
			  PMSelectionDataProviderPtr dataProvider_r )
    : PMObject( name_r, edition_r, arch_r )
    , _dataProvider( dataProvider_r )
{
}

void
PMSelection::startRetrieval () const
{
    _dataProvider->startRetrieval ();
}

void
PMSelection::stopRetrieval () const
{
    _dataProvider->stopRetrieval ();
}

// overlay virtual PMObject functions

const std::string
PMSelection::summary(const std::string& lang) const { return _dataProvider->summary(lang); }

const std::list<std::string>
PMSelection::description(const std::string& lang) const { return _dataProvider->description(lang); }

const std::list<std::string>
PMSelection::insnotify(const std::string& lang) const { return _dataProvider->insnotify(lang); }

const std::list<std::string>
PMSelection::delnotify(const std::string& lang) const { return _dataProvider->delnotify(lang); }

const FSize
PMSelection::size() const { return _dataProvider->size(); }

/**
 * access functions for PMSelection attributes
 */

const std::string
PMSelection::category () const { return _dataProvider->category(); }
const bool
PMSelection::visible () const { return _dataProvider->visible(); }
const std::list<std::string>
PMSelection::suggests() const { return _dataProvider->suggests(); }
const std::list<std::string>
PMSelection::inspacks(const std::string& lang) const { return _dataProvider->inspacks(lang); }
const std::list<std::string>
PMSelection::delpacks(const std::string& lang) const { return _dataProvider->delpacks(lang); }
const FSize
PMSelection::archivesize() const { return _dataProvider->archivesize(); }
const std::string
PMSelection::order() const { return _dataProvider->order(); }


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelection::~PMSelection
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
PMSelection::~PMSelection()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelection::getAttributeName
//	METHOD TYPE : string
//
//	DESCRIPTION :
//
string PMSelection::getAttributeName( PMSelectionAttribute attr ) const
{
    if (attr < PMSelection::PMSEL_ATTR_BEGIN)
	return getAttributeName ((PMObject::PMObjectAttribute)attr);
  switch ( attr ) {

#define ENUM_OUT(V) case ATTR_##V: return #V; break
    ENUM_OUT( CATEGORY );
    ENUM_OUT( VISIBLE );
    ENUM_OUT( SUGGESTS );
    ENUM_OUT( INSPACKS );
    ENUM_OUT( DELPACKS );
    ENUM_OUT( ARCHIVESIZE );
#undef ENUM_OUT

  ///////////////////////////////////////////////////////////////////
  // no default: let compiler warn '... not handled in switch'
  ///////////////////////////////////////////////////////////////////
  case PMSEL_NUM_ATTRIBUTES:
    // illegal attr value
    break;
  }
  // HERE: illegal attr value or forgott do adjust switch.
  ERR << "Illegal SelectionAttribute(" << attr << ')' << endl;
  return "";
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelection::getAttributeValue
//	METHOD TYPE : PkgAttributeValue
//
//	DESCRIPTION :
//
PkgAttributeValue PMSelection::getAttributeValue( PMSelectionAttribute attr ) const
{
  if ( !_dataProvider ) {
    ERR << "No DataProvider for " << *this << endl;
    return PkgAttributeValue();
  }
  return _dataProvider->getAttributeValue( this, attr );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelection::dumpOn
//	METHOD TYPE : ostream &
//
//	DESCRIPTION :
//
ostream & PMSelection::dumpOn( ostream & str ) const
{
  PMObject::dumpOn( str );
  return str;
}

