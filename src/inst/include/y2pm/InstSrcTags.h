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

  File:       InstSrcTags.h

  Author:     Gabriele Strattner <gs@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose:	Class containing tags, e.g. used in media cache file.
                The tag sets are needed for the TagParser. 
/-*/

#include <fstream>
#include <y2util/CommonPkdParser.h>


///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : InstSrcMediaTags
/**
 * @short provides the tag set for the media file
 * (to feed the tag parser) 
 *
 **/
class InstSrcMediaTags : public CommonPkdParser::TagSet
{

public:
    enum Tags {
	ARCH,		// architecture
	TYPE,		// type found on medium
	URL,		// URL
	PRODUCTDIR,	// product dir below _url
	ACTIVATE,	// 1 = true (default activated), 0 = false
	MEDIA,		// _media_vendor, _media_id, _media_count
	NUM_TAGS
    };

public:
    InstSrcMediaTags( ) 
	: TagSet()	{

	CommonPkdParser::Tag* t;
	createTag( "=Arch", ARCH );
	createTag( "=Type", TYPE );
	createTag( "=URL", URL );
	createTag( "=ProductDir", PRODUCTDIR );
	createTag( "=Default_activate", ACTIVATE );
	t = createTag( "+Media", MEDIA );
	t->setEndTag("-Media");
    };

private:

    CommonPkdParser::Tag* createTag( std::string tagName, Tags tagEnum ) {
	
	CommonPkdParser::Tag* t;
	t = new CommonPkdParser::Tag( tagName, CommonPkdParser::Tag::ACCEPTONCE );
	this->addTag(t);
	addTagByIndex( tagEnum, t );

	return t;
    }
    
};
