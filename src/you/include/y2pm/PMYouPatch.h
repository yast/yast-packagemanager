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

  File:       PMYouPatch.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Defines the YouPatch object.

/-*/
#ifndef PMYouPatch_h
#define PMYouPatch_h

#include <iosfwd>
#include <string>

#include <y2util/Pathname.h>

#include <y2pm/PMYouPatchDataProviderPtr.h>
#include <y2pm/PMYouPatchPtr.h>

#include <y2pm/PMObject.h>
#include <y2pm/PMPackagePtr.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMYouPatch
/**
 * The Package.
 **/
class PMYouPatch : virtual public Rep, public PMObject {
  REP_BODY(PMYouPatch);

  public:

    /**
     * Attributes provided by PMYouPatch
     **/
    enum PMYouPatchAttribute {
      PMYOU_ATTR_BEGIN = PMOBJ_NUM_ATTRIBUTES,
      ATTR_KIND = PMYOU_ATTR_BEGIN,
      ATTR_SHORTDESCRIPTION,
      ATTR_LONGDESCRIPTION,
      ATTR_SIZE,
      ATTR_BUILDTIME,
      ATTR_MINYAST2VERSION,
      // last entry
      PMYOU_NUM_ATTRIBUTES
    };

	// overlay virtual PMObject functions
	const std::string summary() const;
	const std::list<std::string> description() const;
	const std::list<std::string> insnotify() const;
	const std::list<std::string> delnotify() const;
	const FSize size() const;

    /**
     * Definitions for the kind of the patch.
     */
    enum Kind {
      kind_invalid = 0,
      kind_security = 1,
      kind_recommended = 2,
      kind_optional = 4,
      kind_document = 8,
      kind_yast = 16,
      kind_all = 31
    };
    /**
     * Set kind attribute.
     */
    void setKind( Kind kind ) { _kind = kind; }
    /**
     * Return kind attribute.
     */
    Kind kind() const { return _kind; }
    /**
     * Return translated kind label.
     */
    static std::string kindLabel( Kind kind );

    /**
     * Set short description attribute.
     */
    void setShortDescription( const std::string &str ) { _shortDescription = str; }
    /**
     * Return short description attribute.
     */
    std::string shortDescription() const { return _shortDescription; }

    /**
     * Set short description attribute.
     */
    void setLongDescription( const std::string &str ) { _longDescription = str; }
    /**
     * Return short description attribute.
     */
    std::string longDescription() const { return _longDescription; }

    /**
     * Set pre information attribute.
     */
    void setPreInformation( const std::string &str ) { _preInformation = str; }
    /**
     * Return pre information attribute.
     */
    std::string preInformation() const { return _preInformation; }

    /**
     * Set post information attribute.
     */
    void setPostInformation( const std::string &str ) { _postInformation = str; }
    /**
     * Return post information attribute.
     */
    std::string postInformation() const { return _postInformation; }

    /**
     * Set minimum yast version attribute.
     */
    void setMinYastVersion( const std::string &str ) { _minYastVersion = str; }
    /**
     * Return minimum yast version attribute.
     */
    std::string minYastVersion() const { return _minYastVersion; }

    /**
     * Set if only installed packages should be updated.
     */
    void setUpdateOnlyInstalled( bool v ) { _updateOnlyInstalled = v; }
    /**
     * Return if only installed packages should be updated.
     */
    bool updateOnlyInstalled() const { return _updateOnlyInstalled; }

    /**
     * Add a package to this patch.
     *
     */
    void addPackage( const PMPackagePtr &pkg );
    /**
     * Return list of all packages belonging to this patch.
     */
    std::list<PMPackagePtr> packages() const { return _packages; }

    void setLocalFile( const Pathname &localFile ) { _localFile = localFile; }
    Pathname localFile() const { return _localFile; }

  private:
    std::string _shortDescription, _longDescription;
    std::string _preInformation, _postInformation;
    std::string _minYastVersion;
    Kind _kind;
    bool _updateOnlyInstalled;

    std::list<PMPackagePtr> _packages;

    Pathname _localFile;

  protected:

    PMYouPatchDataProviderPtr _dataProvider;

  protected:

    /**
     * Provide DataProvider access to the underlying Object
     **/
    virtual PMDataProviderPtr dataProvider() const { return _dataProvider; }

  public:

    PMYouPatch( const PkgName &    name_r,
		const PkgEdition & edition_r,
                const PkgArch &arch_r,
		PMYouPatchDataProviderPtr dataProvider_r );

    virtual ~PMYouPatch();

    void startRetrieval() const;
    void stopRetrieval() const;

  public:

    virtual std::ostream & dumpOn( std::ostream & str ) const;
};

///////////////////////////////////////////////////////////////////

#endif // PMYouPatch_h
