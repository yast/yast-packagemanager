/*---------------------------------------------------------------------\
|                                                                      |
|                      __   __    ____ _____ ____                      |
|                      \ \ / /_ _/ ___|_   _|___ \                     |
|                       \ V / _` \___ \ | |   __) |                    |
|                        | | (_| |___) || |  / __/                     |
|                        |_|\__,_|____/ |_| |_____|                    |
|                                                                      |
|                               core system                            |
|                                                     (C) 2002 SuSE AG |
\----------------------------------------------------------------------/

   File:       Wget.h
   Purpose:    Declare interface to mount program
   Author:     Ludwig Nussel <lnussel@suse.de>
   Maintainer: Ludwig Nussel <lnussel@suse.de>

/-*/


// -*- C++ -*-

#ifndef mount_h
#define mount_h
#include <set>
#include <string>

#include <y2pm/MediaResult.h>
#include <y2util/ExternalProgram.h>

/**
 * @short Interface to the mount program
 */

class Mount
{
    public:

	/**
	* Create an new instance.
	*/
	Mount();

	/**
	* Clean up.
	*/
	~Mount();

	/**
	* mount device
	*
	* @param source what to mount (e.g. /dev/hda3)
	* @param target where to mount (e.g. /mnt)
	* @param filesystem which filesystem to use (e.g. reiserfs) (-t parameter)
	* @param options mount options (e.g. ro) (-o parameter)
	*/

	MediaResult mount (const std::string& source,
		  const std::string& target,
		  const std::string& filesystem,
		  const std::string& options);

	/** umount device
	 *
	 * @param path device or mountpoint to umount
	 * */
	MediaResult umount (const std::string& path);

    private:

	/** The connection to the mount process.
	 * */
	ExternalProgram *process;

	/**
	 * Run mount with the specified arguments and handle stderr.
	 * @param n_opts The number of arguments
	 * @param options Array of the arguments, @ref n_opts elements
	 * @param stderr_disp How to handle stderr, merged with stdout by default
	 * */
	void run(const char *const *argv,
			  ExternalProgram::Stderr_Disposition stderr_disp =
			  ExternalProgram::Stderr_To_Stdout);

	/** Return the exit status of the process, closing the connection if
	 * not already done.
	 * */
	int Status();

	/** Forcably kill the process
	 * */
	void Kill();


	/** The exit code of the process, or -1 if not yet known.
	 * */
	int exit_code;
};

#endif
