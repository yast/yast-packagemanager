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

   File:	MediaDIR.cc

   Author:	Klaus Kaempf <kkaempf@suse.de>
   Maintainer:	Klaus Kaempf <kkaempf@suse.de>

   Purpose:	Implementation class for MediaHandler
		This class handles access to a directory
/-*/

#include <iostream>

#include <y2util/Y2SLog.h>
#include <y2pm/MediaDIR.h>

#include <sys/mount.h>
#include <errno.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : MediaDIR
//
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaDIR::MediaDIR
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
MediaDIR::MediaDIR (const Url& url)
    : MediaHandler (url)
{
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaDIR::~MediaDIR
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
MediaDIR::~MediaDIR()
{
    if (_attachPoint != "") {	// release if still mounted
	release ();
    }
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaDIR::dumpOn
//	METHOD TYPE : ostream &
//
//	DESCRIPTION :
//
ostream &
MediaDIR::dumpOn( ostream & str ) const
{
    return MediaHandler::dumpOn(str);
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaDIR::attachTo
//	METHOD TYPE : MediaResult
//
//	DESCRIPTION : attach media at path
//
MediaResult
MediaDIR::attachTo (const Pathname & to)
{
    // make directory 'present'
    _attachPoint = to;

    return E_none;
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaDIR::attach
//	METHOD TYPE : MediaResult
//
//	DESCRIPTION : release attached media
//
MediaResult
MediaDIR::release (bool eject)
{
    // make directory 'vanish'
    _attachPoint = "";
    return E_none;
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaDIR::provideFile
//	METHOD TYPE : MediaResult
//
//	DESCRIPTION :
//	provide file denoted by path to 'attached path'
//	filename is interpreted relative to the attached url
//	and a path prefix is preserved to destination

MediaResult
MediaDIR::provideFile (const Pathname & filename) const
{
    // no retrieval needed, DIR is mounted at destination

    Pathname src = _attachPoint + filename;
    PathInfo info(src);
    
    if(!info.isFile())
    {
	    D__ << src.asString() << " does not exist" << endl;
	    return E_system;
    }
    return E_none;
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaDIR::findFile
//	METHOD TYPE : MediaResult
//
//	DESCRIPTION :
//	find file denoted by pattern
//	filename is interpreted relative to the attached url
//
//	pattern is a string with an optional trailing '*'
//
const Pathname *
MediaDIR::findFile (const Pathname & dirname, const string & pattern) const
{
    return scanDirectory (dirname, pattern);
}


///////////////////////////////////////////////////////////////////
//
//	METHOD NAME : MediaDIR::getDirectory
//	METHOD TYPE : const std::list<std::string> *
//
//	DESCRIPTION :
//	get directory denoted by path to Attribute::A_StringArray

const std::list<std::string> *
MediaDIR::dirInfo (const Pathname & dirname) const
{
    return readDirectory (dirname);
}



///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaDIR::getInfo
//	METHOD TYPE : const PathInfo *
//
 //	DESCRIPTION :
//	get file information

const PathInfo *
MediaDIR::fileInfo (const Pathname & filename) const
{
    // no retrieval needed, DIR is mounted at destination
    return new PathInfo (filename);
}

