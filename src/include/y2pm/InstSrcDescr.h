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

/-*/
#ifndef InstSrcDescr_h
#define InstSrcDescr_h

#include <iosfwd>

#include <string>
#include <list>
#include <y2pm/InstSrcDescrPtr.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : InstSrcDescr
/**
 *
 **/
class REP_CLASS(InstSrcDescr) {
  REP_BODY(InstSrcDescr)

  public:

    InstSrcDescr();

    virtual ~InstSrcDescr();
  
  private:
    std::string _vendor;
    std::string _default_language;
    std::string _default_timezone;
    std::string _version;
    int _release;
    std::string _name;
    std::string _product;
    std::string _id;
    std::string _type;
    int _numparts;
    std::list<std::string> _architectures;

  public:
    /** SuSE */
    const std::string& Vendor() { return _vendor; }
    /** de */
    const std::string& Language() { return _default_language; }
    /** Europe/Berlin */
    const std::string& Timezone() { return _default_timezone; }
    /** 8.1 */
    const std::string& Version() { return _version; }
    /** 0 */
    int Release() { return _release; }
    /** SuSE Linux */
    const std::string& Name() { return _name; }
    /** Professional, SLES */
    const std::string& Product() { return _product; }
    /** id to identify medium */
    const std::string& Id() { return _id; }
    /** Full, Update/Patch, Maintenance, Customer, ... */
    const std::string& Type() { return _type; }
    /** eg number cds */
    int NumParts() { return _numparts; }
    /** i386, ppc, noarch */
    const std::list<std::string>& Architectures() { return _architectures; }

    void setVendor( const std::string& vendor) { _vendor = vendor; }
    void setLanguage( const std::string& language) { _default_language = language; }
    void setTimezone( const std::string& timezone) { _default_timezone = timezone; }
    void setVersion( const std::string& version) { _version = version; }
    void setRelease( int release ) { _release = release; }
    void setName( const std::string& name) { _name = name; }
    void setProduct( const std::string& product) { _product = product; }
    void setId( const std::string& id) { _id = id; }
    void setType( const std::string& type) { _type = type; }
    void setNumParts( int num ) { _numparts = num; }
    void setArchitectures( const std::list<std::string>& archs) { _architectures = archs; }

    bool operator==(const InstSrcDescr& d) {return (_id == d._id); }

    virtual std::ostream & dumpOn( std::ostream & str ) const;
};

///////////////////////////////////////////////////////////////////

#endif // InstSrcDescr_h

