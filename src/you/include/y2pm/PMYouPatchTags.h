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
            PATCHNAME,
            PATCHVERSION,
            REQUIRES,
            PROVIDES,
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
	    NUM_TAGS,
            PRESCRIPT,
            POSTSCRIPT
	};
    private:
	std::string _locale;
    public:
	YOUPatchTagSet(const std::string& locale) : TagSet(), _locale(locale) 
	{
	    CommonPkdParser::Tag *t;
	    t = createTag("Patchname",PATCHNAME);
	    t = createTag("Patchversion",PATCHVERSION);
	    t = createTag("Requires",REQUIRES);
	    t = createTag("Provides",PROVIDES);
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
	    t->setEndTag("noitamrofnierp",CommonPkdParser::Tag::ENDTAG_COMPLETELYREVERSED);
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
	    t = createTag("Prescript",PRESCRIPT);
	    t = createTag("Postscript",POSTSCRIPT);
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
            PKGVERSION,
            OBSOLETES,
            REQUIRES,
            PROVIDES,
            CONFLICTS,
            PATCHRPMBASEVERSIONS,
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
            createTag("Version",PKGVERSION);
            createTag("Obsoletes",OBSOLETES);
            createTag("Requires",REQUIRES);
            createTag("Provides",PROVIDES);
            createTag("Conflicts",CONFLICTS);
            createTag("PatchRpmBaseVersions",PATCHRPMBASEVERSIONS);
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
