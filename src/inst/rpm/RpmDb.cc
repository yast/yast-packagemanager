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

#include <string>
#include <list>
#include <vector>

#include <y2util/Y2SLog.h>
#include <y2util/TagParser.h>
#include <y2util/ExternalDataSource.h>
#include <y2pm/RpmDb.h>
#include <y2pm/PkgEdition.h>
#include <y2pm/PkgRelation.h>
#include <y2pm/PMPackage.h>

#define ORIGINALRPMPATH "/var/lib/rpm/"
#define RPMPATH "/var/lib/"
#define RPMDBNAME "packages.rpm"

using namespace std;

/*-------------------------------------------------------------*/
/* Create all parent directories of @param name, as necessary  */
/*-------------------------------------------------------------*/
static void
create_directories(string name)
{
  size_t pos = 0;

  while (pos = name.find('/', pos + 1), pos != string::npos)
    mkdir (name.substr(0, pos).c_str(), 0777);
}



/****************************************************************/
/* public member-functions					*/
/****************************************************************/

/*-------------------------------------------------------------*/
/* creates a RpmDb					       */
/*-------------------------------------------------------------*/
RpmDb::RpmDb(string name_of_root)
{
   rootfs = name_of_root;
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

   if ( temporary )
   {
      // Removing all files of the temporary DB
      string command = "rm -R ";

      command += dbPath;
      ::system ( command.c_str() );
   }

   M__  << "~RpmDb() end" << endl;
}


/*--------------------------------------------------------------*/
/* Initialize the rpm database					*/
/* If Flag "createNew" is set, than it will be created, if not	*/
/* exist --> returns DbNewCreated if successfully created 	*/
/*--------------------------------------------------------------*/
DbStatus RpmDb::initDatabase( bool createNew )
{
    string       dbFilename = rootfs;
    struct stat  dummyStat;
    DbStatus	 dbStatus = DB_OK;

    DBG << "calling initDatabase" << endl;

    dbFilename = dbFilename + ORIGINALRPMPATH + RPMDBNAME;
    if (  stat( dbFilename.c_str(), &dummyStat ) != -1 )
    {
       // DB found
       dbPath = ORIGINALRPMPATH;
       DBG << "Setting dbPath to " << dbPath.c_str() << endl;
    }
    else
    {
       ERR << "dbFilename not found " << dbFilename.c_str() << endl;

       // DB not found
       dbStatus = DB_NOT_FOUND;

       if ( createNew )
       {
	  // New rpm-DB will be created
	  create_directories(rootfs + ORIGINALRPMPATH);
	  const char *const opts[] = { "--initdb" };
	  run_rpm(sizeof(opts) / sizeof(*opts), opts);
	  if ( systemStatus() != 0 )
	  {
	     // error
	     dbStatus = DB_ERROR_CREATED;
	  }
	  else
	  {
	     dbStatus = DB_NEW_CREATED;
	  }
       }
    }

    if ( dbStatus == DB_OK )
    {
       // Check, if it is an old rpm-Db
       const char *const opts[] = { "-q", "rpm" };
       string output;

       run_rpm(sizeof(opts) / sizeof(*opts), opts);
       if ( !systemReadLine ( output ) )
       {
	  // error
	  dbStatus = DB_ERROR_CHECK_OLD_VERSION;
	  ERR << "Error occured while checking old version." << endl;
       }
       else
       {
	  if ( output.find ( "old format database is present" ) !=
	       string::npos )
	  {
	     dbStatus = DB_OLD_VERSION;
	     WAR <<  "RPM-Db on the system is old"  << endl;
	  }
	  else
	  {
	     if ( systemStatus() != 0 )
	     {
		// error
		dbStatus = DB_ERROR_CHECK_OLD_VERSION;
		ERR << "Error occured while checking old version."  << endl;
	     }
	  }
       }
    }

    return dbStatus;
}

/*--------------------------------------------------------------*/
/* Creating a temporary rpm-database.				*/
/* If copyOldRpm == true than the rpm-database from		*/
/* /var/lib/rpm will be copied.					*/
/*--------------------------------------------------------------*/
bool RpmDb::createTmpDatabase ( bool copyOldRpm )
{
   // searching a non-existing rpm-path
   int counter = 0;
   struct stat  dummyStat;
   string rpmPath;
   string saveDbPath = dbPath;
   bool ok = true;
   char number[10];

   number[0] = 0;

   rpmPath = rootfs + RPMPATH + "rpm.new";
   for ( counter = 0; 
	counter < 1000 && stat( rpmPath.c_str(), &dummyStat ) != -1;
	counter++)
   {
      // search free rpm-path
      snprintf ( number, 10, "%d", counter);
      rpmPath = rootfs + RPMPATH + "rpm.new." + number;
   }

   if ( mkdir ( rpmPath.c_str(), S_IRWXU ) == -1 )
   {
      ok = false;
      ERR << "ERROR command: mkdir " << rpmPath.c_str() << endl;

   }

   // setting global dbpath
   dbPath = RPMPATH;
   if ( counter == 0 )
   {
      dbPath = dbPath + "rpm.new";
   }
   else
   {
      dbPath = dbPath + "rpm.new." + number;
   }

   if ( ok )
   {
      const char *const opts[] = { "--initdb" };
      run_rpm(sizeof(opts) / sizeof(*opts), opts);
      if ( systemStatus() != 0 )
      {
	 // error
	 ok = false;
	 ERR << "ERROR command: rpm --initdb  --dbpath " <<
		  dbPath.c_str() << endl;
      }
   }

   if ( ok && copyOldRpm )
   {
      // copy old RPM-DB into temporary RPM-DB

      string command = "cp -a ";
      command = command + rootfs + ORIGINALRPMPATH + "* " +
	 rpmPath;

      if ( system ( command.c_str() ) == 0 )
      {
	 ok = true;
      }
      else
      {
	 ok = false;
	 ERR << "ERROR command: " << command.c_str() << endl;
      }

      if ( ok )
      {
	 const char *const opts[] = { "--rebuilddb" };
	 run_rpm(sizeof(opts) / sizeof(*opts), opts);
	 if ( systemStatus() != 0 )
	 {
	    // error
	    ok = false;
	    ERR << "ERROR command: rpm --rebuilddb  --dbpath " <<
		     dbPath.c_str() << endl;
	 }
      }
   }

   if ( ok )
   {
      temporary = true;
   }
   else
   {
      // setting global dbpath
      dbPath = saveDbPath;
   }

   return ( ok );
}

/*--------------------------------------------------------------*/
/* Installing the rpm-database to /var/lib/rpm, if the		*/
/* current has been created by "createTmpDatabase".		*/
/*--------------------------------------------------------------*/
bool RpmDb::installTmpDatabase( void )
{
   bool ok = true;
   string oldPath;
   struct stat  dummyStat;
   int counter = 1;

   DBG << "calling installTmpDatabase" << endl;

   if ( !temporary  )
   {
      DBG << "RPM-database have not to be updated." << endl;
      return ( true );
   }

   if ( dbPath.length() <= 0 )
   {
      ERR << "RPM-DB is not initialized." << endl;
      return ( false );
   }

   if ( ok )
   {
      // creating path for saved rpm-DB
      oldPath = rootfs + RPMPATH + "rpm.old";
      while ( counter < 1000 && stat( oldPath.c_str(), &dummyStat ) != -1 )
      {
	 // search free rpm-path
	 char number[10];
	 snprintf ( number, 10, "%d", counter++);
	 oldPath = rootfs + RPMPATH + "rpm.old." + number;
      }

      if ( mkdir ( oldPath.c_str(), S_IRWXU ) == -1 )
      {
	 ERR << "ERROR command: mkdir %s" << oldPath.c_str() << endl;
	 ok = false;
      }
   }

   if ( ok )
   {
      // saving old rpm
      string command = "cp -a ";
      command = command + rootfs + ORIGINALRPMPATH + "* " +oldPath;

      if ( system ( command.c_str() ) == 0)
      {
	 ok = true;
      }
      else
      {
	 ERR << "ERROR command: " << command.c_str() << endl;
	 ok = false;
      }
   }


   if ( ok )
   {
      string command = "cp -a ";
      command = command + rootfs + dbPath + "/* " +
	 rootfs + ORIGINALRPMPATH;

      if ( system ( command.c_str() ) == 0)
      {
	 ok = true;
      }
      else
      {
	 ERR << "ERROR command: " << command.c_str() << endl;
	 ok = false;
      }
   }

   if ( ok )
   {
      // remove temporary RPM-DB
      string command = "rm -R ";

      command += rootfs + dbPath;
      system ( command.c_str() );

      temporary = false;
      dbPath = ORIGINALRPMPATH;
   }

   return ( ok );
}

#if 0
/*--------------------------------------------------------------*/
/* Evaluate all installed packages				*/
/* Returns false, if an error has been occured.			*/
/*--------------------------------------------------------------*/
bool RpmDb::getInstalledPackages ( PackList &packageList )
{
   bool ok = true;

   const char *const opts[] = {
      "-qa",  "--queryformat", "%{RPMTAG_NAME}\\n"
     };

   packageList.clear();

   run_rpm(sizeof(opts) / sizeof(*opts), opts,
	   ExternalProgram::Discard_Stderr);

   if ( process == NULL )
      return false;

   string value;

   string output = process->receiveLine();

   while ( output.length() > 0 )
   {
      string::size_type ret;

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

      packageList.insert ( value );
      output = process->receiveLine();
   }

   if ( systemStatus() != 0 )
   {
      ok = false;
   }

   return ( ok );
}

#endif

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
	
	PkgRelation dep(cdep_Ci.name,cdep_Ci.compare,cdep_Ci.version.c_str());
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
bool RpmDb::getPackages (std::list<PMPackagePtr>& pkglist)
{
    string rpmquery;

    // this enum tells the position in rpmquery string
    enum {
	RPM_NAME,
	RPM_VERSION,
	RPM_RELEASE,
	RPM_INSTALLTIME,
	RPM_BUILDTIME,
	RPM_GROUP,
	RPM_ARCH,
	RPM_REQUIRES,
	RPM_PROVIDES,
	RPM_OBSOLETES,
	RPM_CONFLICTS,

	NUM_RPMTAGS	
    };

    rpmquery += "%{RPMTAG_NAME};%{RPMTAG_VERSION};%{RPMTAG_RELEASE};";
    rpmquery += "%{RPMTAG_INSTALLTIME};%{RPMTAG_BUILDTIME};%{RPMTAG_GROUP};";
    rpmquery += "%{RPMTAG_ARCH};";
    rpmquery += "[%{REQUIRENAME},%{REQUIREFLAGS},%{REQUIREVERSION},];";
    rpmquery += "[%{PROVIDENAME},%{PROVIDEFLAGS},%{PROVIDEVERSION},];";
    rpmquery += "[%{OBSOLETENAME},%{OBSOLETEFLAGS},%{OBSOLETEVERSION},];";
    rpmquery += "[%{CONFLICTNAME},%{CONFLICTFLAGS},%{CONFLICTVERSION},]";
    rpmquery += "\\n";

    const char* const opts[] =
    {
	"-q",
	"-a",
	"--queryformat",
	rpmquery.c_str()
    };

    run_rpm(sizeof(opts) / sizeof(*opts), opts,
	ExternalProgram::Discard_Stderr);

    if(!process)
	return false;

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
				pkgattribs[RPM_NAME],
				edi,
				pkgattribs[RPM_ARCH]);

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
	    D__ << p << endl;
	}
	
	output = process->receiveLine();
    }
    
    return true;
}

#if 0
/*--------------------------------------------------------------*/
/* Evaluate all installed packages WITH all Information		*/
/* Returns false, if an error has been occured.			*/
/*--------------------------------------------------------------*/
bool RpmDb::getInstalledPackagesInfo ( InstalledPackageMap &packageMap )
{
   bool ok = true;

   const char *const opts1[] = {
      "-qa",  "--queryformat", "%{RPMTAG_NAME};%{RPMTAG_VERSION}-%{RPMTAG_RELEASE};%{RPMTAG_INSTALLTIME};%{RPMTAG_BUILDTIME};%{RPMTAG_GROUP}\\n"
     };

   const char *const opts2[] = {
      "-qa",  "--queryformat", "%{RPMTAG_NAME};%{OBSOLETES};%{PROVIDES}\\n"
     };

   const char *const opts3[] = {
      "-qa",  "--queryformat", "%{RPMTAG_NAME};[ %{REQUIRENAME} %{REQUIREFLAGS} %{REQUIREVERSION}];[ %{CONFLICTNAME} %{CONFLICTFLAGS} %{CONFLICTVERSION}] \\n"
     };

   packageMap.clear();

   run_rpm(sizeof(opts1) / sizeof(*opts1), opts1,
	   ExternalProgram::Discard_Stderr);

   if ( process == NULL )
      return false;

   string value;

   string output = process->receiveLine();

   while ( output.length() > 0 )
   {
      string::size_type ret;

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

      // parse line
      string::size_type begin;
      string::size_type end;
      const string 	seperator(";");
      int counter = 1;
      InstalledPackageElement package;

      begin = value.find_first_not_of ( seperator );
      while ( begin != string::npos )
      {
	 // each entry separated by ;

	 end = value.find_first_of ( seperator, begin );
	 if ( end == string::npos )
	 {
	    // end of line
	    end = value.length();
	 }
	 switch ( counter )
	 {
	    case 1:
	       package.packageName = value.substr ( begin, end-begin );
	       break;
	    case 2:
	       package.version = value.substr ( begin, end-begin );
	       break;
	    case 3:
	       package.installtime = atol(
				(value.substr ( begin, end-begin )).c_str());
	       break;
	    case 4:
	       package.buildtime = atol(
				(value.substr ( begin, end-begin )).c_str());
	       break;
	    case 5:
	       package.rpmgroup = value.substr ( begin, end-begin );
	       break;
	   default:
	      break;
	 }
	 counter++;
	 // next entry
	 begin = value.find_first_not_of ( seperator, end );
      }

      packageMap.insert(pair<const string, const InstalledPackageElement >
			( package.packageName, package ) );
      output = process->receiveLine();
   }

   if ( systemStatus() != 0 )
   {
      ok = false;
   }

   // Evaluate obsoletes and provides

   if ( ok )
   {
         run_rpm(sizeof(opts2) / sizeof(*opts2), opts2,
	   ExternalProgram::Discard_Stderr);

	 if ( process == NULL )
	    ok = false;
   }

   if ( ok )
   {

      output = process->receiveLine();

      while ( output.length() > 0 )
      {
	 string::size_type ret;

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

	 // parse line
	 string::size_type begin;
	 string::size_type end;
	 const string 	seperator(";");
	 int counter = 1;
	 InstalledPackageElement *package = NULL;
	 InstalledPackageMap::iterator pos;

	 begin = value.find_first_not_of ( seperator );
	 while ( begin != string::npos )
	 {
	    // each entry separated by ;

	    end = value.find_first_of ( seperator, begin );
	    if ( end == string::npos )
	    {
	       // end of line
	       end = value.length();
	    }
	    switch ( counter )
	    {
	       case 1:
		  pos = packageMap.find ( value.substr ( begin, end-begin ) );
		  if ( pos != packageMap.end() )
		  {
		     package = &(pos->second);
		  }
		  else
		  {
		     package = NULL;
		  }
		  break;
	    case 2:
	       if ( package != NULL &&
		    value.substr ( begin, end-begin ) != "(none)" )
	       {
		  package->obsoletes = value.substr ( begin, end-begin );
	       }
	       break;
	    case 3:
	       if ( package != NULL &&
		    value.substr ( begin, end-begin ) != "(none)" )
	       {
		  package->provides = value.substr ( begin, end-begin );
	       }
	       break;
	    default:
	       break;
	    }
	    counter++;
	    // next entry
	    begin = value.find_first_not_of ( seperator, end );
	 }
	 output = process->receiveLine();
      }
   }

   if ( systemStatus() != 0 )
   {
      ok = false;
   }


   // Evaluate requires and conflicts

   if ( ok )
   {
         run_rpm(sizeof(opts3) / sizeof(*opts3), opts3,
	   ExternalProgram::Discard_Stderr);

	 if ( process == NULL )
	    ok = false;
   }

   if ( ok )
   {
      output = process->receiveLine();

      while ( output.length() > 0 )
      {
	 string::size_type ret;

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

	 // parse line
	 string::size_type begin;
	 string::size_type end;
	 const string 	seperator(";");
	 int counter = 1;
	 InstalledPackageElement *package = NULL;
	 InstalledPackageMap::iterator pos;

	 begin = value.find_first_not_of ( seperator );
	 while ( begin != string::npos )
	 {
	    // each entry separated by ;

	    end = value.find_first_of ( seperator, begin );
	    if ( end == string::npos )
	    {
	       // end of line
	       end = value.length();
	    }
	    switch ( counter )
	    {
	       case 1:
		  pos = packageMap.find ( value.substr ( begin, end-begin ) );
		  if ( pos != packageMap.end() )
		  {
		     package = &(pos->second);
		  }
		  else
		  {
		     package = NULL;
		  }
		  break;
	    case 2:
	       if ( package != NULL )
	       {
		  package->requires = value.substr ( begin, end-begin );
	       }
	       break;
	    case 3:
	       if ( package != NULL )
	       {
		  package->conflicts = value.substr ( begin, end-begin );
	       }
	       break;
	    default:
	       break;
	    }
	    counter++;
	    // next entry
	    begin = value.find_first_not_of ( seperator, end );
	 }

	 output = process->receiveLine();
      }
   }

   if ( systemStatus() != 0 )
   {
      ok = false;
   }

   return ( ok );
}
#endif

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
bool RpmDb::checkPackage( string packagePath, string version )
{
    bool ok = true;

    bool md5ok = false;
    bool gpgok = false;


   if ( version != "" )
   {
      // Checking Version
      const char *const opts[] = {
	 "-qp", "--qf", "%{RPMTAG_VERSION}-%{RPMTAG_RELEASE} ",
	 packagePath.c_str()
      };
      run_rpm(sizeof(opts) / sizeof(*opts), opts,
	      ExternalProgram::Discard_Stderr);

      if ( process == NULL )
	 return false;

      string value;
      char buffer[4096];
      size_t nread;
      while ( nread = process->receive(buffer, sizeof(buffer)), nread != 0)
	 value.append(buffer, nread);
      if ( systemStatus() != 0 )
      {
	 // error
	 ok = false;
      }

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
      DBG <<  "comparing version " << version << " <-> " << value << endl;
      if ( version != value )
      {
	 ok = false;
      }
   }

    if ( ok )
    {
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
	    ok = false;
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
		string rest = value.substr(pos+1);
		if(rest.find("NOT OK") == string::npos)
		{
		    if(rest.find("md5") != string::npos)
		    {
			md5ok = true;
		    }
		    if(rest.find("gpg") != string::npos)
		    {
			gpgok = true;
		    }
		    else
			DBG << "gpg signature missing" << endl;
		}
		else
		{
		    md5ok = gpgok = false;
		}
	    }

	    output = process->receiveLine();
	}

	if ( systemStatus() != 0 )
	{
	    // error
	    ok = false;
	}
    }

    return ( ok && md5ok && gpgok );
}


#if 0
/*--------------------------------------------------------------*/
/* Query Version of a package.					*/
/* Returns "" if an error has been occured.			*/
/*--------------------------------------------------------------*/
string RpmDb::queryPackageVersion( string packageName )
{
  return queryPackage("%{RPMTAG_VERSION}-%{RPMTAG_RELEASE} ", packageName );
}


/*--------------------------------------------------------------*/
/* Query Release of a package.					*/
/* Returns "" if an error has been occured.			*/
/*--------------------------------------------------------------*/
string RpmDb::queryPackageRelease( string packageName )
{
  return queryPackage("%{RPMTAG_RELEASE} ", packageName );
}


/*--------------------------------------------------------------*/
/* Query installation-time of a package.			*/
/* Returns 0 if an error has been occured.			*/
/*--------------------------------------------------------------*/
long RpmDb::queryPackageInstallTime( string packageName )
{
  string installTime = queryPackage("%{RPMTAG_INSTALLTIME} ", packageName );

  return ( atol ( installTime.c_str() ) );
}


/*--------------------------------------------------------------*/
/* Query build-time of a package.				*/
/* Returns 0 if an error has been occured.			*/
/*--------------------------------------------------------------*/
long RpmDb::queryPackageBuildTime( string packageName )
{
  string buildTime = queryPackage("%{RPMTAG_BUILDTIME} ", packageName );

  return ( atol ( buildTime.c_str() ) );
}


/*--------------------------------------------------------------*/
/* Query summary of a package.					*/
/* Returns "" if an error has been occured.			*/
/*--------------------------------------------------------------*/
string RpmDb::queryPackageSummary( string packageName )
{
  return queryPackage("%{RPMTAG_SUMMARY} ", packageName );
}

#endif

/*--------------------------------------------------------------*/
/* Query the current package using the specified query format	*/
/*--------------------------------------------------------------*/
string RpmDb::queryPackage(const char *format, string packageName)
{

  const char *const opts[] = {
    "-q", "--qf", format,  packageName.c_str()
  };
  run_rpm(sizeof(opts) / sizeof(*opts), opts, ExternalProgram::Discard_Stderr);

  if ( process == NULL )
     return "";

  string value;
  char buffer[4096];
  size_t nread;
  while ( nread = process->receive(buffer, sizeof(buffer)), nread != 0)
    value.append(buffer, nread);
  systemStatus();

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
void RpmDb::run_rpm(int n_opts, const char *const *options,
		       ExternalProgram::Stderr_Disposition disp)
{
  exit_code = -1;
  int argc = n_opts + 5 /* rpm --root <root> --dbpath <path> */
             + 1 /* NULL */;

  // Create the argument array
  const char *argv[argc];
  int i = 0;
  argv[i++] = "rpm";
  argv[i++] = "--root";
  argv[i++] = rootfs.c_str();
  argv[i++] = "--dbpath";
  argv[i++] = dbPath.c_str();
  for (int j = 0; j < n_opts; j++)
  {
    argv[i++] = options[j];
  }

  string output = "";
  int k;
  for ( k = 0; k < argc-1; k++ )
  {
     output = output + " " + argv[k];
  }
  argv[i] = 0;
  DBG << "rpm command: " << output << endl;

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
   DBG << endl;
   if ( process == NULL )
      return -1;

   exit_code = process->close();
   process->kill();
   delete process;
   process = 0;

   D__ << "exit code " << exit_code << endl;

  return exit_code;
}

/*--------------------------------------------------------------*/
/* Forcably kill the rpm process				*/
/*--------------------------------------------------------------*/
void RpmDb::systemKill()
{
  if (process) process->kill();
}
