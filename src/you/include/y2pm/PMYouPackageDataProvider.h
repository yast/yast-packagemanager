/*---------------------------------------------------------------------\
|                                                                      |
|                      __   __    ____ _____ ____                      |
|                      \ \ / /_ _/ ___|_   _|___ \                     |
|                       \ V / _` \___ \ | |   __) |                    |
|                        | | (_| |___) || |  / __/                     |
|                        |_|\__,_|____/ |_| |_____|                    |
|                                                                      |
|                               core system                            |
|                                                     (C) 2002 SuSE AG |
\----------------------------------------------------------------------/

   File:       PMYouPackageDataProvider.h
   Purpose:    Implements PackageDataProvider for YOU patches.
   Author:     Cornelius Schumacher <cschum@suse.de>
   Maintainer: Cornelius Schumacher <cschum@suse.de>

/-*/

#ifndef PMYouPackageDataProvider_h
#define PMYouPackageDataProvider_h

#include <y2pm/PMYouPackageDataProviderPtr.h>
#include <y2pm/PMPackageDataProvider.h>
#include <y2pm/PMPackage.h>
#include <y2pm/PMObject.h>
#include <y2pm/PMYouPatchInfo.h>

#include <y2util/YRpmGroupsTree.h>

class PMYouPackageDataProvider : virtual public Rep, public PMPackageDataProvider {
	REP_BODY(PMYouPackageDataProvider);

        PMYouPatchInfoPtr _patchInfo;

    public:

	PMYouPackageDataProvider( const PMYouPatchInfoPtr & );

	virtual ~PMYouPackageDataProvider();

    public:
        /**
          Set label describing the package.
        */
        void setSummary( const PMPackagePtr &pkg, const std::string &label );

        /**
          Get label of package.
        */
        std::string summary( const PMPackage & pkg_r ) const;

        /**
          Set label describing the package source.
        */
        void setSrcLabel( const PMPackagePtr &pkg, const std::string &label );

        /**
          Get label of package source.
        */
        std::string instSrcLabel( const PMPackage & pkg_r ) const;

        /**
          Set size value for given package.
        */
        void setSize( const PMPackagePtr &pkg, const FSize & );

        /**
          Get size for given package.
        */
        FSize size( const PMPackage & ) const;

        /**
         * Set location value for given package.
         */
        void setLocation( const PMPackagePtr &pkg, const std::string &str );

        /**
          Get location for given package.
        */
        std::string location( const PMPackage & ) const;

        /**
          Get RPM group.
        */
        std::string group( const PMPackage & pkg_r ) const;

        /**
          Get RPM group tree item object.
        */
        YStringTreeItem *group_ptr( const PMPackage & pkg_r ) const;

        /**
          Set RPM group.
        */
        void setRpmGroup( const PMPackagePtr &, const std::string &group );

        /**
          Set external url for given package.
        */
        void setExternalUrl( const PMPackagePtr &pkg, const std::string &str );

        /**
          Get external url for given package.
        */
        std::string externalUrl( const PMPackage & ) const;

        /**
          Set base versions for patch RPM.
        */
        void setPatchRpmBaseVersions( const PMPackagePtr &pkg,
                                      const std::list<PkgEdition> &editions );

        /**
          Get base versions for patch RPM.
        */
        std::list<PkgEdition> patchRpmBaseVersions( const PMPackage & ) const;

        /**
          Set file size of RPM archive.
        */
        void setArchiveSize( const PMPackagePtr &, const FSize & );
        
        /**
          Get file size of RPM archive.
        */
        FSize archivesize( const PMPackage & ) const;
    
        /**
          Set file size of patch RPM.
        */
        void setPatchRpmSize( const PMPackagePtr &, const FSize & );
        
        /**
          Get file size of patch RPM.
        */
        FSize patchRpmSize( const PMPackage & ) const;
    

    private:
        std::map<PMPackagePtr,std::string> _summaries;
        std::map<PMPackagePtr,std::string> _srcLabels;
        std::map<PMPackagePtr,FSize> _sizes;
        std::map<PMPackagePtr,std::string> _locations;
        std::map<PMPackagePtr,std::string> _externalUrls;
        std::map<PMPackagePtr,std::list<PkgEdition> > _patchRpmBaseVersions;        
        std::map<PMPackagePtr,YStringTreeItem *> _rpmGroups;
        std::map<PMPackagePtr,FSize> _archiveSizes;
        std::map<PMPackagePtr,FSize> _patchRpmSizes;
};

#endif // PMYouPackageDataProvider_h
