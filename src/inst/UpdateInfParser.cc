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

  File:		UpdateInfParser.cc

  Author:	Klaus Kaempf <kkaempf@suse.de>
  Maintainer:	Klaus Kaempf <kkaempf@suse.de>

  Purpose:	parse /var/lib/YaST/update.inf for update

/-*/

#include <iostream>
#include <fstream>
#include <cstring>
#include <cctype>
#include <cstdlib>

#include <y2util/Y2SLog.h>
#include <y2util/stringutil.h>
#include <y2util/TaggedFile.h>
#include <y2pm/InstTargetError.h>

#include <y2pm/UpdateInfParser.h>

using namespace std;

///////////////////////////////////////////////////////////////////
// public
//	METHOD NAME : UpdateInfParser::UpdateInfParser
//	METHOD TYPE : Constructor
//
//	DESCRIPTION : initialize parser

UpdateInfParser::UpdateInfParser()
{
    // initialize tagset

    _tagset.setAllowMultipleSets (false);	// multiple tagsets per file
    _tagset.setAllowUnknownTags (true);		// skip unknown tags

    // for the 'update.inf' file

    _tagset.addTag ("Basesystem",	    BASESYSTEM,	TaggedFile::SINGLE, TaggedFile::START);
    _tagset.addTag ("Distribution_Name",    DISTNAME,	TaggedFile::SINGLE);
    _tagset.addTag ("Distribution_Version", DISTVERSION,TaggedFile::SINGLE);
    _tagset.addTag ("Distribution_Release", DISTRELEASE,TaggedFile::SINGLE);
//    _tagset.addTag ("Prohibited",	    PROHIBITED,	TaggedFile::MULTIOLD);
//    _tagset.addTag ("Notarx",		    NOTARX,	TaggedFile::MULTIOLD);
//    _tagset.addTag ("Nobackup",	    NOTBACKUP,	TaggedFile::MULTIOLD);
    _tagset.addTag ("DefaultSrcPatchFTP",   FTPPATCH,	TaggedFile::SINGLE);
    _tagset.addTag ("DefaultInstsrcFTP",    FTPSOURCES,	TaggedFile::MULTIOLD);
    _tagset.getTagByIndex (FTPSOURCES)->setEndTag ("PTFCrstsniTluafed");

    _parser.asOldstyle (true);
}


///////////////////////////////////////////////////////////////////
// public
//	METHOD NAME : UpdateInfParser::~UpdateInfParser
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
UpdateInfParser::~UpdateInfParser()
{
    // _tagset destructor will clean up _tagset
}


///////////////////////////////////////////////////////////////////
// public
//
//	METHOD NAME : UpdateInfParser::fromPath
//	METHOD TYPE : PMError
//
//	DESCRIPTION : pass packages data from path to attributes

PMError
UpdateInfParser::fromPath (const Pathname& path)
{
    std::ifstream infstream (path.asString().c_str());

    if (!infstream.is_open())
    {
	ERR << "Cant open " << path.asString() << endl;
	return InstTargetError::E_UpdateInf_not_found;
    }

    //---------------------------------------------------------------
    // assign set repeatedly

    for (;;)
    {
	TaggedFile::assignstatus status = _tagset.assignSet (_parser, infstream);

	if (status == TaggedFile::REJECTED_EOF)
	    break;

	if (status == TaggedFile::ACCEPTED_FULL)
	{
	    break;
	}
	else
	{
	    ERR << path << ":" << _parser.lineNumber() << endl;
	    ERR << "Status " << (int)status << ", Last tag read: " << _parser.currentTag();
	    if (!_parser.currentLocale().empty()) ERR << "." << _parser.currentLocale();
	    ERR << endl;
	    return InstTargetError::E_UpdateInf_malformed;
	}
    }

    _basesystem  = _tagset.getTagByIndex(BASESYSTEM)->Data();
    _distname    = _tagset.getTagByIndex(DISTNAME)->Data();
    _distversion = _tagset.getTagByIndex(DISTVERSION)->Data();
    _distrelease = _tagset.getTagByIndex(DISTRELEASE)->Data();
    _ftppatch    = _tagset.getTagByIndex(FTPPATCH)->Data();
    TagRetrievalPos pos = _tagset.getTagByIndex(FTPSOURCES)->Pos();
    pos.retrieveData (infstream, _ftpsources);

    return PMError::E_ok;
}
