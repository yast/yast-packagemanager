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

  File:       PMYouPatchInfo.cc

  Author:     Cornelius Schumacher <cschum@suse.de>
  Maintainer: Cornelius Schumacher <cschum@suse.de>

  Purpose: Read the patch info file.

/-*/

#include <iostream>
#include <fstream>

#include <y2util/Y2SLog.h>

#include <y2pm/InstSrcError.h>
#include <y2pm/PMYouPatch.h>

#include <y2pm/PMYouPatchInfo.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMYouPatchInfo
//
///////////////////////////////////////////////////////////////////


PMYouPatchInfo::PMYouPatchInfo( const string &lang )
{
    _tagset = new YOUPatchTagSet( lang );
    _tagset->setEncoding(CommonPkdParser::Tag::UTF8);
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMYouPatchInfo::readFile
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError PMYouPatchInfo::readFile( const string &path, const string &fileName,
                                  list<PMYouPatchPtr> &patches )
{
    std::cerr << "starting parser" << std::endl;

    TagParser parser;
    std::string tagstr;

    std::ifstream commonpkdstream( ( path + fileName ).c_str() );
    if(!commonpkdstream)
    {
	std::cerr << "file not found" << std::endl;
	return PMError( InstSrcError::E_bad_url );
    }


    bool repeatassign = false;
    bool parse = true;
    while( parse && parser.lookupTag(commonpkdstream))
    {
	tagstr = parser.startTag();

	do
	{
	    switch(_tagset->assign(tagstr.c_str(),parser,commonpkdstream))
	    {
		case CommonPkdParser::Tag::ACCEPTED:
		    repeatassign = false;
		    break;
		case CommonPkdParser::Tag::REJECTED_NOMATCH:
//		    std::cerr << "unknown tag " << tagstr << std::endl;
		    repeatassign = false;
		    break;
		case CommonPkdParser::Tag::REJECTED_FULL:
		    if( tagstr != "Filename" )
		    {
			std::cerr << "syntax error" << std::endl;
			parse = false;
		    }
//		    std::cerr << "tagset already has " << tagstr << std::endl;
		    _tagset->clear();
		    repeatassign = true;
		    break;
		case CommonPkdParser::Tag::REJECTED_NOENDTAG:
		    repeatassign = false;
		    parse = false;
		    break;
	    }
	} while(repeatassign);
    }

    if ( !parse )
        return PMError( InstSrcError::E_error );

    unsigned int pos = fileName.find( '-' );
    if ( pos < 0 ) return PMError( InstSrcError::E_error );
    
    string name = fileName.substr( 0, pos );
    string version = fileName.substr( pos + 1, fileName.length() - pos );

    PMYouPatchPtr p( new PMYouPatch( PkgName( name ),
                                     PkgEdition( version.c_str() ) ) );
    
    string value = tagValue( YOUPatchTagSet::KIND );
    PMYouPatch::Kind kind = PMYouPatch::kind_invalid;
    if ( value == "security" ) { kind = PMYouPatch::kind_security; }
    else if ( value == "recommended" ) { kind = PMYouPatch::kind_recommended; }
    else if ( value == "document" ) { kind = PMYouPatch::kind_document; }
    else if ( value == "optional" ) { kind = PMYouPatch::kind_optional; }
    else if ( value == "YaST2" ) { kind = PMYouPatch::kind_yast; }
    p->setKind( kind );
    
    p->setShortDescription( tagValue( YOUPatchTagSet::SHORTDESCRIPTION ) );
    p->setLongDescription( tagValue( YOUPatchTagSet::LONGDESCRIPTION ) );
    p->setPreInformation( tagValue( YOUPatchTagSet::PREINFORMATION ) );
    p->setPostInformation( tagValue( YOUPatchTagSet::POSTINFORMATION ) );
    p->setMinYastVersion( tagValue( YOUPatchTagSet::MINYAST2VERSION ) );

    value = tagValue( YOUPatchTagSet::UPDATEONLYINSTALLED );
    if ( value == "true" ) { p->setUpdateOnlyInstalled( true ); }
    else { p->setUpdateOnlyInstalled( false ); }

    patches.push_back( p );
  
    return PMError();
}

string PMYouPatchInfo::tagValue( YOUPatchTagSet::Tags tagIndex )
{
    CommonPkdParser::Tag *tag = _tagset->getTagByIndex( tagIndex );
    if ( !tag ) {
        cerr << "tag not found" << endl;
        return "";
    }
    
    return tag->Data();
}
