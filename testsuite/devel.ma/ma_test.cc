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
#include <y2pm/PMLanguageManager.h>
#include <y2pm/PMLanguage.h>
#include <y2pm/InstTarget.h>
#include <y2pm/Timecount.h>
#include <y2pm/PMPackageImEx.h>
#include <y2pm/InstTargetSelDB.h>
#include <y2pm/RpmHeaderCache.h>
#include <y2pm/RpmHeader.h>

#include "PMCB.h"

using namespace std;

#define TMGR Y2PM::instTarget()
#define PMGR Y2PM::packageManager()
#define SMGR Y2PM::selectionManager()
#define LMGR Y2PM::languageManager()
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
      (*it)->dumpStateOn( str ) << " " << *it << endl;
    }
  }
  str << "---[dumpSelWhatIf]---------------------------------------------------" << endl;
  return str;
}

ostream & dumpLangWhatIf( ostream & str, bool all = false  )
{
  str << "+++[dumpLangWhatIf]+++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
  for ( PMManager::PMSelectableVec::const_iterator it = LMGR.begin(); it != LMGR.end(); ++it ) {
    if ( all || (*it)->to_modify() ) {
      (*it)->dumpStateOn( str ) << " " << *it << endl
	<< (*it)->name() << " " << (*it)->theObject()->summary() << endl;
    }
  }
  str << "---[dumpLangWhatIf]---------------------------------------------------" << endl;
  return str;
}

bool doSolve( PMManager & mgr_r ) {
  PkgDep::ResultList good;
  PkgDep::ErrorResultList bad;
  if ( ! mgr_r.solveInstall( good, bad ) ) {
    WAR << "SOLVE: failed packages: " << bad.size() << endl;
    WAR << bad << endl;
    //for( PkgDep::ErrorResultList::const_iterator p = bad.begin();
//	 p != bad.end(); ++p ) {
  //    out << *p << endl;
    //}
    return false;
  }
  return true;
}

ostream & dump( ostream & str, const Url & url ) {
  str << url << endl;
  str << url.isValid() << " - " << url.saveAsString() << endl;
  str << "  " << "prot :" << url.protocolString() << ":" << endl;
  str << "  " << "user :" << url.username() << ":" << endl;
  str << "  " << "pass :" << url.password() << ":" << endl;
  str << "  " << "host :" << url.host() << ":" << endl;
  str << "  " << "port :" << url.port() << ":" << endl;
  str << "  " << "path :" << url.path() << ":" << endl;
  return str;
}

#include <y2pm/InstYou.h>
#include <y2pm/PMYouPatchManager.h>

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

  if ( 1 ) {
    //Y2PM::setNotRunningFromSystem();
    //Y2PM::setCacheToRamdisk( false );
    //Y2PM::noAutoInstSrcManager();
    Timecount _t("",false);
    _t.start( "Launch InstTarget" );
    Y2PM::instTargetInit("/");
    _t.start( "Launch PMLanguageManager" );
    Y2PM::languageManager();
    _t.start( "Launch PMPackageManager" );
    Y2PM::packageManager();
    _t.start( "Launch PMSelectionManager" );
    Y2PM::selectionManager();
    _t.start( "Launch InstSrcManager" );
    Y2PM::instSrcManager();
    _t.stop();
    INT << "Total Packages   " << PMGR.size() << endl;
    INT << "Total Selections " << SMGR.size() << endl;
    INT << "Total Languages  " << LMGR.size() << endl;
  }

#if 0
  dumpLangWhatIf( SEC, true );
  doSolve( PMGR );

  LMGR["de_DE"]->user_set_offSystem();
  LMGR["af"]->user_set_onSystem();
  doSolve( PMGR );

  PkgSelectables pkgs( LMGR.getLangPackagesFor( "af" ) );


  dumpPkgWhatIf( INT );
#endif

  SEC << "STOP" << endl;
  return 0;
}

