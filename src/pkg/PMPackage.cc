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
    , _dataProvider( dataProvider_r )
{
    if ( !_dataProvider ) {
	ERR << "No DataProvider for PMPackage()" << endl;
	abort ();
    }
}

/**
 * hint before accessing multiple attributes
 */
void
PMPackage::startRetrieval() const
{
    _dataProvider->startRetrieval();
}

/**
 * hint after accessing multiple attributes
 */
void
PMPackage::stopRetrieval() const
{
    return _dataProvider->stopRetrieval();
}

// cant define functions in header because PMPackageDataProvider
// is incomplete there

const std::string
PMPackage::summary() const { return _dataProvider->summary (); }

const std::list<std::string>
PMPackage::description() const { return _dataProvider->description (); }

const std::list<std::string>
PMPackage::insnotify() const { return _dataProvider->insnotify (); }

const std::list<std::string>
PMPackage::delnotify() const { return _dataProvider->delnotify (); }

const FSize
PMPackage::size () const { return _dataProvider->size (); }

const Date
PMPackage::buildtime() const { return _dataProvider->buildtime (); }

const std::string
PMPackage::buildhost() const { return _dataProvider->buildhost (); }

const Date
PMPackage::installtime() const { return _dataProvider->installtime (); }

const std::string
PMPackage::distribution() const { return _dataProvider->distribution (); }

const std::string
PMPackage::vendor() const { return _dataProvider->vendor (); }

const std::string
PMPackage::license() const { return _dataProvider->license (); }

const std::string
PMPackage::packager() const { return _dataProvider->packager (); }

const std::string
PMPackage::group() const { return _dataProvider->group (); }

const std::list<std::string>
PMPackage::changelog() const { return _dataProvider->changelog (); }

const std::string
PMPackage::url() const { return _dataProvider->url (); }

const std::string
PMPackage::os() const { return _dataProvider->os (); }

const std::list<std::string>
PMPackage::prein() const { return _dataProvider->prein (); }

const std::list<std::string>
PMPackage::postin() const { return _dataProvider->postin (); }

const std::list<std::string>
PMPackage::preun() const { return _dataProvider->preun (); }

const std::list<std::string>
PMPackage::postun() const { return _dataProvider->postun (); }

const std::string
PMPackage::sourcerpm() const { return _dataProvider->sourcerpm (); }

const FSize
PMPackage::archivesize() const { return _dataProvider->archivesize (); }

const std::list<std::string>
PMPackage::authors() const { return _dataProvider->authors (); }

const std::list<std::string>
PMPackage::filenames() const { return _dataProvider->filenames (); }

	// suse packages values
const std::list<std::string>
PMPackage::recommends() const { return _dataProvider->recommends (); }

const std::list<std::string>
PMPackage::suggests() const { return _dataProvider->suggests (); }

const std::string
PMPackage::location() const { return _dataProvider->location (); }

const int
PMPackage::medianr() const { return _dataProvider->medianr (); }

const std::list<std::string>
PMPackage::keywords() const { return _dataProvider->keywords (); }

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

