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

   File:	MediaAccess.h

   Author:	Klaus Kaempf <kkaempf@suse.de>
   Maintainer:	Klaus Kaempf <kkaempf@suse.de>

   Purpose:	Handle access to a medium

/-*/
#ifndef MediaAccess_h
#define MediaAccess_h

#include <iosfwd>
#include <string>

#include <y2util/Pathname.h>
#include <y2util/PathInfo.h>
#include <y2util/Url.h>

#include <y2pm/MediaHandler.h>
#include <y2pm/MediaAccessPtr.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : MediaAccess
/**
 *
 **/
class REP_CLASS(MediaAccess) {

	REP_BODY(MediaAccess)

	typedef enum media_type {
	    Unknown = 0,
	    CD, DVD, NFS, DIR, DISK, FTP, SMB, HTTP, HTTPS
	} MediaType;

	// type of media
	MediaType _type;

	// destination directory for file retrieval
	Pathname _destination;

	// handler for 'physical' media
	// == 0 if not open
	MediaHandler * _handler;

    public:
	// constructor

	MediaAccess (void);

	// open url
	MediaResult open (const Url& url);

	// close url
	void close (void);

	// get Handler
	MediaHandler *handler (void) const;

	// get destination for file retrieval
	const Pathname & getAttachPoint (void) const;

	// attach media to directory
	MediaResult attachTo (const Pathname & to);

	// release attached media
	// if eject=true, physically eject media (i.e. CD-ROM)
	MediaResult release (bool eject = false);

	// provide file denoted by path to 'destination'
	// filename is interpreted relative to the attached url
	// and a path prefix is preserved to destination
	MediaResult provideFile (const Pathname & filename) const;

	// find file denoted by pattern
	// filename is interpreted relative to the attached url
	virtual const Pathname * findFile (const Pathname & dirname, const std::string & pattern) const;

	// get file information
	const std::list<std::string> * dirInfo (const Pathname & filename) const;

	// get file information
	const PathInfo * fileInfo (const Pathname & filename) const;

	// clean up a file from destination
	// if filename == "", the whole destination is cleared
	MediaResult cleanUp (const Pathname & filename) const;

	virtual ~MediaAccess();

    public:

	virtual std::ostream & dumpOn( std::ostream & str ) const;
};

///////////////////////////////////////////////////////////////////

#endif // MediaAccess_h

