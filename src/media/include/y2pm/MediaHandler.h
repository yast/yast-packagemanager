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

   File:	MediaHandler.h

   Author:	Klaus Kaempf <kkaempf@suse.de>
   Maintainer:	Klaus Kaempf <kkaempf@suse.de>

   Purpose:	Abstract base class for 'physical' media access

/-*/
#ifndef MediaHandler_h
#define MediaHandler_h

#include <iosfwd>
#include <string>
#include <list>

#include <y2util/Pathname.h>
#include <y2util/PathInfo.h>
#include <y2util/Url.h>

#include <y2pm/MediaError.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : MediaHandler
/**
 *
 **/
class MediaHandler {

    public:

        /**
	 * default error class
	 **/
        typedef MediaError Error;

    protected:

	const Url _url;	// which device

	Pathname _attachPoint;		// attached at

	/** scan directory 'dirname' for first file matching pattern
	 * */
	const Pathname *scanDirectory (const Pathname & dirname, const std::string & pattern) const;

	/** read directory 'dirname' completely to string list
	 * */
	const std::list<std::string> * readDirectory (const Pathname & dirname) const;

    public:
	// constructor
	MediaHandler (const Url& url);

	/** attach media at path */
	virtual PMError attachTo (const Pathname & to) = 0;

	/** return current attach directory */
	virtual Pathname & getAttachPoint (void) { return _attachPoint; }

	/** release attached media
	 *
	 * @param eject if true, physically eject the media * (i.e. CD-ROM)
	 * */
	virtual PMError release (bool eject = false) = 0;

	/** provide file denoted by path at 'attached path' filename is
	 * interpreted relative to the attached url and a path prefix is
	 * preserved to destination
	 * */
	virtual PMError provideFile (const Pathname & filename) const = 0;

	/** find file denoted by pattern
	 * filename is interpreted relative to the attached url
	 * */
	virtual const Pathname * findFile (const Pathname & dirname, const std::string & pattern) const = 0;

	/** get directory denoted by path to a string list
	 * */
	virtual const std::list<std::string> * dirInfo (const Pathname & dirname) const = 0;

	/** get file information
	 * */
	virtual const PathInfo * fileInfo (const Pathname & filename) const = 0;

	virtual ~MediaHandler();

    public:

	virtual std::ostream & dumpOn( std::ostream & str ) const;
};

///////////////////////////////////////////////////////////////////

#define	MEDIA_HANDLER_API						\
	PMError attachTo (const Pathname & path);			\
	PMError release (bool eject = false);			\
	PMError provideFile (const Pathname & filename) const;	\
	const Pathname * findFile (const Pathname & dirname, const std::string & pattern) const;	\
	const std::list<std::string> * dirInfo (const Pathname & dirname) const;\
	const PathInfo * fileInfo (const Pathname & filename) const;


#endif // MediaHandler_h

