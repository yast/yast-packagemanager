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

   Purpose:	Handle media description and general information

/-*/
#ifndef MediaInfo_h
#define MediaInfo_h

#include <iosfwd>
#include <string>

#include <y2util/Attribute.h>

#include <MediaInfoPtr.h>
#include <MediaHandler.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : MediaInfo
/**
 * MediaInfo hold information about a medium
 **/
class REP_CLASS(MediaInfo) {
	REP_BODY(MediaInfo);

	string	_ID;
	int	_number;
	int	_count;
	string	_product;
	string	_version;
	string	_release;
	string  _vendor;
	int	_architectures;
	string	_label;

	MediaHandler *_handler;

	void parseMediaFile (const Pathname & filename);
	void parseSuSEFile (const Pathname & mountpoint, const Pathname & susefile);

    public:

	typedef enum info_types {
	    I_Id, I_Number, I_Count, I_Product,
	    I_Version, I_Release, I_Vendor,
	    I_Architectures, I_Label
	} info_attr;

	MediaInfo (MediaHandler *handler);

	~MediaInfo();

	Attribute * attribute (info_attr what) const;

    public:

	std::ostream & dumpOn( std::ostream & str ) const;
};

///////////////////////////////////////////////////////////////////

#endif // MediaInfo_h

