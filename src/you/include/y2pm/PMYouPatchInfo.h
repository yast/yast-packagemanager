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

  File:       PMYouPatchInfo.h

  Author:     Cornelius Schumacher <cschum@suse.de>
  Maintainer: Cornelius Schumacher <cschum@suse.de>

  Purpose: Utility class for reading the patch information

/-*/
#ifndef PMYouPatchInfo_h
#define PMYouPatchInfo_h

#include <list>
#include <string>

#include <y2util/CommonPkdParser.h>

#include <y2pm/PMError.h>

#include <y2pm/PMYouPatchPtr.h>

class YOUPatchTagSet : public CommonPkdParser::TagSet
{
    public:
	enum Tags {
	    KIND,
	    SHORTDESCRIPTION,
	    LONGDESCRIPTION,
            PREINFORMATION,
            POSTINFORMATION,
	    SIZE,
	    BUILDTIME,
	    MINYAST1VERSION,
	    MINYAST2VERSION,
	    UPDATEONLYINSTALLED,
	    PACKAGES,
	    NUM_TAGS
	};
    private:
	std::string _locale;
    public:
	YOUPatchTagSet(const std::string& locale) : TagSet(), _locale(locale) 
	{
	    CommonPkdParser::Tag *t;
	    t = createTag("Kind",KIND);
	    t = createTag("Shortdescription.",SHORTDESCRIPTION);
	    t->setType(CommonPkdParser::Tag::ACCEPTLOCALEONLY);
	    t->setDefaultLocale("english");
	    t->setPreferredLocale(locale);
	    _localetags.push_back(t);
	    t = createTag("Longdescription.",LONGDESCRIPTION);
	    t->setType(CommonPkdParser::Tag::ACCEPTLOCALEONLY);
	    t->setDefaultLocale("english");
	    t->setPreferredLocale(locale);
	    t->setEndTag("noitpircsedgnol",CommonPkdParser::Tag::ENDTAG_COMPLETELYREVERSED);
	    t = createTag("Preinformation.",PREINFORMATION);
	    t->setType(CommonPkdParser::Tag::ACCEPTLOCALEONLY);
	    t->setDefaultLocale("english");
	    t->setPreferredLocale(locale);
	    t->setEndTag("noitamrofnier",CommonPkdParser::Tag::ENDTAG_COMPLETELYREVERSED);
	    t = createTag("Postinformation.",POSTINFORMATION);
	    t->setType(CommonPkdParser::Tag::ACCEPTLOCALEONLY);
	    t->setDefaultLocale("english");
	    t->setPreferredLocale(locale);
	    t->setEndTag("noitamrofnitsop",CommonPkdParser::Tag::ENDTAG_COMPLETELYREVERSED);
	    _localetags.push_back(t);
	    t = createTag("Size",SIZE);
	    t = createTag("Buildtime",BUILDTIME);
	    t = createTag("MinYaST1Version",MINYAST1VERSION);
	    t = createTag("MinYaST2Version",MINYAST1VERSION);
	    t = createTag("UpdateOnlyInstalled",UPDATEONLYINSTALLED);
	    t = createTag("Packages",PACKAGES);
	    t->setEndTag("Segakcap");
	}
        
        CommonPkdParser::Tag *createTag( const std::string &tagname, int num )
        {
            CommonPkdParser::Tag *t = new CommonPkdParser::Tag( tagname,
                CommonPkdParser::Tag::ACCEPTONCE );
            this->addTag( t );
            addTagByIndex( num, t );
            
            return t;
        }
};

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMYouPatchInfo
/**
 * The Package.
 **/
class PMYouPatchInfo {

  public:
    /**
     * Constructor
     *
     * @param lang  language to be parsed.
     **/
    PMYouPatchInfo( const std::string &lang );

    /**
     * Read patch info from file.
     *
     * @param path    File path of package info file.
     * @param patches List of patch objects where the results are stored.
     **/
    PMError readFile( const std::string &path, const std::string &name,
                      std::list<PMYouPatchPtr> &patches );
  
  protected:
    std::string tagValue( YOUPatchTagSet::Tags tag );

  private:
    CommonPkdParser::TagSet* _tagset;
};

///////////////////////////////////////////////////////////////////

#endif // PMYouPatchInfo_h
