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
#include <y2/ExternalProgram.h>

/**
 * @short Interface to the wget program
 */

enum WgetStatus { WGET_OK, WGET_ERROR_FILE, 
		  WGET_ERROR_LOGIN, WGET_ERROR_CONNECT };


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
   
  WgetStatus getFile ( const string url, const string destFilename );
   
  /**
   * Set password and user
   */
    
  void setUser( const string username, const string passwd);

  /**
   * Set password and user of the proxy
   */
    
  void setProxyUser( const string username, const string passwd);
    
  /**
   * Error status to string
   */
  string error_string ( WgetStatus status );
    
private:

   
  /** 
   * user
   */
    string user;
    string password;

  /** 
   * proxy user 
   */
    string proxyUser;
    string proxyPassword;

    
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
  bool systemReadLine(string &line);

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
