#include <iomanip>
#include <fstream>
#include <string>
#include <list>

#include <Y2PM.h>
#include <y2pm/RpmDb.h>

#include <y2pm/InstSrcManager.h>
#include <y2pm/InstSrc.h>
#include <y2pm/InstSrcDescr.h>
#include <y2pm/MediaAccess.h>
#include <y2pm/PMPackageManager.h>
#include <y2pm/PMSelectionManager.h>
#include <y2pm/InstTarget.h>
#include <y2pm/Timecount.h>
#include <y2pm/PMPackageImEx.h>

#include <y2util/Y2SLog.h>
#include <y2util/Date.h>
#include <y2util/stringutil.h>
#include <y2util/ExternalProgram.h>

using namespace std;

#define TMGR Y2PM::instTarget()
#define PMGR Y2PM::packageManager()
#define SMGR Y2PM::selectionManager()
#define ISM  Y2PM::instSrcManager()

ostream & operator <<( ostream & str, const list<string> & t ) {
  stringutil::dumpOn( str, t, true );
  return str;
}

void dataDump( ostream & str, constPMPackagePtr p ) {
  str << p << endl;
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


class Logfile {
  Logfile( const Logfile & );
  Logfile & operator=( const Logfile & );
  private:
    static ofstream _log;
    static unsigned _refcnt;
    static Pathname _fname;
    static void openLog() {
      if ( !_fname.empty() ) {
	_log.clear();
	_log.open( _fname.asString().c_str(), std::ios::out|std::ios::app );
	if( !_log )
	  ERR << "Could not open logfile '" << _fname << "'" << endl;
      }
    }
    static void closeLog() {
      _log.clear();
      _log.close();
    }
    static void refUp() {
      if ( !_refcnt )
	openLog();
      ++_refcnt;
    }
    static void refDown() {
      --_refcnt;
      if ( !_refcnt )
	closeLog();
    }
  public:
    Logfile() { refUp(); }
    ~Logfile() { refDown(); }
    ostream & operator()() { return _log; }
    static void setFname( const Pathname & fname_r ) {
      if ( _refcnt )
	closeLog();
      _fname = fname_r;
      if ( _refcnt )
	openLog();
    }
};
Pathname Logfile::_fname;
ofstream Logfile::_log;
unsigned Logfile::_refcnt = 0;

/******************************************************************
**
**
**	FUNCTION NAME : main
**	FUNCTION TYPE : int
**
**	DESCRIPTION :
*/
int main()
{
  Y2Logging::setLogfileName("-");
  MIL << "START" << endl;

  {
    Logfile flog;
    flog() << "test1" << endl;
    flog.setFname( "." );
    flog() << "test2" << endl;
  }
  Logfile::setFname( "log_test" );
  {
    Logfile flog;
    flog() << "test3";
    {
      Logfile flog2;
      Logfile::setFname( "log_test2" );
      flog2() << "test4" << endl;
    }
    flog() << endl;
  }
  {
    Logfile::setFname( "" );
    Logfile flog;
    flog() << "not" << endl;
  }

  SEC << "STOP" << endl;
  return 0;









  //Y2PM::noAutoInstSrcManager();
  Timecount _t( "Launch InstTarget" );
  Y2PM::instTarget(true,"/");
  _t.start( "Launch PMPackageManager" );
  Y2PM::packageManager();
  _t.start( "Launch PMSelectionManager" );
  Y2PM::selectionManager();
  _t.start( "Launch InstSrcManager" );
  Y2PM::instSrcManager();
  _t.stop();
  INT << "Total Packages "   << PMGR.size() << endl;
  INT << "Total Selections " << SMGR.size() << endl;

  SEC << ISM.cacheCopyTo( "/tmp/mnt" ) << endl;

  SEC << "STOP" << endl;
  return 0;
}


