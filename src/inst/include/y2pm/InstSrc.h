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

   File:       InstSrc.h

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

   Purpose:	Class for installation sources
		Defines (provides access to)
		- media access (a pointer to MediaAccess)
		- source description (product, version, vendor, ...)
		- contents (list of package, list of selections, ...)
/-*/
#ifndef InstSrc_h
#define InstSrc_h

#include <iosfwd>
#include <list>
#include <string>

#include <y2util/Pathname.h>

#include <y2pm/InstSrc.h>		// pointer to self
#include <y2pm/MediaAccess.h>		// physical media access class
#include <y2pm/InstSrcDescr.h>		// source description
#include <y2pm/InstSrcData.h>		// source content

#include <y2pm/PMSolvable.h>
#include <y2pm/PMPackage.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : InstSrc
/**
 *
 **/
class InstSrc: virtual public Rep {
  REP_BODY(InstSrc)

  protected:

    /**
     * direct media access
     */
    MediaAccess	*_media;

    /**
     * description of media
     */
    InstSrcDescr *_descr;

    /**
     * content of media
     */
    InstSrcData *_data;

  public:
    /**
     * constructor
     *   used for access to new/unknown media
     * @param: URL specification for media access
     *
     * keeps the media open 
     */
    InstSrc ( const std::string & mediaurl );

    /**
     * constructor
     *   used for access to known media
     * @param: Pathname specification of content file
     *
     * does not open the media
     */
    InstSrc ( const Pathname & contentfile );

    /**
     * destructor
     */
    ~InstSrc();

  public:

    //-----------------------------
    // general functions

    /**
     * clean up, e.g. remove all caches
     */
    bool Erase();

    /**
     * @return description of Installation source
     * This is needed by the InstSrcMgr
     */
    const InstSrcDescr *getDescription() const;

    /**
     * register this source (store cache files etc)
     * return pathname of saved content file
     */
    const Pathname registerSource (void) const;

    //-----------------------------
    // activation status

    /**
     * return activation status
     */
    bool getActivation() const;

    /**
     * temporary (de)activate source
     */
    void setActivation (bool yesno);

    //-----------------------------
    // source content access

    /**
     * return the number of selections on this source
     */
    int numSelections() const;

    /**
     * return the number of packages on this source
     */
    int numPackages() const;

    /**
     * return the number of patches on this source
     */
    int numPatches() const;

    /**
     * generate PMSolvable objects for each selection on the source
     * @return list of PMSolvablePtr on this source
     */
    std::list<PMSolvablePtr> getSelections();
    
    /**
     * generate PMPackage objects for each Item on the source
     * @return list of PMPackagePtr on this source
     * */
    std::list<PMPackagePtr> getPackages();

    /**
     * generate PMSolvable objects for each patch on the source
     * @return list of PMSolvablePtr on this source
     */
    std::list<PMSolvablePtr> getPatches();

    std::ostream & dumpOn( std::ostream & str ) const;
};

///////////////////////////////////////////////////////////////////

#endif // InstSrc_h

