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

   File:       Mount.cc
   Purpose:    Implement interface to mount program
   Author:     Ludwig Nussel <lnussel@suse.de>
   Maintainer: Ludwig Nussel <lnussel@suse.de>

/-*/


#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <iostream>
#include <string>
#include <fstream>

#include <y2util/ExternalDataSource.h>
#include <y2util/Y2SLog.h>
#include <y2pm/Mount.h>

#ifndef N_
#define N_(STR) STR
#endif

using namespace std;

Mount::Mount()
{
    process = 0;
    exit_code = -1;
}

Mount::~Mount()
{
   M__ <<  "~Mount()" << endl;

   if ( process )
      delete process;

   process = NULL;

   M__ << "~Mount() end" << endl;   
}

MediaResult Mount::mount ( const string& source,
			    const string& target,
			    const string& filesystem,
			    const string& options)
{
    const char *const argv[] = {
	"/bin/mount",
	"-t", filesystem.c_str(),
	"-o", options.c_str(),
	source.c_str(),
	target.c_str(),
	NULL
     };

    MediaResult ok = E_none;

    this->run(argv, ExternalProgram::Stderr_To_Stdout);

    if ( process == NULL )
    {
	return E_mount_failed;
    }

    string value;
    string output = process->receiveLine();

    // parse error messages
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

	if  ( value.find ( "is already mounted on" ) != string::npos )
	{
	    ok = E_already_mounted;
	}
	else if  ( value.find ( "ermission denied" ) != string::npos )
	{
	    ok = E_no_permission;
	}
	else if  ( value.find ( "wrong fs type" ) != string::npos )
	{
	    ok = E_invalid_filesystem;
	}


	output = process->receiveLine();            
    }

    int status = Status();

    if ( status == 0 && ok != E_none )
    {
	D__ << endl;
	// strange
	ok = E_none;
    }
    else if ( status != 0 && ok == E_none )
    {
	D__ << endl;
	ok = E_mount_failed;
    }

    return ( ok );
}

MediaResult Mount::umount (const string& path)
{
    const char *const argv[] = {
	"/bin/umount",
	path.c_str(),
	NULL
     };

    MediaResult ok = E_none;

    this->run(argv, ExternalProgram::Stderr_To_Stdout);

    if ( process == NULL )
    {
	return E_mount_failed;
    }

    string value;
    string output = process->receiveLine();

    // parse error messages
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

	if  ( value.find ( "not mounted" ) != string::npos )
	{
	//    ok = E_already_mounted;
	}

	if  ( value.find ( "device is busy" ) != string::npos )
	{
	    ok = E_busy;
	}

	output = process->receiveLine();            
    }

    int status = Status();

    if ( status == 0 && ok != E_none )
    {
	D__ << endl;
	// strange
	ok = E_none;
    }
    else if ( status != 0 && ok == E_none )
    {
	D__ << endl;
	ok = E_mount_failed;
    }

    return ( ok );
}

void Mount::run(const char *const *argv,
		       ExternalProgram::Stderr_Disposition disp)
{
  exit_code = -1;

  if ( process != NULL )
  {
     delete process;
     process = NULL;
  }
  // Launch the program

    for(int i=0;argv[i];i++) DBG << argv[i] << " ";
    DBG<<endl;
  process = new ExternalProgram(argv, disp, false, -1, true);
}

/*--------------------------------------------------------------*/
/* Return the exit status of the Mount process, closing the	*/
/* connection if not already done				*/
/*--------------------------------------------------------------*/
int Mount::Status()
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

/* Forcably kill the process */
void Mount::Kill()
{
  if (process) process->kill();
}
