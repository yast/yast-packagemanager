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

#ifndef InstSrcTags_h
#define InstSrcTags_h

#include <fstream>
#include <y2util/CommonPkdParser.h>



const std::string TypeTag = "=Type";
const std::string UrlTag  = "=URL";
const std::string ProdDirTag = "=ProductDir";
const std::string DefActTag = "=Default_activate";
const std::string MediaBTag = "+Media";
const std::string MediaETag = "-Media";
const std::string ContentBTag = "+Product";
const std::string ContentETag = "-Product";
const std::string ArchBTag = "+Arch";
const std::string ArchETag = "-Arch";
const std::string DefBaseTag = "=DefaultBase";
const std::string LabelMapBTag = "+LabelMap";
const std::string LabelMapETag = "-LabelMap";
const std::string LinguasBTag = "+Linguas";
const std::string LinguasETag = "-Linguas";
const std::string LabelTag = "=Label";
const std::string LangTag = "=Language";
const std::string TimeTag = "=Timezone";
const std::string DescrDirTag = "=DescriptionDir";
const std::string DataDirTag = "=DataDir";


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
	TYPE,		// type found on medium
	URL,		// URL
	PRODUCTDIR,	// product dir below _url
	ACTIVATE,	// 1 = true (default activated), 0 = false
	MEDIA,		// _media_vendor, _media_id, _media_count
	PRODUCT, 	// _content_product, _content_baseproduct, _content_vendor
	DEFBASE,	// _content_defaultbase
	ARCH,		// _content_archmap
	LANGUAGE,	// _content_language
	LABEL,		// _content_label
	LABELMAP,	// _content_labelmap
	LINGUAS,	// _content_linguas
	TIMEZONE,	// _content_timezone
	DESCRDIR,	// _content_descrdir
	DATADIR,	// _content_datadir
	NUM_TAGS
    };

public:
    InstSrcMediaTags( ) 
	: TagSet()	{

	CommonPkdParser::Tag* t;
	createTag( TypeTag, TYPE );
	createTag( UrlTag, URL );
	createTag( ProdDirTag, PRODUCTDIR );
	createTag( DefActTag, ACTIVATE );
	t = createTag( MediaBTag, MEDIA );
	t->setEndTag( MediaETag );
	t = createTag( ContentBTag, PRODUCT );
	t->setEndTag( ContentETag );
	t = createTag( ArchBTag, ARCH );
	t->setEndTag( ArchETag );
	createTag( DefBaseTag, DEFBASE );
	t = createTag( LabelMapBTag, LABELMAP);
	t->setEndTag( LabelMapETag );
	t = createTag( LinguasBTag, LINGUAS );
	t->setEndTag( LinguasETag );
	createTag( LabelTag, LABEL );
	createTag( LangTag, LANGUAGE );
	createTag( TimeTag, TIMEZONE );
	createTag( DescrDirTag, DESCRDIR );
	createTag( DataDirTag, DATADIR );
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

#endif // InstSrcTags_h
