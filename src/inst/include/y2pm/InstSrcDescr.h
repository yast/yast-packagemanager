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

   Purpose:	installation source description
		keeps all information to identify an installation
		source, but does not have knowledge about its contents
		see InstSrcData.h for the contents

		This class automagically determines the type
		of media (SuSE CD, Patch CD, plain directory with rpms, ...)
		by 'probing'
/-*/
#ifndef InstSrcDescr_h
#define InstSrcDescr_h

#include <iosfwd>

#include <string>
#include <list>

#include <y2util/Pathname.h>
#include <y2pm/InstSrcDescrPtr.h>	// pointer to self

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : InstSrcDescr

class REP_CLASS(InstSrcDescr) {
  REP_BODY(InstSrcDescr)

  public:

    /**
     * constructor
     * initialization with new media
     */
    InstSrcDescr (MediaAccess *media);

    /**
     * constructor
     * initialization with known media
     */
    InstSrcDescr (const Pathname & descrcachefile);

    virtual ~InstSrcDescr();
  
  private:
    // all this data is saved to a cache file when
    // this sources gets registered (for later use)

    /**
     * a source might be temporarely disabled
     */
    bool _activated;

    std::string _url;		// url for media access
    int count;			// media count (if spawned on multiple cds)
    std::string _id;		// ID code for media match

    // data from suse/setup/descr/info
    std::string _vendor;
    std::string _default_language;
    std::string _default_timezone;
    std::string _version;
    int _release;
    std::string _name;
    std::string _product;
    std::string _type;
    int _numparts;
    std::list<std::string> _architectures;

    // internal helper function
    // used to parse suse/setup/descr/info
    void parseSuSEFile (const Pathname & mountpoint, const Pathname & susefile, bool new_media);

  public:
    std::string getURL (void) const { return _url; }

    /**
     * write media description to cache file
     * @return pathname of written cache
     * writes private data to an ascii file
     */
    const Pathname writeCache (void);

    //-----------------------------
    // activation status

    /**
     * return activation status
     */
    bool getActivation() const;

    /**
     * temporary (de)activate source
     */
    void setActivation (bool yesno);

    const std::string& Vendor() const { return _vendor; }
    const std::string& Language() const { return _default_language; }
    const std::string& Timezone() const { return _default_timezone; }
    const std::string& Version() const { return _version; }
    int Release() const { return _release; }
    const std::string& Name() const { return _name; }
    const std::string& Product() const { return _product; }
    const std::string& Id() const { return _id; }
    int NumParts() const { return _numparts; }
    const std::list<std::string>& Architectures() const { return _architectures; }

    bool operator==(const InstSrcDescr& d) {return (_id == d._id); }

    virtual std::ostream & dumpOn( std::ostream & str ) const;
};

///////////////////////////////////////////////////////////////////

#endif // InstSrcDescr_h

