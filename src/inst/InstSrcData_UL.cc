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

  File:       InstSrcData_UL.cc

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose:

/-*/

#include <iostream>
#include <fstream>

#include <ctype.h>

#include <y2util/Y2SLog.h>
#include <y2util/PathInfo.h>
#include <y2util/stringutil.h>

#include <y2pm/PMPackagePtr.h>
#include <y2pm/InstSrcData_UL.h>
#include <y2pm/PMULPackageDataProvider.h>
#include <y2pm/PMULPackageDataProviderPtr.h>

#include <y2pm/InstSrcDescr.h>
#include <y2pm/MediaAccess.h>

#include <y2pm/PkgName.h>
#include <y2pm/PkgEdition.h>
#include <y2pm/PkgArch.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//	CLASS NAME : InstSrcData_ULPtr
//	CLASS NAME : constInstSrcData_ULPtr
///////////////////////////////////////////////////////////////////
IMPL_DERIVED_POINTER(InstSrcData_UL,InstSrcData,InstSrcData);

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcData_UL::InstSrcData_UL
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
InstSrcData_UL::InstSrcData_UL()
    : InstSrcData ("")
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcData_UL::~InstSrcData_UL
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
InstSrcData_UL::~InstSrcData_UL()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcData_UL::dumpOn
//	METHOD TYPE : ostream &
//
//	DESCRIPTION :
//
ostream & InstSrcData_UL::dumpOn( ostream & str ) const
{
  Rep::dumpOn( str );
  return str;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcData_UL::tryGetDescr
//	METHOD TYPE : PMError
//
//	DESCRIPTION : try to read content data (describing the product)
//			and fill InstSrcDescrPtr class
//
PMError InstSrcData_UL::tryGetDescr( InstSrcDescrPtr & ndescr_r,
				     MediaAccessPtr media_r, const Pathname & product_dir_r )
{
    MIL << "InstSrcData_UL::tryGetDescr(" << product_dir_r << ")" << endl;

    ndescr_r = 0;
    PMError err;

    InstSrcDescrPtr ndescr( new InstSrcDescr );

    ///////////////////////////////////////////////////////////////////
    // parse InstSrcDescr from media_r and fill ndescr
    ///////////////////////////////////////////////////////////////////

    Pathname contentname = product_dir_r + "/content";
    MIL << "provideFile (" << contentname << ")" << endl;
    err = media_r->provideFile (contentname);
    if (err != Error::E_ok)
    {
	return err;
    }

    contentname = media_r->getAttachPoint() + contentname;
    MIL << "ifstream(" << contentname << ")" << endl;
    std::ifstream content (contentname.asString().c_str());
    if (!content)
    {
	return Error::E_open_file;
    }

    err = Error::E_ok;

    PkgName pname, bname;
    PkgEdition pversion, bversion;	// base product
    InstSrcDescr::ArchMap archmap;
    InstSrcDescr::LabelMap labelmap;

  while (content.good())
  {
    char lbuf[201];

    if (!content.getline (lbuf, 200, '\n'))
    {
      if (content.eof())
	break;
      MIL << "getine() failed" << endl;
      err = Error::E_no_instsrc_on_media;
      break;
    }

    char *lptr = lbuf;
    while (!isblank (*lptr)) lptr++;
    if (*lptr == 0)		// empty value
	continue;
    *lptr++ = 0;
    while (isblank (*lptr)) lptr++;
    if (*lptr == 0)		// empty value
	continue;
    char *vptr = lptr;		// vptr == value
    while (*lptr) lptr++;
    lptr--;

    if (strcmp (lbuf, "PRODUCT") == 0)
    {
	pname = PkgName (vptr);
    }
    else if (strcmp (lbuf, "VERSION") == 0)
    {
	pversion = PkgEdition (vptr);
    }
    else if (strcmp (lbuf, "BASEPRODUCT") == 0)
    {
	bname = PkgName (vptr);
    }
    else if (strcmp (lbuf, "BASEVERSION") == 0)
    {
	bversion = PkgEdition (vptr);
    }
    else if (strcmp (lbuf, "VENDOR") == 0)
    {
	ndescr->set_content_vendor (vptr);
    }
    else if (strcmp (lbuf, "DEFAULTBASE") == 0)
    {
	ndescr->set_content_defaultbase (vptr);
    }
    else if (strncmp (lbuf, "ARCH.", 5) == 0)
    {
	std::list<Pathname> pathlist;
	char *path = vptr;
	while (*vptr)
	{
	    if (isblank (*vptr))
	    {
		*vptr++ = 0;
		pathlist.push_back (Pathname (path));
		path = vptr;
	    }
	    vptr++;
	}
	pathlist.push_back (Pathname (path));
	archmap[lbuf+5] = pathlist;
    }
    else if (strcmp (lbuf, "LINGUAS") == 0)
    {
	std::list<LangCode> langlist;
	char *lang = vptr;
	while (*vptr)
	{
	    if (isblank (*vptr))
	    {
		*vptr++ = 0;
		langlist.push_back (LangCode (lang));
		lang = vptr;
	    }
	    vptr++;
	}
	langlist.push_back (LangCode (lang));
	ndescr->set_content_linguas (langlist);
    }
    else if (strcmp (lbuf, "LANGUAGE") == 0)
    {
	ndescr->set_content_language (LangCode (vptr));
    }
    else if (strcmp (lbuf, "LABEL") == 0)
    {
	ndescr->set_content_label (vptr);
    }
    else if (strncmp (lbuf, "LABEL.", 6) == 0)
    {
	labelmap[LangCode(lbuf+6)] = vptr;
    }
    else if (strcmp (lbuf, "TIMEZONE") == 0)
    {
	ndescr->set_content_timezone (vptr);
    }
    else if (strcmp (lbuf, "DESCRDIR") == 0)
    {
	ndescr->set_content_descrdir (vptr);
    }
    else if (strcmp (lbuf, "DATADIR") == 0)
    {
	ndescr->set_content_datadir (vptr);
    }
    else if (strcmp (lbuf, "REQUIRES") == 0)
    {
	ndescr->set_content_requires (PkgRelation (PkgName (vptr), NONE, PkgEdition ()));
    }
  }

  ndescr->set_content_product (PkgNameEd (pname, pversion));
  ndescr->set_content_baseproduct (PkgNameEd (bname, bversion));
  ndescr->set_content_archmap (archmap);
  ndescr->set_content_labelmap (labelmap);

  ///////////////////////////////////////////////////////////////////
  // done
  ///////////////////////////////////////////////////////////////////
  if ( !err ) {
    ndescr_r = ndescr;
  }
  return err;
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcData_UL::tryGetData
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError InstSrcData_UL::tryGetData( InstSrcDataPtr & ndata_r,
				     MediaAccessPtr media_r, const Pathname & descr_dir_r )
{
    MIL << "InstSrcData_UL::tryGetData(" << descr_dir_r << ")" << endl;
    int count = 0;

    ndata_r = 0;
    PMError err;

    std::list<PMPackagePtr> *pacs = new (std::list<PMPackagePtr>);
    InstSrcDataPtr ndata( new InstSrcData (media_r) );

    ///////////////////////////////////////////////////////////////////
    // parse packages file and fill into ndata
    ///////////////////////////////////////////////////////////////////

    std::string tagstr;

    Pathname packagesname = descr_dir_r + "/packages";
    MIL << "provideFile (" << packagesname << ")" << endl;
    err = media_r->provideFile (packagesname);
    if (err != Error::E_ok)
    {
	return err;
    }

    packagesname = media_r->getAttachPoint() + packagesname;
    MIL << "fopen(" << packagesname << ")" << endl;

    TagCacheRetrieval *retrieval = new TagCacheRetrieval (packagesname);

    std::ifstream& packages = retrieval->getStream();

    if( !packages)
    {
	return Error::E_open_file;
    }

    CommonPkdParser::TagSet* tagset;
    tagset = new InstSrcData_ULTags ();

    bool parse = true;

    TagParser & parser = retrieval->getParser();

    while( parse && parser.lookupTag (packages))
    {
	bool repeatassign = false;

	tagstr = parser.startTag();

	do
	{
	    switch( tagset->assign (tagstr.c_str(), parser, packages))
	    {
		case CommonPkdParser::Tag::ACCEPTED:
		    repeatassign = false;
		    err = Error::E_ok;
		    break;
		case CommonPkdParser::Tag::REJECTED_NOMATCH:
		    repeatassign = false;
		    break;
		case CommonPkdParser::Tag::REJECTED_FULL:
		    pacs->push_back (Tag2Package( retrieval, tagset ));
		    count++;
		    tagset->clear();
		    repeatassign = true;
		    err = Error::E_ok;
		    break;
		case CommonPkdParser::Tag::REJECTED_NOENDTAG:
		    repeatassign = false;
		    parse = false;
		    break;
	    }
	} while( repeatassign );
    }

    if (parse)
    {
	pacs->push_back (Tag2Package( retrieval, tagset ));
	count++;

	ndata->setPackages (pacs);
    }
    tagset->clear();

    if( !parse )
	std::cerr << "*** parsing was aborted ***" << std::endl;
    else
	std::cerr << "*** parsed " << count << " packages ***" << std::endl;

    ///////////////////////////////////////////////////////////////////
    // done
    ///////////////////////////////////////////////////////////////////

    if ( !err )
    {
	ndata_r = ndata;
    }
    else
    {
	ERR << "tryGetData failed: " << err << endl;
    }

    return err;
}

PMPackagePtr
InstSrcData_UL::Tag2Package( TagCacheRetrieval *retrieval, CommonPkdParser::TagSet * tagset )
{
    // PACKAGE
    string single ((tagset->getTagByIndex(InstSrcData_ULTags::PACKAGE))->Data());

    std::vector<std::string> splitted;

    stringutil::split (single, splitted, " ", false);
//MIL << "-----------------------------" << endl;
//MIL << splitted[0] << "-" << splitted[1] << "-" << splitted[2] << "." << splitted[3] << endl;

    // Pkg -> PMPackage
    PkgName name (splitted[0]);
    PkgEdition edition (splitted[1].c_str(), splitted[2].c_str());
    PkgArch arch (splitted[3]);

    PMULPackageDataProviderPtr dataprovider ( new PMULPackageDataProvider (retrieval));
    PMPackagePtr pac( new PMPackage (name, edition, arch, dataprovider));
    CommonPkdParser::Tag *tagptr;
#define SET_VALUE(tagname,value) \
    dataprovider->setAttributeValue (pac, (PMPackage::PMPackageAttribute)PMPackage::ATTR_##tagname, value)
#define SET_POS(tagname,begin,end) \
    dataprovider->setAttributeValue (pac, (PMPackage::PMPackageAttribute)PMPackage::ATTR_##tagname, begin, end)
#define GET_TAG(tagname) \
    tagset->getTagByIndex(InstSrcData_ULTags::tagname)
#define SET_MULTI(tagname) \
    do { tagptr = GET_TAG (tagname); SET_POS (tagname, tagptr->posDataStart(), tagptr->posDataEnd()); } while (0)
#define SET_SINGLE(tagname) \
    SET_VALUE (tagname, (tagset->getTagByIndex(InstSrcData_ULTags::tagname))->Data())

    // -> PMPackage::PMSolvable::PMSolvableAttribute::ATTR_NAME
    SET_VALUE (NAME, splitted[0]);
    // -> PMPackage::PMSolvable::PMSolvableAttribute::ATTR_VERSION
    SET_VALUE (VERSION, splitted[1]);
    // -> PMPackage::PMSolvable::PMSolvableAttribute::ATTR_RELEASE
    SET_VALUE (RELEASE, splitted[2]);
    // -> PMPackage::PMSolvable::PMSolvableAttribute::ATTR_ARCH
    SET_VALUE (ARCH, splitted[3]);

    // REQUIRES, list of requires tags
    // -> PMPackage::PMSolvable::PMSolvableAttribute::ATTR_REQUIRES
    SET_MULTI (REQUIRES);

    // PREREQUIRES, list of pre-requires tags
    // -> PMPackage::PMSolvable::PMSolvableAttribute::ATTR_PREREQUIRES
    SET_MULTI (PREREQUIRES);

    // PROVIDES, list of provides tags
    // -> PMPackage::PMSolvable::PMSolvableAttribute::ATTR_PROVIDES
    SET_MULTI (PROVIDES);

    // CONFLICTS, list of conflicts tags
    // -> PMPackage::PMSolvable::PMSolvableAttribute::ATTR_CONFLICTS
    SET_MULTI (CONFLICTS);

    // OBSOLETES, list of obsoletes tags
    // -> PMPackage::PMSolvable::PMSolvableAttribute::ATTR_OBSOLETES
    SET_MULTI (OBSOLETES);

    // RECOMMENDS, list of recommends tags
    // FIXME Where to put RECOMMENDS ?
    //SET_MULTI (RECOMMENDS);

    // SUGGESTS, list of suggests tags
    // FIXME Where to put SUGGESTS ?
    //SET_MULTI (SUGGESTS);

    // LOCATION, file location
    // FIXME Where to put LOCATION ?
    //SET_SINGLE (LOCATION);

    // SIZE, packed and unpacked size
    // -> PMPackage::PMObjectAttribute::ATTR_SIZE (installed)
    // -> PMPackage::PMPackageAttribute::ATTR_ARCHIVESIZE (package)
    stringutil::split ((tagset->getTagByIndex(InstSrcData_ULTags::SIZE))->Data(), splitted, " ", false);
    dataprovider->setAttributeValue (pac, PMPackage::ATTR_ARCHIVESIZE, splitted[0]);
    dataprovider->setAttributeValue (pac, (PMPackage::PMPackageAttribute)PMPackage::ATTR_SIZE, splitted[1]);

    // BUILDTIME, buildtime
    // -> PMPackage::PMPackageAttribute::ATTR_BUILDTIME
    SET_SINGLE (BUILDTIME);

    // SOURCE, source package
    // PMPackage::PMPackageAttribute::ATTR_SOURCERPM
    SET_SINGLE (SOURCERPM);

    // GROUP, rpm group
    // -> PMPackage::PMPackageAttribute::ATTR_GROUP
    SET_SINGLE (GROUP);

    // LICENSE, license
    // -> PMPackage::PMPackageAttribute::ATTR_LICENSE
    SET_SINGLE (LICENSE);

    // AUTHORS, list of authors
    // -> PMPackage::PMPackageAttribute::ATTR_AUTHOR
    SET_MULTI (AUTHOR);

    // SHAREWITH, package to share data with
    // FIXME
    //string sharewith ((tagset->getTagByIndex(InstSrcData_ULTags::SHAREWITH))->Data());

    // KEYWORDS, list of keywords
    // FIXME Where to put KEYWORDS
    //SET_MULTI (KEYWORDS);

    return pac;
}
