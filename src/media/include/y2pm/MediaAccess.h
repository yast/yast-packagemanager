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

#include <y2pm/MediaError.h>
#include <y2pm/MediaHandler.h>
#include <y2pm/MediaAccessPtr.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : MediaAccess
/**
 * @short Handle access to a medium
 **/
class MediaAccess : virtual public Rep {
	REP_BODY(MediaAccess);

    public:

        /**
	 * default error class
	 **/
        typedef MediaError Error;

    private:

	enum MediaType {
	    Unknown = 0,
	    CD, DVD, NFS, DIR, DISK, FTP, SMB, HTTP, HTTPS
	};

	/**
	 * type of media
	 **/
	MediaType _type;

	/**
	 * handler for 'physical' media
	 * == 0 if not open
	 **/
	MediaHandler * _handler;

    public:

       /**
        * constructor
        **/
	MediaAccess (void);

	/**
	 * open url
	 **/
	PMError open (const Url& url);

	/**
	 * True if media is open
	 **/
	bool isOpen() const { return( _handler != 0 ); }

	/**
	 * close url
	 **/
	void close (void);

	/**
	 * attach media
	 **/
	PMError attach (void);

	/**
	 * get destination for file retrieval
	 **/
	const Pathname & getAttachPoint (void) const;

	/**
	 * release attached media
	 * if eject=true, physically eject media (i.e. CD-ROM)
	 **/
	PMError release (bool eject = false);

	/**
	 * provide file denoted by path to 'destination'
	 * filename is interpreted relative to the attached url
	 * and a path prefix is preserved to destination
	 **/
	PMError provideFile (const Pathname & filename) const;

	/**
	 * find file denoted by pattern
	 * filename is interpreted relative to the attached url
	 **/
	virtual const Pathname * findFile (const Pathname & dirname, const std::string & pattern) const;

	/**
	 * get file information
	 **/
	const std::list<std::string> * dirInfo (const Pathname & filename) const;

	/**
	 * get file information
	 **/
	const PathInfo * fileInfo (const Pathname & filename) const;

	/**
	 * clean up a file from destination
	 * if filename == "", the whole destination is cleared
	 **/
	PMError cleanUp (const Pathname & filename) const;

	virtual ~MediaAccess();

    public:

	virtual std::ostream & dumpOn( std::ostream & str ) const;
};

///////////////////////////////////////////////////////////////////

#endif // MediaAccess_h

