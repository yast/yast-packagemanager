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
//#include <y2pm/PMPackageDataProvider_UL.h>
//#include <y2pm/PMPackageDataProviderPtr.h>

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

    bool parse = true;

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
		    Tag2Package( tagset );
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
	Tag2Package( tagset );
	count++;
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

static
string & vector2string(vector<string> multi)
{
    static string out;
    out = "";
    for (vector<string>::iterator pos = multi.begin(); pos < multi.end(); pos++)
    {
	if (!out.empty())
	    out += " ";
	out += *pos;
    }
    return out;
}

PMPackagePtr
InstSrcData_UL::Tag2Package( CommonPkdParser::TagSet * tagset )
{
    // PACKAGE
    string package ((tagset->getTagByIndex(InstSrcData_ULTags::PACKAGE))->Data());
    std::vector<std::string> multi;
    stringutil::split (package, multi, " ", false);
MIL << "------------------------------------------------------------" << endl;
MIL << multi[0] << "-" << multi[1] << "-" << multi[2] << "." << multi[3] << endl;

    // Pkg -> PMPackage
    PkgName name (multi[0]);
    PkgEdition edition (multi[1].c_str(), multi[2].c_str());
    PkgArch arch (multi[3]);
    PMPackagePtr pac( new PMPackage (name, edition, arch, PMPackageDataProviderPtr() ));
#warning NULL PMPackageDataProviderPtr used to create a package.

    // REQUIRES, list of requires tags
    multi =  (tagset->getTagByIndex(InstSrcData_ULTags::REQUIRES))->MultiData();
MIL << "Requires: " << vector2string(multi) << endl;

    // PREREQUIRES, list of pre-requires tags
    multi =  (tagset->getTagByIndex(InstSrcData_ULTags::PREREQUIRES))->MultiData();
MIL << "PreRequires: " << vector2string(multi) << endl;

    // PROVIDES, list of provides tags
    multi =  (tagset->getTagByIndex(InstSrcData_ULTags::PROVIDES))->MultiData();
MIL << "Provides: " << vector2string(multi) << endl;

    // CONFLICTS, list of conflicts tags
    multi =  (tagset->getTagByIndex(InstSrcData_ULTags::CONFLICTS))->MultiData();
MIL << "Conflicts: " << vector2string(multi) << endl;

    // OBSOLETES, list of obsoletes tags
    multi =  (tagset->getTagByIndex(InstSrcData_ULTags::OBSOLETES))->MultiData();
MIL << "Obsoletes: " << vector2string(multi) << endl;

    // RECOMMENDS, list of recommends tags
    multi =  (tagset->getTagByIndex(InstSrcData_ULTags::RECOMMENDS))->MultiData();
MIL << "Recommends: " << vector2string(multi) << endl;

    // SUGGESTS, list of suggests tags
    multi =  (tagset->getTagByIndex(InstSrcData_ULTags::SUGGESTS))->MultiData();
MIL << "Suggests: " << vector2string(multi) << endl;

    // LOCATION, file location
    string location ((tagset->getTagByIndex(InstSrcData_ULTags::LOCATION))->Data());
MIL << "Location: " << location << endl;

    // SIZE, packed and unpacked size
    string size ((tagset->getTagByIndex(InstSrcData_ULTags::SIZE))->Data());
MIL << "Size: " << size << endl;

    // BUILDTIME, buildtime
    string buildtime ((tagset->getTagByIndex(InstSrcData_ULTags::BUILDTIME))->Data());
MIL << "Buildtime: " << buildtime << endl;

    // SOURCE, source package
    string source ((tagset->getTagByIndex(InstSrcData_ULTags::SOURCE))->Data());
MIL << "Source: " << source << endl;

    // GROUP, rpm group
    string group ((tagset->getTagByIndex(InstSrcData_ULTags::GROUP))->Data());
MIL << "Group: " << group << endl;

    // LICENSE, license
    string license ((tagset->getTagByIndex(InstSrcData_ULTags::LICENSE))->Data());
MIL << "License: " << license << endl;

    // AUTHORS, list of authors
    multi = (tagset->getTagByIndex(InstSrcData_ULTags::AUTHORS))->MultiData();
MIL << "Authors: " << vector2string(multi) << endl;

    // SHAREWITH, package to share data with
    string sharewith ((tagset->getTagByIndex(InstSrcData_ULTags::SHAREWITH))->Data());
MIL << "SharedWith: " << sharewith << endl;

    // KEYWORDS, list of keywords
    multi = (tagset->getTagByIndex(InstSrcData_ULTags::KEYWORDS))->MultiData();
MIL << "Keywords: " << vector2string(multi) << endl;

//    PMPackageDataProviderPtr ( new InstSrcProvide_UL());
//    pac->setDataProvider (new
    return pac;
}


