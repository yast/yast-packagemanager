extern "C" {
#include <rpm/rpmlib.h>
#include <rpm/rpmdb.h>
#include <rpm/rpmmacro.h>
extern int _hdr_debug;
}
#include <iomanip>
#include <fstream>
#include <string>
#include <list>

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

//#include <PkgModuleFunctions.h>

#include "PMCB.h"

using namespace std;

#define TMGR Y2PM::instTarget()
#define PMGR Y2PM::packageManager()
#define SMGR Y2PM::selectionManager()
#define ISM  Y2PM::instSrcManager()

ostream & operator <<( ostream & str, const list<string> & t ) {
  stringutil::dumpOn( str, t, true );
  return str;
}

ostream & operator<<( ostream & str, const PkgSplitSet & obj ) {
  for ( PkgSplitSet::const_iterator it = obj.begin(); it != obj.end(); ++it ) {
    str << *it << endl;
  }
  return str;
}

/******************************************************************
 ******************************************************************/
struct MediaChangeReceive : public InstSrcManagerCallbacks::MediaChangeCallback
{
  virtual bool isSet() {
    return true;
  }
  virtual string changeMedia( constInstSrcPtr instSrc,
			      const string & error,
			      const string & url,
			      const string & product,
			      int current,
			      int expected ) {
    string ret = MediaChangeCallback::changeMedia( instSrc, error, url, product, current, expected );
    ret = "C";
    INT << "MediaChange return '" << ret << "' on " << error << " exp " << expected << " (" << current << ")" << endl;
    return ret;
  }
};
static MediaChangeReceive mediaChangeReceive ;
/******************************************************************
 ******************************************************************/
struct CommitProvideReceive : public Y2PMCallbacks::CommitProvideCallback
{
  unsigned _attempt;
  string _name;
  bool   _isRemote;

  virtual void reportbegin() {
    SEC << "+++" << endl;
    _attempt = 0;
  }
  virtual void reportend()   {
    SEC << "---" << endl;
  }
  virtual void start( constPMPackagePtr pkg, bool sourcepkg ) {
    // remember values to send on attempt
    _isRemote = pkg->isRemote();
    if ( sourcepkg ) {
      _name = pkg->nameEd() + ".src";
    } else {
      _name = pkg->nameEdArch();
    }
  }
  virtual CBSuggest attempt( unsigned cnt ) {
    _attempt = cnt;
    CBSuggest ret = CommitProvideCallback::attempt( cnt );
    INT << "CommitProvide attempt " << _name << "(" << cnt << ") retrun " << ret << endl;
    return ret;
  }
  virtual CBSuggest result( PMError error, const Pathname & localpath ) {
    CBSuggest ret = CommitProvideCallback::result( error, localpath );
    if ( _attempt == 1 )
      ret = CBSuggest::RETRY;
    INT << "CommitProvide result " << _name << "(" << error << ") retrun " << ret << endl;
    return ret;
  }
  virtual void stop( PMError error, const Pathname & localpath ) {
  }
};
static CommitProvideReceive commitProvideReceive;
/******************************************************************
 ******************************************************************/
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

/******************************************************************
 ******************************************************************/

string s;

Pathname root( "/tmp/rpmdbs" );
//Pathname dbPath( "/fibo" );
Pathname dbPath( "/mand" );
Pathname db3path( root+dbPath+"packages.rpm" );

static void instPkgCb( int pc, void * )
{
  INT << "  at " << pc << "%" << endl;
}

void Pdb( ) {
  {
    librpmDb::db_const_iterator it;
    unsigned cnt = 0;
    for ( ; *it; ++it ) {
      ++cnt;
    }
    SEC << "db_const_iterator: " << cnt << " " << it.dbError() << endl;
  }
}

void xx( RpmDb & db ) {
  set<string> known_pubkeys;
  set<PkgEdition> pubkeys( db.pubkeys() );
  for( set<PkgEdition>::const_iterator it = pubkeys.begin(); it != pubkeys.end(); ++it ) {
    known_pubkeys.insert( (*it).asString() );
  }
  DBG << "--pubkeys----" << endl;
  for ( set<string>::const_iterator it = known_pubkeys.begin(); it != known_pubkeys.end(); ++it ) {
    DBG << *it << endl;
  }
  DBG << "-------------" << endl;
}

extern void TcommitCkeckMediaGpg( PMPackagePtr pkg_r );

/******************************************************************
 ******************************************************************/
int mmain( int argc, const char * argv[] );
int main( int argc, const char * argv[] ) {
  Y2Logging::setLogfileName("-");
  SEC << "START" << endl;
  int ret = 0;

  if ( 0 ) {
    Y2PM::noAutoInstSrcManager();
    Timecount _t( "Launch InstTarget" );
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

  #define WHICH(err) (err?ERR:INT)

  PMError err;
  Url mediaurl( "cd:///" );
  MediaAccessPtr media( new MediaAccess );

  err = media->open( mediaurl );
  WHICH(err) << "open media:" << mediaurl << " " << err << endl;

  err = media->attach();
  WHICH(err) << "attach media: " << err << endl;

  err = media->release();
  WHICH(err) << "release media: " << err << endl;

  err = media->release( true );
  WHICH(err) << "release and eject media: " << err << endl;


#if 0
  set<string> allpks;
  //allpks.insert( "aaa_base" );
  allpks.insert( "test" );
  allpks.insert( "test1" );
  allpks.insert( "test2" );
  allpks.insert( "test3" );
  allpks.insert( "test11" );
  allpks.insert( "test22" );
  allpks.insert( "test33" );
  allpks.insert( "test111" );
  allpks.insert( "test222" );
  allpks.insert( "test333" );
#define forall for ( set<string>::const_iterator it = allpks.begin(); it != allpks.end(); ++it )

  Y2PM::instSrcManager();
  forall {
    DBG << "  " << PMGR[*it]->user_set_install() << endl;
    DBG << "  " << PMGR[*it]->providesSources() << endl;
    DBG << "  " << PMGR[*it]->set_source_install( true ) << endl;
    SEC << PMGR[*it] << endl;
  }

  Y2PM::instTargetUpdate();
  forall {
    //DBG << "  " << PMGR[*it]->user_set_install() << endl;
    SEC << PMGR[*it] << endl;
  }

  InstSrcManagerCallbacks::mediaChangeReport.redirectTo( mediaChangeReceive );

  Y2PMCallbacks::commitProvideReport.redirectTo( commitProvideReceive );
  InstSrcManagerCallbacks::mediaChangeReport.redirectTo( mediaChangeReceive );

  forall {
    SEC << PMGR[*it] << endl;
  }

  list<string> errors;
  list<string> remaining;
  list<string> srcremaining;
  Y2PM::commitPackages( 0, errors, remaining, srcremaining );
#endif

  SEC << "STOP -> " << ret << endl;
  return ret;
}

/******************************************************************
**
**
**	FUNCTION NAME : mmain
**	FUNCTION TYPE : int
**
**	DESCRIPTION :
*/
int mmain( int argc, const char * argv[] )
{
  _rpmdb_debug = 0;
  _hdr_debug = 0;


  return 0;
}

