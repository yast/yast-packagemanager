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

  File:       PMYouPatchTags.h

  Author:     Cornelius Schumacher <cschum@suse.de>
  Maintainer: Cornelius Schumacher <cschum@suse.de>

  Purpose: Tag definitions for CommonPkdParser.

/-*/
#ifndef PMYouPatchTags_h
#define PMYouPatchTags_h

#include <string>

#include <y2util/CommonPkdParser.h>

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

class YOUPackageTagSet : public CommonPkdParser::TagSet
{
    public:
	enum Tags {
            FILENAME,
            SERIES,
            RPMNAME,
            INSTPATH,
            SIZE,
            BUILDTIME,
            BUILTFROM,
            FLAG,
            RPMGROUP,
            COPYRIGHT,
            AUTHORNAME,
            VERSION,
            OBSOLETES,
            REQUIRES,
            PROVIDES,
            CONFLICTS,
            NUM_TAGS
        };

        YOUPackageTagSet() : TagSet()
        {
            createTag("Filename",FILENAME);
            createTag("Series",SERIES);
            createTag("RpmName",RPMNAME);
            createTag("InstPath",INSTPATH);
            createTag("Size",SIZE);
            createTag("Buildtime",BUILDTIME);
            createTag("BuiltFrom",BUILTFROM);
            createTag("Flag",FLAG);
            createTag("RpmGroup",RPMGROUP);
            createTag("Copyright",COPYRIGHT);
            createTag("AuthorName",AUTHORNAME);
            createTag("Version",VERSION);
            createTag("Obsoletes",OBSOLETES);
            createTag("Requires",REQUIRES);
            createTag("Provides",PROVIDES);
            createTag("Conflicts",CONFLICTS);
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

#endif
