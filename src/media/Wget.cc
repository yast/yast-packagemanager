/*************************************************************
 *
 *     YaST2      SuSE Labs                        -o)
 *     --------------------                        /\\
 *                                                _\_v
 *           www.suse.de / www.suse.com
 * ----------------------------------------------------------
 *
 * File:	  Wget.cc
 *
 * Author: 	  Stefan Schubert <schubi@suse.de>
 *
 * Description:   Handling of the wget request
 *
 * $Header$
 *
 *************************************************************/

/*
 * $Log$
 * Revision 1.9  2002/09/09 13:56:12  lnussel
 * - remove proxy settings from wget class, works automatically through ~/.wgetrc
 * - make wget function that only accepts strings private, adapt PMYouPatchPaths.cc
 * - use saveAsString in InstSrcDescr
 *
 * Revision 1.8  2002/09/05 12:57:04  cschum
 * Add optional Cookie support to Wget class.
 * Support Cookies when getting the server list from www.suse.de.
 *
 * Revision 1.7  2002/09/04 10:03:18  cschum
 * Don't truncate destination file in case of a download error.
 *
 * Revision 1.6  2002/09/04 09:22:42  cschum
 * Implemented user/password authentification.
 *
 * Revision 1.5  2002/08/12 13:26:27  cschum
 * Return ok, when the http server sent an ok.
 *
 * Revision 1.4  2002/08/02 11:11:27  ma
 * Malplaced 'N_()' macro removed.
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
 */

#include <sys/stat.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <fstream>

#include <y2util/ExternalDataSource.h>
#include <y2util/Y2SLog.h>
#include <y2util/PathInfo.h>

#include <y2pm/Wget.h>

#define HTTP_OK "200 OK"
#define WRONGUSER "401 Unauthorized"
#define AUTHREQUIRED "401 Authorization Required"
#define NOTFOUND "404 Not Found"
#define PROXYAUTH "407 Proxy Authentication Required"
#define SERVERERROR "500 Internal Server Error"

#define CONNREFUSED "failed: Connection refused"

#ifndef N_
#define N_(STR) STR
#endif

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
/* creates a Wget					       */
/*-------------------------------------------------------------*/
Wget::Wget()
{
    process = 0;
    exit_code = -1;
}

/*--------------------------------------------------------------*/
/* Cleans up						       	*/
/*--------------------------------------------------------------*/
Wget::~Wget()
{
   M__ <<  "~Wget()" << endl;

   if ( process )
      delete process;

   process = NULL;

   M__ << "~Wget() end" << endl;
}

/*--------------------------------------------------------------*/
/* Error status to string					*/
/*--------------------------------------------------------------*/
string Wget::error_string ( WgetStatus status )
{
    string ret = "";

    switch ( status )
    {
	case WGET_OK:
	    ret = N_("Ok");
	    break;
	case WGET_ERROR_FILE:
	    ret = N_("file not found");
	    break;
	case WGET_ERROR_CONNECT:
	    ret = N_("connection failed");
	    break;
	case WGET_ERROR_LOGIN:
	    ret = N_("login failed");
	    break;
	case WGET_ERROR_PROXY_AUTH:
	    ret = N_("proxy authentication required");
	    break;
	case WGET_ERROR_SERVER:
	    ret = N_("remote server error");
	    break;
	case WGET_ERROR_INVALID_URL:
	    ret = N_("invalid URL");
	    break;
	default:
	    ret = N_("unexpected error");
	    break;
    }

    return ret;
}

void Wget::setCookiesFile( const string &filename )
{
    _cookiesFile = filename;
}

WgetStatus Wget::getFile( const Url &url_i, const Pathname &destination )
{
    Url url(url_i);
    if ( !url.isValid() ) return WGET_ERROR_INVALID_URL;

    string username = url.getUsername();
    string password = url.getPassword();

    // *ugh* wget treats path as relative with only one slash and prepends the
    // users home dir on the ftp server for non-anonymous connections
    url.setPath(string("/") + url.getPath());
    
    if ( !username.empty() && !password.empty() ) {
        string host = url.getHost();

	// clear username and password to make them not appear in command line
	url.setUsername(string());
	url.setPassword(string());

        string home = getenv( "HOME" );

        string netrcStr = "machine " + host + " login " + username +
                          " password " + password;
        string netrcFile = home + "/.netrc";
        // Create name for backup file
        string netrcBackup = home + "/.netrc.orig.yast.76545";

        bool netrcExists = PathInfo( netrcFile ).isExist();
        if ( netrcExists ) rename( netrcFile.c_str(), netrcBackup.c_str() );

        ofstream out( netrcFile.c_str() );
        out << netrcStr << endl;

        WgetStatus status = getFile( url.asString(), destination.asString() );

        unlink( netrcFile.c_str() );
        if ( netrcExists ) rename( netrcBackup.c_str(), netrcFile.c_str() );

        return status;
    } else {
        return getFile( url.asString(), destination.asString() );
    }
}

/*--------------------------------------------------------------*/
/* Retrieving a file 						*/
/*--------------------------------------------------------------*/
WgetStatus Wget::getFile ( const string& url, const string& destFilename )
{
   WgetStatus ok = WGET_OK;

   // Create name for temporary file used as original download detination.
   string dest = destFilename + ".new.yast.65675";

   const char *const opts[] = {
       "-O", dest.c_str(), url.c_str()
   };

   create_directories( dest );

   run_wget(sizeof(opts) / sizeof(*opts), opts,
	    ExternalProgram::Stderr_To_Stdout);

   if ( process == NULL ) {
       unlink( dest.c_str() );
       return WGET_ERROR_UNEXPECTED;
   }

   string value;
   string output = process->receiveLine();
   bool firstUnauthorized = false;

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

      D__ << "stdout: " << value << endl;

      if  ( value.find ( WRONGUSER ) != string::npos )
      {
	  // wget returns always one unauthorize error message
	  if ( !firstUnauthorized )
	  {
	      firstUnauthorized = true;
	  }
	  else
	  {
	      ok = WGET_ERROR_LOGIN;
	  }
      }

      if  ( value.find ( NOTFOUND ) != string::npos )
      {
	  ok = WGET_ERROR_FILE;
      }
      if  ( value.find ( AUTHREQUIRED ) != string::npos )
      {
	  ok = WGET_ERROR_LOGIN;
      }
      if  ( value.find ( PROXYAUTH ) != string::npos )
      {
	  ok = WGET_ERROR_PROXY_AUTH;
      }
      if  ( value.find ( SERVERERROR ) != string::npos )
      {
	  ok = WGET_ERROR_SERVER;
      }
      if  ( value.find ( CONNREFUSED ) != string::npos )
      {
	  ok = WGET_ERROR_CONNECT;
      }
      if ( value.find( HTTP_OK ) != string::npos )
      {
	  ok = WGET_OK;
      }

	// ftp
      if  ( value.find ( "The server refuses login" ) != string::npos )
      {
	  ok = WGET_ERROR_LOGIN;
      }
      else if  ( value.find ( "No such directory" ) != string::npos
		|| value.find ( "No such file" ) != string::npos )
      {
	  ok = WGET_ERROR_FILE;
      }

      output = process->receiveLine();
   }

   if ( systemStatus() != 0
	&& ok == WGET_OK )
   {
       ok = WGET_ERROR_UNEXPECTED;
   }

   if ( ok == WGET_OK ) {
       rename( dest.c_str(), destFilename.c_str() );
   } else {
       unlink( dest.c_str() );
   }

   return ( ok );
}



/****************************************************************/
/* private member-functions					*/
/****************************************************************/

/*--------------------------------------------------------------*/
/* Run wget with the specified arguments, handling stderr	*/
/* as specified  by disp					*/
/*--------------------------------------------------------------*/
void Wget::run_wget(int n_opts, const char *const *options,
		       ExternalProgram::Stderr_Disposition disp)
{
  exit_code = -1;
  int argc = n_opts + 1 /** wget */ + 2  /* tries, waitretry */ + 1 /** 0 */;

  if ( !_cookiesFile.empty() ) {
      argc += 2;
  }

  // Create the argument array
  const char *argv[argc];
  int i = 0;
  string loadCookies;
  string saveCookies;

  argv[i++] = "wget";

  argv[i++] = "--tries=3";
  argv[i++] = "--waitretry=2";

  if ( !_cookiesFile.empty() ) {
      loadCookies = "--load-cookies=" + _cookiesFile;
      saveCookies = "--save-cookies=" + _cookiesFile;
      argv[i++] = loadCookies.c_str();
      argv[i++] = saveCookies.c_str();
  }

  for (int j = 0; j < n_opts; j++)
  {
    argv[i++] = options[j];
  }

  string output = "";
  int k;
  for ( k = 0; k < i; k++ )
  {
     output = output + " " + argv[k];
  }
  argv[i++] = 0;

  if(i>argc)
      INT << "argument count exceeded" << endl;

  D__ << "wget command: " << output << endl;

  if ( process != NULL )
  {
     delete process;
     process = NULL;
  }
  // Launch the program
  process = new ExternalProgram(argv, disp, false, -1, true);
}

/*--------------------------------------------------------------*/
/* Read a line from the wget process				*/
/*--------------------------------------------------------------*/
bool Wget::systemReadLine(string &line)
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
/* Return the exit status of the wget process, closing the	*/
/* connection if not already done				*/
/*--------------------------------------------------------------*/
int Wget::systemStatus()
{
   if ( process == NULL )
      return -1;

   exit_code = process->close();
   process->kill();
   delete process;
   process = 0;

   D__ << "exit code: " << exit_code << endl;

   return exit_code;
}

/*--------------------------------------------------------------*/
/* Forcably kill the wget process				*/
/*--------------------------------------------------------------*/
void Wget::systemKill()
{
  if (process) process->kill();
}
