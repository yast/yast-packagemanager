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

#include <Y2PM.h>
#include <y2pm/PMSelection.h>
#include <y2pm/PMSelectionDataProvider.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMSelection
//
///////////////////////////////////////////////////////////////////

IMPL_DERIVED_POINTER( PMSelection, PMSolvable );

///////////////////////////////////////////////////////////////////
//
// PMSelection attribute retrieval via dataProvider, or default values.
//
///////////////////////////////////////////////////////////////////
#define DP_GET(ATTR)         if ( _dataProvider ) return _dataProvider->ATTR( *this ); return PMSelectionDataProvider::ATTR()
#define DP_ARG_GET(ATTR,ARG) if ( _dataProvider ) return _dataProvider->ATTR( *this, ARG ); return PMSelectionDataProvider::ATTR( ARG )
// PMObject attributes
std::string               PMSelection::summary    ( const LangCode& locale ) const { DP_ARG_GET( summary, locale ); }
std::list<std::string>    PMSelection::description( const LangCode& locale ) const { DP_ARG_GET( description, locale ); }
std::list<std::string>    PMSelection::insnotify  ( const LangCode& locale ) const { DP_ARG_GET( insnotify, locale ); }
std::list<std::string>    PMSelection::delnotify  ( const LangCode& locale ) const { DP_ARG_GET( delnotify, locale ); }
FSize                     PMSelection::size()            const { DP_GET( size ); }
bool                      PMSelection::providesSources() const { DP_GET( providesSources ); }
std::string               PMSelection::instSrcLabel()    const { DP_GET( instSrcLabel ); }
Vendor                    PMSelection::instSrcVendor()   const { DP_GET( instSrcVendor ); }
unsigned                  PMSelection::instSrcRank()     const { DP_GET( instSrcRank ); }
// PMSelection attributes
std::string               PMSelection::category()        const { DP_GET( category ); }
bool                      PMSelection::visible()         const { DP_GET( visible ); }
std::list<std::string>    PMSelection::suggests()        const { DP_GET( suggests ); }
std::list<PMSelectionPtr> PMSelection::suggests_ptrs()         { DP_GET( suggests_ptrs ); }
std::list<std::string>    PMSelection::recommends()      const { DP_GET( recommends ); }
std::list<PMSelectionPtr> PMSelection::recommends_ptrs()       { DP_GET( recommends_ptrs ); }
std::list<std::string>    PMSelection::inspacks     ( const LangCode& locale ) const { DP_ARG_GET( inspacks, locale ); }
std::list<std::string>    PMSelection::delpacks     ( const LangCode& locale ) const { DP_ARG_GET( delpacks, locale ); }
PM::LocaleSet             PMSelection::supportedLocales() const { DP_GET( supportedLocales ); }
std::set<PMSelectablePtr> PMSelection::inspacks_ptrs( const LangCode& locale )       { DP_ARG_GET( inspacks_ptrs, locale ); }
std::set<PMSelectablePtr> PMSelection::delpacks_ptrs( const LangCode& locale )       { DP_ARG_GET( delpacks_ptrs, locale ); }
FSize                     PMSelection::archivesize()     const { DP_GET( archivesize ); }
std::string               PMSelection::order()           const { DP_GET( order ); }

const bool                PMSelection::isBase()          const { DP_GET( isBase ); }

// physical access to the sel file.
PMError PMSelection::provideSelToInstall( Pathname & path ) const { DP_ARG_GET( provideSelToInstall, path ); }
#undef DP_ARG_GET
#undef DP_GET
///////////////////////////////////////////////////////////////////

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
  if ( !_dataProvider ) {
    WAR << "NULL DataProvider for " << *this << endl;
  }
}

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
//	METHOD NAME : PMSelection::inspacks_ptrs
//	METHOD TYPE : std::set<PMSelectablePtr>
//
std::set<PMSelectablePtr> PMSelection::inspacks_ptrs( const PM::LocaleSet & locales )
{
  set<PMSelectablePtr> ret;
  for ( PM::LocaleSet::const_iterator it = locales.begin(); it != locales.end(); ++it ) {
    set<PMSelectablePtr> res = inspacks_ptrs( *it );
    ret.insert( res.begin(), res.end() );
  }
  return ret;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelection::delpacks_ptrs
//	METHOD TYPE : std::set<PMSelectablePtr>
//
std::set<PMSelectablePtr> PMSelection::delpacks_ptrs( const PM::LocaleSet & locales )
{
  set<PMSelectablePtr> ret;
  for ( PM::LocaleSet::const_iterator it = locales.begin(); it != locales.end(); ++it ) {
    set<PMSelectablePtr> res = delpacks_ptrs( *it );
    ret.insert( res.begin(), res.end() );
  }
  return ret;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelection::inspacks_ptrs
//	METHOD TYPE : set<PMSelectablePtr>
//
//	DESCRIPTION :
//
set<PMSelectablePtr> PMSelection::inspacks_ptrs()
{
  set<PMSelectablePtr> ret = inspacks_ptrs( LangCode( "" ) );
  set<PMSelectablePtr> res = inspacks_ptrs( Y2PM::getRequestedLocales() );
  ret.insert( res.begin(), res.end() );
  return ret;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelection::delpacks_ptrs
//	METHOD TYPE : set<PMSelectablePtr>
//
//	DESCRIPTION :
//
set<PMSelectablePtr> PMSelection::delpacks_ptrs()
{
  set<PMSelectablePtr> ret = delpacks_ptrs( LangCode( "" ) );
  set<PMSelectablePtr> res = delpacks_ptrs( Y2PM::getRequestedLocales() );
  ret.insert( res.begin(), res.end() );
  return ret;
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

