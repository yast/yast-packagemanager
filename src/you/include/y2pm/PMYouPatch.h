/*---------------------------------------------------------------------\
|                                                                      |
|                      __   __    ____ _____ ____                      |
|                      \ \ / /_ _/ ___|_   _|___ \                     |
|                       \ V / _` \___ \ | |   __) |                    |
|                        | | (_| |___) || |  / __/                     |
|                        |_|\__,_|____/ |_| |_____|                    |
|                                                                      |
|                               core system                            |
|                                                    (C) SuSE Linux AG |
\----------------------------------------------------------------------/

  File:       PMYouPatch.h

  Author:     Michael Andres <ma@suse.de>
              Cornelius Schumacher <cschum@suse.de>
  Maintainer: Cornelius Schumacher <cschum@suse.de>

  Purpose: Defines the YouPatch object.

/-*/
#ifndef PMYouPatch_h
#define PMYouPatch_h

#include <iosfwd>
#include <string>

#include <y2util/Pathname.h>
#include <y2util/FSize.h>

#include <y2pm/PMYouPatchPtr.h>

#include <y2pm/PMObject.h>
#include <y2pm/PMPackagePtr.h>

/**
  Information about YOU patch extra files.
*/
class PMYouFile
{
  public:
    PMYouFile( const std::string &name, const FSize &size )
      : _name( name ), _size( size ) {}

    std::string name() const { return _name; }
    FSize size() const { return _size; }

  private:
    std::string _name;
    FSize _size;
};

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
     * Definitions for the kind of the patch.
     */
    enum Kind {
      kind_invalid = 0,
      kind_yast = 1,
      kind_security = 2,
      kind_recommended = 4,
      kind_optional = 8,
      kind_document = 16,
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
    std::string kindLabel() { return kindLabel( _kind ); }
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
     * Set if only installed packages should be updated.
     */
    void setUpdateOnlyInstalled( bool v ) { _updateOnlyInstalled = v; }
    /**
     * Return if only installed packages should be updated.
     */
    bool updateOnlyInstalled() const { return _updateOnlyInstalled; }

    /**
     * Set name of script which is executed before installation of patch.
     */
    void setPreScript( const std::string &preScript ) { _preScript = preScript; }
    /**
     * Return name of script which is executed before installation of patch.
     */
    std::string preScript() const { return _preScript; }

    /**
     * Set name of script which is executed after installation of patch.
     */
    void setPostScript( const std::string &postScript ) { _postScript = postScript; }
    /**
     * Return name of script which is executed after installation of patch.
     */
    std::string postScript() const { return _postScript; }

    /**
     * Add a package to this patch.
     *
     */
    void addPackage( const PMPackagePtr &pkg );
    /**
     * Return list of all packages belonging to this patch.
     */
    std::list<PMPackagePtr> packages() const { return _packages; }

    /**
     * Set name of file where patch info is stored on local system.
     */
    void setLocalFile( const Pathname &localFile ) { _localFile = localFile; }
    /**
     * Return name of file where patch info is stored on local system.
     */
    Pathname localFile() const { return _localFile; }

    /**
      Set size of patch.
    */
    void setPatchSize( const FSize &size );

    /**
      Return size of patch.
    */
    FSize patchSize() const;

    /**
     * Return full name in the format "name-version-release".
     *
     */
    std::string fullName() const;

    /**
      Set flag indicating, if the patch contains packages which are alread
      installed in some version.
    */
    void setPackagesInstalled( bool installed ) { _packagesInstalled = installed; }

    /**
      Return, if patch contains installed patches.
    */
    bool packagesInstalled() const { return _packagesInstalled; }


    /**
      Set list of extra files.
    */
    void setFiles( const std::list<PMYouFile> &files );
    
    /**
      Add extra file to patch.
    */
    void addFile( const PMYouFile &file );

    /**
      Return list of YOU patch extra files.
    */
    std::list<PMYouFile> files() const { return _files; }


  public:

    /**
     * PMObject attributes that should be realized by each concrete Object.
     * @see PMObject
     **/
    virtual std::string            summary()     const { return shortDescription(); }
    virtual std::list<std::string> description() const;
    virtual std::list<std::string> insnotify()       const;
    virtual std::list<std::string> delnotify()       const { return PMObject::delnotify(); }
    virtual FSize                  size()            const { return patchSize(); }
    virtual bool                   providesSources() const { return PMObject::providesSources(); }

    virtual std::string            instSrcLabel()    const { return PMObject::instSrcLabel(); }
    virtual Vendor                 instSrcVendor()   const { return PMObject::instSrcVendor(); }
    virtual unsigned               instSrcRank()     const { return PMObject::instSrcRank(); }

  private:

    std::string _shortDescription, _longDescription;
    std::string _preInformation, _postInformation;
    Kind _kind;
    bool _updateOnlyInstalled;
    std::string _preScript, _postScript;
    FSize _patchSize;

    std::list<PMPackagePtr> _packages;

    Pathname _localFile;

    bool _packagesInstalled;

    std::list<PMYouFile> _files;

  public:

    PMYouPatch( const PkgName &    name_r,
		const PkgEdition & edition_r,
                const PkgArch &    arch_r );

    virtual ~PMYouPatch();

  public:

    virtual std::ostream & dumpOn( std::ostream & str ) const;
};

///////////////////////////////////////////////////////////////////

#endif // PMYouPatch_h
