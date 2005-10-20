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

  Textdomain "packagemanager"

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
#include <y2pm/PMYouMedia.h>
#include <y2pm/PMLocale.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMYouPatchInfo
//
///////////////////////////////////////////////////////////////////

IMPL_BASE_POINTER(PMYouPatchInfo);

PMYouPatchInfo::PMYouPatchInfo( PMYouSettingsPtr settings )
  : _mediaDirLastVisited( "" ), _doneDirectory( false )
{
    _packageTagSet.setAllowMultipleSets( true );
    _packageTagSet.setAllowUnknownTags( true );

    _settings = settings;

    _packageDataProvider = new PMYouPackageDataProvider( this );
}

PMYouPatchInfo::~PMYouPatchInfo()
{
}

static PMError parseDelta(PMPackagePtr pkg, PMYouPackageDataProviderPtr provider,  const string &line )
{
  PMError err;

  if(!provider) { INT << "got NULL" << endl; return err; }
  if(line.empty()) return err;

  D__ << "Delta: " << line << endl;

  vector<string> deltaData;
  stringutil::split( line, deltaData );
  if ( deltaData.size() < 7 ) // must match nr of fields below
  {
    string text = "Error parsing 'Deltas' attribute.";
    text += " ";
    text += stringutil::form(
	"Line '%s' doesn't have form 'filename size md5sum name-edition-release buildtime srcmd5sum seq'.",
	line.c_str() );
    return PMError( YouError::E_parse_error, text );
  }

  string filename = deltaData[0];
  FSize size = FSize(deltaData[1], FSize::B);
  string md5sum = deltaData[2];
  PkgNameEd ned = PkgNameEd::fromString(deltaData[3]);
  Date buildtime = deltaData[4];
  string srcmd5 = deltaData[5];
  string seq = deltaData[6]; // must match check above

  PMPackageDelta delta(filename, size, md5sum, ned, buildtime, srcmd5, seq);
  provider->addDelta(pkg, delta);

  return PMError();
}



///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMYouPatchInfo::createPackage
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError PMYouPatchInfo::createPackage( const PMYouPatchPtr &patch, std::istream& strm )
{
  PMError err;
  string value = tagValue( YOUPackageTagSet::FILENAME );

  if ( value.empty() ) {
    DBG << "No Filename. Skipping this package." << endl;
    return PMError(YouError::E_parse_error, _("No filename.  Skipping this package."));
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

  value = tagValue( YOUPackageTagSet::PATCHRPMMD5 );
  _packageDataProvider->setPatchRpmMD5sum( pkg, value );

  value = tagValue( YOUPackageTagSet::MD5SUM );
  _packageDataProvider->setMD5sum( pkg, value );

  // remember position since retrieveData will seek in the stream.
  std::istream::pos_type posbeforedelta = strm.tellg();
  std::istream::iostate state = strm.rdstate();
  do
  {
    TaggedFile::Tag *tag = _packageTagSet.getTagByIndex( YOUPackageTagSet::DELTAS );
    if ( !tag ) break;

    list<string> data;
    bool success = tag->Pos().retrieveData( strm, data );
    if ( !success ) {
      E__ << "Can't retrieve data." << endl;
      break;
    }

    list<string>::const_iterator it;
    for( it = data.begin(); it != data.end(); ++it )
    {
      PMError error = parseDelta( pkg, _packageDataProvider, *it );
      if(error)
      {
	ERR << error << endl;
	if(!err)
	  err = error;
      }
    }
  } while(0);

  // seek to position we stored before so parsing can continue where it was.
  strm.seekg(posbeforedelta);
  strm.clear(state);

  return err;
}

static std::string asString(TaggedFile::assignstatus status)
{
    switch(status)
    {
	case TaggedFile::ACCEPTED:
	    return "ACCEPTED";
	case TaggedFile::ACCEPTED_FULL:
	    return "ACCEPTED_FULL";
	case TaggedFile::REJECTED_EOF:
	    return "REJECTED_EOF";
	case TaggedFile::REJECTED_NOMATCH:
	    return "REJECTED_NOMATCH";
	case TaggedFile::REJECTED_LOCALE:
	    return "REJECTED_LOCALE";
	case TaggedFile::REJECTED_NOLOCALE:
	    return "REJECTED_NOLOCALE";
	case TaggedFile::REJECTED_FULL:
	    return "REJECTED_FULL";
	case TaggedFile::REJECTED_NOENDTAG:
	    return "REJECTED_NOENDTAG";
    }
    return "?";
}

std::ostream& operator<<(std::ostream& os, TaggedFile::assignstatus status)
{
  os << asString(status);
  return os;
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

  PMError err;

  while ( !pkgstream.eof() ) {
    TaggedFile::assignstatus status = _packageTagSet.assignSet( parser, pkgstream );

    DBG << status << endl;

    if ( status == TaggedFile::REJECTED_EOF ) {
      D__ << "EOF" << endl;
      err = YouError::E_parse_error;
      err.setDetails(stringutil::form(_("Patch %s: unexpected end of file"), patch->fullName().c_str()));
      break;
    } else if ( status == TaggedFile::ACCEPTED_FULL ) {
      D__ << "parse complete" << endl;
      err = createPackage( patch, pkgstream );
      if(err)
	break;
    } else {
      D__ << parser.lineNumber() << ": " << "Status " << (int)status << endl;
      D__ << "Last tag read: " << parser.currentTag();
      if (!parser.currentLocale().empty()) D__ << "." << parser.currentLocale();
      D__ << endl;
      err = YouError::E_parse_error;
      err.setDetails(
	  stringutil::form(_("Patch %s: parse error in line %d"),
	      patch->fullName().c_str(),
	      parser.lineNumber()));
      break;
    }
  }

  return err;
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
      string text = "Error parsing 'Files' attribute.";
      text += " ";
      text += stringutil::form(
          "Line '%s' doesn't have form 'url size'.",
          (*it).c_str() );
      return PMError( YouError::E_parse_error, text );
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
    p->setFileName( fileName );

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
    else if ( value == "patchlevel" ) { kind = PMYouPatch::kind_patchlevel; }
    else if ( value == "document" ) { kind = PMYouPatch::kind_document; }
    else if ( value == "optional" ) { kind = PMYouPatch::kind_optional; }
    else if ( value == "YaST2" ) { kind = PMYouPatch::kind_yast; }
    p->setKind( kind );

    p->setShortDescription( tagValueLocale( YOUPatchTagSet::SHORTDESCRIPTION, patchstream ) );
    p->setLongDescription( tagValueLocale( YOUPatchTagSet::LONGDESCRIPTION, patchstream ) );
    p->setPreInformation( tagValueLocale( YOUPatchTagSet::PREINFORMATION, patchstream ) );
    p->setPostInformation( tagValueLocale( YOUPatchTagSet::POSTINFORMATION, patchstream ) );
    p->setPreScript( tagValue( YOUPatchTagSet::PRESCRIPT, patchstream ) );
    p->setUpdateScript( tagValue( YOUPatchTagSet::UPDATESCRIPT, patchstream ) );
    p->setPostScript( tagValue( YOUPatchTagSet::POSTSCRIPT, patchstream ) );

    value = tagValue( YOUPatchTagSet::UPDATEONLYINSTALLED, patchstream );
    p->setUpdateOnlyInstalled( value == "true" );

    value = tagValue( YOUPatchTagSet::UPDATEONLYNEW, patchstream );
    p->setUpdateOnlyNew( value == "true" );

    PMError error;

    value = stringutil::trim(tagMultiValue( YOUPatchTagSet::PACKAGES, patchstream ));
    if(!value.empty())
    {
	error = parsePackages( value, p );
	if ( error ) {
	  return error;
	}
    }

    value = tagMultiValue( YOUPatchTagSet::FILES, patchstream );
    error = parseFiles( value, p );
    if ( error ) return error;

    return error;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMYouPatchInfo::readDir
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError PMYouPatchInfo::readDir( vector<PMYouPatchPtr> &patches,
                                 bool useMediaDir )
{
    if ( !_doneDirectory ) {
      PMError error = getDirectory( useMediaDir );
      if ( error ) return error;
    }

    if ( _totalPatchFileCount <= 0 ) return PMError();

    patches.reserve(_totalPatchFileCount);

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
          #warning no release? (mir)
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
  if ( useMediaDir ) {
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
  _mediaMap.clear();

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

    Pathname mediaMapFile = patchPath + _settings->mediaMapFileName();
    DBG << "MediaMap file: " << mediaMapFile << endl;
    error = media.provideFile( mediaMapFile );

    if ( error ) {
      WAR << "Unable to read '" << _settings->mediaMapFileName() << "'" << endl;
    } else {
      Pathname mediaMap = media.localRoot() + patchPath +
                         _settings->mediaMapFileName();

      readMediaMap( mediaMap );
    }
  }

  #warning Shouldnt MediaAccess::release() be called in ~MediaAccess()?
  media.release();

  _doneDirectory = true;

  return PMError();
}

void PMYouPatchInfo::readMediaMap( const Pathname &file )
{
  string line;
  ifstream in( file.asString().c_str() );
  while( getline( in, line ) ) {
    if ( !line.empty() && (*line.begin() != '#') ) {
      vector<string> fields;
      stringutil::split( line, fields );
      if ( fields.size() != 2 ) {
        ERR << "Illegal line in media map file: '" << line << "'" << endl;
      } else {
        string patchFileName = fields[ 0 ];
        int mediaNumber = atoi( fields[ 1 ].c_str() );
        D__ << "Media map: " << patchFileName << " " << mediaNumber << endl;
        _mediaMap[ patchFileName ] = mediaNumber;
      }
    }
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMYouPatchInfo::getPatches
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError PMYouPatchInfo::getPatches( vector<PMYouPatchPtr> &patches )
{
    PMYouServer server = _settings->patchServer();

    Url url = server.url();
    url.setUsername( "" );
    url.setPassword( "" );
    server.setUrl( url );

    SysConfig *cfg = _settings->config();
    cfg->writeEntry( "LastServer", server.toString() );
    cfg->writeEntry( "LastServerType", server.typeAsString() );
    cfg->save();

    PMError error = processMediaDir();
    if ( error ) return error;

    return readDir( patches );
}


PMError PMYouPatchInfo::processMediaDir()
{
    // have we visited this directory just before?
    Url url = _settings->patchUrl();
    if (url == _mediaDirLastVisited)
        return PMError();   // yes, nothing to do

    // reset this in case of error
    _mediaDirLastVisited = Url();

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

    PMYouMediaPtr mediaInstance = new PMYouMedia( media );
    error = mediaInstance->readInfo( 1 );
    if ( error ) {
      _settings->setMasterMedia( 0 );
    } else {
      _settings->setMasterMedia( mediaInstance );
    }

    Pathname path = _settings->mediaPatchesFile();
    error = media.provideFile( path );
    if ( error ) {
        // tried  to fetch mediamap, which is required only for patches
        // that stretch multiple CDs. Not a problem.
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

        list<string> exclusiveProducts;
        while( getline( in, line ) ) {
          if ( !line.empty() && (*line.begin() != '#') ) {
            exclusiveProducts.push_back( line );
          }
        }
        _settings->filterExclusiveProducts( exclusiveProducts );
    }
    
    media.release();

    // been there, read completely
    _mediaDirLastVisited = url;

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

int PMYouPatchInfo::mediaNumber( const PMYouPatchPtr &patch )
{
  map<string,int>::const_iterator it = _mediaMap.find( patch->fileName() );
  if ( it == _mediaMap.end() ) return -1;
  return it->second;
}
