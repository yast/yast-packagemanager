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

  File:       PMPackageDataProvider.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Common interface to be realized by all PackageDataProvider.

/-*/
#ifndef PMPackageDataProvider_h
#define PMPackageDataProvider_h

#include <iosfwd>
#include <string>

#include <y2util/FSize.h>
#include <y2util/Date.h>
#include <y2util/YRpmGroupsTree.h>

#include <y2pm/PMPackageDataProviderPtr.h>

#include <y2pm/PMPackage.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMPackageDataProvider
/**
 * @short Common interface to be realized by all PackageDataProvider.
 *
 * PMPackage attribute retrieval. The PMPackage this pointer is passed
 * in case the concrete DataProvider has do some lookup.
 *
 * Default values provided by static members
 *
 * @see DataProvider
 **/
class PMPackageDataProvider : virtual public Rep {
  REP_BODY(PMPackageDataProvider);

  public:

    ///////////////////////////////////////////////////////////////////
    // default for unprovided values
    ///////////////////////////////////////////////////////////////////

    // PMObject attributes
    static std::string            summary()     { return std::string(); }
    static std::list<std::string> description() { return std::list<std::string>(); }
    static std::list<std::string> insnotify()   { return std::list<std::string>(); }
    static std::list<std::string> delnotify()   { return std::list<std::string>(); }
    static FSize                  size()        { return FSize(0); }

    // PMPackage attributes
    static Date                   buildtime()   { return Date(0); }
    static std::string            buildhost()   { return std::string(); }
    static Date                   installtime() { return Date(0); }
    static std::string            distribution(){ return std::string(); }
    static std::string            vendor()      { return std::string(); }
    static std::string            license()     { return std::string(); }
    static std::string            packager()    { return std::string(); }
    static std::string            group()       { return std::string(); }
    static YStringTreeItem *      group_ptr()   { return 0; }
    static std::list<std::string> changelog()   { return std::list<std::string>(); }
    static std::string            url()         { return std::string(); }
    static std::string            os()          { return std::string(); }
    static std::list<std::string> prein()       { return std::list<std::string>(); }
    static std::list<std::string> postin()      { return std::list<std::string>(); }
    static std::list<std::string> preun()       { return std::list<std::string>(); }
    static std::list<std::string> postun()      { return std::list<std::string>(); }
    static std::string            sourcerpm()   { return std::string(); }
    static FSize                  archivesize() { return FSize(0); }
    static std::list<std::string> authors()     { return std::list<std::string>(); }
    static std::list<std::string> filenames()   { return std::list<std::string>(); }
    // suse packages values
    static std::list<std::string> recommends()  { return std::list<std::string>(); }
    static std::list<std::string> suggests()    { return std::list<std::string>(); }
    static std::string            location()    { return std::string(); }
    static unsigned int           medianr()     { return 0; }
    static std::list<std::string> keywords()    { return std::list<std::string>(); }

  protected:

    PMPackageDataProvider();

    virtual ~PMPackageDataProvider();

    /**
     * In case concrete PackageDataProvider wants PMPackagePtr for lookup.
     **/
    PMPackagePtr mkPtr( const PMPackage & pkg_r ) const { return const_cast<PMPackage*>(&pkg_r); }

  public:

    ///////////////////////////////////////////////////////////////////
    // Overload values you can provide
    ///////////////////////////////////////////////////////////////////

    // PMObject attributes
    virtual std::string            summary     ( const PMPackage & pkg_r ) const { return summary(); }
    virtual std::list<std::string> description ( const PMPackage & pkg_r ) const { return description(); }
    virtual std::list<std::string> insnotify   ( const PMPackage & pkg_r ) const { return insnotify(); }
    virtual std::list<std::string> delnotify   ( const PMPackage & pkg_r ) const { return delnotify(); }
    virtual FSize                  size        ( const PMPackage & pkg_r ) const { return size(); }

    // PMPackage attributes
    virtual Date                   buildtime   ( const PMPackage & pkg_r ) const { return buildtime(); }
    virtual std::string            buildhost   ( const PMPackage & pkg_r ) const { return buildhost(); }
    virtual Date                   installtime ( const PMPackage & pkg_r ) const { return installtime(); }
    virtual std::string            distribution( const PMPackage & pkg_r ) const { return distribution(); }
    virtual std::string            vendor      ( const PMPackage & pkg_r ) const { return vendor(); }
    virtual std::string            license     ( const PMPackage & pkg_r ) const { return license(); }
    virtual std::string            packager    ( const PMPackage & pkg_r ) const { return packager(); }
    virtual std::string            group       ( const PMPackage & pkg_r ) const { return group(); }
    virtual YStringTreeItem *      group_ptr   ( const PMPackage & pkg_r ) const { return group_ptr(); }
    virtual std::list<std::string> changelog   ( const PMPackage & pkg_r ) const { return changelog(); }
    virtual std::string            url         ( const PMPackage & pkg_r ) const { return url(); }
    virtual std::string            os          ( const PMPackage & pkg_r ) const { return os(); }
    virtual std::list<std::string> prein       ( const PMPackage & pkg_r ) const { return prein(); }
    virtual std::list<std::string> postin      ( const PMPackage & pkg_r ) const { return postin(); }
    virtual std::list<std::string> preun       ( const PMPackage & pkg_r ) const { return preun(); }
    virtual std::list<std::string> postun      ( const PMPackage & pkg_r ) const { return postun(); }
    virtual std::string            sourcerpm   ( const PMPackage & pkg_r ) const { return sourcerpm(); }
    virtual FSize                  archivesize ( const PMPackage & pkg_r ) const { return archivesize(); }
    virtual std::list<std::string> authors     ( const PMPackage & pkg_r ) const { return authors(); }
    virtual std::list<std::string> filenames   ( const PMPackage & pkg_r ) const { return filenames(); }
    // suse packages values
    virtual std::list<std::string> recommends  ( const PMPackage & pkg_r ) const { return recommends(); }
    virtual std::list<std::string> suggests    ( const PMPackage & pkg_r ) const { return suggests(); }
    virtual std::string            location    ( const PMPackage & pkg_r ) const { return location(); }
    virtual unsigned int           medianr     ( const PMPackage & pkg_r ) const { return medianr(); }
    virtual std::list<std::string> keywords    ( const PMPackage & pkg_r ) const { return keywords(); }

};

///////////////////////////////////////////////////////////////////

#endif // PMPackageDataProvider_h

