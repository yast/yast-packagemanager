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

ostream & operator <<( ostream & str, const InstSrcManager::ISrcIdList & obj ) {
  str << '[' << endl;
  for ( InstSrcManager::ISrcIdList::const_iterator it = obj.begin(); it != obj.end(); ++it ) {
    str << "  " << *it << endl;
  }

  return str << ']';
}

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

/******************************************************************
 ******************************************************************/
int mmain( int argc, const char * argv[] );
int main( int argc, const char * argv[] ) {
  set_log_filename("-");
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

  //Y2PM::noAutoInstSrcManager();
  Y2PM::instSrcManager();

  newSrc( "/schnell/CD-ARCHIVE/8.2/SuSE-8.2-DVD-i386-RC2/CD1" );
  newSrc( "/schnell/CD-ARCHIVE/8.1/SuSE-8.1-DVD-i386-Int-RC5" );
  newSrc( "/schnell/CD-ARCHIVE/8.0/suse80-dvd-de-i386-RC4" );

  ISM.disableAllSources();
  return 0;

  MIL << "ISM.getSources " << ISM.getSources() << endl;
  MIL << "ISM.instOrderSources " << ISM.instOrderSources() << endl;

  InstSrcManager::InstOrder order;
  {
    InstSrcManager::ISrcIdList obj( ISM.getSources() );
    order.reserve( obj.size() );
    for ( InstSrcManager::ISrcIdList::const_iterator it = obj.begin(); it != obj.end(); ++it ) {
      DBG << (*it)->srcID() << endl;
      order.insert( order.begin(), (*it)->srcID() );
    }
  }

  ISM.setInstOrder( order );
  MIL << "ISM.instOrderSources " << ISM.instOrderSources() << endl;

  {
    InstSrcManager::ISrcIdList obj( ISM.getSources() );
    for ( InstSrcManager::ISrcIdList::const_iterator it = obj.begin(); it != obj.end(); ++it ) {
      DBG << (*it)->srcID() << " -> " << ISM.instOrderIndex( *it ) << endl;
    }
  }


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

