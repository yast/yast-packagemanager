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

   File:	MediaInfo.cc

   Author:	Klaus Kaempf <kkaempf@suse.de>
   Maintainer:	Klaus Kaempf <kkaempf@suse.de>

   Purpose:	Handle media description and general information

/-*/
#include <stdio.h>

#include <ctype.h>

#include <y2pm/MediaInfo.h>
#if 0
///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : MediaInfo

// constructor

// initializes member elements by accessing media
// and retrieving data
// if full==true, retrieve full meta-data (product, vendor, ...)
// if full==false, just retrieve media-Id

MediaInfo::MediaInfo (MediaAccess *media, bool new_media)
    : _handler (media->handler())
{
    if (_handler)
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
}


~MediaInfo::MediaInfo()
{
    handler->release ();
}


// parse ".media" file
void
MediaInfo::parseMediaFile (const Pathname & filename, bool new_media)
{
    return;
}

// .S.u.S.E-disk-nnn.mmmmmmm file found
//

void
MediaInfo::parseSuSEFile (const Pathname & mountpoint, const Pathname & susefile, bool new_media)
{
    char buf[101];
    char *sptr = susefile.asString().c_str();
    chat *dot;

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
    _ID = string (sptr);

    if (!new_media)
	return;

    // if not CD1, dont expect further info
    if (_number != 1)
	return;

    // CD1, look for suse/setup/descr/info

    FILE *info = fopen ((mountpoint + "suse/setup/descr/info").asString().c_str(), "r");

fprintf (stderr, "MediaInfo::parseSuSEFile(%s) = %p\n", filename.asString().c_str(), info);
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
	    _release = value;
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

///////////////////////////////////////////////////////////////////

#endif
