/*---------------------------------------------------------------------\
|                                                                      |
|                      __   __    ____ _____ ____                      |
|                      \ \ / /_ _/ ___|_   _|___ \                     |
|                       \ V / _` \___ \ | |   __) |                    |
|                        | | (_| |___) || |  / __/                     |
|                        |_|\__,_|____/ |_| |_____|                    |
|                                                                      |
|                               core system                            |
|                                                        (C) SuSE GmbH |
\----------------------------------------------------------------------/

   File:	MediaHandler.cc

   Author:	Klaus Kaempf <kkaempf@suse.de>
   Maintainer:	Klaus Kaempf <kkaempf@suse.de>

   Purpose:	Abstract base class for 'physical' media access

/-*/

#include <iostream>

#include <MediaHandler.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : MediaHandler
//
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaHandler::MediaHandler
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
MediaHandler::MediaHandler (const string & device, const string & path)
    : _device (device)
    , _path (path)
    , _attachedTo (string (""))
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaHandler::~MediaHandler
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
MediaHandler::~MediaHandler()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaHandler::dumpOn
//	METHOD TYPE : ostream &
//
//	DESCRIPTION :
//
ostream & MediaHandler::dumpOn( ostream & str ) const
{
    str << "MediaHandler (" << _device << ":" << _path << ")";
    return str;
}

///////////////////////////////////////////////////////////////////
// PROTECTED
//
//
//	METHOD NAME : MediaHandler::scanDirectory
//	METHOD TYPE : ostream &
//
//	DESCRIPTION :
//
const Pathname *
MediaHandler::scanDirectory (const Pathname & dirname, const string & pattern) const
{
    // prepend mountpoint to dirname
    Pathname *fullpath = new Pathname (_attachedTo + dirname);

    // open mounted directory
    DIR *dir = opendir (fullpath->asString().c_str());
    struct dirent *entry;
    if (dir == 0)
    {
	return 0;
    }

    // scan directory

    while ((entry = readdir (dir)) != 0)
    {
	char *dptr = entry->d_name;		// directory name pointer
	const char *pptr = pattern.c_str();		// pattern pointer

	// match pattern

	while ((*dptr != 0)
		&& (*pptr != 0))
	{
	    if (*dptr == *pptr)		// pattern matches
	    {
		dptr++;
		pptr++;
	    }
	    else if (*pptr == '*')	// wildcard matches
	    {
		pptr++;			// assume '*' at end of pattern
		break;
	    }
	    else
		break;			// no match
	}

	if (*pptr == 0)			// match !
	{
	    *fullpath += entry->d_name;
	    closedir (dir);
	    return fullpath;
	}
    }
    closedir (dir);
    return 0;		// no match
}
