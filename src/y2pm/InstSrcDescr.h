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
#include <y2util/Vendor.h>
#include <y2util/Pathname.h>
#include <y2util/LangCode.h>

#include <y2pm/InstSrcDescrPtr.h>
#include <y2pm/F_Media.h>

#include <y2pm/InstSrc.h>
#include <y2pm/ProductIdent.h>
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
class InstSrcDescr : public CountedRep {
  REP_BODY(InstSrcDescr);

  public:

    /**
     * default error class
     **/
    typedef InstSrcError Error;

    typedef std::map<std::string,std::list<PkgArch> >   ArchMap;
    typedef std::map<LangCode,std::string>              LabelMap;
    typedef std::list<LangCode>                         LinguasList;

    static const unsigned NO_RANK = unsigned(-1);

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
    static const Pathname _cache_file;

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
     * Url for media access (media's root dir, for packages)
     **/
    Url _url;

    /**
     * Product dir below _url
     **/
    Pathname _product_dir;

    ///////////////////////////////////////////////////////////////////
    // InstSrcManager related/provided data. Can not be retrieved from media.
    ///////////////////////////////////////////////////////////////////

    /**
     * Should InstSrc be activated by default.
     **/
    bool _default_activate;

    /**
     * Rank of this InstSrc.(0 is highest)
     **/
    unsigned _default_rank;

    /**
     * whether base packages for deltas should be fetched from this source
     * */
    bool _usefordeltas;

    ///////////////////////////////////////////////////////////////////
    // media file ( _url / media.N / media )
    ///////////////////////////////////////////////////////////////////

    Vendor      _media_vendor;		// media vendor
    std::string _media_id;		// ID code for media match
    unsigned    _media_count;		// total number if is split up on multiple media
    bool        _media_doublesided;     // true if doublesided media
    F_Media::LabelMap _media_labels;    // media names to use in UI

    ///////////////////////////////////////////////////////////////////
    // content file ( _url / _product_dir / content )
    ///////////////////////////////////////////////////////////////////

    ProductIdent _content_product;
    PkgNameEd    _content_distproduct;
    ProductIdent _content_baseproduct;
    Vendor       _content_vendor;

    std::string _content_defaultbase;
    ArchMap     _content_archmap;

    PkgRelation _content_requires;

    LangCode    _content_language;
    LinguasList _content_linguas;
    std::string _content_label;
    LabelMap    _content_labelmap;

    std::string _content_timezone;

    /**
     * package description directory below _product_dir
     **/
    Pathname _content_descrdir;

    /**
     * package data directory below _product_dir
     **/
    Pathname _content_datadir;

    std::string _content_flags;

    std::string _content_relnotesurl;

    std::string _content_youurl;
    std::string _content_youtype;
    std::string _content_youpath;

  public:

    ///////////////////////////////////////////////////////////////////
    // access functions
    ///////////////////////////////////////////////////////////////////

    InstSrc::Type        type()                const { return _type; }
    const Url &          url()                 const { return _url; }
    const Pathname &     product_dir()         const { return _product_dir; }

    bool                 default_activate()    const { return _default_activate; }
    unsigned             default_rank()        const { return _default_rank; }
    bool                 usefordeltas()        const { return _usefordeltas; }

    const Vendor &       media_vendor()        const { return _media_vendor; }
    const std::string &  media_id()            const { return _media_id; }
    const unsigned &     media_count()         const { return _media_count; }
    bool                 media_doublesided()   const { return _media_doublesided; }
    const PM::LocaleString & media_label( unsigned number_r ) const {
      F_Media::LabelMap::const_iterator found( _media_labels.find( number_r ) );
      return( found == _media_labels.end() ? F_Media::_noLabel : found->second );
    }
    const ProductIdent & content_product()     const { return _content_product; }
    const PkgNameEd &    content_distproduct() const { return _content_distproduct; }
    const ProductIdent & content_baseproduct() const { return _content_baseproduct; }
    const Vendor &       content_vendor()      const { return _content_vendor; }
    const std::string &  content_defaultbase() const { return _content_defaultbase; }
    const ArchMap &      content_archmap()     const { return _content_archmap; }
    const PkgRelation &  content_requires()    const { return _content_requires; }
    const LangCode &     content_language()    const { return _content_language; }
    const LinguasList &  content_linguas()     const { return _content_linguas; }
    const std::string &  content_label()       const { return _content_label; }
    const LabelMap &     content_labelmap()    const { return _content_labelmap; }
    const std::string &  content_timezone()    const { return _content_timezone; }
    const Pathname &     content_descrdir()    const { return _content_descrdir; }
    const Pathname &     content_datadir()     const { return _content_datadir; }
    const std::string &  content_flags()       const { return _content_flags; }
    const std::string &  content_relnotesurl() const { return _content_relnotesurl; }
    const std::string &  content_youurl()      const { return _content_youurl; }
    const std::string &  content_youtype()     const { return _content_youtype; }
    const std::string &  content_youpath()     const { return _content_youpath; }

    ///////////////////////////////////////////////////////////////////
    // convenience functions
    ///////////////////////////////////////////////////////////////////

    /**
     * Package description directory below _url
     **/
    Pathname descrdir() const { return _product_dir + _content_descrdir; }

    /**
     * Package data directory below _url
     **/
    Pathname datadir() const { return _product_dir + _content_datadir; }

    /**
     * Content label according to Y2PM::getPreferredLocale
     **/
    std::string label() const;

  public:

    //////////////////////////////////////////////////////////////////
    // set value functions
    ///////////////////////////////////////////////////////////////////

    void set_type( InstSrc::Type val_r )                      { _type = val_r; }
    void set_url( const Url & val_r )                         { _url = val_r; }
    void set_product_dir( const Pathname & val_r )            { _product_dir = val_r; }

    void set_default_activate( bool val_r )                   { _default_activate = val_r; }
    void set_default_rank( unsigned val_r )                   { _default_rank = val_r; }
    void set_usefordeltas( bool val_r )                       { _usefordeltas = val_r; }

    void set_media_vendor( const Vendor & val_r )             { _media_vendor = val_r; }
    void set_media_id( const std::string & val_r )            { _media_id = val_r; }
    void set_media_count( unsigned val_r )                    { _media_count = val_r; }
    void set_media_doublesided( bool val_r )                  { _media_doublesided = val_r; }
    void set_media_labels( const F_Media::LabelMap & val_r )  { _media_labels = val_r; }

    void set_content_product( const ProductIdent & val_r )    { _content_product = val_r; }
    void set_content_distproduct( const PkgNameEd & val_r )   { _content_distproduct = val_r; }
    void set_content_baseproduct( const ProductIdent & val_r ){ _content_baseproduct = val_r; }
    void set_content_vendor( const Vendor & val_r )           { _content_vendor = val_r; }
    void set_content_defaultbase( const std::string & val_r ) { _content_defaultbase = val_r; }
    void set_content_archmap( const ArchMap & val_r )         { _content_archmap = val_r; }
    void set_content_requires( const PkgRelation & val_r )    { _content_requires = val_r; }
    void set_content_language(const LangCode & val_r )        { _content_language = val_r; }
    void set_content_linguas( const LinguasList & val_r )     { _content_linguas = val_r; }
    void set_content_label( const std::string & val_r )       { _content_label = val_r; }
    void set_content_labelmap( const LabelMap & val_r )       { _content_labelmap = val_r; }
    void set_content_timezone( const std::string & val_r )    { _content_timezone = val_r; }
    void set_content_descrdir( const Pathname & val_r )       { _content_descrdir = val_r; }
    void set_content_datadir( const Pathname & val_r )        { _content_datadir = val_r; }
    void set_content_flags( const std::string & val_r )       { _content_flags = val_r; }
    void set_content_relnotesurl( const std::string & val_r ) { _content_relnotesurl = val_r; }
    void set_content_youurl( const std::string & val_r )      { _content_youurl = val_r; }
    void set_content_youtype( const std::string & val_r )     { _content_youtype = val_r; }
    void set_content_youpath( const std::string & val_r )     { _content_youpath = val_r; }

  public:

    /**
     * Return whether this is a base product. (it has no baseproduct)
     **/
    bool isBaseProduct() const { return _content_baseproduct.undefined(); }

    /**
     * Return whether this has base product base_r. If base_r is NULL,
     * whether this is a base product.
     **/
    bool hasBaseProduct( const constInstSrcDescrPtr & base_r ) const {
      if ( base_r ) {
	return _content_baseproduct.sameProduct( base_r->content_product() );
      }
      return isBaseProduct();
    }

    /**
     * Test for equal content_product.
     **/
    static bool sameContentProduct( const constInstSrcDescrPtr & rhs, const constInstSrcDescrPtr & lhs );

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

  public:

    /**
     * Used by readCache to actually read data from stream.
     **/
    static PMError readStream( InstSrcDescrPtr & ndescr_r, std::istream & str );

    /**
     * Used by writeCache to actually write the data to stream.
     **/
    PMError writeStream( std::ostream & str ) const;
};

///////////////////////////////////////////////////////////////////

#endif // InstSrcDescr_h

