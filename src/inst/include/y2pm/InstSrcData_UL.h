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

  File:       InstSrcData_UL.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Concrete InstSrcData able to handle UnitedLinux style layout.

/-*/
#ifndef InstSrcData_UL_h
#define InstSrcData_UL_h

#include <iosfwd>
#include <fstream>
#include <y2util/CommonPkdParser.h>

#include <y2pm/InstSrcData_ULPtr.h>
#include <y2pm/InstSrcData.h>
#include <y2pm/PMPackage.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : InstSrcData_UL
/**
 * @short Concrete InstSrcData able to handle UnitedLinux style layout.
 **/
class InstSrcData_UL : virtual public Rep, public InstSrcData {
  REP_BODY(InstSrcData_UL);

  public:

    /**
     * default error class
     **/
    typedef InstSrcError Error;


  public:

    InstSrcData_UL();

    virtual ~InstSrcData_UL();

  public:

    virtual std::ostream & dumpOn( std::ostream & str ) const;

  public:

    /**
     * Any concrete InstSrcData must realize this, as it knows the expected
     * layout on the media. Expect MediaAccessPtr to be open and attached.
     *
     * Return the InstSrcDescr retrieved from the media via ndescr_r,
     * or NULL and PMError set.
     **/
    static PMError tryGetDescr( InstSrcDescrPtr & ndescr_r,
				MediaAccessPtr media_r, const Pathname & produduct_dir_r );

    static PMError tryGetData( InstSrcDataPtr & ndata_r,
				MediaAccessPtr media_r, const Pathname & descr_dir_r );

    private:
	static PMPackagePtr Tag2Package( CommonPkdParser::TagSet * tagset );
};

///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : InstSrcData_ULTags
/**
 * @short provides the tag set for the packages file
 * (to feed the tag parser) 
 *
 **/
class InstSrcData_ULTags : public CommonPkdParser::TagSet
{

public:
    enum Tags {
	VERSION,	// general file format version
	PACKAGE,	// name version release arch
	REQUIRES,	// list of requires tags
	PROVIDES,	// list of provides tags
	CONFLICTS,	// list of conflicts tags
	OBSOLETES,	// list of obsoletes tags
	RECOMMENDS,	// list of recommends tags
	SUGGESTS,	// list of suggests tags
	LOCATION,	// file location
	SIZE,		// packed and unpacked size
	BUILDTIME,	// buildtime
	SOURCE,		// source package
	GROUP,		// rpm group
	LICENSE,	// license
	AUTHORS,	// list of authors
	SHAREWITH,	// package to share data with
	KEYWORDS,	// list of keywords
	NUM_TAGS
    };

public:
    InstSrcData_ULTags( ) 
	: TagSet()	{

	CommonPkdParser::Tag* t;
	createTag( "=Ver", VERSION);		// general file format version
	createTag( "=Pkg", PACKAGE);		// name version release arch
	t = createTag( "+Req", REQUIRES);	// list of requires tags
	t->setEndTag("-Req");
	t = createTag( "+Prv", PROVIDES);	// list of provides tags
	t->setEndTag("-Prv");
	t = createTag( "+Con", CONFLICTS);	// list of conflicts tags
	t->setEndTag("-Con");
	t = createTag( "+Obs", OBSOLETES);	// list of obsoletes tags
	t->setEndTag("-Obs");
	t = createTag( "+Rec", RECOMMENDS);	// list of recommends tags
	t->setEndTag("-Rec");
	t = createTag( "+Sug", SUGGESTS);	// list of suggests tags
	t->setEndTag("-Sug");
	createTag( "=Loc", LOCATION);		// file location
	createTag( "=Siz", SIZE);		// packed and unpacked size
	createTag( "=Tim", BUILDTIME);		// buildtime
	createTag( "=Src", SOURCE);		// source package
	createTag( "=Grp", GROUP);		// rpm group
	createTag( "=Lic", LICENSE);		// license
	t = createTag( "+Aut", AUTHORS);	// list of authors
	t->setEndTag("-Aut");
	createTag( "=Shr", SHAREWITH);		// package to share data with
    };

private:

    CommonPkdParser::Tag* createTag( std::string tagName, Tags tagEnum ) {
	
	CommonPkdParser::Tag* t;
	t = new CommonPkdParser::Tag( tagName, CommonPkdParser::Tag::ACCEPTONCE );
	this->addTag(t);
	addTagByIndex( tagEnum, t );

	return t;
    }
    
};

#endif // InstSrcData_UL_h

