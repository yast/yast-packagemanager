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

  File:		ULSelectionParser.cc

  Author:	Klaus Kaempf <kkaempf@suse.de>
  Maintainer:	Klaus Kaempf <kkaempf@suse.de>

  Purpose:	parse UnitedLinux compatible selection file

/-*/

#include <iostream>
#include <fstream>
#include <cstring>
#include <cctype>
#include <cstdlib>

#include <y2util/Y2SLog.h>
#include <y2util/PathInfo.h>
#include <y2util/stringutil.h>
#include <y2util/TaggedFile.h>

#include <y2pm/PMPackagePtr.h>
#include <y2pm/PMSelection.h>
#include <y2pm/ULSelectionParser.h>
#include <y2pm/PMULSelectionDataProvider.h>
#include <y2pm/PMULSelectionDataProviderPtr.h>

#include <y2pm/InstSrcError.h>
#include <y2pm/MediaAccess.h>

#include <y2pm/PkgName.h>
#include <y2pm/PkgEdition.h>
#include <y2pm/PkgArch.h>

#include <Y2PM.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//	CLASS NAME : ULSelectionParserPtr
//	CLASS NAME : constULSelectionParserPtr
///////////////////////////////////////////////////////////////////
IMPL_BASE_POINTER(ULSelectionParser);

///////////////////////////////////////////////////////////////////
// public
//	METHOD NAME : ULSelectionParser::ULSelectionParser
//	METHOD TYPE : Constructor
//
//	DESCRIPTION : initialize parser and tag set for selections

ULSelectionParser::ULSelectionParser(const InstSrcPtr source)
    : _source (source)
{
    // initialize tagset

    _tagset.setAllowMultipleSets (false);	// one tagset per file
    _tagset.setAllowUnknownTags (true);		// skip unknown tags

    _tagset.addTag ("Sel", SELECTION, TaggedFile::SINGLE, TaggedFile::START);
    _tagset.addTag ("Sum", SUMMARY,   TaggedFile::SINGLE, TaggedFile::ALLOWLOCALE);
    _tagset.addTag ("Req", REQUIRES,  TaggedFile::MULTI);
    _tagset.addTag ("Prv", PROVIDES,  TaggedFile::MULTI);
    _tagset.addTag ("Obs", OBSOLETES, TaggedFile::MULTI);
    _tagset.addTag ("Con", CONFLICTS, TaggedFile::MULTI);
    _tagset.addTag ("Rec", RECOMMENDS,TaggedFile::MULTI);
    _tagset.addTag ("Sug", SUGGESTS,  TaggedFile::MULTI);

    _tagset.addTag ("Siz", SIZE,      TaggedFile::SINGLE);
    _tagset.addTag ("Cat", CATEGORY,  TaggedFile::SINGLE);
    _tagset.addTag ("Vis", VISIBLE,   TaggedFile::SINGLE);
    _tagset.addTag ("Ord", ORDER,     TaggedFile::SINGLE);
    _tagset.addTag ("Ins", INSPACKS,  TaggedFile::MULTI, TaggedFile::ALLOWLOCALE);
    _tagset.addTag ("Del", DELPACKS,  TaggedFile::MULTI, TaggedFile::ALLOWLOCALE);
}


///////////////////////////////////////////////////////////////////
// public
//	METHOD NAME : ULSelectionParser::~ULSelectionParser
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
ULSelectionParser::~ULSelectionParser()
{
    // _tagset destructor will clean up _tagset
}

///////////////////////////////////////////////////////////////////
// private
//	METHOD NAME : toProvider
//	METHOD TYPE : PMSelectionPtr
//
//	DESCRIPTION : pass selection data from tagset to dataprovider
#warning fixme
inline bool
isAllowedArch (const PkgArch& arch)
{
    for (PM::ArchSet::const_iterator it = Y2PM::allowedArchs().begin();
	 it != Y2PM:: allowedArchs().end(); ++it)
    {
	if (*it == arch)
	    return true;
    }
    return false;
}


PMSelectionPtr
ULSelectionParser::toProvider (PMULSelectionDataProviderPtr dataprovider)
{
#define GET_TAG(tagname) \
    _tagset.getTagByIndex(tagname)

    //---------------------------------------------------------------
    // SELECTION
    vector<string> splitted;
    stringutil::split( GET_TAG(SELECTION)->Data(), splitted );

    if ( splitted.empty() ) {
	ERR << "No '=Sel' value found" << endl;
	return PMSelectionPtr();
    }

    PkgName    name( splitted[0] );
    PkgEdition edition;			// default empty edition
    PkgArch    arch( "noarch" );	// default "noarch"

    switch ( splitted.size() ) {
    case 4: // name version release arch
      edition = PkgEdition( splitted[1], splitted[2] );
      arch = PkgArch( splitted[3] );
      break;
    case 3: // name version release [arch]
      edition = PkgEdition( splitted[1], splitted[2] );
      break;
    case 2: // name [version release] arch
      arch = PkgArch( splitted[1] );
      break;
    case 1: // name [version release arch]
      break;
    default:
      ERR << "Illegal '=Sel' value found: " << GET_TAG(SELECTION)->Data() << endl;
      return PMSelectionPtr();
      break;
    }

    //---------------------------------------------------------------
    // drop selections not allowed for current architecture
    if ( ! isAllowedArch(arch) ) {
      DBG << "Drop '" << GET_TAG(SELECTION)->Data() << "' on '" << Y2PM::baseArch() << "'" << endl;
      return PMSelectionPtr();
    }

    //---------------------------------------------------------------
#warning STORE InstSrcPtr in DataProvider
    PMSelectionPtr selection( new PMSelection (name, edition, arch, dataprovider));
    TagCacheRetrievalPtr selcache = dataprovider->getCacheRetrieval();
    if (!selcache)
    {
	ERR << "No selcache!" << endl;
	return selection;
    }
    selcache->startRetrieval();
    TaggedFile::Tag *tagptr;	// for SET_CACHE
#define SET_VALUE(tagname,value) \
    do { dataprovider->_attr_##tagname = value; } while (0)
#define SET_CACHE(tagname) \
    do { tagptr = GET_TAG (tagname); \
	 dataprovider->_attr_##tagname = tagptr->Pos(); } while (0)
#define SET_LCACHE(tagname) \
    do { tagptr = GET_TAG (tagname); \
	 dataprovider->_attr_##tagname = tagptr->PosMap(); } while (0)

    std::list<std::string> sellist;
    PMSolvable::PkgRelList_type rellist;
    if (selcache->retrieveData (GET_TAG(REQUIRES)->Pos(), sellist))
    {
	rellist = PMSolvable::StringList2PkgRelList (sellist);
	selection->setRequires (rellist);
    }
    sellist.clear();
    if (selcache->retrieveData (GET_TAG(PROVIDES)->Pos(), sellist))
    {
	rellist = PMSolvable::StringList2PkgRelList (sellist);
	selection->setProvides (rellist);
    }
    sellist.clear();
    if (selcache->retrieveData (GET_TAG(CONFLICTS)->Pos(), sellist))
    {
	rellist = PMSolvable::StringList2PkgRelList (sellist);
	selection->setConflicts (rellist);
    }
    sellist.clear();
    if (selcache->retrieveData (GET_TAG(OBSOLETES)->Pos(), sellist))
    {
	rellist = PMSolvable::StringList2PkgRelList (sellist);
	selection->setObsoletes (rellist);
    }

    SET_LCACHE (SUMMARY);
    std::string category = GET_TAG(CATEGORY)->Data();
    SET_VALUE (CATEGORY, category);
    SET_VALUE (ISBASE, (strncmp (category.c_str(), "base", 4) == 0));
    SET_VALUE (VISIBLE, GET_TAG(VISIBLE)->Data() == "true");
    SET_VALUE (ORDER, GET_TAG(ORDER)->Data());
    if (dataprovider->_attr_ORDER.empty())
	SET_VALUE (ORDER, name);		// use name if order not given
    sellist.clear();
    if (selcache->retrieveData (GET_TAG(RECOMMENDS)->Pos(), sellist))
    {
	SET_VALUE (RECOMMENDS, sellist);
    }
    sellist.clear();
    if (selcache->retrieveData (GET_TAG(SUGGESTS)->Pos(), sellist))
    {
	SET_VALUE (SUGGESTS, sellist);
    }

    stringutil::split ((GET_TAG(SIZE))->Data(), splitted );
    if (splitted.size() > 0)
    {
	SET_VALUE (ARCHIVESIZE, FSize (atoll(splitted[0].c_str())));
	if (splitted.size() > 1)
	{
	    SET_VALUE (SIZE, FSize (atoll(splitted[1].c_str())));
	}
    }

    SET_LCACHE (INSPACKS);
    SET_LCACHE (DELPACKS);

#undef SET_VALUE
#undef GET_TAG
#undef SET_CACHE
#undef SET_LCACHE

    selcache->stopRetrieval();

    return selection;
}


///////////////////////////////////////////////////////////////////
// public
//
//	METHOD NAME : ULSelectionParser::fromPath
//	METHOD TYPE : PMError
//
//	DESCRIPTION : pass selection data from path to selection

PMError
ULSelectionParser::fromPath (const Pathname& path, PMSelectionPtr& selection)
{
    std::ifstream selstream (path.asString().c_str());

    if (!selstream.is_open())
    {
	ERR << "Cant open " << path.asString() << endl;
	return InstSrcError::E_open_file;
    }

    PMULSelectionDataProviderPtr dataprovider ( new PMULSelectionDataProvider (path));

    // find initial version tag

    // find any initial tag
    TaggedParser::TagType type = _parser.lookupTag (selstream);
    if ((type != TaggedParser::SINGLE)
	|| (_parser.currentTag() != "Ver")
	|| (!_parser.currentLocale().empty()))
    {
	ERR << path << ": Initial '=Ver:' tag missing" << endl;
	return InstSrcError::E_data_bad_selection;
    }

    string version = _parser.data();
    if (version != "4.0" && version != "3.0" )
    {
	ERR << path << ": Version '" << version << "' != 4.0" << endl;
	return InstSrcError::E_data_bad_selection;
    }

    // assign set

    TaggedFile::assignstatus status = _tagset.assignSet (_parser, selstream);

    if (status != TaggedFile::ACCEPTED_FULL)
    {
	ERR << "Parse failed" << endl;
	return InstSrcError::E_data_bad_selection;
    }

    selection = toProvider (dataprovider);
    if (!selection)
    {
	ERR << path << ":" << _parser.lineNumber() << endl;
	ERR << "Status " << (int)status << ", Last tag read: " << _parser.currentTag();
	if (!_parser.currentLocale().empty()) ERR << "." << _parser.currentLocale();
	ERR << endl;
	return InstSrcError::E_data_bad_selection;
    }

    return PMError::E_ok;
}


///////////////////////////////////////////////////////////////////
// public
//
//	METHOD NAME : ULSelectionParser::fromMediaDir
//	METHOD TYPE : PMError
//
//	DESCRIPTION : pass selection data from media directory
//		to selections list
//		Parses all selections in given directory

PMError
ULSelectionParser::fromMediaDir (std::list<PMSelectionPtr>& selections,
		MediaAccessPtr media_r, const Pathname& descr_dir_r )
{
    PMError err;

    selections.clear();

    // --------------------------------
    // setup selections access
    // read <descr_dir_r>/"selections" to std::list<std::string> selection_names

    std::list<std::string> selection_names;

    //-----------------------------------------------------
    // try "selections" file first

    Pathname filename = descr_dir_r + "/selections";

    err = media_r->provideFile ( filename );			// try <descr>/selections first
    if ( !err )
    {
	//-----------------------------------------------------
	// read names of *.sel files from file

	Pathname fullpath = media_r->localPath (filename);
	while ( true )
	{
	    std::ifstream selstream (fullpath.asString().c_str());
	    if (!selstream)
	    {
		ERR << "Cant open " << fullpath << ": " << InstSrcError::E_open_file << endl;
		break;
	    }

	    while (selstream.good())
	    {
		char lbuf[201];

		if (!selstream.getline (lbuf, 200, '\n'))
		{
		    if (selstream.eof())
			break;
		    MIL << "getline() failed" << endl;
		    break;
		}
		if ((lbuf[0] == '#')		// comment
		    || (lbuf[0] == 0))		// empty
		{
		    continue;
		}
		selection_names.push_back (lbuf);
	    }
	    break;
	}
	// implicit close of selstream
    }
    else
    {
	//-----------------------------------------------------
	// no "selections" file, try "*.sel"

	std::list<std::string> all_names;
	err = media_r->dirInfo (all_names, descr_dir_r, false);		// get all names not starting with '.'
	if (err)
	{
	    ERR << "dirInfo failed for '" << descr_dir_r << "' " << err << endl;
	    return err;
	}
	for (std::list<std::string>::iterator fileIt = all_names.begin();
	     fileIt != all_names.end(); ++fileIt)
	{
	    if (fileIt->find (".sel") != string::npos)		// filter "*.sel"
		selection_names.push_back (*fileIt);
	}
    }
    MIL << "*** Expecting " << selection_names.size() << " selections ***" << endl;


    //---------------------------------------------------------------
    // parse all selections to selmap first
    // then fill all requires, conflicts, suggests _ptrs_ from this map
    //

    for (std::list<std::string>::iterator selfile = selection_names.begin();
	 selfile != selection_names.end();
	 ++selfile)
    {
	Pathname filename = descr_dir_r + *selfile;
	err = media_r->provideFile ( filename );

	Pathname fullpath = media_r->localPath (filename);

	PMSelectionPtr selection;

	// fill selection
	err = fromPath (fullpath, selection);

	if (!err)
	    selections.push_back (selection);

    } // for ()

    MIL << "*** parsed " << selections.size() << " selections ***" << std::endl;

    return PMError::E_ok;
}
