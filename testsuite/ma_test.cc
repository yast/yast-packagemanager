

#include <iomanip>
#include <list>
#include <string>

#include <y2util/Y2SLog.h>

#include <Y2PM.h>
#include <y2pm/RpmDb.h>

#include <y2pm/InstSrcManager.h>
#include <y2pm/InstSrc.h>
#include <y2pm/InstSrcDescr.h>
#include <y2pm/MediaAccess.h>

using namespace std;
#undef  Y2LOG
#define Y2LOG "PM_ma_test"

std::ostream & operator<<( std::ostream & str, const PkgDep::ResultList & lst ) {
  str << "+++ResultList+++" << endl;
  for ( PkgDep::ResultList::const_iterator it = lst.begin(); it != lst.end(); ++it ) {
    str << *it << endl;
  }
  str << "---ResultList---" << endl;
  return str;
}

std::ostream & operator<<( std::ostream & str, const PkgDep::ErrorResultList & lst ) {
  str << "+++ErrorResultList+++" << endl;
  for ( PkgDep::ErrorResultList::const_iterator it = lst.begin(); it != lst.end(); ++it ) {
    str << *it << endl;
  }
  str << "---ErrorResultList---" << endl;
  return str;
}

/*****************************************************************
 *****************************************************************/

inline string dec( unsigned i ) {
  static char b[5];
  sprintf( b, "%u", i );
  return b;
}

inline void On( string n ) {
  PMSelectablePtr s( Y2PM::packageManager()[n] );
  if ( s && s->user_set_install() ) {
    SEC << "ins: " << n << endl;
  }
}
inline void Off( string n ) {
  PMSelectablePtr s( Y2PM::packageManager()[n] );
  if ( s && s->user_set_delete() ) {
    SEC << "del: " << n << endl;
  }
}
inline PMError addSrc( const string & url_r ) {
  InstSrcManager::ISrcIdList idlist;
  PMError err = Y2PM::instSrcManager().scanMedia( idlist, url_r );
  return err;
}

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
  M__ << "Y2SLOG_DEBUG IS ON" << endl;
  MIL << "START" << endl;

  //Y2PM::noAutoInstSrcManager();
  PMPackageManager &   PMGR( Y2PM::packageManager() );
  PMSelectionManager & SMGR( Y2PM::selectionManager() );
  InstSrcManager &     MGR( Y2PM::instSrcManager() );
  InstTarget &         TMGR( Y2PM::instTarget(true,"/") );
  INT << "Total Packages "   << PMGR.size() << endl;
  INT << "Total Selections " << SMGR.size() << endl;
  SEC << "===============================================================" << endl;

  PMSelectablePtr tp = PMGR["3ddiag"];
  if ( !tp ) {
    ERR << "No testpkg " << tp << endl;
    return 0;
  }
  MIL << tp->user_set_install() << endl;
  MIL << tp<< endl;

  PkgDep::ResultList      good;
  PkgDep::ErrorResultList bad;
  INT << "Solve: " << PMGR.solveInstall( good, bad ) << endl;

  MIL << "Good: " << good << endl;
  MIL << "Bad: " << bad << endl;

  SEC << "END " << endl;
  MGR.disableAllSources();
  return 0;
}
