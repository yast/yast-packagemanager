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

   File:       InstSrcDescr.h

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

   Purpose:	Installation source description
		Keeps all information to identify and describe an installation
		source, but does not have knowledge about its contents (i.e the
		packages, selectins, etc ). See InstSrcData for the contents.

/-*/
#ifndef InstSrcDescr_h
#define InstSrcDescr_h

#include <iosfwd>

#include <list>
#include <string>

#include <y2util/Url.h>
#include <y2util/Pathname.h>
#include <y2util/LangCode.h>

#include <y2pm/InstSrcDescrPtr.h>

#include <y2pm/InstSrc.h>
#include <y2pm/PkgRelation.h>
#include <y2pm/PkgArch.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : InstSrcDescr
/**
 * @short Installation source description
 *
 * Keeps all information to identify and describe an installation
 * source, but does not have knowledge about its contents (i.e the
 * packages, selectins, etc ). See InstSrcData for the contents.
 *
 * See also <A HREF="../media/media-descr.html">media-descr.html</A>
 **/
class InstSrcDescr : virtual public Rep {
  REP_BODY(InstSrcDescr);

  public:

    /**
     * default error class
     **/
    typedef InstSrcError Error;

    typedef std::map<PkgArch,std::list<Pathname> > ArchMap;
    typedef std::map<LangCode,std::string>         LabelMap;
    typedef std::list<LangCode>                    LinguasList;

  public:

    InstSrcDescr();

    virtual ~InstSrcDescr();

  private:

    ///////////////////////////////////////////////////////////////////
    // Basic data NOT TO BE STORED IN CACHE (e.g. InstTaget/UI related)
    ///////////////////////////////////////////////////////////////////

    /**
     * Name of the cachefile to read or write.
     **/
    static const std::string _cache_file;

    /**
     * The architecture of the target system (determined by hwinfo).
     **/
    PkgArch _base_arch;

  private:

    ///////////////////////////////////////////////////////////////////
    // CACHE DATA start here
    ///////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////
    // InstSrc related/provided data. Can not be retrieved from media.
    ///////////////////////////////////////////////////////////////////

    /**
     * Type of InstSrc found on media.
     **/
    InstSrc::Type _type;

    /**
     * Url for media access (media's root dir)
     **/
    Url _url;

    /**
     * Product dir below _url
     **/
    Pathname _product_dir;

    /**
     * Should Instsrc be activated by default.
     **/
    bool _default_acitivate;

    ///////////////////////////////////////////////////////////////////
    // media file ( _url / media.N / media )
    ///////////////////////////////////////////////////////////////////

    std::string _media_vendor;		//
    std::string _media_id;		// ID code for media match
    std::string _media_count;		//

    ///////////////////////////////////////////////////////////////////
    // content file ( _url / _product_dir / content )
    ///////////////////////////////////////////////////////////////////

    PkgNameEd   _content_product;
    PkgNameEd   _content_baseproduct;
    std::string _content_vendor;


    PkgArch _content_defaultarch;
    ArchMap _content_archmap;

    PkgRelation _content_requires;


    LangCode _content_language;
    LinguasList _content_linguas;
    std::string _content_label;
    LabelMap _content_labelmap;

    std::string _content_timezone;

    /**
     * package description directory below _product_dir
     **/
    Pathname _content_descrdir;

    /**
     * package data directory below _product_dir
     **/
    Pathname _content_datadir;

  public:

    ///////////////////////////////////////////////////////////////////
    // access functions
    ///////////////////////////////////////////////////////////////////

    const PkgArch &      base_arch()           const { return _base_arch; }
    InstSrc::Type        type()                const { return _type; }
    const Url &          url()                 const { return _url; }
    const Pathname &     product_dir()         const { return _product_dir; }
    bool                 default_acitivate()   const { return _default_acitivate; }

    const std::string &  media_vendor()        const { return _media_vendor; }
    const std::string &  media_id()            const { return _media_id; }
    const std::string &  media_count()         const { return _media_count; }

    const PkgNameEd &    content_product()     const { return _content_product; }
    const PkgNameEd &    content_baseproduct() const { return _content_baseproduct; }
    const std::string &  content_vendor()      const { return _content_vendor; }
    const PkgArch &      content_defaultarch() const { return _content_defaultarch; }
    const ArchMap &      content_archmap()     const { return _content_archmap; }
    const PkgRelation &  content_requires()    const { return _content_requires; }
    const LangCode &     content_language()    const { return _content_language; }
    const LinguasList &  content_linguas()     const { return _content_linguas; }
    const std::string &  content_label()       const { return _content_label; }
    const LabelMap &     content_labelmap()    const { return _content_labelmap; }
    const std::string &  content_timezone()    const { return _content_timezone; }
    const Pathname &     content_descrdir()    const { return _content_descrdir; }
    const Pathname &     content_datadir()     const { return _content_datadir; }

  public:

    virtual std::ostream & dumpOn( std::ostream & str ) const;

  public:

    /**
     * Create new InstSrcDescr reading data from cache description
     **/
    static PMError readCache( InstSrcDescrPtr & ndescr_r, const Pathname & cache_dir_r );

    /**
     * Write data to cache description
     **/
    PMError writeCache( const Pathname & cache_dir_r ) const;
};

///////////////////////////////////////////////////////////////////

#endif // InstSrcDescr_h

