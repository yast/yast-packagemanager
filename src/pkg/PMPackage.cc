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
#include <y2pm/PkgDu.h>

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
#define DP_ARG_GET(ATTR,ARG) if ( _dataProvider ) return _dataProvider->ATTR( *this, ARG ); return PMPackageDataProvider::ATTR( ARG )
// PMObject attributes
std::string            PMPackage::summary()      const { DP_GET( summary ); }
std::list<std::string> PMPackage::description()  const { DP_GET( description ); }
std::list<std::string> PMPackage::insnotify()    const { DP_GET( insnotify ); }
std::list<std::string> PMPackage::delnotify()    const { DP_GET( delnotify ); }
FSize                  PMPackage::size()         const { DP_GET( size ); }
bool                   PMPackage::providesSources() const { DP_GET( providesSources ); }
std::string            PMPackage::instSrcLabel() const { DP_GET( instSrcLabel ); }
Vendor                 PMPackage::instSrcVendor()const { DP_GET( instSrcVendor ); }
unsigned               PMPackage::instSrcRank()  const { DP_GET( instSrcRank ); }
// PMPackage attributes
PkgSplitSet            PMPackage::splitprovides()const { DP_GET( splitprovides ); }
Date                   PMPackage::buildtime()    const { DP_GET( buildtime ); }
std::string            PMPackage::buildhost()    const { DP_GET( buildhost ); }
Date                   PMPackage::installtime()  const { DP_GET( installtime ); }
std::string            PMPackage::distribution() const { DP_GET( distribution ); }
Vendor                 PMPackage::vendor()       const { DP_GET( vendor ); }
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
std::string            PMPackage::sourceloc()    const { DP_GET( sourceloc ); }
FSize                  PMPackage::sourcesize()   const { DP_GET( sourcesize ); }
FSize                  PMPackage::archivesize()  const { DP_GET( archivesize ); }
std::list<std::string> PMPackage::authors()      const { DP_GET( authors ); }
std::list<std::string> PMPackage::filenames()    const { DP_GET( filenames ); }
// suse packages values
std::list<std::string> PMPackage::recommends()   const { DP_GET( recommends ); }
std::list<std::string> PMPackage::suggests()     const { DP_GET( suggests ); }
std::string            PMPackage::location()     const { DP_GET( location ); }
unsigned int           PMPackage::medianr()      const { DP_GET( medianr ); }
std::list<std::string> PMPackage::keywords()     const { DP_GET( keywords ); }
std::string            PMPackage::externalUrl()  const { DP_GET( externalUrl ); }
std::list<PkgEdition>  PMPackage::patchRpmBaseVersions() const { DP_GET( patchRpmBaseVersions ); }
FSize                  PMPackage::patchRpmSize() const { DP_GET( patchRpmSize ); }

// package file comes from remote
bool		       PMPackage::isRemote()	 const { DP_GET( isRemote ); }
// physical access to the rpm file.
PMError                PMPackage::providePkgToInstall(Pathname& path) const { DP_ARG_GET( providePkgToInstall, path ); }
// physical access to the src.rpm file.
PMError                PMPackage::provideSrcPkgToInstall(Pathname& path) const { DP_ARG_GET( provideSrcPkgToInstall, path ); }
// who's providing this package
constInstSrcPtr  PMPackage::source()		 const { DP_GET ( source ); }
#undef DP_GET

// dudata is special
PkgDu & PMPackage::du( PkgDu & dudata_r ) const {
  if ( _dataProvider )
    _dataProvider->du( *this, dudata_r );
  else
    PMPackageDataProvider::du( dudata_r );
  return dudata_r;
}
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
		      PMPackageDataProviderPtr dataProvider_r )
    : PMObject( name_r, edition_r, arch_r )
    , _pkgdu( * new PkgDuSlave )
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
  delete &_pkgdu;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPackage::du_add
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool PMPackage::du_add( PkgDuMaster & master_r ) const
{
  return _pkgdu.add( *this, master_r );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPackage::du_sub
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool PMPackage::du_sub( PkgDuMaster & master_r ) const
{
  return _pkgdu.sub( *this, master_r );
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

