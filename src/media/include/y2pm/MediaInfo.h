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

   File:	MediaInfo.h

   Author:	Klaus Kaempf <kkaempf@suse.de>
   Maintainer:	Klaus Kaempf <kkaempf@suse.de>

   Purpose:	Deduce media description and general information
		from MediaHandler

		Helper class for InstSrc to retrieve media
		data from a physical media (MediaHandler)

/-*/
#ifndef MediaInfo_h
#define MediaInfo_h

#include <iosfwd>
#include <string>

#include <y2pm/MediaInfoPtr.h>
#include <y2pm/MediaAccess.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : MediaInfo
class REP_CLASS(MediaInfo) {
	REP_BODY(MediaInfo);

	std::string		_ID;			// Media ID
	std::string		_number;		// current number
	std::string		_count;			// total number
	std::string		_product;		// product name
	std::string		_version;		// product version
	std::string		_release;		// product release
	std::string		_vendor;		// media vendor
	std::list<std::string>	_architectures;		// supported architectures
	std::string		_label;			// (translated) label

	MediaHandler *_handler;				// physical media handler

	/**
	 * helper function to parse new .media file
	 */
	void parseMediaFile (const Pathname & filename, bool new_media);

	/**
	 * helper function to parse old suse/setup/descr/info file
	 */
	void parseSuSEFile (const Pathname & mountpoint, const Pathname & susefile, bool new_media);

    public:

	/**
	 * Constructor
	 * Initialize class and attach to handler for physical media
	 * This will immediately try to access the media and
	 * retrieve as much data as possible.
	 * It's up to the caller to interprete the data (and possibly
	 * destroy the object instance and ask for a different media
	 * via MediaAccess.
	 * @param media		Pointer to media class for access
	 * @param new_media	== false, if just ID and number are needed
	 *			(media should be known, just check for right media)
	 *			== true, to retrieve full media information
	 *			from a new media
	 */
	MediaInfo (MediaAccess *media, bool new_media = false);

	// always valid
	const std::string & getID (void) const { return _ID; }
	const std::string & getNumber (void) const { return _number; }

	// only valid if new_media == true in constructor
	const std::string & getCount (void) const { return _count; }
	const std::string & getProduct (void) const { return _product; }
	const std::string & getVersion (void) const { return _version; }
	const std::string & getRelease (void) const { return _release; }
	const std::string & getVendor (void) const { return _vendor; }
	const std::list<std::string> & getArchitectures (void) const { _architectures; }
	const std::string & getLabel (void) const { return _label; }

	~MediaInfo();

    public:

	std::ostream & dumpOn( std::ostream & str ) const;
};

///////////////////////////////////////////////////////////////////

#endif // MediaInfo_h

