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

  File:		UpdateInfParser.h

  Author:	Klaus Kaempf <kkaempf@suse.de>
  Maintainer:	Klaus Kaempf <kkaempf@suse.de>

  Purpose: Parse /var/lib/YaST/update.inf
/-*/
#ifndef UpdateInfParser_h
#define UpdateInfParser_h

#include <string>
#include <fstream>

#include <y2util/Pathname.h>
#include <y2util/TaggedParser.h>
#include <y2util/TaggedFile.h>

#include <y2pm/PMError.h>
#include <y2pm/MediaAccessPtr.h>

class UpdateInfParser {
    private:
		
	// tag ids for the TaggedParser
	enum Tags {
	    BASESYSTEM, DISTNAME, DISTVERSION, DISTRELEASE,
	    FTPPATCH, FTPSOURCES,
	    NUM_TAGS
	};


	// our parser
	TaggedParser _parser;
	// our set of tags, initialized in constructor
	TaggedFile::TagSet _tagset;

	std::string _basesystem;
	std::string _distname;
	std::string _distversion;
	std::string _distrelease;
	std::string _ftppatch;
	std::list<std::string> _ftpsources;

    public:
	UpdateInfParser ();
	~UpdateInfParser();

	// parse 'update.inf' from path to attributes
	// localepath is only for initialization of the dataprovider
	// fromPathLocale() does the parsing of the locale file
	PMError fromPath (const Pathname& path);

	const std::string& basesystem() { return _basesystem; }
	const std::string& distname() { return _distname; }
	const std::string& distversion() { return _distversion; }
	const std::string& distrelease() { return _distrelease; }
	const std::string& ftppatch() { return _ftppatch; }
	const std::list<std::string>& ftpsources() { return _ftpsources; }
};

#endif //UpdateInfParser_h
