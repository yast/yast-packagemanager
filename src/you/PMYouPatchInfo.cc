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

#include <y2util/Y2SLog.h>

#include <y2pm/InstSrcError.h>
#include <y2pm/PMYouPatch.h>
#include <y2pm/MediaAccess.h>
#include <y2pm/PMYouPackageDataProvider.h>
#include <y2pm/PMPackage.h>

#include <y2pm/PMYouPatchInfo.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMYouPatchPaths
//
///////////////////////////////////////////////////////////////////

PMYouPatchPaths::PMYouPatchPaths( const string &product, const string &version,
                                  const string &arch )
  : _arch( arch )
{
  _patchPath = arch + "/update/";
  if ( product != "SuSE-Linux" ) {
    _patchPath += product + "/";
    _patchUrl = Url( "http://support.suse.de/" );
  } else {
    _patchUrl = Url( "ftp://ftp.suse.com/pub/suse/" );
  }
  _patchPath += version + "/patches/";
}

void PMYouPatchPaths::setPatchPath( const Pathname &path )
{
  _patchPath = path;
}

Pathname PMYouPatchPaths::patchPath()
{
  return _patchPath;
}

void PMYouPatchPaths::setPatchUrl( const Url &url )
{
  _patchUrl = url;
}

Url PMYouPatchPaths::patchUrl()
{
  return _patchUrl;
}

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMYouPatchInfo
//
///////////////////////////////////////////////////////////////////

PMYouPatchInfo::PMYouPatchInfo( const string &lang )
  : _paths( 0 )
{
    _patchtagset = new YOUPatchTagSet( lang );
    _patchtagset->setEncoding(CommonPkdParser::Tag::UTF8);

    _packagetagset = new YOUPackageTagSet();

    _patchFiles = new list<string>;
    
    _packageProvider = new PMYouPackageDataProvider();
//    _packageProvider = PMYouPackageDataProviderPtr( new PMYouPackageDataProvider() );
}

PMYouPatchInfo::~PMYouPatchInfo()
{
    delete _patchFiles;
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

  value = tagValue( YOUPackageTagSet::VERSION );
  PkgEdition edition( value );

  string archValue;
  if ( _paths ) archValue = _paths->arch();
  PkgArch arch( archValue );  

  PMPackagePtr pkg( new PMPackage( name, edition, arch, _packageProvider ) );
  patch->addPackage( pkg );

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

    TagParser parser;
    string tagstr;

    _patchtagset->clear();

    std::ifstream commonpkdstream( ( path + fileName ).asString().c_str() );
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

    unsigned int pos = fileName.find( '-' );
    if ( pos < 0 ) {
      E__ << "No '-' in '" << fileName << "'" << endl;
      return PMError( InstSrcError::E_error );
    }

    string name = fileName.substr( 0, pos );
    string version = fileName.substr( pos + 1, fileName.length() - pos );

    // ma: NULL PMYouPatchDataProviderPtr provided to be able to compile.
    // Finaly we should make shure that there is one, or we don't need it at all.
    PMYouPatchPtr p( new PMYouPatch( PkgName( name ),
                                     PkgEdition( version.c_str() ),
				     PMYouPatchDataProviderPtr() ) );

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
                                 list<PMYouPatchPtr> &patches )
{
    MediaAccessPtr media( new MediaAccess );

    PMError error = media->open( baseUrl );
    if ( error != PMError::E_ok ) {
      E__ << "MediaAccess::open() failed." << endl;
      return error;
    }

    error = media->attach( );
    if ( error != PMError::E_ok ) {
      E__ << "MediaAccess::attach() failed." << endl;
      return error;
    }

    D__ << "Attach point: " << media->getAttachPoint() << endl;

    const list<string> *patchFiles = media->dirInfo ( patchPath );

    if ( !patchFiles ) {
      W__ << "dirInfo failed." << endl;
      error = media->provideFile( patchPath + "directory" );
      if ( error != PMError::E_ok ) {
        E__ << "no directory file found." << endl;
        return error;
      }
      Pathname dirFile = media->getAttachPoint() + patchPath + "directory";

      patchFiles = _patchFiles;
      _patchFiles->clear();

      string buffer;
      ifstream in( dirFile.asString().c_str() );
      while( getline( in, buffer ) ) {
        _patchFiles->push_back( buffer );
      }
    }

    list<string>::const_iterator it;
    for( it = patchFiles->begin(); it != patchFiles->end(); ++it ) {
        if ( *it == "." || *it == ".." || *it == "directory" ) continue;
        error = media->provideFile( patchPath + *it );
        if ( error != PMError::E_ok ) {
            E__ << error << patchPath + *it << endl;
            cerr << "ERR: " << *it << endl;
        } else {
            Pathname path = media->getAttachPoint() + patchPath;
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
PMError PMYouPatchInfo::getPatches( PMYouPatchPaths *paths,
                                    list<PMYouPatchPtr> &patches )
{
    _paths = paths;
    return readDir( paths->patchUrl(), paths->patchPath(), patches );
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
