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

#include <set>
#include <string>
#include <list>
#include <vector>

#include <y2pm/RpmDbPtr.h>

#include <y2util/ExternalProgram.h>
#include <y2pm/PMSolvable.h>
#include <y2pm/PMPackagePtr.h>
#include <y2pm/PMRpmPackageDataProviderPtr.h>

/**
 * @short Interface to the rpm program
 */

enum DbStatus { DB_OK, DB_NOT_FOUND, DB_OLD_VERSION , DB_NEW_CREATED,
                DB_ERROR_CREATED, DB_ERROR_CHECK_OLD_VERSION };

class RpmDb: virtual public Rep
{
    REP_BODY(RpmDb)

    public:

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
	    RPMINST_IGNORESIZE = 0x10
	};

	/**
	 * Create an new instance.
	 * name_of_root The name of the install root
	 */
	RpmDb(std::string name_of_root );

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
	 * If Flag "createNew" is set, than it will be created, if not
	 * exist --> returns DbNewCreated if successfully created 
	 */
	DbStatus initDatabase( bool createNew = false);

	/**
	 * Creating a temporary rpm-database.
	 * If copyOldRpm == true than the rpm-database from
	 * /var/lib/rpm will be copied.
	 */
	bool createTmpDatabase(bool copyOldRpm = false );

	/**
	 * Installing the rpm-database to /var/lib/rpm, if the
	 * current has been created by "createTmpDatabase".
	 */
	bool installTmpDatabase( void );

	/** acquire data about installed packages
	 *
	 * @param pkglist where to store newly created PMPackages
	 * */
	bool getPackages (std::list<PMPackagePtr>& pkglist);

	/**
	 * Check rpm with rpm --checksig
	 * 
	 * @param filename which file to check
	 * @param version check if package really contains this version, leave emtpy to skip check
	 * @param md5 md5sum for whole file, leave empty to skip check (not yet implemented)
	 */
	bool checkPackage( std::string filename, std::string version = "", std::string md5 = "" );


	/**
	 * Returns the current-path of the rpm-DB
	 * */
	const std::string& queryCurrentDBPath ( void ) { return dbPath; };

	/**
	 * general query of a package
	 *
	 * @param format query format as rpm understands it
	 * @param packagelabel full label (name-version-relase)
	 * of the package to query. If you don't use the full label
	 * but only the name, the return value could be the result
	 * of multiple packages with different versions installed.
	 */
	std::string queryPackage(const char *format, std::string packagelabel);

	/** set parameters to use on installation/update
	 *
	 * @param flags which parameters to use by default, eg. RPMINST_NODOCS|RPMINST_NOSCRIPTS
	 * */
	void setInstFlags(unsigned flags) { _rpminstflags = flags; }
    
	/** install rpm package
	 *
	 * @param filename file to install
	 * @param flags which rpm options to use, will be or'ed with flags set by setInstFlags()
	 *
	 * @return success
	 * */
	bool installPackage(const std::string& filename,
	    unsigned flags = RPMINST_NODEPS|RPMINST_FORCE|RPMINST_IGNORESIZE );
	
	/** remove rpm package
	 *
	 * @param label label of the rpm package to remove. always specify the
	 * full label (name-version-release) as multiple packages with same
	 * name but different versions could be installed
	 * @param iflags which rpm options to use
	 *
	 * @return success
	 * */
	bool removePackage(const std::string& label, unsigned iflags);

	/** set callback function for reporting progress of package
	 * installation
	 *
	 * @param func callback function, must accept double as argument
	 * */
	void setProgressCallback(void (*func)(double))
	    { _progressfunc = func; }

    private:

	/** progress callback */
	void (*_progressfunc)(double);

	/** parameters to use on installation/update
	 * */
	unsigned _rpminstflags;

	/** dataprovider that is given to every created package
	 * */
	PMRpmPackageDataProviderPtr _dataprovider;

	/** 
	 * The name of the install root.
	 */
	std::string rootfs;


	/**
	 * current Path of the DB-path ( without "packages.rpm" )
	 */
	std::string dbPath; 

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

	/* parse string of the form name/number/version into rellist. number is
	 * the rpm number representing the operator <, <=, = etc. number&64
	 * means prerequires
	 *
	 * @param depstr string to evaluate
	 * @param deps reference to a list which will be cleared and filled with dependencies
	 * @param prereq reference to a list which holds prerequire
	 * dependencies
	 * @param ignore_prereqs if set to true, prereq will not be touched and
	 * all dependencies will be added to deps. Just pass a dummy list for
	 * prereq in this case
	 * */
	void rpmdeps2rellist ( const std::string& depstr,
			PMSolvable::PkgRelList_type& deps,
			PMSolvable::PkgRelList_type& prereq,
			bool ignore_prereqs = false);

	/** wrapper for _progressfunc, does nothing if it's unset
	 * */
	void ReportProgress(double p)
	    { if(_progressfunc != NULL) (*_progressfunc)(p); }
    
    public: // static members

	/** split string into tokens delimited by a one character
	 * seperator, empty fields will not be removed
	 *
	 * @param in string to tokenize
	 * @param sep separator character
	 * @out out vector of tokens
	 *
	 * @return number of tokens found
	 **/
	static unsigned tokenize(const std::string& in, char sep, std::vector<std::string>& out);
};

#endif
