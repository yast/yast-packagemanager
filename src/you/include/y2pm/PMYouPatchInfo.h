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
#include <y2util/Url.h>
#include <y2util/Pathname.h>

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
//	CLASS NAME : PMYouPatchPaths
/**
 * Pathes for patches.
 **/
class PMYouPatchPaths {

  public:
    PMYouPatchPaths( const std::string &product, const std::string &version,
                     const std::string &arch );

    void setPatchPath( const Pathname & );
    Pathname patchPath();

    void setPatchUrl( const Url & );
    Url patchUrl();

    void setAttachPoint( const Pathname & );
    Pathname attachPoint();

  private:
    Pathname _patchPath;
    Url _patchUrl;
    Pathname _attachPoint;
};


///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMYouPatchInfo
/**
 * Patch information.
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
     * Destructor
     **/
    ~PMYouPatchInfo();

    /**
     * Get patches from specified location.
     *
     * @param paths   Object holding the path location information.
     * @param patches List of patch objects where the results are stored.
     **/
    PMError getPatches( PMYouPatchPaths *paths,
                        std::list<PMYouPatchPtr> &patches );
    
    /**
     * Read patch info files from directory.
     *
     * @param baseUrl    Base of URL where patches are located.
     * @param patchPath  Path of patch directory relative to baseUrl.
     * @param attachPath Path where media is to be attached.
     * @param patches    List of patch objects where the results are stored.
     **/
    PMError readDir( const Url &baseUrl, const Pathname &patchPath,
                     const Pathname &attachPath,
                     std::list<PMYouPatchPtr> &patches );

    /**
     * Read patch info from file.
     *
     * @param path     File path of package info file.
     * @param fileName Name of patch file.
     * @param patches  List of patch objects where the results are stored.
     **/
    PMError readFile( const Pathname &path, const std::string &fileName,
                      std::list<PMYouPatchPtr> &patches );

  protected:
    std::string tagValue( YOUPatchTagSet::Tags tag );

  private:
    CommonPkdParser::TagSet *_tagset;
    std::list<std::string> *_patchFiles;
};

///////////////////////////////////////////////////////////////////

#endif // PMYouPatchInfo_h
