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

   File:       InstData.h

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

    Purpose:	Keeps information about installation descriptions.

		Used for source and target sides.
/-*/
#ifndef InstData_h
#define InstData_h

#include <iosfwd>
#include <list>

#include <y2util/Pathname.h>

#include <y2pm/PMPackagePtr.h>
#include <y2pm/PMSelectionPtr.h>
#include <y2pm/PMYouPatchPtr.h>

#include <y2pm/MediaAccess.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : InstData
/**
 * @short InstData currently is a kludge. Dont' use it.
 **/
class InstData {
  //REP_BODY(InstData);

  private:

    const std::list<PMSelectionPtr> *_selections;
    const std::list<PMPackagePtr>  *_packages;
    const std::list<PMYouPatchPtr> *_patches;

  private:

    /**
     * InstData currently is a kludge. Dont' use it.
     **/
    friend class InstSrcData;

    InstData();

    virtual ~InstData();

  public:

    //-----------------------------
    // cache file handling
    /**
     * write media content data to cache file
     * @param pathname of corresponding InstSrcDescr cache file
     * @return pathname of written cache
     * writes content cache data to an ascii file
     */
    const Pathname writeCache (const Pathname &descrpathname);

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
     * set list of selections
     * @return void
     */
    void setSelections (std::list<PMSelectionPtr> *sels) { _selections = sels; }

    /**
     * generate PMSelection objects for each selection on the source
     * @return list of PMSelectionPtr on this source
     */
    const std::list<PMSelectionPtr> *getSelections (void) const;

    /**
     * set list of packages
     * @return void
     */
    void setPackages (std::list<PMPackagePtr> *pacs) { _packages = pacs; }

    /**
     * generate PMPackage objects for each Item on the source
     * @return list of PMPackagePtr on this source
     * */
    const std::list<PMPackagePtr> *getPackages (void) const;

    /**
     * set list of patches
     * @return void
     */
    void setPatches (std::list<PMYouPatchPtr> *pats) { _patches = pats; }

    /**
     * generate PMSolvable objects for each patch on the source
     * @return list of PMSolvablePtr on this source
     */
    const std::list<PMYouPatchPtr> *getPatches (void) const;

    virtual std::ostream & dumpOn( std::ostream & str ) const;

    /**
     * find list of packages
     * @return list of PMPackagePtr matching name ,[version] ,[release] ,[architecture]
     */
    static const std::list<PMPackagePtr> findPackages (const std::list<PMPackagePtr> *packages, const std::string& name = "", const std::string& version = "", const std::string& release = "", const std::string& arch = "");
};

///////////////////////////////////////////////////////////////////

#endif // InstData_h

