#include <iomanip>
#include <fstream>
#include <string>
#include <list>
#include <set>

#include <y2util/Y2SLog.h>
#include <y2util/Date.h>
#include <y2util/stringutil.h>
#include <y2util/ExternalProgram.h>

#include <Y2PM.h>
#include <y2pm/RpmDb.h>
#include <y2pm/librpmDb.h>

#include <y2pm/InstSrcManager.h>
#include <y2pm/InstSrc.h>
#include <y2pm/InstSrcDescr.h>
#include <y2pm/MediaAccess.h>
#include <y2pm/PMPackageManager.h>
#include <y2pm/PMSelectionManager.h>
#include <y2pm/InstTarget.h>
#include <y2pm/Timecount.h>
#include <y2pm/PMPackageImEx.h>

#include "PMCB.h"

using namespace std;

#define TMGR Y2PM::instTarget()
#define PMGR Y2PM::packageManager()
#define SMGR Y2PM::selectionManager()
#define ISM  Y2PM::instSrcManager()

ostream & operator<<( ostream & str, const PathInfo::direntry & obj ) {
  str << obj.name << "\t" << obj.type;
  return str;
}

template<typename _Ct>
ostream & operator<<( ostream & str, const list<_Ct> & obj ) {
  str << "[" << obj.size() << "]{";
  for ( typename list<_Ct>::const_iterator it = obj.begin(); it != obj.end(); ++it ) {
    if ( it == obj.begin() )
      str << endl;
    str << "  " << *it << endl;
  }
  return str << '}';
}

template<typename _Ct, class Compare>
ostream & operator<<( ostream & str, const set<_Ct, Compare> & obj ) {
  str << "[" << obj.size() << "]{";
  for ( typename set<_Ct, Compare>::const_iterator it = obj.begin(); it != obj.end(); ++it ) {
    if ( it == obj.begin() )
      str << endl;
    str << "  " << *it << endl;
  }
  return str << '}';
}


void dataDump( ostream & str, constPMPackagePtr p ) {
  str << p << " ++++++++++++++++++++++++++++" << endl;
  str << "SUMMARY:       " << p->summary() << endl;
  str << "DESCRIPTION:   " << p->description() << endl;
  str << "INSNOTIFY:     " << p->insnotify() << endl;
  str << "DELNOTIFY:     " << p->delnotify() << endl;
  str << "SIZE:          " << p->size() << endl;
  str << "INSTSRCLABEL:  " << p->instSrcLabel() << endl;
  str << "INSTSRCVENDOR: " << p->instSrcVendor() << endl;
  str << "INSTSRCRANK:   " << p->instSrcRank() << endl;
  str << "BUILDTIME:     " << p->buildtime() << endl;
  str << "BUILDHOST:     " << p->buildhost() << endl;
  str << "INSTALLTIME:   " << p->installtime() << endl;
  str << "DISTRIBUTION:  " << p->distribution() << endl;
  str << "VENDOR:        " << p->vendor() << endl;
  str << "LICENSE:       " << p->license() << endl;
  str << "PACKAGER:      " << p->packager() << endl;
  str << "GROUP:         " << p->group() << endl;
  str << "CHANGELOG:     " << p->changelog() << endl;
  str << "URL:           " << p->url() << endl;
  str << "OS:            " << p->os() << endl;
  str << "PREIN:         " << p->prein() << endl;
  str << "POSTIN:        " << p->postin() << endl;
  str << "PREUN:         " << p->preun() << endl;
  str << "POSTUN:        " << p->postun() << endl;
  str << "SOURCELOC:     " << p->sourceloc() << endl;
  str << "SOURCESIZE:    " << p->sourcesize() << endl;
  str << "ARCHIVESIZE:   " << p->archivesize() << endl;
  str << "AUTHORS:       " << p->authors() << endl;
  str << "FILENAMES:     " << p->filenames() << endl;
  str << "RECOMMENDS:    " << p->recommends() << endl;
  str << "SUGGESTS:      " << p->suggests() << endl;
  str << "LOCATION:      " << p->location() << endl;
  str << "MEDIANR:       " << p->medianr() << endl;
  str << "KEYWORDS:      " << p->keywords() << endl;
  str << "INSTSOURCE:    " << p->source() << endl;
  str << "IS REMOTE:     " << p->isRemote() << endl;
  str << "PROVIDES:      " << p->provides().size() << endl;
  str << "REQUIRES:      " << p->requires().size() << endl;
  str << "CONFLICTS:     " << p->conflicts().size() << endl;
  str << "OBSOLETES:     " << p->obsoletes().size() << endl;
  str << "PREREQUIRES:   " << p->prerequires().size() << endl;
  str << "SPLITPROVIDES: " << p->splitprovides().size() << endl;
  str << "---------------" << endl;
}

void dummyDU()
{
  std::set<PkgDuMaster::MountPoint> mountpoints;
  mountpoints.insert( PkgDuMaster::MountPoint( "/",          FSize(4,FSize::K), FSize(1,FSize::G) ) );
  mountpoints.insert( PkgDuMaster::MountPoint( "/boot",      FSize(4,FSize::K), FSize(1,FSize::G) ) );
  mountpoints.insert( PkgDuMaster::MountPoint( "/bin",       FSize(4,FSize::K), FSize(1,FSize::G) ) );
  mountpoints.insert( PkgDuMaster::MountPoint( "/etc",       FSize(4,FSize::K), FSize(1,FSize::G) ) );
  mountpoints.insert( PkgDuMaster::MountPoint( "/lib",       FSize(4,FSize::K), FSize(1,FSize::G) ) );
  mountpoints.insert( PkgDuMaster::MountPoint( "/opt",       FSize(4,FSize::K), FSize(1,FSize::G) ) );
  mountpoints.insert( PkgDuMaster::MountPoint( "/sbin",      FSize(4,FSize::K), FSize(1,FSize::G) ) );
  mountpoints.insert( PkgDuMaster::MountPoint( "/usr",       FSize(4,FSize::K), FSize(1,FSize::G) ) );
  mountpoints.insert( PkgDuMaster::MountPoint( "/usr/local", FSize(4,FSize::K), FSize(1,FSize::G) ) );
  mountpoints.insert( PkgDuMaster::MountPoint( "/var",       FSize(4,FSize::K), FSize(1,FSize::G) ) );
  MIL << mountpoints;
  PMGR.setMountPoints( mountpoints );
}

InstSrcManager::ISrcId newSrc( const string & url_r ) {
  Url url( url_r );
  InstSrcManager::ISrcIdList idlist;
  Timecount _t( url_r.c_str() );
  PMError err = ISM.scanMedia( idlist, url_r );
  ( err ? ERR : MIL ) << "newSrc: " << idlist.size() << " (" << err << ")" << endl;
  return( idlist.size() ? *idlist.begin() : 0 );
}

ostream & dumpPkgWhatIf( ostream & str, bool all = false )
{
  str << "+++[dumpPkgWhatIf]+++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
  for ( PMManager::PMSelectableVec::const_iterator it = PMGR.begin(); it != PMGR.end(); ++it ) {
    if ( all || (*it)->to_modify() || (*it)->is_taboo() ) {
      (*it)->dumpStateOn( str ) << endl;
    }
  }
  str << "---[dumpPkgWhatIf]---------------------------------------------------" << endl;
  return str;
}

ostream & dumpSelWhatIf( ostream & str, bool all = false  )
{
  str << "+++[dumpSelWhatIf]+++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
  for ( PMManager::PMSelectableVec::const_iterator it = SMGR.begin(); it != SMGR.end(); ++it ) {
    if ( all || (*it)->to_modify() ) {
      (*it)->dumpStateOn( str ) << endl;
    }
  }
  str << "---[dumpSelWhatIf]---------------------------------------------------" << endl;
  return str;
}

void WAIT() {
  INT << "WAIT FOR INPUT...." << endl;
  cout << endl << "GET INPUT: " << flush;
  string s;
  cin >> s;
}


InstSrcDescrPtr mkddesc( const ProductIdent & p, const ProductIdent & b ) {
  InstSrcDescrPtr ret = new InstSrcDescr;
  ret->set_content_product( p );
  ret->set_content_baseproduct( b );
  return ret;
}

constInstSrcDescrPtr getChild( list<constInstSrcDescrPtr> & src_r, list<constInstSrcDescrPtr> & dst_r,
			       constInstSrcDescrPtr base_r ) {
  for ( list<constInstSrcDescrPtr>::iterator it = src_r.begin(); it != src_r.end(); /*++ in loop*/ ) {
    if ( (*it)->hasBaseProduct( base_r ) ) {
      constInstSrcDescrPtr ret = *it;
      dst_r.splice( dst_r.end(), src_r, it );
      return ret;
    } else {
      ++it;
    }
  }
  return 0;
}

void getTree( list<constInstSrcDescrPtr> & src_r, list<constInstSrcDescrPtr> & dst_r,
	      constInstSrcDescrPtr base_r )
{
  constInstSrcDescrPtr got;
  while ( (got = getChild( src_r, dst_r, base_r )) ) {
    getTree( src_r, dst_r, got );
  }
}


list<constInstSrcDescrPtr> sortProdlist( const list<constInstSrcDescrPtr> & _prodlist ) {
  list<constInstSrcDescrPtr> _sortedProdlist;
  /////////////////////////////////////////////////////////////////////////
  list<constInstSrcDescrPtr> scrlist = _prodlist;
  list<constInstSrcDescrPtr> dstlist;
  scrlist.reverse();

  MIL << "SRC   " << scrlist << endl;
  MIL << "RESLT " << dstlist << endl;
  INT << "===================================================" << endl;

  getTree( scrlist, dstlist, 0 );
  MIL << "SRC   " << scrlist << endl;
  MIL << "RESLT " << dstlist << endl;
  INT << "===================================================" << endl;
  getTree( scrlist, dstlist, 0 );
  MIL << "SRC   " << scrlist << endl;
  MIL << "RESLT " << dstlist << endl;
  INT << "===================================================" << endl;

  dstlist.reverse();
  if ( scrlist.size() ) {
    WAR << "Found products without existing baseproduct: " << scrlist << endl;

    dstlist.splice( dstlist.end(), scrlist );
  }

  _sortedProdlist.swap( dstlist );
  /////////////////////////////////////////////////////////////////////////
  return _sortedProdlist;
}

void xx() {
  PkgNameEd noex ( PkgName("noex"),   PkgEdition("dsf") );

  PkgNameEd none ( PkgName("foo"),    PkgEdition("") );
  PkgNameEd core ( PkgName("core"),   PkgEdition("9") );
  PkgNameEd core8( PkgName("core"),   PkgEdition("8") );
  PkgNameEd sls8 ( PkgName("Sles"),   PkgEdition("8") );
  PkgNameEd sls9 ( PkgName("sles"),   PkgEdition("9") );
  PkgNameEd sls93( PkgName("slesSP"), PkgEdition("3") );
  PkgNameEd sld  ( PkgName("sld"),    PkgEdition("1-1") );

  PkgNameEd foo   ( PkgName("foo"),   PkgEdition("1") );
  PkgNameEd dummy ( PkgName("dummy"), PkgEdition("2") );
  PkgNameEd dumm  ( PkgName("noba"),  PkgEdition("99") );


  WAR << TMGR.installProduct( mkddesc( sls8, core8 ) ) << endl;
  INT << TMGR.getProducts() << endl;
  WAR << TMGR.installProduct( mkddesc( sls9, core ) ) << endl;
  INT << TMGR.getProducts() << endl;
  WAR << TMGR.removeProduct( mkddesc( sls9, core ) ) << endl;
  INT << TMGR.getProducts() << endl;
  return;
  list<constInstSrcDescrPtr> prods;

  prods.push_back( mkddesc( sls93,sls9 ) );
  prods.push_back( mkddesc( sld , core ) );
  prods.push_back( mkddesc( dumm, noex ) );
  prods.push_back( mkddesc( sls9, core ) );
  prods.push_back( mkddesc( sls8, core8 ) );
  prods.push_back( mkddesc( dummy, foo ) );
  prods.push_back( mkddesc( core, none ) );
  prods.push_back( mkddesc( foo,  none ) );

  SEC << prods << endl;
  SEC << sortProdlist( prods ) << endl;
}

struct PMSelectionByOrder : public binary_function<PMSelectablePtr, PMSelectablePtr, bool>
{
  bool operator()( const PMSelectablePtr & x, const PMSelectablePtr & y ) {
    return( PMSelection::compareByOrder( x->theObject(), y->theObject() ) < 0 );
  }
};

void pcandlog( constPMSelectablePtr p ) {
  INT << constPMPackagePtr( p->candidateObj() )->source() << endl;
  INT << "   " << p->candidateObj() << endl;
  for ( PMSelectable::PMObjectList::const_iterator it = p->av_begin();
	it != p->av_end(); ++it ) {
    INT << "      " << *it << endl;
  }
}

/******************************************************************
**
**
**	FUNCTION NAME : main
**	FUNCTION TYPE : int
**
**	DESCRIPTION :
*/
int main( int argc, char * argv[] )
{
  y2error( "xxx" );
  set_log_filename( "-" );
  MIL << "START (" << argc << ")" << endl;

  if ( 0 ) {
    //Y2PM::setNotRunningFromSystem();
    //Y2PM::setCacheToRamdisk( false );
    //Y2PM::noAutoInstSrcManager();
    Timecount _t("",false);
    _t.start( "Launch InstTarget" );
    Y2PM::instTargetInit("/");
    _t.start( "Launch PMPackageManager" );
    Y2PM::packageManager();
    _t.start( "Launch PMSelectionManager" );
    Y2PM::selectionManager();
    _t.start( "Launch InstSrcManager" );
    Y2PM::instSrcManager();
    _t.stop();
    INT << "Total Packages "   << PMGR.size() << endl;
    INT << "Total Selections " << SMGR.size() << endl;
  }

  Y2PM::packageManager();
  Y2PM::selectionManager();

  Pathname localp;
  Y2PM::instTargetInit("/");
  Y2PM::instSrcManager();

  pcandlog( PMGR["3ddiag"] );
  pcandlog( PMGR["rpm"] );


#if 0
  Y2PM::setNotRunningFromSystem();

  PMPackagePtr pkg ( PMGR["3ddiag"]->candidateObj() );
  INT << pkg->providePkgToInstall( localp ) << endl;
  INT << localp << endl;

  pkg = PMGR["rpm"]->candidateObj();
  INT << pkg->providePkgToInstall( localp ) << endl;
  INT << localp << endl;



#endif
  ISM.disableAllSources();
  Y2PM::instTargetClose();

#if 0
  PMError err;
  //Url mediaurl_r( "ftp://schnell/CD-ARCHIVE/9.0/SuSE-9.0-FTP-i386-RC1" );
  //Url mediaurl_r( "ftp://machcd2/CDs/SuSE-9.1-DVD-i386-RC2/CD1" );
  Url mediaurl_r( "/tmp/isrc" );
  MediaAccessPtr  media = new MediaAccess;
  if ( (err = media->open( mediaurl_r )) ) {
    ERR << "Failed to open " << mediaurl_r << " " << err << endl;
    return err;
  }
  if ( (err = media->attach()) ) {
    ERR << "Failed to attach media: " << err << endl;
    return err;
  }

  INT << media << endl;

  PathInfo::dircontent content;
  if ( (err = media->dirInfo( content, "" )) ) {
    ERR << "Failed dirInfo: " << err << endl;
    return err;
  }

  MIL << content << endl;

  if ( (err = media->provideDir( "" )) ) {
    ERR << "Failed to provideDir: " << err << endl;
    return err;
  }

  //if ( (err = media->provideFile( "media.1/media" )) ) {
  //  ERR << "Failed to provideFile: " << err << endl;
  //}
  int i;
  cout << "get:" << endl;
  cin >> i;
#endif

#if 0
  MIL << "=========================" << endl;
  MIL << Y2PM::getRequestedLocales() << endl;
  MIL << "=========================" << endl;
  MIL << Y2PM::getRequestedLocales() << endl;
  Y2PM::setRequestedLocales( LangCode("de_AT") );
  MIL << Y2PM::getRequestedLocales() << endl;
  Y2PM::addRequestedLocales( LangCode("de_DE@euro") );
  MIL << Y2PM::getRequestedLocales() << endl;
  Y2PM::delRequestedLocales( LangCode("de_AT") );
  MIL << Y2PM::getRequestedLocales() << endl;
  Y2PM::setRequestedLocales( LangCode("de_DE@euro") );
  MIL << Y2PM::getRequestedLocales() << endl;
#endif

  SEC << "STOP" << endl;
  return 0;
}

