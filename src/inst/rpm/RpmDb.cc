/*---------------------------------------------------------------------\
|                                                                      |
|                      __   __    ____ _____ ____                      |
|                      \ \ / /_ _/ ___|_   _|___ \                     |
|                       \ V / _` \___ \ | |   __) |                    |
|                        | | (_| |___) || |  / __/                     |
|                        |_|\__,_|____/ |_| |_____|                    |
|                                                                      |
|                           Package Management                         |
|                                                     (C) 2002 SuSE AG |
\----------------------------------------------------------------------/

   File:       CommonPkdParser.cc
   Purpose:    Interface to installed RPM system
   Author:     Stefan Schubert <schubi@suse.de>
   Maintainer: Ludwig Nussel <lnussel@suse.de>

   Copied and adapted from agent-targetpkg

/-*/

#include <sys/stat.h>
#include <unistd.h>

#include <cstdlib>

#include <iostream>
#include <string>
#include <list>
#include <vector>

#include <y2util/Y2SLog.h>
#include <y2util/TagParser.h>
#include <y2util/Pathname.h>
#include <y2util/PathInfo.h>
#include <y2util/ExternalDataSource.h>
#include <y2pm/RpmDb.h>
#include <y2pm/PkgEdition.h>
#include <y2pm/PkgRelation.h>
#include <y2pm/PMPackage.h>
#include <y2pm/PMRpmPackageDataProvider.h>

#ifndef _
#define _(X) X
#endif

using namespace std;

IMPL_BASE_POINTER(RpmDb);

/****************************************************************/
/* public member-functions					*/
/****************************************************************/

/*-------------------------------------------------------------*/
/* creates a RpmDb					       */
/*-------------------------------------------------------------*/
RpmDb::RpmDb(string name_of_root) :
    _progressfunc(NULL),
    _progressdata(NULL),
    _rpminstflags(RPMINST_NONE),
    _rootdir(name_of_root),
    _varlibrpm("/var/lib/rpm"),
    _varlib("/var/lib"),
    _rpmdbname("packages.rpm")
{
   process = 0;
   exit_code = -1;
   temporary = false;
   dbPath = "";
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
RpmDb::DbStatus RpmDb::initDatabase( bool createNew )
{
    Pathname     dbFilename;
    struct stat  dummyStat;
    DbStatus	 dbStatus = RPMDB_OK;

    DBG << "calling initDatabase" << endl;

    dbFilename += _rootdir + _varlibrpm + _rpmdbname;
    if (  stat( dbFilename.asString().c_str(), &dummyStat ) != -1 )
    {
	// DB found
	dbPath = _varlibrpm;
	DBG << "Setting dbPath to " << dbPath.asString() << endl;
    }
    else if ( createNew )
    {
	DBG << "creating new database" << endl;
	// New rpm-DB will be created
	if(!PathInfo::mkdir(dbFilename.dirname()))
	{
	    dbPath = _varlibrpm;

	    RpmArgVec opts(1);
	    opts[0] = "--initdb";

	    run_rpm(opts);

	    string rpmerrormsg, str;
	    while(systemReadLine(str))
	    {
		rpmerrormsg+=str;
	    }
	    if ( systemStatus() != 0 )
	    {
		// error
		dbStatus = RPMDB_ERROR_CREATED;
		ERR << "Error creating rpm database, rpm error was: " << rpmerrormsg << endl;
	    }
	    else
	    {
		dbStatus = RPMDB_NEW_CREATED;
	    }
	}
	else
	    dbStatus = RPMDB_ERROR_MKDIR;
    }
    else
    {
	ERR << "dbFilename not found " << dbFilename.asString() << endl;

	// DB not found
	dbStatus = RPMDB_NOT_FOUND;
    }

    // check for installed rpm package
    if ( dbStatus == RPMDB_OK )
    {
       // Check, if it is an old rpm-Db
       RpmArgVec opts(2);
       opts[0] = "-q";
       opts[1] = "rpm";
       string output;

       run_rpm(opts);
       string rpmmsg, str;
       while(systemReadLine(str))
       {
	   rpmmsg+=str;
       }
       
       int status = systemStatus();
       
       if ( rpmmsg.empty() )
       {
	  // error
	  dbStatus = RPMDB_ERROR_CHECK_OLD_VERSION;
	  ERR << "rpm silently failed while checking old rpm version" << endl;
       }
       else
       {
	  if ( rpmmsg.find ( "old format database is present" ) !=
	       string::npos )
	  {
	     dbStatus = RPMDB_OLD_VERSION;
	     WAR <<  "RPM-Db on the system is old"  << endl;
	  }
	  else
	  {
	     if ( status != 0 )
	     {
		// error
		dbStatus = RPMDB_ERROR_CHECK_OLD_VERSION;
		ERR << "checking for old rpm version failed, rpm output was: "
		    << rpmmsg << endl;
	     }
	  }
       }
    }

    _dataprovider = new PMRpmPackageDataProvider(this);

    return dbStatus;
}

// rebuild rpm database
RpmDb::DbStatus RpmDb::rebuildDatabase()
{
    RpmArgVec opts(1);
    DbStatus status = RPMDB_OK;

    DBG << endl;

    opts[0] = "--rebuilddb";

    run_rpm(opts);

    string rpmerrormsg, str;
    while(systemReadLine(str))
    {
	rpmerrormsg+=str;
    }
    if ( systemStatus() != 0 )
    {
	// error
	status = RPMDB_ERROR_REBUILDDB;
	ERR << "Error rebuilding rpm database, rpm error was: " << rpmerrormsg << endl;
    }

    return status;
}

/*--------------------------------------------------------------*/
/* Creating a temporary rpm-database.				*/
/* If copyOldRpm == true than the rpm-database from		*/
/* /var/lib/rpm will be copied.					*/
/*--------------------------------------------------------------*/
RpmDb::DbStatus RpmDb::createTmpDatabase ( bool copyOldRpm )
{
    return RPMDB_OK;
#warning "createTmpDatabase yet implemented"
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

   if (!PathInfo::mkdir( rpmPath, S_IRWXU ))
   {
      err = RPMDB_ERROR_MKDIR;
      ERR << "ERROR command: mkdir " << rpmPath.asString() << endl;

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
      run_rpm(opts);
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
	  run_rpm(opts);
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
RpmDb::DbStatus RpmDb::installTmpDatabase( void )
{
    return RPMDB_OK;
#warning "installTmpDatabase not yet implemented"
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

      if (!PathInfo::mkdir ( oldPath, S_IRWXU ))
      {
	 ERR << "ERROR command: mkdir %s" << oldPath.asString() << endl;
	 err = RPMDB_ERROR_MKDIR;
      }
   }

   if ( !err )
   {
      // saving old rpm
      string command = "cp -a ";
      command = command + Pathname::cat(_rootdir, _varlibrpm).asString() + "/* " + oldPath.asString();

// XXX system
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
      command = command + Pathname::cat(_rootdir, dbPath).asString() + "/* " +
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
unsigned RpmDb::tokenize(const string& in, char sep, vector<string>& out)
{
    unsigned count = 0;
    string::size_type pos1=0, pos2=0;
    while(pos1 != string::npos)
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
    return count;
}


// parse string of the form name/number/version into rellist. number is the rpm
// number representing the operator <, <=, = etc.
void RpmDb::rpmdeps2rellist ( const string& depstr,
		PMSolvable::PkgRelList_type& deps,
		PMSolvable::PkgRelList_type& prereq,
		bool ignore_prereqs)
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
    if(!ignore_prereqs)
	prereq.clear();

    vector<string> depvec;
    tokenize(depstr, ',', depvec);

//    D__ << "split " << depstr << " into " << depvec.size() << " pieces" << endl;

    if(depvec.size()<3) return;

    for(vector<string>::size_type i = 0; i <= depvec.size()-3; i+=3 )
    {
	cdep_Ci.name = depvec[i];
	cdep_Ci.version = depvec[i+2];

	int op = atoi(depvec[i+1].c_str());
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
		ERR << "operator " << op << " invalid" << endl;
		cdep_Ci.compare = NONE;
	    }

	}

	PkgRelation dep(PkgName(cdep_Ci.name),cdep_Ci.compare,cdep_Ci.version.c_str());
//	D__ << dep << endl;
	if(cdep_Ci.isprereq && !ignore_prereqs)
	{
	    prereq.push_back(dep);
	}
	else
	    deps.push_back(dep);

	cdep_Ci.clear();
    }
}

// fill pkglist with installed packages
RpmDb::DbStatus RpmDb::getPackages (std::list<PMPackagePtr>& pkglist)
{
    string rpmquery;

    // this enum tells the position in rpmquery string
    enum {
	RPM_NAME,
	RPM_VERSION,
	RPM_RELEASE,
	RPM_SIZE,
	RPM_BUILDTIME,
	RPM_GROUP,
	RPM_ARCH,
	RPM_REQUIRES,
	RPM_PROVIDES,
	RPM_OBSOLETES,
	RPM_CONFLICTS,

	NUM_RPMTAGS
    };

    // this must match the enum
    rpmquery += "%{RPMTAG_NAME};%{RPMTAG_VERSION};%{RPMTAG_RELEASE};";
    rpmquery += "%{RPMTAG_SIZE};%{RPMTAG_BUILDTIME};%{RPMTAG_GROUP};";
    rpmquery += "%{RPMTAG_ARCH};";
    rpmquery += "[%{REQUIRENAME},%{REQUIREFLAGS},%{REQUIREVERSION},];";
    rpmquery += "[%{PROVIDENAME},%{PROVIDEFLAGS},%{PROVIDEVERSION},];";
    rpmquery += "[%{OBSOLETENAME},%{OBSOLETEFLAGS},%{OBSOLETEVERSION},];";
    rpmquery += "[%{CONFLICTNAME},%{CONFLICTFLAGS},%{CONFLICTVERSION},]";
    rpmquery += "\\n";

    RpmArgVec opts(4);
    opts[0] = "-q";
    opts[1] = "-a";
    opts[2] = "--queryformat";
    opts[3] = rpmquery.c_str();
    run_rpm(opts, ExternalProgram::Discard_Stderr);

    if(!process)
	return RPMDB_ERROR_SUBPROCESS_FAILED;

    string value;
    string output;

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

//	D__ << "stdout: " << value << endl;

	vector<string> pkgattribs;

	tokenize(value,';',pkgattribs);

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
		buildtime = atoi(pkgattribs[RPM_BUILDTIME].c_str());
	    }
	    PkgEdition edi( buildtime, 0,
			    pkgattribs[RPM_VERSION].c_str(),
			    pkgattribs[RPM_RELEASE].c_str()
			);
	    PMPackagePtr p = new PMPackage(
				PkgName(pkgattribs[RPM_NAME]),
				edi,
				PkgArch(pkgattribs[RPM_ARCH]));

	    if(_dataprovider != NULL)
	    {
		_dataprovider->setAttributeValue(p,PMPackage::ATTR_SIZE,pkgattribs[RPM_SIZE]);
		_dataprovider->setAttributeValue(p,PMPackage::ATTR_GROUP,pkgattribs[RPM_GROUP]);
	    }

	    p->setDataProvider(_dataprovider);

	    PMSolvable::PkgRelList_type requires;
	    PMSolvable::PkgRelList_type prerequires;
	    PMSolvable::PkgRelList_type provides;
	    PMSolvable::PkgRelList_type obsoletes;
	    PMSolvable::PkgRelList_type conflicts;

	    PMSolvable::PkgRelList_type dummy;

	    rpmdeps2rellist(pkgattribs[RPM_REQUIRES],requires,prerequires);
	    rpmdeps2rellist(pkgattribs[RPM_PROVIDES],provides,dummy,true);
	    rpmdeps2rellist(pkgattribs[RPM_OBSOLETES],obsoletes,dummy,true);
	    rpmdeps2rellist(pkgattribs[RPM_CONFLICTS],conflicts,dummy,true);

	    p->setRequires(requires);
	    p->setPreRequires(prerequires);
	    p->setProvides(provides);
	    p->setObsoletes(obsoletes);
	    p->setConflicts(conflicts);

	    pkglist.push_back(p);
	    // D__ << pkgattribs[RPM_NAME] << " " << endl;
//	    D__ << p << endl;
	}

	output = process->receiveLine();
    }

    if ( systemStatus() != 0 )
    {
	return RPMDB_ERROR_SUBPROCESS_FAILED;
    }

    return RPMDB_OK;
}


#if 0
/*--------------------------------------------------------------*/
/* Check package, if it is correctly installed.			*/
/* Returns false, if an error has been occured.			*/
/*--------------------------------------------------------------*/
bool RpmDb::checkPackage ( string packageName, FileList &fileList )
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
bool RpmDb::queryInstalledFiles ( FileList &fileList, string packageName )
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
bool RpmDb::queryDirectories ( FileList &fileList, string packageName )
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
unsigned RpmDb::checkPackage( string packagePath, string version, string md5 )
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

	string value = queryPackage("%{RPMTAG_VERSION}-%{RPMTAG_RELEASE}",packagePath,false).firstLine();

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
PkgAttributeValue RpmDb::queryPackage(const char *format, string packageName, bool installed)
{
    PkgAttributeValue value;
    RpmArgVec opts(4);
    if(installed)
	opts[0] = "-q";
    else
	opts[0] = "-qp";
    opts[1] = "--queryformat";
    opts[2] = format;
    opts[3] = packageName.c_str();
    run_rpm(opts, ExternalProgram::Discard_Stderr);

  if ( process == NULL )
     return value;

  string line;
  char buffer[4096];
  size_t nread;
/*  while ( nread = process->receive(buffer, sizeof(buffer)), nread != 0)
    value.append(buffer, nread);
*/
  while(systemReadLine(line))
  {
    value.push_back(line);
  }
  systemStatus();
/*
  if ( value.length() >= 1 && value.at(value.length()-1) == ' ' )
  {
     if ( value.length() > 1 )
     {
	// remove last blank
	string dummy = value.substr(0,value.length()-1);
	value = dummy;
     }
     else
     {
	value = "";
     }
  }
  */

  return value;
}

#if 0
/*--------------------------------------------------------------*/
/* Evaluate all files of a package which have been changed	*/
/* since last installation or update.				*/
/*--------------------------------------------------------------*/
bool RpmDb::queryChangedFiles ( FileList &fileList, string packageName )
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
void RpmDb::run_rpm(const RpmArgVec& options,
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
bool RpmDb::systemReadLine(string &line)
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
int RpmDb::systemStatus()
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
void RpmDb::systemKill()
{
  if (process) process->kill();
}


// inststall package filename with flags iflags
bool RpmDb::installPackage(const string& filename, unsigned iflags)
{
    unsigned flags = iflags|_rpminstflags;

    RpmArgVec opts;

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

    //XXX maybe some log for the user too?
    DBG << "Installing " << filename << endl;

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
		ReportProgress(percent);
	    }
	}
	else
	    rpmmsg += line+'\n';
    }
    int rpm_status = systemStatus();
    if (rpm_status != 0)
    {
	ERR << "rpm failed, message was: " << rpmmsg << endl;
	return false;
    }
    return true;
}

// remove package named label
bool RpmDb::removePackage(const string& label, unsigned iflags)
{
    unsigned flags = iflags|_rpminstflags;

    RpmArgVec opts;

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

    return true;
}

string RpmDb::checkPackageResult2string(unsigned code)
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

/******************************************************************
**
**
**	FUNCTION NAME : operator<<
**	FUNCTION TYPE : std::ostream &
**
*/
std::ostream & operator<<( std::ostream & str, const RpmDb::DbStatus & obj )
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

