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

#include <y2util/ExternalProgram.h>
#include <y2pm/PMSolvable.h>
#include <y2pm/PMPackagePtr.h>

/**
 * @short Interface to the rpm program
 */

enum DbStatus { DB_OK, DB_NOT_FOUND, DB_OLD_VERSION , DB_NEW_CREATED,
                DB_ERROR_CREATED, DB_ERROR_CHECK_OLD_VERSION };

class RpmDb
{
    public:

	typedef std::set<std::string> FileList;

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

	bool checkPackage ( std::string packageName, FileList &fileList );

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
	 * FIXME whats the version good for?
	 */
	bool checkPackage( std::string packagePath, std::string version = "" );


	/**
	 * Returns the current-path of the rpm-DB
	 * */
	const std::string& queryCurrentDBPath ( void ) { return dbPath; };

    private:

	/**
	 * general query of a package
	 *
	 * @param format query format as rpm understands it
	 * @param packagelabel full label (name-version-relase) of the package to query
	 */
	std::string queryPackage(const char *format, std::string packagelabel);

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

	/**
	 * Run rpm with the specified arguments and handle stderr.
	 * @param n_opts The number of arguments
	 * @param options Array of the arguments, @ref n_opts elements
	 * @param stderr_disp How to handle stderr, merged with stdout by default
	 */
	void run_rpm(int n_opts, const char *const *options,
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
    public:
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
