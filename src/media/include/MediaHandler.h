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

#include <y2util/Pathname.h>
#include <y2util/PathInfo.h>
#include <y2util/Attribute.h>

#include <MediaResult.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : MediaHandler
/**
 *
 **/
class MediaHandler {

    protected:

	const string & _device;		// which device
	const string & _path;		// path on device
	Pathname _attachedTo;		// attached at

	// scan directory 'dirname' for first file matching pattern
	const Pathname *scanDirectory (const Pathname & dirname, const string & pattern) const;

    public:
	// constructor

	MediaHandler (const string & device, const string & path);

	// attach media at path
	virtual MediaResult attach (const Pathname & to) = 0;

	// return current attach directory
	virtual Pathname & getAttachPoint (void);

	// release attached media
	virtual MediaResult release (void) = 0;

	// get file denoted by path to 'attached path'
	// filename is interpreted relative to the attached url
	// and a path prefix is preserved to destination
	virtual MediaResult getFile (const Pathname & filename) const = 0;

	// find file denoted by pattern
	// filename is interpreted relative to the attached url
	virtual const Pathname * findFile (const Pathname & dirname, const string & pattern) const = 0;

	// get directory denoted by path to Attribute::A_StringArray
	virtual const Attribute * dirInfo (const Pathname & dirname) const = 0;

	// get file information
	virtual const PathInfo * fileInfo (const Pathname & filename) const = 0;

	virtual ~MediaHandler();

    public:

	virtual std::ostream & dumpOn( std::ostream & str ) const;
};

///////////////////////////////////////////////////////////////////

#define	MEDIA_HANDLER_API						\
	MediaResult attach (const Pathname & path);			\
	MediaResult release (void);					\
	MediaResult getFile (const Pathname & filename) const;		\
	const Pathname * findFile (const Pathname & dirname, const string & pattern) const;	\
	const Attribute * dirInfo (const Pathname & dirname) const;\
	const PathInfo * fileInfo (const Pathname & filename) const;


#endif // MediaHandler_h

