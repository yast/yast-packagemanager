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

  File:       ULParsePackagesLang.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose:

/-*/
#ifndef ULParsePackagesLang_h
#define ULParsePackagesLang_h

#include <iostream>
#include <fstream>
#include <string>

#include <y2util/Pathname.h>
#include <y2util/LangCode.h>
#include <y2util/TaggedParser.h>
#include <y2util/TaggedFile.h>
#include <y2util/TagCacheRetrievalPtr.h>

#include <y2pm/InstSrcError.h>
#include <y2pm/PkgIdent.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : ULParsePackagesLang
/**
 *
 **/
class ULParsePackagesLang {

  friend std::ostream & operator<<( std::ostream & str, const ULParsePackagesLang & obj );

  ULParsePackagesLang & operator=( const ULParsePackagesLang & );
  ULParsePackagesLang            ( const ULParsePackagesLang & );

  public:

    typedef InstSrcError Error;

    struct Entry : public PkgIdent {
      friend std::ostream & operator<<( std::ostream & str, const  Entry & obj );

      TagCacheRetrievalPtr retrieval;

      TagRetrievalPos posSUMMARY;
      TagRetrievalPos posDESCRIPTION;
      TagRetrievalPos posINSNOTIFY;
      TagRetrievalPos posDELNOTIFY;
      TagRetrievalPos posLICENSETOCONFIRM;

      Entry() {}
      Entry( const PkgIdent & pkgident_r, TagCacheRetrievalPtr retrieval_r )
	: PkgIdent( pkgident_r )
	, retrieval( retrieval_r )
      {}
    };

  private:

    // Tag ids for the TaggedParser
    enum Tags {
      PACKAGE,	        // name version release arch
      SUMMARY,	        // short summary (label)
      DESCRIPTION,      // long description
      INSNOTIFY,	// install notification
      DELNOTIFY,	// delete notification
      LICENSETOCONFIRM,	// license to confirm upon install
      // LAST ENTRY:
      NUM_TAGS
    };

  private:

    const Pathname _file;
    std::ifstream  _stream;
    std::string    _version;

    TaggedParser         _parser;
    TaggedFile::TagSet   _tagset;
    TagCacheRetrievalPtr _retrieval;

  private:

    void getData( Entry & entry_r );

  public:

    ULParsePackagesLang( const Pathname & file_r );
    ~ULParsePackagesLang();

    TagCacheRetrievalPtr getRetrieval() { return _retrieval; }

    TaggedFile::assignstatus getEntry( Entry & entry_r );

    Entry noEntry() const { return Entry( PkgIdent(), _retrieval ); }
};

///////////////////////////////////////////////////////////////////

#endif // ULParsePackagesLang_h
