/*---------------------------------------------------------------------\
|                                                                      |
|                      __   __    ____ _____ ____                      |
|                      \ \ / /_ _/ ___|_   _|___ \                     |
|                       \ V / _` \___ \ | |   __) |                    |
|                        | | (_| |___) || |  / __/                     |
|                        |_|\__,_|____/ |_| |_____|                    |
|                                                                      |
|                            Package Management                        |
|                                                     (C) 2002 SuSE AG |
\----------------------------------------------------------------------/

   File:       RpmDb.cc
   Purpose:    Interface to installed RPM system
   Author:     Stefan Schubert <schubi@suse.de>
   Maintainer: Ludwig Nussel <lnussel@suse.de>

   Copied and adapted from agent-targetpkg

/-*/

#include <sys/stat.h>
#include <unistd.h>

#include <cstdlib>

#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <vector>

#include <y2util/Date.h>
#include <y2util/FSize.h>
#include <y2util/Y2SLog.h>
#include <y2util/TagParser.h>
#include <y2util/Pathname.h>
#include <y2util/PathInfo.h>
#include <y2util/ExternalDataSource.h>
#include <y2util/diff.h>

#include <y2pm/RpmDb.h>
#include <y2pm/PkgEdition.h>
#include <y2pm/PkgRelation.h>
#include <y2pm/PMPackage.h>
#include <y2pm/PMRpmPackageDataProvider.h>
#include <y2pm/PMRpmPackageDataProviderPtr.h>

#include <Y2PM.h>

#ifndef _
#define _(X) X
#endif

using namespace std;

IMPL_BASE_POINTER(RpmDb);

#define FAILIFNOTINITIALIZED \
	if(_old_present) return Error::E_RpmDB_old_db; \
	if(!_initialized) return Error::E_RpmDB_not_initialized;

#define WARNINGMAILPATH "/var/adm/notify/warnings"

/****************************************************************/
/* public member-functions					*/
/****************************************************************/

/*-------------------------------------------------------------*/
/* creates a RpmDb					       */
/*-------------------------------------------------------------*/
RpmDb::RpmDb() :
    _progressfunc(NULL),
    _progressdata(NULL),
    _rootdir("/"),
    _varlibrpm("/var/lib/rpm"),
    _varlib("/var/lib"),
    _rpmdbname("packages.rpm"),
    _creatednew(false),
    _old_present(false),
    _initialized(false)
{
   process = 0;
   exit_code = -1;
   temporary = false;
   dbPath = "";
   //XXX dunno, copied from old yast2 installer
   setenv( "RPM_IgnoreFailedSymlinks", "1", 1 );
}

/*--------------------------------------------------------------*/
/* Cleans up						       	*/
/*--------------------------------------------------------------*/
RpmDb::~RpmDb()
{
   M__ << "~RpmDb()" << endl;

   if ( process )
      delete process;

   process = NULL;
// only needed with createTmpDatabase
#if 0
   if ( temporary )
   {
      // Removing all files of the temporary DB

      string command = "rm -R ";

//XXX system
      command += dbPath.asString();
      ::system ( command.c_str() );
   }
#endif

   M__  << "~RpmDb() end" << endl;
}


/*--------------------------------------------------------------*/
/* Initialize the rpm database					*/
/* If Flag "createNew" is set, than it will be created, if not	*/
/* exist --> returns DbNewCreated if successfully created 	*/
/*--------------------------------------------------------------*/
PMError
RpmDb::initDatabase( string name_of_root, bool createNew )
{
    Pathname     dbFilename;
    struct stat  dummyStat;
    PMError	 dbStatus = Error::E_ok;

    DBG << "calling initDatabase" << endl;

    _rootdir = name_of_root;
    dbFilename += _rootdir + _varlibrpm + _rpmdbname;
    if (  stat( dbFilename.asString().c_str(), &dummyStat ) != -1 )
    {
	// DB found
	dbPath = _varlibrpm;
	_initialized = true;
	DBG << "Setting dbPath to " << dbPath.asString() << endl;
    }
    else if ( createNew )
    {
	DBG << "creating new database" << endl;
	// New rpm-DB will be created
	if(!PathInfo::assert_dir(dbFilename.dirname()))
	{
	    dbPath = _varlibrpm;

	    RpmArgVec opts(1);
	    opts[0] = "--initdb";

	    run_rpm (opts);

	    string rpmerrormsg, str;
	    while (systemReadLine (str))
	    {
		rpmerrormsg+=str;
	    }
	    if ( systemStatus() != 0 )
	    {
		// error
		dbStatus = Error::E_RpmDB_create_failed;
		ERR << "Error creating rpm database, rpm error was: " << rpmerrormsg << endl;
	    }
	    else
	    {
		_creatednew = true;
		_initialized = true;
		dbStatus = Error::E_ok;
	    }
	}
	else
	    dbStatus = Error::E_RpmDB_mkdir_failed;
    }
    else
    {
	ERR << "dbFilename not found " << dbFilename.asString() << endl;

	// DB not found
	dbStatus = Error::E_RpmDB_not_found;
    }

    // check for installed rpm package
    if ( !_creatednew && dbStatus == Error::E_ok )
    {
       // Check, if it is an old rpm-Db
       RpmArgVec opts(2);
       opts[0] = "-q";
       opts[1] = "rpm";
       string output;

       run_rpm (opts);
       string rpmmsg, str;
       while (systemReadLine (str))
       {
	   rpmmsg+=str;
       }

       int status = systemStatus();

       if ( rpmmsg.empty() )
       {
	  // error
	  dbStatus = Error::E_RpmDB_check_old_version_failed;
	  ERR << "rpm silently failed while checking old rpm version" << endl;
       }
       else
       {
	  if ( rpmmsg.find ( "old format database is present" ) !=
	       string::npos )
	  {
	     _old_present = true;
	     WAR <<  "RPM-Db on the system is old"  << endl;
	  }
	  else
	  {
	     if ( status != 0 )
	     {
		// error
		dbStatus = Error::E_RpmDB_check_old_version_failed;
		ERR << "checking for old rpm version failed, rpm output was: "
		    << rpmmsg << endl;
	     }
	  }
       }
    }

    return dbStatus;
}

// rebuild rpm database
PMError
RpmDb::rebuildDatabase()
{
    RpmArgVec opts(1);
    PMError status = Error::E_ok;

    FAILIFNOTINITIALIZED

    DBG << endl;

    opts[0] = "--rebuilddb";

    run_rpm (opts);

    string rpmerrormsg, str;
    while (systemReadLine(str))
    {
	rpmerrormsg+=str;
    }
    if ( systemStatus() != 0 )
    {
	// error
	status = Error::E_RpmDB_rebuilddb_failed;
	ERR << "Error rebuilding rpm database, rpm error was: " << rpmerrormsg << endl;
    }

    return status;
}

/*--------------------------------------------------------------*/
/* Creating a temporary rpm-database.				*/
/* If copyOldRpm == true than the rpm-database from		*/
/* /var/lib/rpm will be copied.					*/
/*--------------------------------------------------------------*/
PMError
RpmDb::createTmpDatabase ( bool copyOldRpm )
{
    FAILIFNOTINITIALIZED

    return Error::E_ok;
#if 0
   // searching a non-existing rpm-path
   int counter = 0;
   struct stat  dummyStat;
   Pathname rpmPath;
   Pathname saveDbPath = dbPath;
   DbStatus err = RPMDB_OK;
   char number[10];

   number[0] = 0;

   rpmPath = _rootdir + _varlib + "rpm.new";
   for ( counter = 0;
	counter < 1000 && stat( rpmPath.asString().c_str(), &dummyStat ) != -1;
	counter++)
   {
      // search free rpm-path
      snprintf ( number, 10, "%d", counter);
      rpmPath = _rootdir + _varlib + "rpm.new.";
      rpmPath.extend(number);
   }

   if (!PathInfo::assert_dir( rpmPath, S_IRWXU ))
   {
      err = RPMDB_ERROR_MKDIR;
      ERR << "ERROR command: assert_dir " << rpmPath.asString() << endl;

   }

   // setting global dbpath
   dbPath = _varlib;
   if ( counter == 0 )
   {
      dbPath = dbPath + "rpm.new";
   }
   else
   {
      dbPath = dbPath + "rpm.new.";
      dbPath.extend(number);
   }

   if ( !err )
   {
      RpmArgVec opts(1);
      opts[0] = "--initdb";
      run_rpm (opts);
      if ( systemStatus() != 0 )
      {
	 // error
	 err = RPMDB_ERROR_INITDB;
	 ERR << "ERROR command: rpm --initdb  --dbpath " <<
		  dbPath.asString() << endl;
      }
   }

   if ( !err && copyOldRpm )
   {
      // copy old RPM-DB into temporary RPM-DB

      string command = "cp -a ";
      command = command + Pathname::cat(_rootdir, _varlibrpm).asString() + "/* " +
	 rpmPath.asString();

//XXX system
      if ( system ( command.c_str() ) == 0 )
      {
	 err = RPMDB_OK;
      }
      else
      {
	 err = RPMDB_ERROR_COPY_TMPDB;
	 ERR << "ERROR command: " << command.c_str() << endl;
      }

      if ( !err )
      {
	 RpmArgVec opts(1);
	 opts[0] = "--rebuilddb";
	 run_rpm (opts);
	 if ( systemStatus() != 0 )
	 {
	    // error
	    err = RPMDB_ERROR_REBUILDDB;
	    ERR << "ERROR command: rpm --rebuilddb  --dbpath " <<
		     dbPath.asString() << endl;
	 }
      }
   }

   if ( !err )
   {
      temporary = true;
   }
   else
   {
      // setting global dbpath
      dbPath = saveDbPath;
   }

   return ( err );
#endif
}

/*--------------------------------------------------------------*/
/* Installing the rpm-database to /var/lib/rpm, if the		*/
/* current has been created by "createTmpDatabase".		*/
/*--------------------------------------------------------------*/
PMError
RpmDb::installTmpDatabase( void )
{
    FAILIFNOTINITIALIZED

    return Error::E_ok;
#if 0
   DbStatus err = RPMDB_OK;
   Pathname oldPath;
   struct stat  dummyStat;
   int counter = 1;

   DBG << "calling installTmpDatabase" << endl;

   if ( !temporary  )
   {
      DBG << "RPM-database does not have to be updated." << endl;
      return ( RPMDB_OK );
   }

   if ( dbPath.empty() )
   {
      ERR << "RPM-DB is not initialized." << endl;
      return ( RPMDB_ERROR_NOT_INITIALIZED );
   }

   if ( !err )
   {
      // creating path for saved rpm-DB
      oldPath = _rootdir + _varlib + "rpm.old";
      while ( counter < 1000 && stat( oldPath.asString().c_str(), &dummyStat ) != -1 )
      {
	 // search free rpm-path
	 char number[10];
	 snprintf ( number, 10, "%d", counter++);
	 oldPath = _rootdir + _varlib + "rpm.old.";
	 oldPath.extend(number);
      }

      if (!PathInfo::assert_dir ( oldPath, S_IRWXU ))
      {
	 ERR << "ERROR command: assert_dir %s" << oldPath.asString() << endl;
	 err = RPMDB_ERROR_MKDIR;
      }
   }

   if ( !err )
   {
      // saving old rpm
      string command = "cp -a ";
      command = command + Pathname::cat(_rootdir, _varlibrpm).asString() + "/* " + oldPath.asString();

// XXX system */
      if ( system ( command.c_str() ) == 0)
      {
	 err = RPMDB_OK;
      }
      else
      {
	 ERR << "ERROR command: " << command.c_str() << endl;
	 err = RPMDB_ERROR_COPY_TMPDB;
      }
   }


   if ( !err )
   {
      string command = "cp -a ";
      command = command + Pathname::cat(_rootdir, dbPath).asString() + "/* " + // */
	 Pathname::cat(_rootdir, _varlibrpm).asString();

      if ( system ( command.c_str() ) == 0)
      {
	 err = RPMDB_OK;
      }
      else
      {
	 ERR << "ERROR command: " << command.c_str() << endl;
	 err = RPMDB_ERROR_COPY_TMPDB;
      }
   }

   if ( !err )
   {
      // remove temporary RPM-DB
      string command = "rm -R ";

      command += Pathname::cat(_rootdir, dbPath).asString();
      system ( command.c_str() );

      temporary = false;
      dbPath = _varlibrpm;
   }

   return ( err );
#endif
}

// split in into pieces seperated by sep, return vector out
// produce up to max tokens. if max is zero the number of tokens is unlimited
//
unsigned
RpmDb::tokenize(const string& in, char sep, unsigned max, vector<string>& out)
{
    unsigned count = 0;
    string::size_type pos1=0, pos2=0;
    while(pos1 != string::npos && (max>0?count<max-1:true))
    {
	count++;
	pos2 = in.find(sep,pos1);
	if(pos2 != string::npos)
	{
	    out.push_back(in.substr(pos1,pos2-pos1));
	    pos1=pos2+1;
	}
	else
	{
	    out.push_back(in.substr(pos1));
	    pos1=pos2;
	}
    }
    if(max && count >= max-1 && pos1 != string::npos)
    {
	count++;
	out.push_back(in.substr(pos1));
    }
    return count;
}


// parse string of the form "name,number,version" into rellist.
// number is the rpm number representing the operator <, <=, = etc.
// See output of rpm -q --queryformat [%{REQUIRENAME},%{REQUIREFLAGS},%{REQUIREVERSION},] <package>
//
void
RpmDb::rpmdeps2rellist ( const string& depstr,
		PMSolvable::PkgRelList_type& deps)
{
    enum rpmdep
    {
	DNONE = 0,
	DLT = 2,
	DGT = 4,
	DEQ = 8,
	DGE = GT|EQ,
	DLE = LT|EQ,
	DPREREQ = 64
    };
    struct
    {
	string name;
	rel_op compare;
	string version;
	bool isprereq;
	void clear()
	{
	    name = version.erase();
	    compare=NONE;
	    isprereq = false;
	}
    } cdep_Ci;

    deps.clear();

    vector<string> depvec;
    tokenize(depstr, ',', 0, depvec);

//    D__ << "split " << depstr << " into " << depvec.size() << " pieces" << endl;

    if(depvec.size()<3) return;

    for(vector<string>::size_type i = 0; i <= depvec.size()-3; i+=3 )
    {
	cdep_Ci.name = depvec[i];
	cdep_Ci.version = depvec[i+2];

	int op = atoi(depvec[i+1].c_str());
	op = op & 127; // prereq is 64, all senses above are ignored
	if(op&DPREREQ)
	{
	    cdep_Ci.isprereq=true;
	    op^=DPREREQ;
//	    D__ << "inversion " << op << endl;
	}
	else
	{
	    cdep_Ci.isprereq=false;
	}

	if(op == 0)
	{
	    cdep_Ci.compare = NONE;
	}
	else
	{
	    if(op&DLE)
	    {
		cdep_Ci.compare = LE;
	    }
	    else if(op&DGE)
	    {
		cdep_Ci.compare = GE;
	    }
	    else if(op&DGT)
	    {
		cdep_Ci.compare = GT;
	    }
	    else if(op&DLT)
	    {
		cdep_Ci.compare = LT;
	    }
	    else if(op&DEQ)
	    {
		cdep_Ci.compare = EQ;
	    }
	    else
	    {
		ERR << "operator " << op << " invalid for package " << cdep_Ci.name << "version " << cdep_Ci.version << endl;
		cdep_Ci.compare = NONE;
	    }

	}

	PkgRelation dep(PkgName(cdep_Ci.name),cdep_Ci.compare,PkgEdition::fromString(cdep_Ci.version));
#warning SUSPICIOUS usage of version to create PkgEdition
	// ma: IMHO version part of depend. query may be [epoch:]version-release
	// maybe 'PkgEdition::fromString( cdep_Ci.version )' creates the Edition
	// you need.

//	D__ << dep << endl;

	dep.setPreReq(cdep_Ci.isprereq);

	deps.push_back(dep);

	cdep_Ci.clear();
    }
}

//-------------------------------------------
// return pkglist with installed packages
//
// refreshes _packages
//
const std::list<PMPackagePtr>& 
RpmDb::getPackages (void)
{
    string rpmquery;
    MIL << "RpmDb::getPackages()" << endl;

    _packages.clear();

    if (_old_present) return _packages;
    if (!_initialized) return _packages;

    // this enum tells the position in rpmquery string
    //
    // see pkgattribs[] below
    //
    enum {
	// PMSolvable
	RPM_NAME = 0,
	RPM_VERSION,
	RPM_RELEASE,
	RPM_ARCH,
	RPM_REQUIRES,
	RPM_PROVIDES,
	RPM_OBSOLETES,
	RPM_CONFLICTS,
	RPM_BUILDTIME,		// passed to PkgEdition()
	RPM_SIZE,		// cached in dataprovider
	RPM_GROUP,		// cached in dataprovider
	RPM_SUMMARY,      // summary MUST be last, could contain ';'
	NUM_RPMTAGS
    };

    //
    // Start RPM process
    //

    // build up query for all relevant data (for a PMObject)
    // all other data is read on-demand by the dataprovider
    //
    // !! the query order must match the enum !!
    // The query must end with "\\n" in order to separate
    // the package data
    //
    // Values except summary must neither contain ',' nor ';'
    // 
    rpmquery += "%{RPMTAG_NAME};%{RPMTAG_VERSION};%{RPMTAG_RELEASE};%{RPMTAG_ARCH};";
    rpmquery += "[%{REQUIRENAME},%{REQUIREFLAGS},%{REQUIREVERSION},];";
    rpmquery += "[%{PROVIDENAME},%{PROVIDEFLAGS},%{PROVIDEVERSION},];";
    rpmquery += "[%{OBSOLETENAME},%{OBSOLETEFLAGS},%{OBSOLETEVERSION},];";
    rpmquery += "[%{CONFLICTNAME},%{CONFLICTFLAGS},%{CONFLICTVERSION},];";
    rpmquery += "%{BUILDTIME};%{SIZE};%{GROUP};%{SUMMARY}";
    rpmquery += "\\n";

    RpmArgVec opts(4);
    opts[0] = "-q";
    opts[1] = "-a";
    opts[2] = "--queryformat";
    opts[3] = rpmquery.c_str();
    run_rpm (opts, ExternalProgram::Discard_Stderr);

    if (!process)
    {
	ERR << "RpmDB subprocess start failed" << endl;
	return _packages;
    }

    string value;
    string output;

    output = process->receiveLine();

    //
    // now loop over all packages reported by the rpm process
    // and create (properly filled) PMPackage instances
    //

    while ( output.length() > 0)
    {
	string::size_type         ret;

	// extract \n
	// the queryformat specified "\n" as the package separator

	ret = output.find_first_of ( "\n" );
	if ( ret != string::npos )
	{
	    value.assign ( output, 0, ret );
	}
	else
	{
	    value = output;
	}

//	D__ << "stdout: " << value << endl;

	//
	// parse output to pkgattribs
	// the queryformat specified ";" as the value separator
	//

	vector<string> pkgattribs;

	tokenize (value, ';', NUM_RPMTAGS, pkgattribs);

	if( pkgattribs.size() != NUM_RPMTAGS )
	{
	    ERR << "invalid rpm output:" << value << " size is "<< pkgattribs.size()<< endl;
	}
	else
	{
	    int buildtime = 0;
	    if(!pkgattribs[RPM_BUILDTIME].empty())
	    {
		// XXX: use strtol instead?
		buildtime = atoi (pkgattribs[RPM_BUILDTIME].c_str());
	    }
	    PkgEdition edi( buildtime, 0,
			    pkgattribs[RPM_VERSION],
			    pkgattribs[RPM_RELEASE]);

	    PMRpmPackageDataProviderPtr dataprovider = new PMRpmPackageDataProvider (this);
	    PMPackagePtr p = new PMPackage(
				PkgName(pkgattribs[RPM_NAME]),
				edi,
				PkgArch(pkgattribs[RPM_ARCH]),
			        dataprovider);
	    dataprovider->setPackage (p);

	    PMSolvable::PkgRelList_type requires;
	    PMSolvable::PkgRelList_type provides;
	    PMSolvable::PkgRelList_type obsoletes;
	    PMSolvable::PkgRelList_type conflicts;

	    PMSolvable::PkgRelList_type dummy;

	    rpmdeps2rellist (pkgattribs[RPM_REQUIRES], requires);
	    rpmdeps2rellist (pkgattribs[RPM_PROVIDES], provides);
	    rpmdeps2rellist (pkgattribs[RPM_OBSOLETES], obsoletes);
	    rpmdeps2rellist (pkgattribs[RPM_CONFLICTS], conflicts);

	    p->setRequires (requires);
	    p->setProvides (provides);
	    p->setObsoletes (obsoletes);
	    p->setConflicts (conflicts);

	    dataprovider->_attr_SUMMARY = pkgattribs[RPM_SUMMARY];
	    dataprovider->_attr_SIZE = FSize (atoll(pkgattribs[RPM_SIZE].c_str()));
	    dataprovider->_attr_GROUP = Y2PM::packageManager().addRpmGroup(pkgattribs[RPM_GROUP]);

	    _packages.push_back (p);
	    // D__ << pkgattribs[RPM_NAME] << " " << endl;
//	    D__ << p << endl;
	}

	output = process->receiveLine();
    }

    if (systemStatus() != 0)
    {
	ERR << "RpmDB subprocess stop failed" << endl;
    }
    return _packages;
}


#if 0
/*--------------------------------------------------------------*/
/* Check package, if it is correctly installed.			*/
/* Returns false, if an error has been occured.			*/
/*--------------------------------------------------------------*/
bool
RpmDb::checkPackage ( string packageName, FileList &fileList )
{
   bool ok = true;
   struct stat  dummyStat;


   const char *const opts[] = {
      "-ql", packageName.c_str()
     };

   run_rpm(sizeof(opts) / sizeof(*opts), opts,
	   ExternalProgram::Discard_Stderr);

   if ( process == NULL )
      return false;

   string value;
   fileList.clear();

   string output = process->receiveLine();

   while ( output.length() > 0)
   {
      string::size_type 	ret;

      // extract \n
      ret = output.find_first_of ( "\n" );
      if ( ret != string::npos )
      {
	 value.assign ( output, 0, ret );
      }
      else
      {
	 value = output;
      }

      // checking, if file exists
      if (  lstat( (rootfs+value).c_str(), &dummyStat ) == -1 )
      {
	 // file not found
	 ok = false;
	 fileList.insert ( value );
      }
      output = process->receiveLine();
   }

   if ( systemStatus() != 0 )
   {
      ok = false;
   }
   return ( ok );
}

/*--------------------------------------------------------------*/
/* Evaluate all files of a package which have to be installed.  */
/* ( are listed in the rpm-DB )					*/
/*--------------------------------------------------------------*/
bool
RpmDb::queryInstalledFiles ( FileList &fileList, string packageName )
{
   bool ok = true;
   const char *const opts[] = {
      "-ql", packageName.c_str()
     };

   run_rpm(sizeof(opts) / sizeof(*opts), opts,
	   ExternalProgram::Discard_Stderr);

   if ( process == NULL )
      return false;

   string value;
   fileList.clear();

   string output = process->receiveLine();

   while ( output.length() > 0)
   {
      string::size_type 	ret;

      // extract \n
      ret = output.find_first_of ( "\n" );
      if ( ret != string::npos )
      {
	 value.assign ( output, 0, ret );
      }
      else
      {
	 value = output;
      }

      fileList.insert ( value );

      output = process->receiveLine();
   }

   if ( systemStatus() != 0 )
   {
      ok = false;
   }

   return ( ok );
}


/*------------------------------------------------------------------*/
/* Evaluate all directories of a package which have been installed. */
/* ( are listed in the rpm-DB )					    */
/*------------------------------------------------------------------*/
bool
RpmDb::queryDirectories ( FileList &fileList, string packageName )
{
   bool ok = true;
   const char *const opts[] = {
      "-qlv", packageName.c_str()
     };

   run_rpm(sizeof(opts) / sizeof(*opts), opts,
	   ExternalProgram::Discard_Stderr);

   if ( process == NULL )
      return false;

   string value;
   char buffer[15000];
   size_t nread;
   fileList.clear();

   while (  nread = process->receive(buffer, sizeof(buffer)), nread != 0)
   {
      string output(buffer);
      string::size_type 	begin, end;

      begin = output.find_first_not_of ( "\n" );
      while ( begin != string::npos )
      {
	 // splitt the output in package-names
	 string value ="";
	 end = output.find_first_of ( "\n", begin );

	 // line-end ?
	 if ( end == string::npos )
	 {
	    end= output.length();
	 }

	 value.assign ( output, begin, end-begin );
	 begin = output.find_first_not_of ( "\n", end );

	 string::size_type fileBegin, fileEnd;
	 string dirname = "";
	 fileBegin = value.find_first_of ( '/' );
	 if ( fileBegin != string::npos )
	 {
	    fileEnd = value.find_first_of ( " ", fileBegin );

	    if ( fileEnd == string::npos )
	    {
	       // end reached
	       dirname.assign (value, fileBegin, string::npos);
	    }
	    else
	    {
	       dirname.assign ( value, fileBegin, fileEnd-fileBegin );
	    }

	    if ( value[0] != 'd' )
	    {
	       // is not a directory --> filename extract
	       fileEnd = dirname.find_last_of ( "/" );
	       dirname.assign ( dirname, 0, fileEnd );
	    }

	    fileList.insert ( dirname );
	 }
      }
   }

   if ( systemStatus() != 0 )
   {
      ok = false;
   }

   return ( ok );
}

#endif


/*--------------------------------------------------------------*/
/* Checking the source rpm <rpmpath> with rpm --chcksig and     */
/* the version number.						*/
/*--------------------------------------------------------------*/
unsigned
RpmDb::checkPackage( string packagePath, string version, string md5 )
{
    unsigned result = 0;

    if(!md5.empty())
    {
	//TODO
	WAR << "md5sum check not yet implemented" << endl;
	return CHK_INCORRECT_FILEMD5;
    }

    // checking --checksig
    const char *const argv[] = {
	"rpm", "--checksig",  packagePath.c_str(), 0
    };

    exit_code = -1;

    string output = "";
    unsigned int k;
    for ( k = 0; k < (sizeof(argv) / sizeof(*argv)) -1; k++ )
    {
	output = output + " " + argv[k];
    }

    DBG << "rpm command: " << output << endl;

    if ( process != NULL )
    {
	delete process;
	process = NULL;
    }
    // Launch the program
    process = new ExternalProgram( argv, ExternalProgram::Stderr_To_Stdout, false, -1, true);


    if ( process == NULL )
    {
	result |= CHK_OTHER_FAILURE;
	D__ << "create process failed" << endl;
    }

    string value;
    output = process->receiveLine();

    while ( output.length() > 0)
    {
	string::size_type         ret;

	// extract \n
	ret = output.find_first_of ( "\n" );
	if ( ret != string::npos )
	{
	    value.assign ( output, 0, ret );
	}
	else
	{
	    value = output;
	}

	D__ << "stdout: " << value << endl;

	string::size_type pos;
	if((pos = value.find(packagePath)) != string::npos)
	{
	    string rest = value.substr(pos+packagePath.length()+1);
	    if(rest.find("NOT OK") == string::npos)
	    {
		// see what checks are ok
		if(rest.find("md5") == string::npos)
		{
		    result |= CHK_MD5SUM_MISSING;
		}
		if(rest.find("gpg") == string::npos)
		{
		    result |= CHK_GPGSIG_MISSING;
		}
	    }
	    else
	    {
		// see what checks are not ok
		if(rest.find("MD5") != string::npos)
		{
		    result |= CHK_INCORRECT_PKGMD5;
		}
		else
		{
		    result |= CHK_MD5SUM_MISSING;
		}

		if(rest.find("GPG") != string::npos)
		{
		    result |= CHK_INCORRECT_GPGSIG;
		}
		else
		{
		    result |= CHK_GPGSIG_MISSING;
		}
	    }
	}

	output = process->receiveLine();
    }

    if ( result == 0 && systemStatus() != 0 )
    {
	// error
	result |= CHK_OTHER_FAILURE;
    }

    if ( !version.empty() )
    {

	string value;
	queryPackage (packagePath, "%{RPMTAG_VERSION}-%{RPMTAG_RELEASE}", value);

	D__ <<  "comparing version " << version << " <-> " << value << endl;
	if ( version != value )
	{
	    result |= CHK_INCORRECT_VERSION;
	}
    }

    return ( result );
}


/*--------------------------------------------------------------*/
/* Query the current package using the specified query format	*/
/*--------------------------------------------------------------*/

bool
RpmDb::queryPackage (const std::string& package, const char *qparam, const char *format, std::string& result_r)
{
    RpmArgVec opts(4);

    if(_old_present) return false;
    if(!_initialized) return false;

    opts[0] = qparam;
    opts[1] = "--queryformat";
    opts[2] = format;
    opts[3] = package.c_str();
    run_rpm (opts, ExternalProgram::Discard_Stderr);

    if ( process == NULL )
	return false;

    systemReadLine (result_r);
    systemStatus();

    return true;
}

bool
RpmDb::queryPackage (const std::string& package, const char *qparam, const char *format, std::list<std::string>& result_r)
{
    RpmArgVec opts(4);

    if(_old_present) return false;
    if(!_initialized) return false;

    opts[0] = qparam;
    opts[1] = "--queryformat";
    opts[2] = format;
    opts[3] = package.c_str();
    run_rpm (opts, ExternalProgram::Discard_Stderr);

    if ( process == NULL )
	return false;

    string line;
    while (systemReadLine (line))
    {
	result_r.push_back (line);
    }
    systemStatus();

    return true;
}

bool
RpmDb::queryPackage (constPMPackagePtr package, const char *format, std::string& result_r)
{
    return queryPackage ((const string &)package->name() + "-" + package->edition().as_string(), "-q", format, result_r);
}

bool
RpmDb::queryPackage (constPMPackagePtr package, const char *format, std::list<std::string>& result_r)
{
    return queryPackage ((const string &)package->name() + "-" + package->edition().as_string(), "-q", format, result_r);
}

bool
RpmDb::queryPackage (const Pathname& path, const char *format, std::string& result_r)
{
    return queryPackage (path.asString(), "-qp", format, result_r);
}

bool
RpmDb::queryPackage (const Pathname& path, const char *format, std::list<std::string>& result_r)
{
    return queryPackage (path.asString(), "-qp", format, result_r);
}

#if 0
/*--------------------------------------------------------------*/
/* Evaluate all files of a package which have been changed	*/
/* since last installation or update.				*/
/*--------------------------------------------------------------*/
bool
RpmDb::queryChangedFiles ( FileList &fileList, string packageName )
{
   bool ok = true;

   fileList.clear();


   const char *const opts[] = {
      "-V", packageName.c_str(),
      "--nodeps",
      "--noscripts",
      "--nomd5" };

   run_rpm(sizeof(opts) / sizeof(*opts), opts,
	   ExternalProgram::Discard_Stderr);

   if ( process == NULL )
      return false;

   string value;
   fileList.clear();

   string output = process->receiveLine();

   while ( output.length() > 0)
   {
      string::size_type 	ret;

      // extract \n
      ret = output.find_first_of ( "\n" );
      if ( ret != string::npos )
      {
	 value.assign ( output, 0, ret );
      }
      else
      {
	 value = output;
      }

      if ( value.length() > 12 &&
	   ( value[0] == 'S' || value[0] == 's' ||
	     ( value[0] == '.' &&
	       value[7] == 'T' )))
      {
	 // file has been changed
	 string filename;

	 filename.assign ( value, 11, value.length() - 11 );
	 filename = rootfs + filename;
	 fileList.insert ( filename );
      }

      output = process->receiveLine();
   }
   systemStatus();

   return ( ok );
}
#endif



/****************************************************************/
/* private member-functions					*/
/****************************************************************/

/*--------------------------------------------------------------*/
/* Run rpm with the specified arguments, handling stderr	*/
/* as specified  by disp					*/
/*--------------------------------------------------------------*/
void
RpmDb::run_rpm(const RpmArgVec& options,
		       ExternalProgram::Stderr_Disposition disp)
{

    exit_code = -1;

    RpmArgVec args(5);
    args[0] = "rpm";
    args[1] = "--root";
    args[2] = _rootdir.asString().c_str();
    args[3] = "--dbpath";
    args[4] = dbPath.asString().c_str();

    const char* argv[args.size()+options.size()+2];
    unsigned argc = 0;

//    D__ << "rpm command: ";

    for(RpmArgVec::iterator it=args.begin();it<args.end();++it)
    {
	argv[argc++]=*it;
//	D__ << *it << " ";
    }
    for(RpmArgVec::const_iterator it2=options.begin();it2<options.end();++it2)
    {
	argv[argc++]=*it2;
//	D__ << *it2 << " ";
    }

    argv[argc] = 0;

//    D__ << endl;

    if ( process != NULL )
    {
	delete process;
	process = NULL;
    }
    // Launch the program
    process = new ExternalProgram(argv, disp, false, -1, true);
}

/*--------------------------------------------------------------*/
/* Read a line from the rpm process				*/
/*--------------------------------------------------------------*/
bool
RpmDb::systemReadLine(string &line)
{
    line.erase();

    if ( process == NULL )
	return false;

    line = process->receiveLine();

    if (line.length() == 0)
	return false;

    if (line[line.length() - 1] == '\n')
	line.erase(line.length() - 1);

    return true;
}

/*--------------------------------------------------------------*/
/* Return the exit status of the rpm process, closing the	*/
/* connection if not already done				*/
/*--------------------------------------------------------------*/
int
RpmDb::systemStatus()
{
   if ( process == NULL )
      return -1;

   exit_code = process->close();
   process->kill();
   delete process;
   process = 0;

//   D__ << "exit code " << exit_code << endl;

  return exit_code;
}

/*--------------------------------------------------------------*/
/* Forcably kill the rpm process				*/
/*--------------------------------------------------------------*/
void
RpmDb::systemKill()
{
  if (process) process->kill();
}


// generate diff mails for config files
void RpmDb::processConfigFiles(const string& line, const string& name, const char* typemsg, const char* difffailmsg, const char* diffgenmsg)
{
    string msg = line.substr(9);
    string::size_type pos1 = string::npos;
    string::size_type pos2 = string::npos;
    string file1;
    string file2;

    pos1 = msg.find(typemsg);
    for(;;)
    {
	if( pos1 == string::npos )
	    break;

	pos2=pos1+strlen(typemsg);

	if( pos2 >= msg.length() )
	    break;
	
	file1 = msg.substr(0,pos1);
	file2 = msg.substr(pos2);
	string out;
	int ret = Diff::differ(file1,file2,out,25);
	if(ret)
	{
	    Pathname notifydir = Pathname(_rootdir) + WARNINGMAILPATH;
	    if(PathInfo::assert_dir(notifydir) != 0)
	    {
		ERR << "Could not create " << notifydir.asString() << endl;
		break;
	    }
	    string file = name + '_' + file1;
	    for(string::size_type pos = file.find('/'); pos != string::npos; pos = file.find('/'))
	    {
		file[pos] = '_';
	    }
	    file = (notifydir + file).asString();
	    ofstream notify(file.c_str());
	    if(!notify)
	    {
		ERR << "Could not open " <<  file << endl;
		break;
	    }

	    notify << name << endl;
	    if(ret>1)
	    {
		ERR << "diff failed" << endl;
		notify << stringutil::form(difffailmsg,
		    file1.c_str(), file2.c_str()) << endl;
	    }
	    else
	    {
		notify << stringutil::form(diffgenmsg,
		    file1.c_str(), file2.c_str()) << endl;
		notify << out << endl;
	    }
	    notify.close();
	}
	else
	{
	    WAR << "rpm created " << file2 << " but it is not different from " << file2 << endl;
	}
	break;
    }
}

// inststall package filename with flags iflags
PMError
RpmDb::installPackage(const string& filename, unsigned flags)
{
    RpmArgVec opts;

    FAILIFNOTINITIALIZED

    opts.push_back("-U");
    opts.push_back("--percent");

    if(flags&RPMINST_NODOCS)
	opts.push_back("--excludedocs");
    if(flags&RPMINST_NOSCRIPTS)
	opts.push_back("--noscripts");
    if(flags&RPMINST_FORCE)
	opts.push_back("--force");
    if(flags&RPMINST_NODEPS)
	opts.push_back("--nodeps");
    if(flags&RPMINST_IGNORESIZE)
	opts.push_back("--ignoresize");
    if(flags&RPMINST_JUSTDB)
	opts.push_back("--justdb");


    opts.push_back(filename.c_str());

    // %s = filename of rpm package
    _progresslogstream << stringutil::form(_("Installing %s"), Pathname::basename(filename).c_str()) << endl;

    run_rpm( opts, ExternalProgram::Stderr_To_Stdout);

    string line;
    string rpmmsg;
    double old_percent = 0.0;

    while (systemReadLine(line))
    {
	if (line.substr(0,2)=="%%")
	{
	    double percent;
	    sscanf (line.c_str () + 2, "%lg", &percent);
	    if (percent >= old_percent + 5.0)
	    {
		old_percent = int (percent / 5) * 5;
		ReportProgress(static_cast<int>(percent));
	    }
	}
	else
	    rpmmsg += line+'\n';

	if( line.substr(0,8) == "warning:" )
	{
	    processConfigFiles(line, Pathname::basename(filename), " saved as ",
		// %s = filenames
		_("rpm saved %s as %s, but it was impossible to generate a diff"),
		// %s = filenames
		_("rpm saved %s as %s.\nHere are the first 25 lines of difference:\n"));
	    processConfigFiles(line, Pathname::basename(filename), " created as ",
		// %s = filenames
		_("rpm created %s as %s, but it was impossible to generate a diff"),
		// %s = filenames
		_("rpm created %s as %s.\nHere are the first 25 lines of difference:\n"));
	}
    }
    int rpm_status = systemStatus();
    if (rpm_status != 0)
    {
	// %s = filename of rpm package
	_progresslogstream << stringutil::form(_("%s failed"), Pathname::basename(filename).c_str()) << endl;
	ERR << "rpm failed, message was: " << rpmmsg << endl;
	_progresslogstream << _("rpm output:") << endl << rpmmsg << endl;
	return Error::E_RpmDB_subprocess_failed;
    }
    // %s = filename of rpm package
    _progresslogstream << stringutil::form(_("%s ok"), Pathname::basename(filename).c_str()) << endl;
    if(!rpmmsg.empty())
    {
	_progresslogstream << _("Additional rpm output:") << endl << rpmmsg << endl;
    }
    return Error::E_ok;
}

// remove package named label
PMError
RpmDb::removePackage(const string& label, unsigned flags)
{
    RpmArgVec opts;

    FAILIFNOTINITIALIZED

    opts.push_back("-e");

    if(flags&RPMINST_NOSCRIPTS)
	opts.push_back("--noscripts");
    if(flags&RPMINST_NODEPS)
	opts.push_back("--nodeps");
    if(flags&RPMINST_JUSTDB)
	opts.push_back("--justdb");

    opts.push_back(label.c_str());

    //XXX maybe some log for the user too?
    DBG << "Removing " << label << endl;

    run_rpm(opts, ExternalProgram::Stderr_To_Stdout);

    string rpmmsg;
    string line;

    while (systemReadLine(line))
    {
	rpmmsg += line+'\n';
    }
    int rpm_status = systemStatus();
    if (rpm_status != 0)
    {
	ERR << "rpm failed, message was: " << rpmmsg << endl;
	return Error::E_RpmDB_subprocess_failed;
    }
    return Error::E_ok;
}

string
RpmDb::checkPackageResult2string(unsigned code)
{
    string msg;
    // begin of line characters
    string bol = " - ";
    // end of line characters
    string eol = "\n";
    if(code == 0)
	return string(_("Ok"))+eol;

    //translator: these are different kinds of how an rpm package can be broken
    msg = _("Package is not OK for the following reasons:");
    msg += eol;

    if(code&CHK_INCORRECT_VERSION)
    {
	msg += bol;
	msg+=_("Package contains different version than expected");
	msg += eol;
    }
    if(code&CHK_INCORRECT_FILEMD5)
    {
	msg += bol;
	msg+=_("Package file has incorrect MD5 sum");
	msg += eol;
    }
    if(code&CHK_GPGSIG_MISSING)
    {
	msg += bol;
	msg+=_("Package is not signed");
	msg += eol;
    }
    if(code&CHK_MD5SUM_MISSING)
    {
	msg += bol;
	msg+=_("Package has no MD5 sum");
	msg += eol;
    }
    if(code&CHK_INCORRECT_GPGSIG)
    {
	msg += bol;
	msg+=_("Package has incorrect signature");
	msg += eol;
    }
    if(code&CHK_INCORRECT_PKGMD5)
    {
	msg += bol;
	msg+=_("Package archive has incorrect MD5 sum");
	msg += eol;
    }
    if(code&CHK_OTHER_FAILURE)
    {
	msg += bol;
	msg+=_("rpm failed for unkown reason, see log file");
	msg += eol;
    }

    return msg;
}

bool RpmDb::setInstallationLogfile( const std::string& filename )
{
    if(_progresslogstream.is_open())
    {
	_progresslogstream.clear();
	_progresslogstream.close();
    }

    if(filename.empty())
	return true;

    _progresslogstream.clear();
    _progresslogstream.open(filename.c_str());
    if(!_progresslogstream)
    {
	ERR << "Could not open " << filename << endl;
	return false;
    }
    return true;
}

#if 0
/******************************************************************
**
**
**	FUNCTION NAME : operator<<
**	FUNCTION TYPE : std::ostream &
**
*/
std::ostream & operator<<( std::ostream & str, const
RpmDb::DbStatus & obj )
{
#define ENUM_OUT(V) case RpmDb::V: return str << #V; break
  switch ( obj ) {
    ENUM_OUT( RPMDB_OK );
    ENUM_OUT( RPMDB_NOT_FOUND );
    ENUM_OUT( RPMDB_OLD_VERSION );
    ENUM_OUT( RPMDB_NEW_CREATED );
    ENUM_OUT( RPMDB_ERROR_CREATED );
    ENUM_OUT( RPMDB_ERROR_CHECK_OLD_VERSION );
    ENUM_OUT( RPMDB_ERROR_MKDIR );
    ENUM_OUT( RPMDB_ERROR_INITDB );
    ENUM_OUT( RPMDB_ERROR_COPY_TMPDB );
    ENUM_OUT( RPMDB_ERROR_REBUILDDB );
    ENUM_OUT( RPMDB_ERROR_NOT_INITIALIZED );
    ENUM_OUT( RPMDB_ERROR_SUBPROCESS_FAILED );
  // default: let compiler warn 'not handled in switch'
  case RpmDb::RPMDB_NUM_ERRORS: break;
  }
  return str << "RPMDB_ERROR_UNKNOWN";
#undef ENUM_OUT
}
#endif
