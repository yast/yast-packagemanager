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

  Purpose: Defines the Package object.

/-*/

#include <iostream>

#include <y2util/Y2SLog.h>

#include <y2pm/PMPackage.h>
#include <y2pm/PMPackageDataProvider.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMPackage
//
///////////////////////////////////////////////////////////////////

IMPL_DERIVED_POINTER( PMPackage, PMObject, PMSolvable );

///////////////////////////////////////////////////////////////////
//
// PMPackage attribute retrieval via dataProvider, or default values.
//
///////////////////////////////////////////////////////////////////
#define DP_GET(ATTR) if ( _dataProvider ) return _dataProvider->ATTR( *this ); return PMPackageDataProvider::ATTR()
// PMObject attributes
std::string            PMPackage::summary()      const { DP_GET( summary ); }
std::list<std::string> PMPackage::description()  const { DP_GET( description ); }
std::list<std::string> PMPackage::insnotify()    const { DP_GET( insnotify ); }
std::list<std::string> PMPackage::delnotify()    const { DP_GET( delnotify ); }
FSize                  PMPackage::size()         const { DP_GET( size ); }
// PMPackage attributes
std::list<std::string> PMPackage::splitprovides()const { DP_GET( splitprovides ); }
Date                   PMPackage::buildtime()    const { DP_GET( buildtime ); }
std::string            PMPackage::buildhost()    const { DP_GET( buildhost ); }
Date                   PMPackage::installtime()  const { DP_GET( installtime ); }
std::string            PMPackage::distribution() const { DP_GET( distribution ); }
std::string            PMPackage::vendor()       const { DP_GET( vendor ); }
std::string            PMPackage::license()      const { DP_GET( license ); }
std::string            PMPackage::packager()     const { DP_GET( packager ); }
std::string            PMPackage::group()        const { DP_GET( group ); }
YStringTreeItem *      PMPackage::group_ptr()    const { DP_GET( group_ptr ); }
std::list<std::string> PMPackage::changelog()    const { DP_GET( changelog ); }
std::string            PMPackage::url()          const { DP_GET( url ); }
std::string            PMPackage::os()           const { DP_GET( os ); }
std::list<std::string> PMPackage::prein()        const { DP_GET( prein ); }
std::list<std::string> PMPackage::postin()       const { DP_GET( postin ); }
std::list<std::string> PMPackage::preun()        const { DP_GET( preun ); }
std::list<std::string> PMPackage::postun()       const { DP_GET( postun ); }
std::string            PMPackage::sourcerpm()    const { DP_GET( sourcerpm ); }
FSize                  PMPackage::archivesize()  const { DP_GET( archivesize ); }
std::list<std::string> PMPackage::authors()      const { DP_GET( authors ); }
std::list<std::string> PMPackage::filenames()    const { DP_GET( filenames ); }
// suse packages values
std::list<std::string> PMPackage::recommends()   const { DP_GET( recommends ); }
std::list<std::string> PMPackage::suggests()     const { DP_GET( suggests ); }
std::string            PMPackage::location()     const { DP_GET( location ); }
unsigned int           PMPackage::medianr()      const { DP_GET( medianr ); }
std::list<std::string> PMPackage::keywords()     const { DP_GET( keywords ); }
std::list<std::string> PMPackage::du()	         const { DP_GET( du ); }
#undef DP_GET
///////////////////////////////////////////////////////////////////

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
		      const PkgArch &    arch_r,
		      PMPackageDataProviderPtr dataProvider_r,
		      constInstSrcPtr source )
    : PMObject( name_r, edition_r, arch_r, source )
    , _dataProvider( dataProvider_r )
{
  if ( !_dataProvider ) {
    WAR << "NULL DataProvider for " << *this << endl;
  }
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
//	METHOD NAME : PMPackage::dumpOn
//	METHOD TYPE : ostream &
//
//	DESCRIPTION :
//
ostream & PMPackage::dumpOn( ostream & str ) const
{
  PMObject::dumpOn( str );
  return str;
}

