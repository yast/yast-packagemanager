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

  Purpose:    Interface class for InstSrcData and InstTarget and other
              classes that may provide Packages/Selections/YouPatches.

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
/**
 * @short Interface class for InstSrcData and InstTarget and other classes that may provide Packages/Selections/YouPatches.
 **/
class InstData {

  protected:

    InstData();

    virtual ~InstData();

  public:

    /**
     * generate PMSelection objects for each selection on the source
     *
     * Default implementation provided returning an empty list.
     * @return list of PMSelectionPtr on this source
     */
    virtual const std::list<PMSelectionPtr>& getSelections (void) const = 0;

    unsigned numSelections() const { return getSelections().size(); }

    /**
     * generate PMPackage objects for each Item on the source/target
     *
     * Default implementation provided returning empty list.
     * @return list of PMPackagePtr on this source
     * */
    virtual const std::list<PMPackagePtr>& getPackages (void) const = 0;

    unsigned numPackages() const { return getPackages().size(); }

    /**
     * generate PMSolvable objects for each patch on the source
     *
     * Default implementation provided returning empty list.
     * @return list of PMSolvablePtr on this source
     */
    virtual const std::list<PMYouPatchPtr>& getPatches (void) const = 0;

    unsigned numPatches() const { return getPatches().size(); }

    /**
     * find list of packages
     * @return list of PMPackagePtr matching name ,[version] ,[release] ,[architecture]
     */
    static const std::list<PMPackagePtr> findPackages (const std::list<PMPackagePtr>& packages, const std::string& name = "", const std::string& arch = "", const std::string& version = "", const std::string& release = "");

    /**
     * find list of selections
     * @return list of PMSelectionPtr matching name ,[version] ,[release] ,[architecture]
     */
    static const std::list<PMSelectionPtr> findSelections (const std::list<PMSelectionPtr>& selections, const std::string& arch = "", const std::string& name = "", const std::string& version = "", const std::string& release = "");
};

///////////////////////////////////////////////////////////////////

#endif // InstData_h

