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
#include <iostream.h>
#include <string.h>
#include <fstream>

#include "Wget.h"
#include <y2/ExternalDataSource.h>
#include <ycp/y2log.h>

#define WRONGUSER "401 Unauthorized"
#define NOTFOUND "404 Not Found"

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
    user = "";
    password = "";
    proxyUser = "";
    proxyPassword = "";
    process = 0;
    exit_code = -1;
}

/*--------------------------------------------------------------*/
/* Cleans up						       	*/
/*--------------------------------------------------------------*/
Wget::~Wget()
{
   y2milestone ( "~Wget()" );

   if ( process )
      delete process;

   process = NULL;

   y2milestone ( "~Wget() end" );   
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
	    ret = "ok";
	    break;
	case WGET_ERROR_FILE:
	case WGET_ERROR_CONNECT:
	    ret = "error_path";
	    break;
	case WGET_ERROR_LOGIN:
	    ret = "error_login";
	    break;
	default:
	    ret = "error_path";
	    break;
    }
    
    return ret;
}

/*--------------------------------------------------------------*/
/* Set password and user					*/
/*--------------------------------------------------------------*/
void Wget::setUser( const string username,
			  const string passwd )
{
    user = username;
    password = passwd;
}

/*--------------------------------------------------------------*/
/* Set password and user of a proxy				*/
/*--------------------------------------------------------------*/
void Wget::setProxyUser( const string username,
			 const string passwd )
{
    proxyUser = username;
    proxyPassword = passwd;
}


/*--------------------------------------------------------------*/
/* Retrieving a file 						*/
/*--------------------------------------------------------------*/
WgetStatus Wget::getFile ( const string url, const string destFilename )
{
   WgetStatus ok = WGET_OK;

   const char *const opts[] = {
       "-O", destFilename.c_str(), url.c_str()
     };

   create_directories( destFilename );

   run_wget(sizeof(opts) / sizeof(*opts), opts,
	    ExternalProgram::Stderr_To_Stdout);

   if ( process == NULL )
       return WGET_ERROR_CONNECT;
   
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

      y2debug ( "stdout: %s", value.c_str() );

      if  ( value.find ( WRONGUSER ) != string::npos )
      {
	  // wget returns always ohne unauthorize error message
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
      output = process->receiveLine();            
   }

   if ( systemStatus() != 0
	&& ok == WGET_OK )
   {
       ok = WGET_ERROR_CONNECT;
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
  int argc = n_opts + 5 /* wget --http-user=<user> --http-passwd=<password> */
                        /*  --proxy-user=user  --porxy-passwd=password */
             + 1 /* NULL */;

  // Create the argument array
  const char *argv[argc];
  int i = 0;
  string passwd = "--http-passwd=" + password;
  string usr = "--http-user=" + user;
  string proxyUsr = "--proxy-user=" + proxyUser;
  string proxyPasswd = "--proxy-passwd=" + proxyPassword;
  
  argv[i++] = "wget";
  if ( user != "" )
  {
      argv[i++] = usr.c_str();
  }
  else
  {
      argv[i++] = "";      
  }
  if ( password != "" )
  {
      argv[i++] = passwd.c_str();
  }
  else
  {
      argv[i++] = "";      
  }

  if ( proxyUser != "" )
  {
      argv[i++] = proxyUsr.c_str();
  }
  else
  {
      argv[i++] = "";      
  }
  if ( proxyPassword != "" )
  {
      argv[i++] = proxyPasswd.c_str();
  }
  else
  {
      argv[i++] = "";      
  }

  
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
  y2debug("wget command: %s", output.c_str() );

  if ( process != NULL )
  {
     delete process;
     process = NULL;
  }
  // Launch the program
  process = new ExternalProgram(argv, disp);
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
   
   y2debug( "exit code: %d", exit_code );

   return exit_code;
}

/*--------------------------------------------------------------*/
/* Forcably kill the wget process				*/
/*--------------------------------------------------------------*/
void Wget::systemKill()
{
  if (process) process->kill();
}
