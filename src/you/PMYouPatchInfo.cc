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

  File:       PMYouPatchInfo.ccIMPL_BASE_POINTER(PMSolvable);


  Author:     Cornelius Schumacher <cschum@suse.de>
  Maintainer: Cornelius Schumacher <cschum@suse.de>

  Purpose: Read the patch info file.

/-*/

#include <iostream>
#include <fstream>
#include <sstream>

#include <y2util/Y2SLog.h>
#include <y2util/GPGCheck.h>

#include <Y2PM.h>

#include <y2pm/InstSrcError.h>
#include <y2pm/YouError.h>
#include <y2pm/PMYouPatch.h>
#include <y2pm/MediaAccess.h>
#include <y2pm/PMYouPackageDataProvider.h>
#include <y2pm/PMPackage.h>

#include <y2pm/PMYouPatchInfo.h>

using namespace std;

static const char *langmap[] = {
        "en_GB"         , "english",
        "en_US"         , "english",
        "en"            , "english",
        "de_DE"         , "german",
        "de_CH"         , "german",
        "de"            , "german",
        "fr"            , "french",
        "br_FR"         , "french",
        "fr_FR"         , "french",
        "fr_CH"         , "french",
        "it"            , "italian",
        "it_IT"         , "italian",
        "es"            , "spanish",
        "es_ES"         , "spanish",
        "nl"            , "dutch",
        "nl_NL"         , "dutch",
        "pt"            , "portuguese",
        "pt_PT"         , "portuguese",
        "pt_BR"         , "brazilian",
        "hu"            , "hungarian",
        "hu_HU"         , "hungarian",
        "pl"            , "polish",
        "pl_PL"         , "polish",
        "el_GR"         , "greek",
        "tr_TR"         , "turkish",
        "tr"            , "turkish",
        "ru"            , "russian",
        "ru_RU"         , "russian",
        "ru_RU.KOI8-R"  , "russian",
        "cs"            , "czech",
        "cs_CZ"         , "czech",
        "ja"            , "japanese",
        "ja_JP"         , "japanese",
        "ko"            , "korean",
        "ko_KR"         , "korean",
        0               , 0
};

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMYouPatchInfo
//
///////////////////////////////////////////////////////////////////

IMPL_BASE_POINTER(PMYouPatchInfo);

PMYouPatchInfo::PMYouPatchInfo( const string &lang )
{
    LangCode langCode( lang );
    if ( lang.empty() ) langCode = Y2PM::getPreferredLocale();

    _patchtagset = new YOUPatchTagSet( translateLangCode( langCode ) );
    _patchtagset->setEncoding(CommonPkdParser::Tag::UTF8);

    _packagetagset = new YOUPackageTagSet();

    _paths = new PMYouPatchPaths("noproduct","noversion","noarch");
}

PMYouPatchInfo::~PMYouPatchInfo()
{
    delete _packagetagset;
    delete _patchtagset;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMYouPatchInfo::createPackage
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError PMYouPatchInfo::createPackage( const PMYouPatchPtr &patch )
{
  string value = tagValue( YOUPackageTagSet::FILENAME );
  unsigned int pos = value.find( ".rpm" );
  if ( pos < 0 ) {
    E__ << "No '.rpm' in '" << value << "'" << endl;
    return PMError( InstSrcError::E_error );
  }
  PkgName name( value.substr( 0, pos ) );

  value = tagValue( YOUPackageTagSet::PKGVERSION );
  string version;
  string release;

  pos = value.rfind( '-' );
  if ( pos < 0 ) {
    version = value;
    release = "0";
  } else {
    version = value.substr( 0, pos );
    release = value.substr( pos + 1, value.length() - pos );
  }

  PkgEdition edition( version, release );

  PMYouPackageDataProviderPtr
      dataProvider( new PMYouPackageDataProvider( this ) );

  PMPackagePtr pkg( new PMPackage( name, edition, _paths->baseArch(),
                                   dataProvider ) );
  patch->addPackage( pkg );

  value = tagValue( YOUPackageTagSet::OBSOLETES );
  list<PkgRelation> relations = PkgRelation::parseRelations( value );
  pkg->setObsoletes( relations );

  value = tagValue( YOUPackageTagSet::REQUIRES );
  relations = PkgRelation::parseRelations( value );
  pkg->setRequires( relations );

  value = tagValue( YOUPackageTagSet::PROVIDES );
  relations = PkgRelation::parseRelations( value );
  pkg->setProvides( relations );

  value = tagValue( YOUPackageTagSet::CONFLICTS );
  relations = PkgRelation::parseRelations( value );
  pkg->setConflicts( relations );

  return PMError();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMYouPatchInfo::parsePackages
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError PMYouPatchInfo::parsePackages( const string &packages,
                                       const PMYouPatchPtr &patch  )
{
    TagParser parser;
    string tagstr;

    _packagetagset->clear();

    std::stringstream commonpkdstream;
    commonpkdstream << packages;

    bool repeatassign = false;
    bool parse = true;
    while( parse && parser.lookupTag(commonpkdstream))
    {
	tagstr = parser.startTag();

	do
	{
	    switch(_packagetagset->assign(tagstr.c_str(),parser,commonpkdstream))
	    {
		case CommonPkdParser::Tag::ACCEPTED:
		    repeatassign = false;
		    break;
		case CommonPkdParser::Tag::REJECTED_NOMATCH:
		    repeatassign = false;
		    break;
		case CommonPkdParser::Tag::REJECTED_FULL:
		    if( tagstr != "Filename" )
		    {
			E__ << "syntax error" << std::endl;
			parse = false;
		    }
                    createPackage( patch );
		    _packagetagset->clear();
		    repeatassign = true;
		    break;
		case CommonPkdParser::Tag::REJECTED_NOENDTAG:
		    repeatassign = false;
		    parse = false;
		    break;
	    }
	} while( repeatassign );
    }

    if ( !parse ) {
        E__ << "Parse Error" << endl;
        return PMError( InstSrcError::E_error );
    }

    createPackage( patch );

    return PMError();
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMYouPatchInfo::readFile
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError PMYouPatchInfo::readFile( const Pathname &path, const string &fileName,
                                  list<PMYouPatchPtr> &patches )
{
    D__ << "path: " << path << " fileName: " << fileName << endl;

    string filePath = ( path + fileName ).asString();

    TagParser parser;
    string tagstr;

    _patchtagset->clear();

    std::ifstream commonpkdstream( filePath.c_str() );
    if(!commonpkdstream)
    {
	E__ << "file not found" << endl;
	return PMError( InstSrcError::E_bad_url );
    }

    bool repeatassign = false;
    bool parse = true;
    while( parse && parser.lookupTag(commonpkdstream))
    {
	tagstr = parser.startTag();

	do
	{
	    switch(_patchtagset->assign(tagstr.c_str(),parser,commonpkdstream))
	    {
		case CommonPkdParser::Tag::ACCEPTED:
		    repeatassign = false;
		    break;
		case CommonPkdParser::Tag::REJECTED_NOMATCH:
		    repeatassign = false;
		    break;
		case CommonPkdParser::Tag::REJECTED_FULL:
		    if( tagstr != "Filename" )
		    {
			E__ << "syntax error" << std::endl;
			parse = false;
		    }
		    _patchtagset->clear();
		    repeatassign = true;
		    break;
		case CommonPkdParser::Tag::REJECTED_NOENDTAG:
		    repeatassign = false;
		    parse = false;
		    break;
	    }
	} while( repeatassign );
    }

    if ( !parse ) {
        E__ << "Parse Error" << endl;
        return PMError( InstSrcError::E_error );
    }

// We don't have patch version information yet.
#if 0
    unsigned int pos = fileName.find( '-' );
    if ( pos < 0 ) {
      E__ << "No '-' in '" << fileName << "'" << endl;
      return PMError( InstSrcError::E_error );
    }

    string name = fileName.substr( 0, pos );
    string version = fileName.substr( pos + 1, fileName.length() - pos );
#else
    string name = fileName;
    string version = "0";
#endif

    PMYouPatchPtr p( new PMYouPatch( PkgName( name ), PkgEdition( version ),
                                     _paths->baseArch() ) );

    p->setLocalFile( path + fileName );

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
    p->setPreScript( tagValue( YOUPatchTagSet::PRESCRIPT ) );
    p->setPostScript( tagValue( YOUPatchTagSet::POSTSCRIPT ) );

    value = tagValue( YOUPatchTagSet::UPDATEONLYINSTALLED );
    if ( value == "true" ) { p->setUpdateOnlyInstalled( true ); }
    else { p->setUpdateOnlyInstalled( false ); }

    value = tagValue( YOUPatchTagSet::PACKAGES );
    PMError error = parsePackages( value, p );
    if ( error != PMError::E_ok ) {
      return error;
    }

    patches.push_back( p );

    return PMError();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMYouPatchInfo::readDir
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError PMYouPatchInfo::readDir( const Url &baseUrl, const Pathname &patchPath,
                                 list<PMYouPatchPtr> &patches, bool checkSig )
{
  int err = PathInfo::assert_dir( _paths->localDir() );
  if ( err ) {
    E__ << "Can't create " << _paths->localDir() << " (errno: " << err << ")"
        << endl;
    return PMError( InstSrcError::E_error );
  }

   PMError error = _media.open( baseUrl, _paths->localDir() );
    if ( error != PMError::E_ok ) {
      E__ << "MediaAccess::open() failed." << endl;
      return error;
    }

    error = _media.attach( );
    if ( error != PMError::E_ok ) {
      E__ << "MediaAccess::attach() failed." << endl;
      return error;
    }

    D__ << "Attach point: " << _media.localRoot() << endl;

    list<string> patchFiles;

    error = _media.provideFile( patchPath + "directory" );
    if ( error ) {
      W__ << "no directory file found." << endl;

      error = _media.dirInfo( patchFiles, patchPath );
      if ( error ) {
        if ( error == MediaError::E_not_supported_by_media ) {
	  E__ << "dirInfo not supported on " << _media << ": " << error << endl;
        }
        return error;
      }
    } else {
      Pathname dirFile = _media.localRoot() + patchPath + "directory";

      string buffer;
      ifstream in( dirFile.asString().c_str() );
      while( getline( in, buffer ) ) {
        patchFiles.push_back( buffer );
      }
    }

    GPGCheck gpg;

    list<string>::const_iterator it;
    for( it = patchFiles.begin(); it != patchFiles.end(); ++it ) {
        if ( *it == "." || *it == ".." || *it == "directory" ) continue;
        error = _media.provideFile( patchPath + *it );
        if ( error != PMError::E_ok ) {
            E__ << error << patchPath + *it << endl;
            cerr << "ERR: " << *it << endl;
        } else {
            Pathname path = _media.localRoot() + patchPath;

            if ( checkSig ) {
                string filePath = ( path + *it ).asString();
                if ( !gpg.check_file( filePath ) ) {
                    E__ << "Signature check for '" << filePath << "' failed."
                        << endl;
                    return PMError( YouError::E_bad_sig_file );
                }
            }

            D__ << "read patch: file: " << *it << endl;
            error = readFile( path, *it, patches );
            if ( error != PMError::E_ok ) {
                return error;
            }
        }
    }

    return PMError();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMYouPatchInfo::getPatches
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError PMYouPatchInfo::getPatches( PMYouPatchPathsPtr paths,
                                    list<PMYouPatchPtr> &patches,
                                    bool checkSig )
{
    _paths = paths;
    return readDir( paths->patchUrl(), paths->patchPath(), patches, checkSig );
}

string PMYouPatchInfo::tagValue( YOUPatchTagSet::Tags tagIndex )
{
    CommonPkdParser::Tag *tag = _patchtagset->getTagByIndex( tagIndex );
    if ( !tag ) {
        return "";
    }

    list<string> data = tag->MultiData();

    if ( data.size() <= 1 ) return tag->Data();

    string result;
    list<string>::const_iterator it;
    for( it = data.begin(); it != data.end(); ++it ) {
      result += *it;
      result += '\n';
    }
    return result;
}

string PMYouPatchInfo::tagValue( YOUPackageTagSet::Tags tagIndex )
{
    CommonPkdParser::Tag *tag = _packagetagset->getTagByIndex( tagIndex );
    if ( !tag ) {
        return "";
    }

    return tag->Data();
}

string PMYouPatchInfo::translateLangCode( const LangCode &lang )
{
    string result = lang;

    const char **code = langmap;
    while( *code ) {

      if ( LangCode( *code ) == lang ) {
        result = *(code + 1);
        break;
      }

      code += 2;
    }

    D__ << "Translated " << lang << " to " << result << endl;

    return result;
}

const string PMYouPatchInfo::location( const PMPackagePtr &pkg ) const
{
  map<PMPackagePtr,string>::const_iterator it = _locations.find( pkg );
  if ( it == _locations.end() ) return "";
  else return it->second;
}

void PMYouPatchInfo::setLocation( const PMPackagePtr &pkg, const string &str )
{
  _locations[ pkg ] = str;
}
