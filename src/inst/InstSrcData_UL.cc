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

#include <y2pm/InstSrcData_UL.h>

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
    MIL << lbuf << "=" << vptr << endl;

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

    InstSrcDataPtr ndata( new InstSrcData );

    ///////////////////////////////////////////////////////////////////
    // parse packages file and fill into ndata
    ///////////////////////////////////////////////////////////////////

    TagParser parser;
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
    std::ifstream packages (packagesname.asString().c_str());

    if( !packages)
    {
	return Error::E_open_file;
    }

    CommonPkdParser::TagSet* tagset;
    tagset = new InstSrcData_ULTags ();
    
    bool repeatassign = false;
    bool parse = true;
    
    while( parse && parser.lookupTag (packages))
    {
	tagstr = parser.startTag();

	do
	{
	    switch( tagset->assign (tagstr.c_str(), parser, packages))
	    {
		case CommonPkdParser::Tag::ACCEPTED:
		    // fill InstSrcDescr 
		    Tag2Package( tagset );
		    count++;
		    repeatassign = false;
		    err = Error::E_ok;
		    break;
		case CommonPkdParser::Tag::REJECTED_NOMATCH:
		    repeatassign = false;
		    break;
		case CommonPkdParser::Tag::REJECTED_FULL:
		    //Tag2Package( tagset );
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

    tagset->clear();

    if( !parse )
	std::cerr << "*** parsing was aborted ***" << std::endl;
    else
	std::cerr << "*** parsed " << count << " packages ***" << std::endl;
    
    ///////////////////////////////////////////////////////////////////
    // done
    ///////////////////////////////////////////////////////////////////

    if ( err == Error::E_ok )
    {
	ndata_r = ndata;
    }

    return err;
}


PMPackagePtr
InstSrcData_UL::Tag2Package( CommonPkdParser::TagSet * tagset )
{
    string package ((tagset->getTagByIndex(InstSrcData_ULTags::PACKAGE))->Data());
    std::vector<std::string> package_splitted;
    stringutil::split (package, package_splitted, " ", false);
//MIL << package_splitted[0] << "-" << package_splitted[1] << "-" << package_splitted[2] << "." << package_splitted[3] << endl;
    PkgName name (package_splitted[0]);
    PkgEdition edition (package_splitted[1].c_str(), package_splitted[2].c_str());
    PkgArch arch (package_splitted[3]);
    PMPackagePtr pac( new PMPackage (name, edition, arch));

    return pac;
}


