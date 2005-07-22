/*---------------------------------------------------------------------\
|                                                                      |
|                      __   __    ____ _____ ____                      |
|                      \ \ / /_ _/ ___|_   _|___ \                     |
|                       \ V / _` \___ \ | |   __) |                    |
|                        | | (_| |___) || |  / __/                     |
|                        |_|\__,_|____/ |_| |_____|                    |
|                                                                      |
|                               core system                            |
|                                         (C) SuSE Linux Products GmbH |
\----------------------------------------------------------------------/

  File:       YUMImpl.cc

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: YUM installation source implementation

/-*/

#include <iostream>
#include <sstream>

#include <y2util/Y2SLog.h>
#include <y2util/gzstream.h>
#include <y2util/stringutil.h>

#include "YUMImpl.h"
#include "YUMPackageDataProvider.h"

#include "Y2PM.h"

using namespace std;

///////////////////////////////////////////////////////////////////
namespace YUM
{ /////////////////////////////////////////////////////////////////

#warning FIX archCompat handling
  /******************************************************************
   **
   **	FUNCTION NAME : archCompat
   **	FUNCTION TYPE : set<PkgArch>
  */
  set<PkgArch> archCompat( const PkgArch & arch )
  {
    static map<PkgArch,string> compatmap;

    if ( compatmap.empty() ) {
#define OUTMAP(a,c) compatmap[PkgArch(a)] = c
      OUTMAP( "i386",	"noarch" );
      OUTMAP( "i486",	"i386 noarch" );
      OUTMAP( "i586",	"i486 i386 noarch" );
      OUTMAP( "i686",	"i586 i486 i386 noarch" );
      OUTMAP( "athlon",	"i686 i586 i486 i386 noarch" );
      OUTMAP( "x86_64",	"athlon i686 i586 i486 i386 noarch" );
#undef OUTMAP
    }

    set<PkgArch> ret;
    ret.insert( arch );
    ret.insert( PkgArch("noarch") );

    map<PkgArch,string>::const_iterator it = compatmap.find( arch );
    if ( it != compatmap.end() ) {
      MIL << "archCompat: " << arch << ": ";
      vector<string> archvec;
      stringutil::split( it->second, archvec );
      for ( unsigned i = 0; i < archvec.size(); ++i ) {
        ret.insert( PkgArch(archvec[i]) );
        MIL << archvec[i] << " ";
      }
      MIL << endl;
    }

    return ret;
  }

  /******************************************************************
   **
   **	FUNCTION NAME : YUM2PkgEdition
   **	FUNCTION TYPE : PkgEdition
   **/
  inline PkgEdition
  YUM2PkgEdition( const string & e_r,
                  const string & v_r, const string & r_r,
                  const string & bt_r = string() )
  {
    unsigned e = 0;
    stringutil::strtonum( e_r, e );
    time_t bt = 0;
    stringutil::strtonum( bt_r, bt );
    return PkgEdition( e, v_r, r_r, bt );
  }

  inline PkgEdition
  YUM2PkgEdition( const YUMDependency & ydep_r )
  { return YUM2PkgEdition( ydep_r.epoch, ydep_r.ver, ydep_r.rel ); }

  inline PkgEdition
  YUM2PkgEdition( constYUMPrimaryDataPtr ydat_r )
  { return YUM2PkgEdition( ydat_r->epoch, ydat_r->ver, ydat_r->rel, ydat_r->timeBuild ); }

  /******************************************************************
   **
   **	FUNCTION NAME : YUM2PkgRelation
   **	FUNCTION TYPE : PkgRelation
   **/
  inline PkgRelation
  YUM2PkgRelation( const YUMDependency & ydep_r )
  {
    PkgRelation ret;
    PkgName     n( ydep_r.name );
    rel_op      op = str2rel_op( ydep_r.flags );

    if ( op == NONE )
      ret = PkgRelation( n );
    else
      {
        PkgEdition ed( YUM2PkgEdition( ydep_r ) );
        ret = PkgRelation( n, op, ed );
      }

    if ( stringutil::strtonum<int>( ydep_r.pre ) )
      ret.setPreReq( true );

    return ret;
  }

  /******************************************************************
   **
   **	FUNCTION NAME : YUM2PkgRelList
   **	FUNCTION TYPE : PMSolvable::PkgRelList_type
  */
  inline PMSolvable::PkgRelList_type
  YUM2PkgRelList( const std::list<YUMDependency> & yumlist )
  {
    PMSolvable::PkgRelList_type rellist;

    for ( std::list<YUMDependency>::const_iterator it = yumlist.begin();
          it != yumlist.end(); ++it )
      {
        if ( it->name.empty() )
          continue;
        rellist.push_back( YUM2PkgRelation( *it ) );
      }

    return rellist;
  }

  ///////////////////////////////////////////////////////////////////
  //
  //	CLASS NAME : Impl
  //
  ///////////////////////////////////////////////////////////////////

  ///////////////////////////////////////////////////////////////////
  //
  //	METHOD NAME : Impl::Impl
  //	METHOD TYPE : Constructor
  //
  Impl::Impl( InstSrcDataYUM & parent, const Pathname & repoDir_r )
  : _parent( parent ), _repodata( repoDir_r )
  {
    Pathname primary( _repodata.primaryFile() );

    if ( primary.empty() )
      {
        WAR << "No 'primary' file" << endl;
      }
    else
      {
        ifgzstream pIn( primary.asString().c_str() );
        YUMPrimaryParser iter( pIn, "" );
        if ( ! scanPrimary( iter ) )
          {
            _packages.clear();
            WAR << "Failed to get 'primary' data from " << primary << endl;
          }
        else
          {
            MIL << "Got " << _packages.size() << " packages from " << primary << endl;
          }
      }
  }

  ///////////////////////////////////////////////////////////////////
  //
  //	METHOD NAME : Impl::dumpOn
  //	METHOD TYPE : std::ostream &
  //
  std::ostream & Impl::dumpOn( std::ostream & str ) const
  {
    Rep::dumpOn( str );
    return str;
  }

  ///////////////////////////////////////////////////////////////////
  //
  //	METHOD NAME : Impl::scanPrimary
  //	METHOD TYPE : bool
  //
  bool
  Impl::scanPrimary( YUMPrimaryParser & iter_r )
  {
    bool ret = true;
    set<PkgArch> compatArch( archCompat( Y2PM::baseArch() ) );

    unsigned pkgs = 0;
    for ( ; ! iter_r.atEnd(); ++iter_r )
      {
        ++pkgs;
        YUMPrimaryDataPtr pdata = *iter_r;

        if ( pdata->type != "rpm" )
          {
            DBG << pdata->name << ": Skip type '" << pdata->type << "'" << endl;
            continue;
          }

        PkgArch arch( pdata->arch );
        if ( compatArch.find( arch ) == compatArch.end() )
          {
            DBG << pdata->name << ": Drop incompatible arch '" << arch << "' (" << Y2PM::baseArch() << ")" << endl;
            continue;
          }

        // create dataprovider and package
        PkgName    name( pdata->name );
        PkgEdition edition( YUM2PkgEdition( pdata ) );

        PackageDataProviderPtr ndp = new PackageDataProvider( &_parent );
        PMPackagePtr nptr = new PMPackage( name, edition, arch, ndp );

        // add PMSolvable data to package
        PMSolvable::PkgRelList_type rellist;
        rellist = YUM2PkgRelList( pdata->provides );

#warning Filtering splitprovides is not job of InstSrc! Solvable itself should handle this.
        PMSolvable::PkgRelList_type oprovides( rellist );
        for ( PMSolvable::PkgRelList_type::iterator it = oprovides.begin(); it != oprovides.end(); /*advance inside*/ )
          {
            if ( it->op() == NONE )
              {
                PkgSplit testsplit( it->name(), /*quiet*/true );
                if ( testsplit.valid() )
                  {
                    ndp->_attr_SPLITPROVIDES.insert( testsplit );
                    it = oprovides.erase( it );
                    continue;
                  }
              }
            ++it;
          }
        nptr->setProvides ( oprovides );

        rellist = YUM2PkgRelList( pdata->requires );
        nptr->setRequires( rellist );

        rellist = YUM2PkgRelList( pdata->conflicts );
        nptr->setConflicts( rellist );

        rellist = YUM2PkgRelList( pdata->obsoletes );
        nptr->setObsoletes( rellist );

        // minor data
        ndp->_attr_SUMMARY = pdata->summary;
        ndp->_attr_DESCRIPTION = stringutil::splitToLines( pdata->description );

        _packages.push_back( nptr );
      }

    if ( iter_r.errorStatus() )
      {
        ERR << iter_r.errorStatus()->msg() << " parsing 'primary' data" << endl;
        ret = false;
      }
    else
      {
        DBG << "Found " << _packages.size() << " packages" << endl;
      }
    return ret;
  }

  /////////////////////////////////////////////////////////////////
} // namespace YUM
///////////////////////////////////////////////////////////////////
