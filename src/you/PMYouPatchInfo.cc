/*---------------------------------------------------------------------\
|                                                                      |
|                      __   __    ____ _____ ____                      |
|                      \ \ / /_ _/ ___|_   _|___ \                     |
|                       \ V / _` \___ \ | |   __) |                    |
|                        | | (_| |___) || |  / __/                     |
|                        |_|\__,_|____/ |_| |_____|                    |
|                                                                      |
|                               core system                            |
|                                                    (C) SUSE LINUX AG |
\----------------------------------------------------------------------/

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
#include <y2pm/PMYouProduct.h>
#include <y2pm/PMYouPatchInfo.h>
#include <y2pm/PMYouSettings.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMYouPatchInfo
//
///////////////////////////////////////////////////////////////////

IMPL_BASE_POINTER(PMYouPatchInfo);

PMYouPatchInfo::PMYouPatchInfo( PMYouSettingsPtr settings )
  : _doneMediaDir( false ), _doneDirectory( false )
{
    _packageTagSet.setAllowMultipleSets( true );
    _packageTagSet.setAllowUnknownTags( true );

    _settings = settings;

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
  PkgEdition edition( value );

  value = tagValue( YOUPackageTagSet::ARCH );
  PkgArch arch;
  if ( value.empty() ) arch = _settings->primaryProduct()->baseArch();
  else arch = PkgArch( value );

  PMPackagePtr pkg( new PMPackage( name, edition, arch,
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

  _packageDataProvider->setSize( pkg, FSize( size ) );
  _packageDataProvider->setArchiveSize( pkg, FSize( rpmSize ) );
  _packageDataProvider->setPatchRpmSize( pkg, FSize( patchRpmSize ) );

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

  bool forceInstall = tagValue( YOUPackageTagSet::FORCEINSTALL ) == "true";
  _packageDataProvider->setForceInstall( pkg, forceInstall );

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

PMError PMYouPatchInfo::parseFiles( const string &files,
                                    const PMYouPatchPtr &patch )
{
  vector<string> fileList;
  stringutil::split( files, fileList, "\n" );

  vector<string>::const_iterator it;
  for( it = fileList.begin(); it != fileList.end(); ++it ) {
    D__ << "File: " << *it << endl;
    vector<string> fileData;
    stringutil::split( *it, fileData );
    if ( fileData.size() != 2 ) {
      return PMError( YouError::E_parse_error,
                      "Error parsing 'Files' attribute" );
    }
    patch->addFile( PMYouFile( fileData[ 0 ], FSize( fileData[ 1 ] ) ) );
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
                                  PMYouPatchPtr &p )
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

    PkgArch arch = _settings->primaryProduct()->baseArch();

    p = new PMYouPatch( PkgName( name ), PkgEdition( version ), arch );

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
    p->setPreScript( tagValue( YOUPatchTagSet::PRESCRIPT, patchstream ) );
    p->setPostScript( tagValue( YOUPatchTagSet::POSTSCRIPT, patchstream ) );

    value = tagValue( YOUPatchTagSet::UPDATEONLYINSTALLED, patchstream );
    if ( value == "true" ) { p->setUpdateOnlyInstalled( true ); }
    else { p->setUpdateOnlyInstalled( false ); }

    value = tagMultiValue( YOUPatchTagSet::PACKAGES, patchstream );
    PMError error = parsePackages( value, p );
    if ( error ) {
      return error;
    }

    value = tagMultiValue( YOUPatchTagSet::FILES, patchstream );
    error = parseFiles( value, p );
    if ( error ) return error;

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
PMError PMYouPatchInfo::readDir( list<PMYouPatchPtr> &patches,
                                 bool useMediaDir )
{
    if ( !_doneDirectory ) {
      PMError error = getDirectory( useMediaDir );
      if ( error ) return error;
    }

    if ( _totalPatchFileCount == 0 ) return PMError();

    MediaAccess media;

    PMError error = media.open( _settings->patchUrl(), _settings->attachPoint() );
    if ( error != PMError::E_ok ) {
      string errMsg = "Can't open " + _settings->patchUrl().asString();
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

    GPGCheck gpg;

    int current = 0;

    list<PMYouProductPtr> products = _settings->products();
    list<PMYouProductPtr>::reverse_iterator itProduct;
    for ( itProduct = products.rbegin(); itProduct != products.rend();
          ++itProduct ) {
      Pathname patchPath = (*itProduct)->patchPath();

      list<string> patchFiles = (*itProduct)->patchFiles();
      list<string>::const_iterator it;
      for( it = patchFiles.begin(); it != patchFiles.end(); ++it ) {
        int progress = current++ * 100 / _totalPatchFileCount;
        
        error = Y2PM::youPatchManager().instYou().progress( progress );
        if ( error ) return error;

        if ( *it == "." || *it == ".." ||
             (*it).substr( 0, 9 ) == "directory" ) continue;

        Pathname filePath = patchPath + *it;
        Pathname localPath = media.localPath( filePath );

        error = media.provideFile( filePath, !_settings->reloadPatches() );
        if ( error ) {
          ERR << "ERR: " << error << ": " << filePath << endl;
          if ( error == MediaError::E_login_failed ) return error;
        } else {
          if ( _settings->checkSignatures() ) {
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
          PMYouPatchPtr patch;
          error = readFile( media.localPath( patchPath ), *it, patch );
          if ( error ) {
              media.release();
              string details = "Patch '" + *it + "'";
              if ( !error.details().empty() ) {
                details += ": " + error.details();
              }
              ERR << error << endl;
              return error;
          }
          patch->setProduct( *itProduct );
          patches.push_back( patch );
          DBG << "Successfully read " << *it << endl;
        }
      }
    }

    Y2PM::youPatchManager().instYou().progress( 100 );

    error = media.release();
    if ( error ) {
        ERR << "Error releasing media." << endl;
        return error;
    }

    return PMError();
}

PMError PMYouPatchInfo::getDirectory( bool useMediaDir )
{
  if ( useMediaDir && !_doneMediaDir ) {
    PMError error = processMediaDir();
    if ( error ) return error;
  }

  int err = PathInfo::assert_dir( _settings->attachPoint() );
  if ( err ) {
    string errMsg = "Can't create " + _settings->attachPoint().asString() +
                    ": " + strerror( err );
    ERR << errMsg;
    return PMError( YouError::E_error, errMsg );
  }

  MediaAccess media;

  PMError error = media.open( _settings->patchUrl(), _settings->attachPoint() );
  if ( error != PMError::E_ok ) {
    string errMsg = "Can't open " + _settings->patchUrl().asString();
    ERR << "MediaAccess::open() failed: " << errMsg << endl;
    error.setDetails( errMsg );
    return error;
  }

  error = media.attach();
  if ( error != PMError::E_ok ) {
    ERR << "MediaAccess::attach() failed." << endl;
    string errMsg = "Attach point: " + media.localRoot().asString();
    error.setDetails( errMsg );
    return error;
  }

  D__ << "Attach point: " << media.localRoot() << endl;

  _totalPatchFileCount = 0;

  list<PMYouProductPtr> products = _settings->products();
  list<PMYouProductPtr>::reverse_iterator it;
  for ( it = products.rbegin(); it != products.rend(); ++it ) {
    PMYouProductPtr product = *it;

    Pathname patchPath = product->patchPath();
    DBG << "Patch path: " << patchPath << endl;

    Pathname directoryFile = patchPath + _settings->directoryFileName();
    DBG << "Directory file: " << directoryFile << endl;

    list<string> patchFiles;

    error = media.provideFile( directoryFile );
    if ( error ) {
      WAR << "Unable to get file " << _settings->directoryFileName() << endl;
      if ( error == MediaError::E_login_failed ||
           error == MediaError::E_proxyauth_failed ||
           error == MediaError::E_write_error ) {
          media.release();
          return error;
      }

      PMError dirError = media.dirInfo( patchFiles, patchPath );
      if ( dirError ) {
        if ( dirError == MediaError::E_not_supported_by_media ) {
	  ERR << "dirInfo not supported on " << media << ": " << error << endl;
          return error;
        } else {
          ERR << dirError << endl;
        }
        media.release();

        string errMsg = "Unable to read the directory '" + patchPath.asString() +
                        "'";
        if ( !dirError.details().empty() ) {
          errMsg += " (" + dirError.details() + ")";
        }
        errMsg += ".";

        dirError.setDetails( errMsg );

        return dirError;
      }
    } else {
      Pathname dirFile = media.localRoot() + patchPath +
                         _settings->directoryFileName();

      readDirectoryFile( dirFile, patchFiles );
    }

    product->setPatchFiles( patchFiles );
    _totalPatchFileCount += patchFiles.size();
  }

#warning Shouldnt MediaAccess::release() be called in ~MediaAccess()?
  media.release();

  _doneDirectory = true;

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
PMError PMYouPatchInfo::getPatches( list<PMYouPatchPtr> &patches )
{
    PMYouServer server = _settings->patchServer();

    Url url = server.url();
    url.setUsername( "" );
    url.setPassword( "" );
    server.setUrl( url );

    _settings->config()->writeEntry( "LastServer", server.toString() );
    _settings->config()->save();
     
    _settings->config()->writeEntry( "LastServerType", server.type() );
    _settings->config()->save();

    y2milestone ( "Wrote: LastServerType=\"%s\" to /var/lib/YaST2/you/config", server.type().c_str() );

    if ( !_doneMediaDir ) {
      PMError error = processMediaDir();
      if ( error ) return error;
    }

    return readDir( patches );
}

PMError PMYouPatchInfo::processMediaDir()
{
    MediaAccess media;

    PMError error = media.open( _settings->patchUrl(), _settings->attachPoint() );
    if ( error ) {
      string errMsg = "Unable to open URL '" + _settings->patchUrl().asString() +
                      "'";
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

    Pathname path = _settings->mediaPatchesFile();
    error = media.provideFile( path );
    if ( error ) {
        string errMsg = "Unable to get file '" + _settings->patchUrl().asString() +
                        "/" + path.asString() + "'";
        error.setDetails( errMsg );
        DBG << error << endl;
    } else {
        string line;
        ifstream in( media.localPath( path ).asString().c_str() );
        if ( in.fail() ) {
          ERR << "Error reading " << path << endl;
          media.release();
          return PMError( YouError::E_read_mediapatches_failed,
                          _settings->patchUrl().asString() );
        }
        getline( in, line );
        D__ << "Read from media1/patches: " << line << endl;
        int pos = line.find( ' ' );
        string prefix = line.substr( 0, pos );
        D__ << "Prefix: " << prefix << endl;

        _settings->setPathPrefix( prefix );
    }
    
    media.release();

    _doneMediaDir = true;

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
    string result = tagValue( tagIndex, input, _settings->locale() );
    if ( result.empty() )
      result = tagValue( tagIndex, input, _settings->defaultLocale() );
    
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

PMYouPackageDataProviderPtr PMYouPatchInfo::packageDataProvider() const
{
  return _packageDataProvider;
}

PMError PMYouPatchInfo::readDirectoryFile( const Pathname &file,
                                           list<string> &patchFiles )
{
  patchFiles.clear();

  string buffer;
  ifstream in( file.asString().c_str() );
  while( getline( in, buffer ) ) {
    if ( !buffer.empty() && (*buffer.begin() != '#') ) {
      patchFiles.push_back( buffer );
    }
  }

  return PMError();
}
