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

   File:       InstSrcDescr.cc

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

   Purpose:	installation source description
		keeps all information to identify an installation
		source, but does not have knowledge about its contents
		see InstSrcData.h for the contents
/-*/

#include <iostream>

#include <y2pm/InstSrcDescr.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : InstSrcDescr
//
///////////////////////////////////////////////////////////////////

IMPL_HANDLES(InstSrcDescr);

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcDescr::InstSrcDescr
//	METHOD TYPE : Constructor
//
//	DESCRIPTION : initialization with new media
//
InstSrcDescr::InstSrcDescr (MediaAccess *media)
{
    MediaHandler _handler = media->handler;
    if (_handler != 0)
    {
	Pathname mountpoint = _handler->getAttachPoint();

	// try new-style ".media" file first

	Pathinfo *info = _handler->fileInfo (".media");
	if (info != 0)
	{
	    // parse new ".media" file
	    parseMediaFile (mountpoint, new_media);
	}
	else
	{
	    // parse old "suse/setup/descr/info" file
	    Pathname susefile = _handler->findFile (".S.u.S.E-disk-*");
	    if (susefile != "")
	    {
		parseSuSEFile (mountpoint, susefile, new_media);
	    }
	}
    }

    return;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcDescr::InstSrcDescr
//	METHOD TYPE : Constructor
//
//	DESCRIPTION : initialization with known media
InstSrcDescr::InstSrcDescr (const Pathname & contentfile)
{
    return;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcDescr::~InstSrcDescr
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
InstSrcDescr::~InstSrcDescr()
{
}


void
InstSrcDescr::parseSuSEFile (const Pathname & mountpoint, const Pathname & susefile, bool new_media)
{
    char buf[101];
    const char *sptr = susefile.asString().c_str();
    char *dot;

    sptr += 14;		// skip ".S.u.S.E-disk-"

    // find CD number "NNN."

    _number = 0;
    while (isdigit (*sptr))
    {
	_number *= 10;
	_number += (*sptr - '0');
	sptr++;
    }

    if (*sptr != '.')		// '.' is separator between number and ID
	return;
    sptr++;
    _id = string (sptr);

    if (!new_media)
	return;

    // if not CD1, dont expect further info
    if (_number != 1)
	return;

    // CD1, look for suse/setup/descr/info

    FILE *info = fopen ((mountpoint + "suse/setup/descr/info").asString().c_str(), "r");

fprintf (stderr, "parseSuSEFile(%s) = %p\n", filename.asString().c_str(), info);
    if (info == 0)
	return;

    while (!feof (info))
    {
	char *ptr;
	char *value;

	if (fgets (buf, 100, info) != buf)
	{
	    break;
	}
	ptr = buf;
	while (*ptr)			// skip keywork
	{
	    if ((*ptr == ' ')
		|| (*ptr == '\t'))
	    {
		*ptr++ = 0;
		break;
	    }
	    ptr++;
	}
	while ((*ptr == ' ')		// skip whitespace
		|| (*ptr == '\t'))
	{
	    ptr++;
	}
	value = ptr;

	while (*ptr)
	{
	    if (*ptr == '\n')		// delete trailing \n
	    {
		*ptr = 0;
		break;
	    }
	    ptr++;
	}

	// check keys, copy value

	if (strcmp (key, "PRODUKT_NAME") == 0)
	{
	    _product = value;
	}
	else if (strcmp (key, "PRODUKT_VERSION") == 0)
	{
	    _version = value;
	}
	else if (strcmp (key, "DISTRIBUTION_RELEASE") == 0)
	{
	    _release = atoi (value);
	}
	else if (strcmp (key, "DIST_STRING") == 0)
	{
	    _vendor = value;
	}
	else if (strcmp (key, "DISTRIBUTION_NAME") == 0)
	{
	    _label = value;
	}
    } // while

    fclose (info);

    return;
}



/**
 * write media description to cache file
 * @return pathname of written cache
 * writes private data to an ascii file
 */
const Pathname
InstSrcDescr::writeCache (void)
{
    return Pathname ("");	// empty == error
}



///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcDescr::dumpOn
//	METHOD TYPE : ostream &
//
//	DESCRIPTION :
//
ostream & InstSrcDescr::dumpOn( ostream & str ) const
{
  Rep::dumpOn( str );
  return str;
}


