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

#include "PMCB.h"

using namespace std;

#define TMGR Y2PM::instTarget()
#define PMGR Y2PM::packageManager()
#define SMGR Y2PM::selectionManager()
#define ISM  Y2PM::instSrcManager()

struct NotifySelState : public PMSelectable::NotifySelState {
  virtual void operator()( PkgName name_r,
			   const SelState & old_r, const SelState & new_r ) {
    DBG << name_r << old_r << " -> " << new_r << endl;
  }
};

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
  set_log_filename( "-" );
  MIL << "START" << endl;

  SelState s;
  MIL << s << endl;

  NotifySelState n;

  PMSelectable t( PkgName("foo"), &n );
  t.user_set_delete();
  t.user_set_delete();
  t.auto_set_install();
  t.appl_set_install();
  t.user_set_taboo();
  t.appl_set_install();
  t.user_set_offSystem();

  SEC << "STOP" << endl;
  return 0;
}


