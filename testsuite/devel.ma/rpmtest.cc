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


void SBS( string name );

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


  INT << TMGR.getMountPoints() << endl;

  SEC << "STOP" << endl;
  return 0;
  if ( 0 ) {
  SEC << "--------------------------------------------------------------------" << endl;
  dumpSelWhatIf( DBG, true );
  SBS( "default" );
  dumpSelWhatIf( DBG );
  dumpPkgWhatIf( DBG );
  }
  SEC << "--------------------------------------------------------------------" << endl;
  SMGR["X11"]->user_set_delete();
  SMGR.activate (PMGR);
  dumpSelWhatIf( DBG );
  dumpPkgWhatIf( DBG );
  SEC << "--------------------------------------------------------------------" << endl;

  if ( 0 ) {
    SBS( "Minimal" );
    dumpSelWhatIf( DBG );
    dumpPkgWhatIf( DBG );
  }

  SEC << "STOP" << endl;
  return 0;
}

#undef Y2LOG
#define Y2LOG "SBS"

bool SetSelectionString (std::string name, bool recursive = false)
{
  PMSelectablePtr selectable = SMGR.getItem(name);
    if (selectable)
    {
      INT << "SetSelectionString " << name << " " << recursive << " " << selectable << endl;
	PMSelectionPtr selection = selectable->theObject();
	if (selection)
	{
	    if (!recursive && selection->isBase())
	    {
		MIL << "Changing base selection, re-setting manager" << endl;
		SMGR.setNothingSelected();
		PMGR.setNothingSelected();
	    }
	    else if (selectable->status() == PMSelectable::S_Install)
	    {
		DBG << "Don't recurse already selected." << endl;
		return true;
	    }
	}

	if (!selectable->user_set_install())
	{
	    ERR << name << "->user_set_install" << endl;
	    return false;
	}
	DBG << name << "->user_set_install" << endl;

	// RECURSION
	// select all recommended selections of a base selection

	if ( selection->isBase() )
	{
	    const std::list<std::string> recommends = selection->recommends();
	    MIL << "Base ! Selecting all required and recommends (" << recommends.size() << ")..." << endl;
	    for (std::list<std::string>::const_iterator it = recommends.begin();
		 it != recommends.end(); ++it)
	    {
		SetSelectionString (*it, true);
	    }
	    MIL << "DONE: Selecting all required and recommends." << endl;
	}

	MIL << "Solve Selections...." << endl;
	PkgDep::ResultList good;
	PkgDep::ErrorResultList bad;
	if ( !SMGR.solveInstall(good, bad) )
	{
	    ERR << bad.size() << " selections failed." << endl;
	    for (PkgDep::ErrorResultList::const_iterator p = bad.begin();
		 p != bad.end(); ++p )
	    {
		DBG << *p << std::endl;
	    }

	    return false;
	}
	return true;
    }
    WAR << "Unknown selection '" << name << "'" << endl;
    return false;
}


bool ActivateSelections ()
{
  MIL << "ActivateSelections..." << endl;
  SMGR.activate (PMGR);
  return true;
}


bool PkgSolve ()
{
  MIL << "Solve Packages..." << endl;
    bool filter_conflicts_with_installed = false;

    PkgDep::ResultList good;
    PkgDep::ErrorResultList bad;

    if (!PMGR.solveInstall(good, bad, filter_conflicts_with_installed))
    {
        unsigned _solve_errors = bad.size();
	ERR << bad.size() << " packages failed." << endl;
	for( PkgDep::ErrorResultList::const_iterator p = bad.begin();
	     p != bad.end(); ++p )
	{
	    DBG << *p << std::endl;
	}

	return false;
    }
    return true;
}

void SBS( string name )
{
  SEC << "SBS (" << name << ")..." << endl;
  PMSelectablePtr selectable = SMGR.getItem( name );
  if ( !selectable ) {
    ERR << "No Selection '" << name << "'" << endl;
    return;
  }
  PMSelectionPtr candidate = selectable->candidateObj();
  if ( !candidate || !candidate->isBase() ) {
    ERR << "No candidate or not base selection " << candidate << endl;
    return;
  }

  MIL << "ClearSelection..." << endl;
  SMGR.setNothingSelected();
  PMGR.setNothingSelected();


  MIL << "SetSelection '" << name << "'..." << endl;
  SetSelectionString ( name );

  ActivateSelections ();

  //PkgSolve();

  SEC << "DONE SBS (" << name << ")" << endl;
}
