/*************************************************************
 *    
 *     YaST2      SuSE Labs                        -o)
 *     --------------------                        /\\
 *                                                _\_v
 *           www.suse.de / www.suse.com
 * ----------------------------------------------------------
 *
 * Author: 	  Stefan Schubert  <schubi@suse.de>
 *
 * File:	  wget.h
 * Description:   main header file for wget repuests
 *
 * $Header$
 *
 *************************************************************/
   
// -*- C++ -*-

#ifndef wget_h
#define wget_h
#include <set>
#include <string>

#include <y2util/ExternalProgram.h>
#include <y2util/Url.h>
#include <y2util/Pathname.h>

/**
 * @short Interface to the wget program
 */

enum WgetStatus { WGET_OK, WGET_ERROR_FILE, 
		  WGET_ERROR_LOGIN, WGET_ERROR_CONNECT,
		  WGET_ERROR_PROXY_AUTH,
		  WGET_ERROR_SERVER,
		  WGET_ERROR_UNEXPECTED,
                  WGET_ERROR_INVALID_URL };


class Wget
{
public:

  /**
   * Create an new instance.
   */
  Wget();

  /**
   * Clean up.
   */
  ~Wget();

  /**
    Get a file. Uses username and password, if included in given url.
  */
  WgetStatus getFile( const Url &url, const Pathname &destination );
   
  /**
    Set file used to load and save cookies. If this isn't set cookies aren't
    handled.
  */
  void setCookiesFile( const std::string &filename );

  /**
   * Error status to string
   */
  std::string error_string ( WgetStatus status );
    
private:

  std::string _cookiesFile;
    
  /**
   * The connection to the wget process.
   */
  ExternalProgram *process;

  /**
   * Run wget with the specified arguments and handle stderr.
   * @param n_opts The number of arguments
   * @param options Array of the arguments, @ref n_opts elements
   * @param stderr_disp How to handle stderr, merged with stdout by default
   */
  void run_wget(int n_opts, const char *const *options,
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

  /**
   * Retrieving a file 
   */
  WgetStatus getFile ( const std::string& url, const std::string& destFilename );
};

#endif
