#include <iomanip>
#include <list>
#include <string>

#include <y2util/Y2SLog.h>
#include <y2util/Date.h>
#include <y2util/ExternalProgram.h>

using namespace std;

static const string Q_A( "%{NAME} %{VERSION} %{RELEASE} %{ARCH}\n" );
static const string Q_F( "%{NAME} %{VERSION} %{RELEASE} %{ARCH} [%{FILEDEVICES} %{FILEINODES} %{FILESIZES} %{FILENAMES}\n]\n" );

int runQuery( const string & query_r )
{
  Date ts = Date::now();
  const char* args[] = { "rpm", "-q", "-a", "--qf", query_r.c_str(), 0 };
  ExternalProgram cmd( args, ExternalProgram::Discard_Stderr );

  list<string> result;
  for ( string line = cmd.receiveLine(); line.size(); line = cmd.receiveLine() ) {
    result.push_back( line );
  }

  int ret = cmd.close();
  Date te = Date::now();
  MIL << (te-ts) << " return " << ret << "(" << result.size() << ") for " << query_r << endl;
  return ret;
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
  SEC << "START" << endl;
  runQuery( Q_A );
  runQuery( Q_F );
  SEC << "STOP" << endl;
  return 0;
}

