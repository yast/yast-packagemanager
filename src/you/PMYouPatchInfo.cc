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
#include <sstream>
#include <stdlib.h>

#include <y2util/Y2SLog.h>
#include <y2util/GPGCheck.h>
#include <y2util/SysConfig.h>
#include <y2util/stringutil.h>

#include <Y2PM.h>

#include <y2pm/InstSrcError.h>
#include <y2pm/YouError.h>
#include <y2pm/PMYouPatch.h>
#include <y2pm/MediaAccess.h>
#include <y2pm/PMYouPackageDataProvider.h>
#include <y2pm/PMPackage.h>
#include <y2pm/PMYouPatchManager.h>
#include <y2pm/InstYou.h>

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

const std::string PMYouPatchInfo::_defaultLocale = "english";

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMYouPatchInfo
//
///////////////////////////////////////////////////////////////////

IMPL_BASE_POINTER(PMYouPatchInfo);

PMYouPatchInfo::PMYouPatchInfo( const string &lang )
{
    _lang = LangCode( lang );
    if ( lang.empty() ) _lang = Y2PM::getPreferredLocale();

    _locale = translateLangCode( _lang );

    _packageTagSet.setAllowMultipleSets( true );
    _packageTagSet.setAllowUnknownTags( true );

    _paths = new PMYouPatchPaths("noproduct","noversion","noarch");

    _packageDataProvider = new PMYouPackageDataProvider( this );
}

PMYouPatchInfo::~PMYouPatchInfo()
{
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

  if ( value.empty() ) {
    DBG << "No Filename. Skipping this package." << endl;
    return PMError();
  }

  string nameStr;
  unsigned int pos = value.find( ".rpm" );
  if ( pos < 0 ) {
    DBG << "No '.rpm' in '" << value << "'" << endl;
    nameStr = value;
  } else {
    nameStr = value.substr( 0, pos );
  }
  PkgName name( nameStr );

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

  PMPackagePtr pkg( new PMPackage( name, edition, _paths->baseArch(),
                                   _packageDataProvider ) );
  patch->addPackage( pkg );

  value = tagValue( YOUPackageTagSet::LABEL );
  _packageDataProvider->setSummary( pkg, value );

  value = tagValue( YOUPackageTagSet::SIZE );
  std::vector<std::string> values;
  stringutil::split( value, values );
  string size;
  string rpmSize;
  if ( values.size() >= 1 ) size = values[ 0 ];
  if ( values.size() >= 2 ) rpmSize = values[ 1 ];

  value = tagValue( YOUPackageTagSet::PATCHRPMSIZE );
  stringutil::split( value, values );
  string patchRpmSize;
  if ( values.size() >= 2 ) patchRpmSize = values[ 1 ];

  D__ << "Size: " << size << " RpmSize: " << rpmSize
      << " PatchRpmSize: " << patchRpmSize << endl;

  _packageDataProvider->setSize( pkg, FSize( atoll( size.c_str() ) ) );
  _packageDataProvider->setArchiveSize( pkg, FSize( atoll( rpmSize.c_str() ) ) );
  _packageDataProvider->setPatchRpmSize( pkg, FSize( atoll( patchRpmSize.c_str() ) ) );

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

  value = tagValue( YOUPackageTagSet::PATCHRPMBASEVERSIONS );
  vector<string> versions;
  stringutil::split( value, versions, " \t" );
  list<PkgEdition> editions;
  vector<string>::const_iterator it;
  for( it = versions.begin(); it != versions.end(); ++it ) {
    editions.push_back( PkgEdition( *it ) );
  }
  _packageDataProvider->setPatchRpmBaseVersions( pkg, editions );

  value = tagValue( YOUPackageTagSet::INSTPATH );
  if ( !value.empty() ) {
    _packageDataProvider->setExternalUrl( pkg, value );
  }

  value = tagValue( YOUPackageTagSet::RPMGROUP );
  _packageDataProvider->setRpmGroup( pkg, value );

  string patchName = patch->name().asString() + "-" + patch->version();
  _packageDataProvider->setSrcLabel( pkg, patchName );

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
  TaggedParser parser;
  parser.asOldstyle (true);

  std::stringstream pkgstream;
  pkgstream << packages;

  while ( !pkgstream.eof() ) {
    TaggedFile::assignstatus status = _packageTagSet.assignSet( parser, pkgstream );

    if ( status == TaggedFile::REJECTED_EOF ) {
      D__ << "EOF" << endl;
      break;
    } else if ( status == TaggedFile::ACCEPTED_FULL ) {
      D__ << "parse complete" << endl;
      createPackage( patch );
    } else {
      D__ << parser.lineNumber() << ": " << "Status " << (int)status << endl;
      D__ << "Last tag read: " << parser.currentTag();
      if (!parser.currentLocale().empty()) D__ << "." << parser.currentLocale();
      D__ << endl;
    }
  }

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
    DBG << "path: " << path << " fileName: " << fileName << endl;

    string filePath = ( path + fileName ).asString();

    TaggedParser parser;
    parser.asOldstyle (true);

    ifstream patchstream( filePath.c_str() );
    if( !patchstream ) {
	ERR << "file not found" << endl;
	return PMError( InstSrcError::E_bad_url );
    }

    TaggedFile::assignstatus status;
    
    status = _patchTagSet.assignSet( parser, patchstream );

    D__ << "assignstatus: " << status << endl;

    if ( status != TaggedFile::ACCEPTED_FULL ) {
        ERR << "Parse Error" << endl;
        return PMError( YouError::E_parse_error );
    }

    string name = tagValue( YOUPatchTagSet::PATCHNAME, patchstream );
    string version = tagValue( YOUPatchTagSet::PATCHVERSION, patchstream );

    D__ << "Name: " << name << endl;
    D__ << "Version: " << version << endl;

    if ( name.empty() ) name = fileName;
    if ( version.empty() ) version = "0";

    PMYouPatchPtr p( new PMYouPatch( PkgName( name ), PkgEdition( version ),
                                     _paths->baseArch() ) );

    p->setLocalFile( path + fileName );

    string value = tagValue( YOUPatchTagSet::REQUIRES, patchstream );
    list<PkgRelation> relations = PkgRelation::parseRelations( value );
    p->setRequires( relations );

    value = tagValue( YOUPatchTagSet::PROVIDES, patchstream );
    relations = PkgRelation::parseRelations( value );
    p->setProvides( relations );

    value = tagValue( YOUPatchTagSet::KIND, patchstream );
    PMYouPatch::Kind kind = PMYouPatch::kind_invalid;
    if ( value == "security" ) { kind = PMYouPatch::kind_security; }
    else if ( value == "recommended" ) { kind = PMYouPatch::kind_recommended; }
    else if ( value == "document" ) { kind = PMYouPatch::kind_document; }
    else if ( value == "optional" ) { kind = PMYouPatch::kind_optional; }
    else if ( value == "YaST2" ) { kind = PMYouPatch::kind_yast; }
    p->setKind( kind );

    p->setShortDescription( tagValueLocale( YOUPatchTagSet::SHORTDESCRIPTION, patchstream ) );
    p->setLongDescription( tagValueLocale( YOUPatchTagSet::LONGDESCRIPTION, patchstream ) );
    p->setPreInformation( tagValueLocale( YOUPatchTagSet::PREINFORMATION, patchstream ) );
    p->setPostInformation( tagValueLocale( YOUPatchTagSet::POSTINFORMATION, patchstream ) );
    p->setMinYastVersion( tagValue( YOUPatchTagSet::MINYAST2VERSION, patchstream ) );
    p->setPreScript( tagValue( YOUPatchTagSet::PRESCRIPT, patchstream ) );
    p->setPostScript( tagValue( YOUPatchTagSet::POSTSCRIPT, patchstream ) );

    value = tagValue( YOUPatchTagSet::UPDATEONLYINSTALLED, patchstream );
    if ( value == "true" ) { p->setUpdateOnlyInstalled( true ); }
    else { p->setUpdateOnlyInstalled( false ); }

    value = tagValue( YOUPatchTagSet::SIZE, patchstream );
    p->setPatchSize( atoll( value.c_str() ) * 1024 );

    value = tagMultiValue( YOUPatchTagSet::PACKAGES, patchstream );
    PMError error = parsePackages( value, p );
    if ( error ) {
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
                                 list<PMYouPatchPtr> &patches, bool reload,
                                 bool checkSig )
{
    int err = PathInfo::assert_dir( _paths->attachPoint() );
    if ( err ) {
      string errMsg = "Can't create " + _paths->attachPoint().asString() +
                      ": " + strerror( err );
      ERR << errMsg;
      return PMError( YouError::E_error, errMsg );
    }

    MediaAccess media;

    PMError error = media.open( baseUrl, _paths->attachPoint() );
    if ( error != PMError::E_ok ) {
      string errMsg = "Can't open " + baseUrl.asString();
      ERR << "MediaAccess::open() failed: " << errMsg << endl;
      error.setDetails( errMsg );
      return error;
    }

    error = media.attach( );
    if ( error != PMError::E_ok ) {
      ERR << "MediaAccess::attach() failed." << endl;
      string errMsg = "Attach point: " + media.localRoot().asString();
      error.setDetails( errMsg );
      return error;
    }

    D__ << "Attach point: " << media.localRoot() << endl;

    list<string> patchFiles;

    Pathname directoryFile = patchPath + _paths->directoryFileName();

    error = media.provideFile( directoryFile );
    if ( error ) {
      WAR << "no directory file found." << endl;
      if ( error == MediaError::E_login_failed ||
           error == MediaError::E_proxyauth_failed ) {
          media.release();
          return error;
      }

      PMError dirError = media.dirInfo( patchFiles, patchPath );
      if ( dirError ) {
        if ( dirError == MediaError::E_not_supported_by_media ) {
	  ERR << "dirInfo not supported on " << media << ": " << error << endl;
        } else {
          ERR << dirError << endl;
        }
        media.release();
        
        string errMsg = "Unable to get '" + directoryFile.asString() +
                        "' or to read the directory.";
        error.setDetails( errMsg );
        
        return error;
      }
    } else {
      Pathname dirFile = media.localRoot() + patchPath +
                         _paths->directoryFileName();

      string buffer;
      ifstream in( dirFile.asString().c_str() );
      while( getline( in, buffer ) ) {
        if ( !buffer.empty() && (*buffer.begin() != '#') ) {
          patchFiles.push_back( buffer );
        }
      }
    }

    GPGCheck gpg;

    int total = patchFiles.size();
    int current = 0;
    list<string>::const_iterator it;
    for( it = patchFiles.begin(); it != patchFiles.end(); ++it ) {
        error = Y2PM::youPatchManager().instYou().progress( current++ * 100 /
                                                            total );
        if ( error ) return error;

        if ( *it == "." || *it == ".." ||
             (*it).substr( 0, 9 ) == "directory" ) continue;

        Pathname filePath = patchPath + *it;
        Pathname localPath = media.localPath( filePath );
        
        error = media.provideFile( filePath, !reload );
        if ( error != PMError::E_ok ) {
            ERR << "ERR: " << error << ": " << filePath << endl;
        } else {
            if ( checkSig ) {
                DBG << "Check signature of '" << localPath << "'" << endl;
                if ( !gpg.check_file( localPath ) ) {
                    ERR << "Signature check for '" << localPath << "' failed."
                        << endl;
                    media.release();
                    return PMError( YouError::E_bad_sig_file, "Patch: " + *it );
                }
                DBG << "Signature ok." << endl;
            }

            D__ << "read patch: file: " << *it << endl;
            error = readFile( media.localPath( patchPath ), *it, patches );
            if ( error != PMError::E_ok ) {
                ERR << "Error reading patch " << *it << endl;
                media.release();
                error.setDetails( "Patch: " + *it );
                return error;
            }
        }
        DBG << "Successfully read " << *it << endl;
    }
    Y2PM::youPatchManager().instYou().progress( 100 );

    error = media.release();
    if ( error ) {
        ERR << "Error releasing media." << endl;
        return error;
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
                                    bool reload, bool checkSig )
{
    _paths = paths;

    Url url = _paths->patchUrl();

    _paths->config()->writeEntry( "LastServer", url.saveAsString() );
    _paths->config()->save();

    PMError error = processMediaDir( url );
    if ( error ) return error;

    error = readDir( url, _paths->patchPath(), patches, reload, checkSig );

    return error;
}

PMError PMYouPatchInfo::processMediaDir( const Url &url )
{
    MediaAccess media;

    PMError error = media.open( url, _paths->attachPoint() );
    if ( error ) {
      string errMsg = "Unable to open URL '" + url.asString() + "'";
      error.setDetails( errMsg );
      ERR << errMsg << endl;
      return error;
    }

    error = media.attach();
    if ( error ) {
      ERR << "MediaAccess::attach() failed." << endl;
      string errMsg = "Attach point: " + media.localRoot().asString();
      error.setDetails( errMsg );
      return error;
    }

    Pathname path = _paths->mediaPatchesFile();
    error = media.provideFile( path );
    if ( error ) {
        string errMsg = "Unable to get file '" + url.asString() + "/" +
                        path.asString() + "'";
        error.setDetails( errMsg );
        WAR << error << endl;
    } else {
        string line;
        ifstream in( media.localPath( path ).asString().c_str() );
        if ( in.fail() ) {
          ERR << "Error reading " << path << endl;
          media.release();
          return PMError( YouError::E_read_mediapatches_failed, url.asString() );
        }
        getline( in, line );
        D__ << "Read from media1/patches: " << line << endl;
        int pos = line.find( ' ' );
        string prefix = line.substr( 0, pos );
        D__ << "Prefix: " << prefix << endl;

        _paths->setPathPrefix( prefix );
    }
    
    media.release();

    return PMError();
}

string PMYouPatchInfo::tagMultiValue( YOUPatchTagSet::Tags tagIndex,
                                      istream& input )
{
    TaggedFile::Tag *tag = _patchTagSet.getTagByIndex( tagIndex );
    if ( !tag ) {
      return "";
    }

    list<string> data;
    bool success = tag->Pos().retrieveData( input, data );
    if ( !success ) {
      E__ << "Can't retrieve data." << endl;
      return "";
    }

    string result;
    list<string>::const_iterator it;
    for( it = data.begin(); it != data.end(); ++it ) {
      result += *it;
      result += '\n';
    }
    return result;
}

string PMYouPatchInfo::tagValueLocale( YOUPatchTagSet::Tags tagIndex,
                                       std::istream &input )
{
    string result = tagValue( tagIndex, input, _locale );
    if ( result.empty() ) result = tagValue( tagIndex, input, _defaultLocale );
    
    return result;
}

string PMYouPatchInfo::tagValue( YOUPatchTagSet::Tags tagIndex,
                                 std::istream &input, const string &locale )
{
    TaggedFile::Tag *tag = _patchTagSet.getTagByIndex( tagIndex );
    if ( !tag ) {
      return "";
    }

    string result;
    bool success = tag->Pos( locale ).retrieveData( input, result );
    if ( !success ) {
      E__ << "Can't retrieve data." << endl;
      return "";
    }
    
    return result;
}

string PMYouPatchInfo::tagValue( YOUPackageTagSet::Tags tagIndex )
{
    TaggedFile::Tag *tag = _packageTagSet.getTagByIndex( tagIndex );
    if ( !tag ) {
      return "";
    }

    string result = tag->Data();
    
    return result;
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

PMYouPackageDataProviderPtr PMYouPatchInfo::packageDataProvider() const
{
  return _packageDataProvider;
}
