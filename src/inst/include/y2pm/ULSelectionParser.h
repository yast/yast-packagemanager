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

  File:		ULSelectionParser.h

  Author:	Klaus Kaempf <kkaempf@suse.de>
  Maintainer:	Klaus Kaempf <kkaempf@suse.de>

  Purpose: Parse UnitedLinux compatible selection files

/-*/
#ifndef ULSelectionParser_h
#define ULSelectionParser_h

#include <string>
#include <list>
#include <utility>
#include <fstream>

#include <y2util/Pathname.h>
#include <y2util/TaggedParser.h>
#include <y2util/TaggedFile.h>

#include <y2pm/PMError.h>
#include <y2pm/MediaAccessPtr.h>
#include <y2pm/PMSelectionPtr.h>
#include <y2pm/PMULSelectionDataProviderPtr.h>
#include <y2pm/InstSrcPtr.h>

#include <y2pm/ULSelectionParserPtr.h>

class ULSelectionParser : virtual public Rep {
    private:
	// the source
	const InstSrcPtr _source;

	// tag ids for the TaggedParser
	enum Tags {
	    SELECTION,	// name version release arch
	    SUMMARY,	// short summary (label)
	    CATEGORY,
	    VISIBLE,
	    ORDER,		// ordering data
	    RECOMMENDS,
	    SUGGESTS,
	    REQUIRES,
	    PROVIDES,
	    CONFLICTS,
	    OBSOLETES,
	    SIZE,
	    INSPACKS,
	    DELPACKS,
	    NUM_TAGS
	};

	// our parser
	TaggedParser _parser;
	// our set of tags, initialized in constructor
	TaggedFile::TagSet _tagset;

	// pass data from current tagset to data provider
	PMSelectionPtr toProvider (PMULSelectionDataProviderPtr dataprovider);

	typedef std::map<std::string, std::pair<PMSelectionPtr, PMULSelectionDataProviderPtr> > selmaptype;
	selmaptype _selmap;

	// convert list of selection names to list of selection pointers
	// by using _selmap
	std::list<PMSelectionPtr> strlist2sellist (const std::list<std::string>& strlist);

	// fill pointers (*_prts attributes in data provider) from _selmap
	void fillPointers (void);

    public:
	ULSelectionParser (const InstSrcPtr source);
	~ULSelectionParser();

	/**
	 * parse a single selection from a local file
	 *
	 * the path is needed for the dataprovider, we can't pass
	 * a stream here.
	 */
	PMError fromPath (const Pathname& path, PMSelectionPtr& selection);

	/**
	 * parse all selections found below descr_dir_r of media_r
	 * to selections.
	 */
	PMError fromMediaDir (std::list<PMSelectionPtr>& selections,
		MediaAccessPtr media_r, const Pathname& descr_dir_r );

};

#endif //ULSelectionParser_h
