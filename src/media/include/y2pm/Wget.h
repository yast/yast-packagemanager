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
   
/*
 * $Log$
 * Revision 1.5  2002/09/05 12:57:06  cschum
 * Add optional Cookie support to Wget class.
 * Support Cookies when getting the server list from www.suse.de.
 *
 * Revision 1.4  2002/09/04 09:22:42  cschum
 * Implemented user/password authentification.
 *
 * Revision 1.3  2002/07/05 12:05:13  lnussel
 * catch more wget errors
 *
 * Revision 1.2  2002/07/02 15:32:45  lnussel
 * added testprogram for ftp method, can already retreive files
 *
 * Revision 1.1  2002/07/02 09:27:25  lnussel
 * fix namespaces to make it compile
 *
 * Revision 1.2  2001/12/06 15:15:38  schubi
 * proxy support added
 *
 * Revision 1.1  2001/10/31 09:07:30  schubi
 * agent for http transfer
 *
 * Revision 1.1.2.2  2001/10/16 15:17:23  schubi
 * new call error_string
 *
 * Revision 1.1.2.1  2001/10/12 13:13:04  schubi
 * new class for http transfer( tested )
 *
 *
 *
 */

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
   * Retrieving a file 
   */
  WgetStatus getFile ( const std::string url, const std::string destFilename );

  /**
    Get a file. Uses username and password, if included in given url.
  */
  WgetStatus getFile( const Url &url, const Pathname &destination );
   
  /**
   * Set password and user of the proxy
   */
  void setProxyUser( const std::string username, const std::string passwd);
    
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

  /** 
   * proxy user 
   */
    std::string proxyUser;
    std::string proxyPassword;

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
};

#endif
