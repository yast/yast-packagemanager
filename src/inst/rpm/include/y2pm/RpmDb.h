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

   File:       RpmDb.h
   Purpose:    Interface to installed RPM system
   Author:     Stefan Schubert  <schubi@suse.de>
   Maintainer: Ludwig Nussel <lnussel@suse.de>

   Copied and adapted from agent-targetpkg

/-*/

// -*- C++ -*-

#ifndef RpmDb_h
#define RpmDb_h

#include <fstream>
#include <set>
#include <string>
#include <list>
#include <vector>

#include <y2pm/RpmDbPtr.h>

#include <y2util/Pathname.h>
#include <y2util/ExternalProgram.h>

#include <y2pm/PMSolvable.h>
#include <y2pm/PMPackagePtr.h>

#include <y2pm/InstTargetError.h>

/**
 * @short Interface to the rpm program
 */

class RpmDb: virtual public Rep
{
    REP_BODY(RpmDb);

    private:
	// list of packages on the target
	// must be updated for each install/remove
	//
	// call getPackages() to refresh this list
	//
	std::list<PMPackagePtr> _packages;

    public:

        /**
	 * default error class
	 **/
        typedef InstTargetError Error;

	typedef std::set<std::string> FileList;

	/** Bits representing rpm installation options, useable as or
	 * combination
	 *
	 * @see installPackage(), removePackage()
	 * */
	enum RpmInstFlag
	{
	    RPMINST_NONE       = 0x00,
	    RPMINST_NODOCS     = 0x01,
	    RPMINST_NOSCRIPTS  = 0x02,
	    RPMINST_FORCE      = 0x04,
	    RPMINST_NODEPS     = 0x08,
	    RPMINST_IGNORESIZE = 0x10,
	    RPMINST_JUSTDB     = 0x20
	};

	/** Bits of possible package corruptions
	 * @see checkPackage
	 * @see checkPackageResult2string
	 * */
	enum checkPackageResult
	{
	    CHK_OK                = 0x00,
	    CHK_INCORRECT_VERSION = 0x01, // package does not contain expected version
	    CHK_INCORRECT_FILEMD5 = 0x02, // md5sum of file is wrong (outside)
	    CHK_GPGSIG_MISSING    = 0x04, // package is not signeed
	    CHK_MD5SUM_MISSING    = 0x08, // package is not signeed
	    CHK_INCORRECT_GPGSIG  = 0x10, // signature incorrect
	    CHK_INCORRECT_PKGMD5  = 0x20, // md5sum incorrect (inside)
	    CHK_OTHER_FAILURE     = 0x40  // rpm failed for some reason
	};

	/**
	 * Create an new instance.
	 * created at InstTarget constructor without knowledge of path
	 * see initDatabase() for the root path
	 */
	RpmDb();

	/**
	 * Clean up.
	 */
	~RpmDb();

	/**
	 * Check package, if it is correctly installed.
	 * Returns false, if an error has been occured.
	 * "filelist" is a list of missing files
	 */

//	bool checkPackage ( std::string packageName, FileList &fileList );

	/**
	 * Initialize the rpm database
	 *
	 * @param name_of_root The name of the install root
	 * @param createNew create a new database if none exists. This
	 * parameter does no harm if a database already exists.
	 */
	PMError initDatabase (std::string name_of_root, bool createNew = false);

	/**
	 * Rebuild the rpm database
	 * */
	PMError rebuildDatabase();

	/**
	 * Creating a temporary rpm-database.
	 * If copyOldRpm == true than the rpm-database from
	 * /var/lib/rpm will be copied.
	 *
	 * not yet implemented
	 */
	PMError createTmpDatabase(bool copyOldRpm = false );

	/**
	 * Installing the rpm-database to /var/lib/rpm, if the
	 * current has been created by "createTmpDatabase".
	 *
	 * not yet implemented
	 */
	PMError installTmpDatabase( void );

	/** acquire data about installed packages
	 *
	 * @param pkglist where to store newly created PMPackages
	 * */
	const std::list<PMPackagePtr>& getPackages ();

	/**
	 * Check rpm with rpm --checksig
	 *
	 * @param filename which file to check
	 * @param version check if package really contains this version, leave emtpy to skip check
	 * @param md5 md5sum for whole file, leave empty to skip check (not yet implemented)
	 *
	 * @return checkPackageResult
	 */
	unsigned checkPackage( std::string filename, std::string version = "", std::string md5 = "" );


	/**
	 * Returns the current-path of the rpm-DB
	 * */
	std::string queryCurrentDBPath ( void ) { return dbPath.asString(); };

	/**
	 * general query of an installed package
	 *
	 * @param package constPMPackagePtr to package
	 * @param format query format as rpm understands it
	 * @param result_r std::string& for single-line values
	 *	  or std::list<std::string>& for multi-line values
	 */
	bool queryPackage (constPMPackagePtr package, const char *format, std::string& result_r);
	bool queryPackage (constPMPackagePtr package, const char *format, std::list<std::string>& result_r);

	/**
	 * general query of an available package file
	 *
	 * @param path full path to an rpm file
	 * @param format query format as rpm understands it
	 * @param result_r std::string& for single-line values
	 *	  or std::list<std::string>& for multi-line values
	 */
	bool queryPackage (const Pathname& path, const char *format, std::string& result_r);
	bool queryPackage (const Pathname& path, const char *format, std::list<std::string>& result_r);

	/** install rpm package
	 *
	 * @param filename file to install
	 * @param flags which rpm options to use
	 *
	 * @return success
	 * */
	PMError installPackage(const std::string& filename, unsigned flags = 0 );

	/** remove rpm package
	 *
	 * @param label label of the rpm package to remove. always specify the
	 * full label (name-version-release) as multiple packages with same
	 * name but different versions could be installed
	 * @param iflags which rpm options to use
	 *
	 * @return success
	 * */
	PMError removePackage(const std::string& label, unsigned flags = 0);

	/** set callback function for reporting progress of package
	 * installation
	 *
	 * @param func callback function, must accept int as argument
	 * @param data arbitrary data to pass when function is called
	 * */
	void setProgressCallback(void (*func)(int,void*), void* data)
	{
	    _progressfunc = func;
	    _progressdata = data;
	}

	/**
	 * @return destination root directory
	 * */
	const std::string& getRoot() const { return _rootdir.asString(); }

	/**
	 * set logfile for progress log.
	 *
	 * @param filename file to log into, empty to disable logging
	 *
	 * @return true if file was successfully opened
	 * */
	bool setInstallationLogfile( const std::string& filename );

    private:

	/** progress callback */
	void (*_progressfunc)(int,void*);

	/** arbitrary data to pass back for progress callback */
	void* _progressdata;

	/**
	 * The name of the install root.
	 */
	Pathname _rootdir;

	/**
	 * current Path of the DB-path ( without "packages.rpm" )
	 */
	Pathname dbPath;

	/*
	 * Flag that it is a temporary rpm-DB
	 */
	bool temporary;;

	/**
	 * The connection to the rpm process.
	 */
	ExternalProgram *process;

	typedef std::vector<const char*> RpmArgVec;

	/**
	 * Run rpm with the specified arguments and handle stderr.
	 * @param n_opts The number of arguments
	 * @param options Array of the arguments, @ref n_opts elements
	 * @param stderr_disp How to handle stderr, merged with stdout by default
	 */
	void run_rpm(const RpmArgVec& options,
	ExternalProgram::Stderr_Disposition stderr_disp =
	ExternalProgram::Stderr_To_Stdout);


	/**
	 * Read a line from the general rpm query
	 */
	bool systemReadLine(std::string &line);

	/**
	 * Return the exit status of the general rpm process,
	 * closing the connection if not already done.
	 */
	int systemStatus();

	/**
	 * Forcably kill the system process
	 */
	void systemKill();

	/**
	 * The exit code of the rpm process, or -1 if not yet known.
	 */
	int exit_code;

	/** /var/lib/rpm */
	Pathname _varlibrpm;

	/** /var/lib */
	Pathname _varlib;

	/** packages.rpm */
	Pathname _rpmdbname;

	/** whether a new database was created */
	bool _creatednew;

	/** whether an old database is present */
	bool _old_present;

	/** whether rpmdb is ready to use */
	bool _initialized;

	/** progress of installation will be logged here */
	std::ofstream _progresslogstream;

	/* parse string of the form name/number/version into rellist. number is
	 * the rpm number representing the operator <, <=, = etc. number&64
	 * means prerequires
	 *
	 * @param depstr string to evaluate
	 * @param deps reference to a list which will be cleared and filled with dependencies
	 * */
	void rpmdeps2rellist ( const std::string& depstr,
			PMSolvable::PkgRelList_type& deps);

	/** wrapper for _progressfunc, does nothing if it's unset
	 * */
	void ReportProgress(int p)
	    { if(_progressfunc != NULL) (*_progressfunc)(p,_progressdata); }

	/** helper for queryPackage
	 * */
	bool queryPackage (const std::string& package, const char *qparam, const char *format, std::string& result_r);
	bool queryPackage (const std::string& package, const char *qparam, const char *format, std::list<std::string>& result_r);

	/**
	 * handle rpm messages like "/etc/testrc saved as /etc/testrc.rpmorig"
	 *
	 * @param line rpm output starting with warning:
	 * @param name name of package, appears in subject line
	 * @param typemsg " saved as " or " created as "
	 * @param difffailmsg what to put into mail if diff failed, must contain two %s for the two files
	 * @param diffgenmsg what to put into mail if diff succeeded, must contain two %s for the two files
	 * */
	void processConfigFiles(const std::string& line, const std::string& name, const char* typemsg, const char* difffailmsg, const char* diffgenmsg);

    public: // static members

	/** split string into tokens delimited by a one character
	 * seperator, empty fields will not be removed
	 *
	 * @param in string to tokenize
	 * @param sep separator character
	 * @param max produce up to this number of tokens, zero for unlimited
	 * @out out vector of tokens
	 *
	 * @return number of tokens found
	 **/
	static unsigned tokenize(const std::string& in, char sep, unsigned max, std::vector<std::string>& out);

	/** create error description of bits set according to
	 * checkPackageResult
	 * */
	static std::string checkPackageResult2string(unsigned code);
};


/**
 * Write enum value as string
 **/
//extern std::ostream & operator<<( std::ostream & str, const RpmDb::DbStatus & obj );

#endif
