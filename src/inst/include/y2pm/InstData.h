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
//
// 	Interface class for InstSrc and InstTarget

class InstData {
  protected:

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
    virtual const Pathname writeCache (const Pathname &descrpathname) = 0;

    //-----------------------------
    // source content access

    /**
     * generate PMSelection objects for each selection on the source
     * @return list of PMSelectionPtr on this source
     */
    virtual const std::list<PMSelectionPtr>& getSelections (void) const = 0;

    /**
     * generate PMPackage objects for each Item on the source/target
     * @return list of PMPackagePtr on this source
     * */
    virtual const std::list<PMPackagePtr>& getPackages (void) const = 0;

    /**
     * generate PMSolvable objects for each patch on the source
     * @return list of PMSolvablePtr on this source
     */
    virtual const std::list<PMYouPatchPtr>& getPatches (void) const = 0;

    virtual std::ostream & dumpOn( std::ostream & str ) const = 0;

    /**
     * find list of packages
     * @return list of PMPackagePtr matching name ,[version] ,[release] ,[architecture]
     */
    static const std::list<PMPackagePtr> findPackages (const std::list<PMPackagePtr>& packages, const std::string& name = "", const std::string& version = "", const std::string& release = "", const std::string& arch = "");

    /**
     * find list of selections
     * @return list of PMSelectionPtr matching name ,[version] ,[release] ,[architecture]
     */
    static const std::list<PMSelectionPtr> findSelections (const std::list<PMSelectionPtr>& selections, const std::string& name = "", const std::string& version = "", const std::string& release = "", const std::string& arch = "");
};

///////////////////////////////////////////////////////////////////

#endif // InstData_h

