/*---------------------------------------------------------------------\
|                                                                      |
|                      __   __    ____ _____ ____                      |
|                      \ \ / /_ _/ ___|_   _|___ \                     |
|                       \ V / _` \___ \ | |   __) |                    |
|                        | | (_| |___) || |  / __/                     |
|                        |_|\__,_|____/ |_| |_____|                    |
|                                                                      |
|                               core system                            |
|                                                    (C) SuSE Linux AG |
\----------------------------------------------------------------------/

  File:       InstYou.cc

  Author:     Cornelius Schumacher <cschum@suse.de>
  Maintainer: Cornelius Schumacher <cschum@suse.de>

  Purpose: YaST Online Update.

  Textdomain "packagemanager"

/-*/

#include <time.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>
#include <fstream>

#include <y2util/Y2SLog.h>
#include <y2util/GPGCheck.h>
#include <y2util/SysConfig.h>
#include <y2util/Digest.h>

#include <Y2PM.h>

#include <y2pm/PMYouPatch.h>
#include <y2pm/PMPackage.h>
#include <y2pm/YouError.h>
#include <y2pm/InstSrcDescr.h>
#include <y2pm/PMYouPackageDataProvider.h>
#include <y2pm/PMYouPatchManager.h>
#include <y2pm/PMPackageManager.h>
#include <y2pm/InstTarget.h>
#include <y2pm/InstTargetError.h>
#include <y2pm/PMYouServers.h>
#include <y2pm/MediaCurl.h>
#include <y2pm/PMYouProduct.h>
#include <y2pm/PMLocale.h>
#include <y2pm/PMYouMedia.h>
#include <y2pm/InstSrcManagerCallbacks.h>
#include <y2pm/RpmDbCallbacks.h>
#include <y2pm/MediaCallbacks.h>

// see comment in destructor
#include <y2pm/InstSrcManager.h>

#include <y2pm/InstYou.h>

using namespace std;

InstYou::Callbacks *InstYou::_callbacks = 0;

class InstYou::DeltaToApply
{
  public:
    PMYouPatchPtr _patch;
    PMPackageDelta _delta;
    PMPackagePtr _pkg;
    PMPackagePtr _basepkg;
    bool _from_filesystem;

    DeltaToApply( PMYouPatchPtr patch, PMPackageDelta delta, PMPackagePtr pkg, PMPackagePtr basepkg, bool from_filesystem)
      : _patch(patch), _delta(delta), _pkg(pkg), _basepkg(basepkg), _from_filesystem(from_filesystem)
      {
      }
};

static void clearDeltasToApplyList(std::vector<InstYou::DeltaToApply*>& list)
{
  for (std::vector<InstYou::DeltaToApply*>::iterator it = list.begin();
	it != list.end(); ++it)
  {
    delete *it;
  }
  list.clear();
}

/** check if sequence info of delta matches the files of the installed packge */
static bool delta_applies_to_filesystem(const PMPackageDelta& delta, bool quickcheck = false );

static PMPackagePtr getPackageForDelta( const PMPackagePtr &pkg, const PMPackageDelta& delta );

static PMError check_md5(const std::string& md5, const Pathname& path)
{
  PMError err;

  if(md5.empty())
    return err;

  string has = PathInfo::md5sum(path);
  if(has != md5)
  {
    err = InstSrcError::E_corrupted_file;
    err.setDetails(
	// translator: filename, md5sum, md5sum
	stringutil::form(_("%s has an md5sum of '%s' but '%s' was expected"),
	  path.asString().c_str(),
	  has.c_str(),
	  md5.c_str()));
  }

  return err;
}

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : InstYou
//
///////////////////////////////////////////////////////////////////

InstYou::InstYou()
{
  _settings = new PMYouSettings();
  _info = new PMYouPatchInfo( _settings );

  init();
}

InstYou::InstYou( const PMYouPatchInfoPtr &info,
                  const PMYouSettingsPtr &paths )
{
  _info = info;
  _settings = paths;

  init();
}

InstYou::~InstYou()
{
#warning MediaAccess destructors
  // XXX for some reason MediaAccess destructors are not called. if removed,
  // remove include file above as well
  if(_usedeltas == ANY_DELTAS)
    Y2PM::instSrcManager().releaseAllMedia();
  releaseSource();
  clearDeltasToApplyList(_deltastoapply);
}

void InstYou::init()
{
  _selectedPatchesIt = _patches.begin();
  _progressTotal = 0;
  _progressCurrent = 0;
  _installedPatches = 0;
  _usedeltas = NO_DELTAS;

  // check whether the necessary tool is installed
  PathInfo pi("/usr/bin/applydeltarpm");
  if(pi.isX())
  {
    SysConfig syscfg( "onlineupdate" );
    string usedeltas = syscfg.readEntry("YOU_USE_DELTAS");

    if(usedeltas == "yes")
    {
      _usedeltas = ANY_DELTAS;
    }
    else if(usedeltas.empty() || usedeltas == "filesystem")
    {
      _usedeltas = ONLY_FS_DELTAS;
    }
    else
    {
      _usedeltas = NO_DELTAS;
    }
  }

  if(_usedeltas == ANY_DELTAS)
  {
    Y2PM::instSrcManager();
  }
}

PMError InstYou::initProduct()
{
  return _settings->initProduct();
}

PMError InstYou::servers( list<PMYouServer> &servers )
{
  PMYouServers youServers( _settings );

  PMError error = youServers.requestServers();

  if ( error ) {
    ERR << "Error fetching servers: " << error << endl;
    return error;
  }

  servers = youServers.servers();

  return PMError();
}

PMError InstYou::readUserPassword()
{
  SysConfig cfg( _settings->passwordFile() );
  _username = cfg.readEntry( "USERNAME_" + _settings->patchUrl().asString() );
  _password = cfg.readEntry( "PASSWORD_" + _settings->patchUrl().asString() );

  _settings->setUsernamePassword( _username, _password );

  return PMError();
}

PMError InstYou::setUserPassword( const string &username,
                                  const string &password,
                                  bool persistent )
{
  _username = username;
  _password = password;

  string u,p;
  if ( persistent ) {
    u = _username;
    p = _password;
  }

  SysConfig cfg( _settings->passwordFile() );
  cfg.writeEntry( "USERNAME_" + _settings->patchUrl().asString(), u );
  cfg.writeEntry( "PASSWORD_" + _settings->patchUrl().asString(), p );
  cfg.save();

  PathInfo::chmod( _settings->passwordFile(), 0600 );

  return PMError();
}

PMError InstYou::retrievePatchDirectory()
{
  if ( !_username.empty() && !_password.empty() ) {
    _settings->setUsernamePassword( _username, _password );
  }

  // Disable media callbacks
  ReportRedirect<MediaCallbacks::DownloadProgressCallback>
    saver( MediaCallbacks::downloadProgressReport, 0 );

  PMError error = _info->getDirectory( true );

  return error;
}

PMError InstYou::retrievePatchInfo()
{
  D__ << "retrievePatchInfo()" << endl;

  withdrawCandidates();
  _patches.clear();

  if ( !_username.empty() && !_password.empty() ) {
    _settings->setUsernamePassword( _username, _password );
  }

  // Disable media callbacks
  ReportRedirect<MediaCallbacks::DownloadProgressCallback>
    saver( MediaCallbacks::downloadProgressReport, 0 );

  PMError error = _info->getPatches( _patches );
  if ( error ) {
    ERR << "Error downloading patchinfos: " << error << endl;
    return error;
  }

  Y2PM::youPatchManager().poolAddCandidates( _patches );

  vector<PMYouPatchPtr>::iterator itPatch;
  for( itPatch = _patches.begin(); itPatch != _patches.end(); ++itPatch ) {
    if ( !_settings->getAll() ) filterArchitectures( *itPatch );
    Y2PM::packageManager().poolAddCandidates( (*itPatch)->packages() );
  }

  // Set correct size depending on if the patch RPM is used or the full one and
  // set the packagesInstalled flag
  for( itPatch = _patches.begin(); itPatch != _patches.end(); ++itPatch ) {
    D__ << "Patch: " << (*itPatch)->name() << endl;
    bool packagesInstalled = false;
    FSize patchSize;

    list<PMPackagePtr> packages = (*itPatch)->packages();
    list<PMPackagePtr>::const_iterator itPkg;
    for ( itPkg = packages.begin(); itPkg != packages.end(); ++itPkg ) {
      DBG << "  Package: " << (*itPkg)->name() << endl;

      bool havedelta = false;
      if(_usedeltas)
      {
	DeltaToApply* delta = FetchSuitableDelta(*itPatch, *itPkg, true);
	if(delta)
	{
	  havedelta = true;
	  _info->packageDataProvider()->setArchiveSize( *itPkg, delta->_delta.size() );
	  delete delta;
	}
      }

      if ( !havedelta && hasPatchRpm( *itPkg ) ) {
        _info->packageDataProvider()->setArchiveSize( *itPkg, (*itPkg)->patchRpmSize() );
        D__ << "    Using patch RPM" << endl;
      }
      FSize archiveSize = (*itPkg)->archivesize();
      D__ << "    ArchiveSize: " << archiveSize << endl;

      if ( (*itPkg)->hasInstalledObj() )
      {
	packagesInstalled = true;
	patchSize += archiveSize;
      }
    }

    list<PMYouFile> files = (*itPatch)->files();
    list<PMYouFile>::const_iterator itFile;
    for ( itFile = files.begin(); itFile != files.end(); ++itFile ) {
      D__ << "  File " << itFile->name() << ": Size: " << itFile->size()
          << endl;
      patchSize += itFile->size();
    }

    (*itPatch)->setPatchSize( patchSize );
    D__ << "  PatchSize: " << patchSize << endl;

    (*itPatch)->setPackagesInstalled( packagesInstalled );
  }

  return PMError();
}

void InstYou::selectPatches( int kinds )
{
  vector<PMYouPatchPtr>::const_iterator it;

  if ( _settings->getAll() ) {
    for( it = _patches.begin(); it != _patches.end(); ++it ) {
      PMSelectablePtr selectable = (*it)->getSelectable();
      if ( !selectable ) {
        INT << "Patch has no selectable." << endl;
        return;
      }
      selectable->user_set_install();
    }
    return;
  }

  PMSelectablePtr yastPatch;

  for( it = _patches.begin(); it != _patches.end(); ++it ) {
    if ( ( (*it)->kind() == PMYouPatch::kind_yast )
         && hasOnlyNewPackages( *it ) ) {
      PMSelectablePtr selectable = (*it)->getSelectable();
      if ( !selectable ) {
        INT << "Patch has no selectable." << endl;
        return;
      }
      yastPatch = selectable;
    }
  }

  if ( yastPatch ) {
    yastPatch->user_set_install();
    DBG << "Select yast patch: " << yastPatch->name() << endl;
  } else {
    for ( it = _patches.begin(); it != _patches.end(); ++it ) {
      D__ << "Check patch " << (*it)->fullName() << " ("
          << (*it)->kindLabel() << ")" << endl;
      if ( (*it)->kind() & kinds ) {
        bool toInstall = false;

        if ( (*it)->updateOnlyNew() ) toInstall = hasNewPackages( *it );
        else toInstall = hasOnlyNewPackages( *it );

        if ( toInstall ) {
          PMSelectablePtr selectable = (*it)->getSelectable();
          if ( !selectable ) {
            INT << "Patch has no selectable." << endl;
            return;
          }

          PMYouPatchPtr candidate = selectable->candidateObj();
          D__ << "Patch " << (*it)->fullName();
          if ( candidate ) {
            D__ << " has candidate." << endl;
            D__ << "Kind: " << candidate->kindLabel() << endl;
            if ( candidate->kind() & kinds ) {
              DBG << "Select patch: " << (*it)->fullName() << endl;
              selectable->user_set_install();
            }
          } else D__ << " has no candidate." << endl;
        }
      }
    }
  }

  // Read Taboo states
  Y2PM::youPatchManager().readSettings();

  updatePackageStates();
}

void InstYou::updatePackageStates()
{
//  D__ << "updatePackageStates()" << endl;

  _totalDownloadSize = 0;

  vector<PMYouPatchPtr>::const_iterator it;
  for ( it = _patches.begin(); it != _patches.end(); ++it ) {
//    D__ << "Patch: " << (*it)->name() << endl;

    bool toInstall = (*it)->isSelected();

    list<PMPackagePtr> packages = (*it)->packages();
    list<PMPackagePtr>::const_iterator itPkg;
    for ( itPkg = packages.begin(); itPkg != packages.end(); ++itPkg ) {
//      D__ << "  Package: " << (*itPkg)->name() << endl;

      bool pkgToInstall = toInstall;

      if ( !(*itPkg)->forceInstall() && (*it)->updateOnlyInstalled() &&
           !(*itPkg)->hasInstalledObj() ) {
//        D__ << "pkgToInstall: false" << endl;
        pkgToInstall = false;
      }

      PMSelectablePtr selectablePkg = (*itPkg)->getSelectable();
      if ( selectablePkg ) {
        if ( pkgToInstall ) {
          selectablePkg->setUserCandidate( *itPkg );
          selectablePkg->user_set_install();
          _totalDownloadSize += (*itPkg)->archivesize();
        } else {
          selectablePkg->clrUserCandidate();
          selectablePkg->user_unset();
        }
      } else {
        INT << "Package has no Selectable." << endl;
      }
    }

    if ( toInstall ) {
      list<PMYouFile> files = (*it)->files();
      list<PMYouFile>::const_iterator itFile;
      for( itFile = files.begin(); itFile != files.end(); ++itFile ) {
        _totalDownloadSize += itFile->size();
      }
    }
  }

  D__ << _totalDownloadSize << endl;
}

PMError InstYou::attachSource()
{
  if ( _media.isAttached() ) return PMError::E_ok;

  int err = PathInfo::assert_dir( _settings->attachPoint() );
  if ( err ) {
    ERR << "Can't create " << _settings->attachPoint() << " (errno: " << err << ")"
        << endl;
    return PMError( InstSrcError::E_error );
  }

  PMError error = _media.open( _settings->patchUrl(), _settings->attachPoint() );
  if ( error ) {
    ERR << "Error opening URL '" << _settings->patchUrl() << "'" << endl;
    return error;
  }
  error = _media.attach();
  if ( error ) {
    ERR << "Error attaching media." << endl;
  }

  return error;
}

PMError InstYou::releaseSource()
{
  PMError error;

  if ( _media.isOpen() && _media.isAttached() ) {
    error = _media.release();
  }

  return error;
}

// sort deltas by source and media nr of base package so packages of one medium
// can be fetched in a row
struct DeltaSortBySourceCriterion
{
  bool operator()(const InstYou::DeltaToApply* lhs, const InstYou::DeltaToApply* rhs)
  {
    if(!lhs || !rhs || !lhs->_basepkg || !rhs->_basepkg)
    {
      return false;
    }
    else if(lhs->_basepkg->source() != rhs->_basepkg->source())
    {
      return lhs->_basepkg->source() < rhs->_basepkg->source();
    }
    else
    {
      return lhs->_basepkg->medianr() < rhs->_basepkg->medianr();
    }
  }
};

PMError InstYou::processPatches()
{
  MIL << "Process patches." << endl;
  ReportRedirect<RpmDbCallbacks::InstallPkgCallback>
    saver1( RpmDbCallbacks::installPkgReport, 0 );
  ReportRedirect<MediaCallbacks::DownloadProgressCallback>
    saver2( MediaCallbacks::downloadProgressReport, 0 );

  resetProgress();
  _installedPatches = 0;
  vector<PMYouPatchPtr> patchesToInstall;

  // Write taboo settings
  // Settings can't be written in destructor, because writeSettings relies on
  // virtual methods.
  Y2PM::youPatchManager().writeSettings();

  bool hasPreInformation = false;

  PMYouPatchPtr patch = firstPatch();
  while ( patch ) {
    string preInfo = patch->preInformation();
    if ( !preInfo.empty() ) {
      if ( !hasPreInformation ) {
        hasPreInformation = true;
        log( _("Show Preinstallation Messages...\n") );
      }
      list<PMYouPatchPtr> patches;
      patches.push_back( patch );
      PMError callbackError = showMessage( "preinfo", patches );
      if ( callbackError == YouError::E_user_skip ) {
        patch->setSkipped( true );
      }
    }

    patch = nextPatch();
  }

  _currentMediaNumber = -1;
  int lastMediaNumber = -1;

  patch =  firstPatch();

  if ( !patch ) {
    log( _("No patches have been selected for installation.\n") );
    return PMError::E_ok;
  }


  PMError error;

  while( patch ) {
    D__ << "Media number, current: " << _currentMediaNumber << " last: "
        << lastMediaNumber << endl;

    if ( _currentMediaNumber > 0 && _currentMediaNumber != lastMediaNumber ) {
      PMError error = installPatches( patchesToInstall );
      if ( error ) {
        ERR << "Install Patches (media " << _currentMediaNumber << "): "
            << error << endl;
        return error;
      }
      patchesToInstall.clear();

      error = verifyMediaNumber( _currentMediaNumber, lastMediaNumber );
      if ( error ) {
        ERR << "Media not found: " << _currentMediaNumber << " " << error
            << endl;
        if ( error = YouError::E_user_skip ) {
          int skipMediaNumber = _currentMediaNumber;
          while( patch && ( skipMediaNumber == _currentMediaNumber ) ) {
            patch->setSkipped( true );
            patch = nextPatch();
          }
          continue;
        } else {
          return error;
        }
      }

      lastMediaNumber = _currentMediaNumber;
    }

    error = attachSource();
    if ( error ) {
      showError( error );
      return error;
    }

    bool skipAll = false;

    string text = stringutil::form( _("Retrieving %s: \"%s\""),
                                    patch->name().asString().c_str(),
                                    patch->summary().c_str() ) + " ... ";
    log( text );

    if ( skipAll || patch->skipped() ) {
      log( _("Skipped\n") );
      patch->setSkipped( true );
    } else {
      patch->setSkipped( false );
      error = retrievePatch( patch );
      if ( !error ) {
        patchesToInstall.push_back( patch );
        log( _("Ok\n") );
      } else {
        DBG << "Patch retrieval error: " << error << endl;

        if ( error == YouError::E_user_abort ) {
          log( _("Aborted\n") );
          PMError callbackError = showError( "abort", "", "" );
          if ( callbackError == YouError::E_user_abort ) break;
          else continue;
        }

        string msg = stringutil::form(
          _("Error downloading patch '%s'.\n"
            "Try again to download the patch, skip this patch, or\n"
            "abort the update?"), patch->name().asString().c_str() );

        PMError callbackError = showError( "skip", msg, error.asString() );

        DBG << "Error callback response: " << callbackError << endl;

        if ( callbackError == YouError::E_user_abort ) {
          log( _("Aborted\n") );
          error = YouError::E_user_abort;
          break;
        } else if ( callbackError == YouError::E_user_skip ) {
          log( _("Skipped\n") );
          patch->setSkipped( true );
          error = PMError::E_ok;
        } else if ( callbackError == YouError::E_user_skip_all ) {
          log( _("Skipped\n") );
          patch->setSkipped( true );
          skipAll = true;
          error = PMError::E_ok;
        } else if ( callbackError == YouError::E_user_retry ) {
          log( _("Retry\n") );
          continue;
        } else {
          if ( error.errClass() == PMError::C_MediaError ) {
            log( _("Error: Cannot get files.\n") );
          } else if ( error == YouError::E_bad_sig_file ||
                      error == YouError::E_bad_sig_rpm ) {
            log( _("Error: Signature check failed.\n") );
          } else {
            log( _("Error\n") );
          }
          return error;
        }
      }
    }

    if ( !incrementProgress() ) return YouError::E_user_abort;

    patch = nextPatch();
  }

  disconnect();

  log("\n");
  if ( error == YouError::E_user_abort ) {
    log(string(_("Download aborted.")) + "\n" );
  } else if ( error ) {
    log( string(_("Download failed.")) + "\n" );
  } else {
#ifdef can_use_file_tag_for_nvidia__bug_46342
    log( string(_("Download finished. You may disconnect from the Internet now.")) + "\n" );
#endif
  }
  log( "\n" );

  if(_usedeltas)
  {
    Pathname orig;

    sort(_deltastoapply.begin(), _deltastoapply.end(), DeltaSortBySourceCriterion());

    for (std::vector<DeltaToApply*>::iterator it = _deltastoapply.begin();
	  it != _deltastoapply.end(); ++it)
    {
      DeltaToApply* delta = *it;
      Pathname rpmPath = delta->_patch->product()->rpmPath( delta->_pkg, false );
      Pathname dest = _media.localPath( rpmPath );

      DBG << "applying delta for " << delta->_pkg->nameEd() << (delta->_from_filesystem?" (filesystem)":"") << endl;

      if(!delta->_from_filesystem)
      {

	error = patchProgress( 0 );
	if ( error ) return error;

	log(stringutil::form(_("Fetching package %s from installation medium"), delta->_basepkg->name()->c_str()) + " ... ");

	error = delta->_basepkg->providePkgToInstall(orig);
	if (error)
	{
	  ERR << "Unable to fetch " << delta->_basepkg->nameEd() << ": " << error << endl;
	  log(stringutil::form(_("Failed: %s\n"), error.asString().c_str() ));
	  return error;
	}
	else
	{
	  ifstream file(orig.asString().c_str());
	  string digest = Digest::digest("MD5", file);

	  if(digest.empty() || digest != delta->_delta.srcmd5())
	  {
	    PMError err(YouError::E_md5sum_mismatch,stringutil::form(_("file '%s' has wrong MD5 checksum"),
			    orig.asString().c_str()));
	    log(err.errstr() + "\n");
	    return err;
	  }

	  log(_("Ok\n"));
	}
      }

      Pathname deltaloc = _media.localPath(delta->_patch->product()->deltaPath( delta->_delta.filename()));
      int ret = PathInfo::assert_dir( dest.dirname() );
      if(ret)
      {
	PMError err(YouError::E_mkdir_failed,dest.dirname().asString());
	log(err.errstr() + "\n");
	return err;
      }

      const char* argv[8] = {0}; // must match max nr of args below

      {
	unsigned i = 0;
	argv[i++] = "applydeltarpm";
	argv[i++] = "-p";
	argv[i++] = "-p";

	if(!delta->_from_filesystem)
	{
	  argv[i++] = "-r";
	  argv[i++] = orig.asString().c_str();
	}
	argv[i++] = deltaloc.asString().c_str();
	argv[i++] = dest.asString().c_str();
	argv[i] = NULL;
      }

      log(delta->_pkg->name().asString() + ": " + string(_("Applying delta")) + " ... ");

      error = patchProgress( 0 );
      if ( error ) return error;

      ExternalProgram prg(argv, ExternalProgram::Stderr_To_Stdout, true);
      string output;
      ProgressCounter counter;
      counter.init(0, 100, 0);
      for ( string line( prg.receiveLine() ); line.length(); line = prg.receiveLine() )
      {
	if(line.find(" percent finished.") != string::npos)
	{
	  unsigned percent = atoi(line.c_str());

	  if(percent > 100)
	    continue;

	  counter.set(percent);
	  if(counter.updateIfNewPercent(3) && counter.percent())
	  {
	    error = patchProgress( counter.percent() );
	    if ( error ) break;
	  }
	}
	else
	{
	  ERR << line << endl;
	  output += line;
	}
      }
      ret = prg.close();

      error = patchProgress( 100 );
      if ( error ) return error;

      if(ret)
      {
	PMError err(YouError::E_reassemble_rpm_from_delta_failed,
	  string("Delta: ") + delta->_delta.filename() + "\nOutput was:\n" + output);
	log(err.errstr() + "\n");
	return err;
      }
      else
	log(_("Ok\n"));

      error = check_md5( delta->_pkg->md5sum(), dest );
      if ( error )
      {
	log(_("File integrity check failed: ") + error.errstr() + "\n");
	return error;
      }

      // if ok
      _info->packageDataProvider()->setLocation( delta->_pkg, rpmPath.asString() );
    }
    clearDeltasToApplyList(_deltastoapply);
  }

  error = installPatches( patchesToInstall );

  if ( error ) {
    ERR << "Install Patches (media " << _currentMediaNumber << "): "
        << error << endl;
    return error;
  }

  list<PMYouPatchPtr> patches;
  for( patch = firstPatch(); patch; patch = nextPatch() ) {
    if ( patch->skipped() ) continue;
    string postInfo = patch->postInformation();
    if ( !postInfo.empty() ) {
      patches.push_back( patch );
    }
  }
  if ( !patches.empty() ) {
    log( _("Show Postinstallation Messages...\n") );
    PMError callbackError = showMessage( "postinfo", patches );
  }

  log( _("Installation finished.\n") );
  log( "\n" );

  if ( _installedPatches == 0 ) {
    log( _("No patches have been installed.") );
  } else if ( _installedPatches == 1 ) {
    log( _("1 patch has been installed.") );
  } else {
    log( stringutil::form( _("%d patches have been installed."),
                           _installedPatches ) );
  }

  if ( error ) {
    showError( "message", _("Installation failed."), "" );
  } else {
    writeLastUpdate();
  }

  withdrawCandidates();

  PMError releaseError = releaseSource();
  if ( releaseError ) {
    ERR << "Error releasing media: " << releaseError << endl;
  }

  return error;
}

PMError InstYou::installPatches( const vector<PMYouPatchPtr> &patches )
{
  if ( _settings->getOnly() || _settings->getAll() ) return PMError::E_ok;

  MIL << "Install patches." << endl;

  bool skipAll = false;

  vector<PMYouPatchPtr>::const_iterator it;
  for( it = patches.begin(); it != patches.end(); ++it ) {
    PMYouPatchPtr patch = *it;

    string text = stringutil::form( _("Installing %s: \"%s\" "),
                                    patch->name().asString().c_str(),
                                    patch->summary().c_str() );
    log( text );

    if ( skipAll || patch->skipped() ) {
      log( _("Skipped\n") );
    } else {
      if ( !patch->skipped() ) {
	PMError error = installPatch( patch );
        if ( !error ) {
          _installedPatches++;
          log( _("Ok\n") );
        } else {
          ERR << "Install error " << patch->name() << ": " << error << endl;
          log( _("Error\n") );

          string msg;
          if ( error == YouError::E_user_abort ) {
            msg = _("Installation of patch was aborted by user.\n\n"
                    "Warning: This can cause an inconsistent system. Select \n"
                    "'Try Again' to repeat installation of this patch.");
          } else {
            msg = stringutil::form ( _("Error installing patch '%s'.\n\n"
                                       "Skip this patch or abort\n"
                                       "the update?"),
                                     patch->name().asString().c_str() );
          }

          string details = error.errstr() + "\n" + error.details();

          PMError callbackError = showError( "skip", msg, details );
          if ( callbackError == YouError::E_user_abort ) {
            log( _("Installation aborted.") );
            error = YouError::E_user_abort;
            break;
          } else if ( callbackError == YouError::E_user_skip ) {
            patch->setSkipped( true );
          } else if ( callbackError == YouError::E_user_skip_all ) {
            patch->setSkipped( true );
            skipAll = true;
          } else if ( callbackError == YouError::E_user_retry ) {
            continue;
          } else {
            break;
          }
        }
      }
    }

    if ( !incrementProgress() ) return YouError::E_user_abort;
  }

  return PMError::E_ok;
}

void InstYou::resetProgress()
{
  _progressCurrent = 0;
  _progressTotal = 0;
  vector<PMYouPatchPtr>::const_iterator it;
  for ( it = _patches.begin(); it != _patches.end(); ++it ) {
    if ( (*it)->isSelected() ) _progressTotal++;
  }

  _progressTotal *= 2;

  D__ << "_progressTotal: " << _progressTotal << endl;
}

bool InstYou::incrementProgress()
{
  ++_progressCurrent;

  int p;
  if ( _progressTotal == 0 ) p = 0;
  else p = _progressCurrent * 100 / _progressTotal;

  PMError err = progress( p );

  if ( err == YouError::E_user_abort ) return false;
  else return true;
}

PMYouPatchPtr InstYou::firstPatch()
{
  _selectedPatchesIt = _patches.begin();

  if ( _selectedPatchesIt == _patches.end() ) return PMYouPatchPtr();

  _currentMediaNumber = _info->mediaNumber( *_selectedPatchesIt );

  if ( (*_selectedPatchesIt)->isSelected() ) return *_selectedPatchesIt;

  return nextPatch();
}

PMYouPatchPtr InstYou::nextPatch()
{
  ++_selectedPatchesIt;

  while ( _selectedPatchesIt != _patches.end() ) {
    int mediaNumber = _info->mediaNumber( *_selectedPatchesIt );
    if ( mediaNumber >= 0 ) _currentMediaNumber = mediaNumber;

    if ( (*_selectedPatchesIt)->isSelected() ) {
      return *_selectedPatchesIt;
    }
    ++_selectedPatchesIt;
  }

  return PMYouPatchPtr();
}

PMError InstYou::installCurrentPatch()
{
  D__ << "Install current Patch" << endl;

  if ( _selectedPatchesIt == _patches.end() ) {
    ERR << "No more patches." << endl;
    return PMError( YouError::E_error );
  }

  return installPatch( *_selectedPatchesIt );
}

PMError InstYou::retrieveCurrentPatch()
{
  if ( _selectedPatchesIt == _patches.end() ) {
    ERR << "No more patches." << endl;
    return PMError( YouError::E_error );
  }

  return retrievePatch( *_selectedPatchesIt );
}

PMError InstYou::installPatch( const PMYouPatchPtr &patch )
{
  D__ << "INSTALL PATCH: " << patch->name() << endl;

  PMError error = patchProgress( 0 );
  if ( error ) return error;

  list<PMPackagePtr> packages = patch->packages();

  int progressTotal = packages.size() + 2; // number of packages plus pre and post script
  int progressCurrent = 0;

  list<PMPackagePtr>::const_iterator itPkg;
  for ( itPkg = packages.begin(); itPkg != packages.end(); ++itPkg ) {
    if ( (*itPkg)->location().empty() &&
         packageToBeInstalled( patch, *itPkg ) ) {
      error = YouError::E_empty_location;
      error.setDetails( "Patch: " + patch->name().asString() +
                        "\nPackage: " + (*itPkg)->name().asString() );
      return error;
    }
  }

  error = executeScript( patch->preScript(), patch->product() );
  D__ << "Prescript: " << error << endl;
  if ( error ) {
    if ( error == YouError::E_user_abort ) return error;
    else return PMError( YouError::E_prescript_failed, error.details() );
  }

  error = patchProgress( 1 );
  if ( error ) return error;

  for ( itPkg = packages.begin(); itPkg != packages.end(); ++itPkg ) {
    error = patchProgress( ( progressCurrent++ + 1 ) * 100 /
                           progressTotal );
    if ( error ) return error;
    if ( !packageToBeInstalled( patch, *itPkg ) ) {
      D__ << "Don't install" << endl;
      continue;
    }

    Pathname fileName;
    if ( (*itPkg)->externalUrl().empty() ) {
      fileName = _media.localPath( (*itPkg)->location() );
    } else {
      fileName = (*itPkg)->location();
    }
    D__ << "INSTALL PKG " << fileName << endl;
    if ( _settings->dryRun() ) {
      cout << "INSTALL: " << fileName << endl;
    } else {
      error = Y2PM::instTarget().installPackage( fileName.asString() );
      if ( error ) {
        E__ << "Installation of RPM " << fileName << " of patch "
            << patch->name() << "failed" << endl;
        //return PMError( YouError::E_rpm_failed, fileName.asString() );
        return error;
      }
    }
  }

  error = patchProgress( 99 );
  if ( error ) return error;

  if ( !_settings->dryRun() ) {
    error = executeScript( patch->postScript(), patch->product() );
    D__ << "Postscript: " << error << endl;
    if ( error ) {
      if ( error == YouError::E_user_abort ) return error;
      else return PMError( YouError::E_postscript_failed, error.details() );
    }

    error = Y2PM::instTarget().installPatch( patch->localFile() );
    if ( error ) {
      E__ << "Error installing patch info." << endl;
      return PMError( YouError::E_install_failed,
                      patch->localFile().asString() );
    }
  }

  error = patchProgress( 100 );
  if ( error ) return error;

  return error;
}

PMError InstYou::executeScript( const string &script,
                                const PMYouProductPtr &product )
{
  if ( script.empty() ) return PMError();

  Pathname scriptPath = product->localScriptPath( script );
  if ( _settings->dryRun() ) {
    cout << "SCRIPT: " << scriptPath << endl;
  } else {
    string extension;
    int pos = script.find( '.' );
    if ( pos >= 0 ) {
      extension = script.substr( pos + 1, script.length() - pos );
    }

    D__ << "Script extension: " << extension << endl;

    PMError error;
    if ( extension == "ycp" ) {
      if ( !_callbacks ) return PMError( YouError::E_callback_missing );

      if ( !_callbacks->executeYcpScript( scriptPath.asString() ) ) {
        return PMError( YouError::E_script_failed, scriptPath.asString() );
      }
    } else {
      PMError error = Y2PM::instTarget().executeScript( scriptPath );
      if ( error ) {
        if ( error == InstTargetError::E_user_abort ) {
          return PMError( YouError::E_user_abort );
        } else {
          return PMError( YouError::E_script_failed, scriptPath.asString() );
        }
      }
    }
  }

  return PMError();
}

class CurlCallbacks : public MediaCurl::Callbacks
{
  public:
    CurlCallbacks( InstYou *instYou, int total )
      : _total( total ), _current( 0 ), _instYou( instYou )
    {
    }

    bool progress( int percent )
    {
      D__ << "Curl Progress: " << percent << endl;

      PMError error = _instYou->patchProgress( ( _current + percent ) * 100 /
                                               _total );

      if ( error ) {
        return false;
      } else {
        return true;
      }
    }

    void setBaseProgress( int current )
    {
      _current = current;
    }

  private:
    int _total;
    int _current;
    InstYou *_instYou;
};

bool InstYou::packageToBeInstalled( const PMYouPatchPtr &patch,
                                    const PMPackagePtr &pkg )
{
  if ( patch->updateOnlyInstalled() ) {
    if ( !pkg->forceInstall() && !pkg->hasInstalledObj() ) {
      D__ << "Package isn't to be installed: '" << pkg->name()
          << "', no installed obj and UpdateOnlyInstalled=true." << endl;
      return false;
    }
  }
  if ( patch->updateOnlyNew() && !isNewerPackage( pkg ) ) {
    D__ << "Package isn't to be installed: '" << pkg->name()
        << "', package not newer and UpdateOnlyNew=true." << endl;
    return false;
  }
  return true;
}

static bool delta_applies_to_filesystem(const PMPackageDelta& delta, bool quickcheck )
{
  if(delta.seq().empty())
    return false;

  unsigned i = 0;
  const char* argv[5] = {0}; // must match max nr of args below
  argv[i++] = "applydeltarpm";
  if(!quickcheck)
    argv[i++] = "-c";
  argv[i++] = "-s";
  argv[i++] = delta.seq().c_str();
  argv[i] = NULL;

  ExternalProgram prg(argv, ExternalProgram::Discard_Stderr);
  if(prg.close() == 0)
  {
    DBG << delta.seq() << " applies from filesystem" << endl;
    return true;
  }
  else
  {
    DBG << delta.seq() << " does NOT apply from filesystem" << endl;
    return false;
  }
}

static PMPackagePtr getPackageForDelta( const PMPackagePtr &pkg, const PMPackageDelta& delta )
{
    DBG << "delta " << delta.filename() << " needs candidate for " <<  pkg->name() << endl;

    PMSelectablePtr sel = pkg->getSelectable();
    if(!sel)
    {
	INT << "selectable NULL" << endl;
	return NULL;
    }

    PMSelectable::PMObjectList::const_iterator it = sel->av_begin();
    PMSelectable::PMObjectList::const_iterator end = sel->av_end();
    for(; it != end; ++it)
    {
	PMPackagePtr p = *it;
	if(!p || p == pkg) continue;
	DBG << "check " << pkg->nameEd() << ' ' << pkg->buildtime() << endl;
	if(!p->source() || !p->source()->descr())
	{
	    INT << "package has no installation source?" << endl;
	    continue;
	}
	else if(!p->source()->descr()->usefordeltas())
	{
	    DBG << "package installation source " << p->source()->descr()->content_label() << " is not usable for deltas" << endl;
	    continue;
	}

	if(p->edition() == delta.ned().edition && p->buildtime() == delta.buildtime())
	{
	    if(p->md5sum().empty() || p->md5sum() != delta.srcmd5())
	    {
	      DBG << pkg->nameEd() << " fits" << endl;
	      return p;
	    }
	}
    }
    DBG << "no matching candiate found" << endl;
    return NULL;
}


static bool toldaboutnotremote = false;
/** return the first usable delta for pkg, NULL otherwise.
 * @param pkg the package to check
 * @param quickcheck whether availability of the delta should only be
 *        checked or also downloaded. quickcheck does not check if the
 *        md5sum of every file in the filesystem matches since that takes
 *        quite some time. when set to false a full check is done to make
 *        really sure though.
 * @return pointer to DeltaToApply. must be freed manually
 * */
InstYou::DeltaToApply* InstYou::FetchSuitableDelta(PMYouPatchPtr patch, PMPackagePtr pkg, bool quickcheck)
{
  if(!_usedeltas)
    return NULL;

  // use deltas only for ftp and http since we cannot write to the mounted
  // media types
  Url url;
  if(_media.isAttached())
    url = _media.url();
  else
    url = _settings->patchUrl();

  switch ( url.protocol() )
  {
#warning FIXME: media api should export _does_download
    case Url::ftp:
    case Url::http:
    case Url::https:
      break;

    default:
      if(!toldaboutnotremote)
      {
	DBG << "media " << url << " is neither ftp nor http. not using deltas" << endl;
	toldaboutnotremote = true;
      }
      return NULL;
  }

  std::list<PMPackageDelta> d = pkg->deltas();
  std::list<PMPackageDelta>::iterator it = d.begin();
  std::list<PMPackageDelta>::iterator end = d.end();
  for(; it != end; ++it)
  {
    bool from_filesystem = false;
    bool fetch_delta = false;
    PMPackagePtr basepkg = NULL;

    if(delta_applies_to_filesystem(*it, quickcheck))
    {
      from_filesystem = true;
      fetch_delta = true;
    }
    else if(_usedeltas == ANY_DELTAS)
    {
      basepkg = getPackageForDelta(pkg, *it);
      if(basepkg)
      {
	DBG << "delta " << it->filename() << " fits " <<  basepkg->nameEd() << endl;
	fetch_delta = true;
      }
      else
      {
	DBG << "no matching package found for delta " << it->filename() << endl;
      }
    }

    if(fetch_delta)
    {
      if(!quickcheck)
      {
	PMError err = retrieveDelta(it->filename(), patch->product(), it->md5sum());
	if(err)
	{
	  ERR << "retrieveDelta: " << err << endl;
	  log(err.asString()+"\n");
	  return NULL;
	}
      }
      return new DeltaToApply(patch, *it, pkg, basepkg, from_filesystem);
    }
  }

  return NULL;
}

PMError InstYou::retrievePatch( const PMYouPatchPtr &patch )
{
  D__ << "PATCH: " << patch->name() << endl;

  RpmDb rpm;

  list<PMPackagePtr> packages = patch->packages();
  list<PMYouFile> files = patch->files();
  int progressTotal = ( packages.size() + files.size() ) * 100 * 100/98;
  int progressCurrent = 0;

  CurlCallbacks callbacks( this, progressTotal );

  MediaCurl::setCallbacks( &callbacks );

  list<PMPackagePtr>::const_iterator itPkg;
  for ( itPkg = packages.begin(); itPkg != packages.end(); ++itPkg )
  {
    callbacks.setBaseProgress( progressCurrent );
    PMError error = patchProgress( progressCurrent * 100 / progressTotal );
    if ( error ) {
      MediaCurl::setCallbacks( 0 );
      return error;
    }
    progressCurrent += 100;

    if ( !_settings->getAll() && !packageToBeInstalled( patch, *itPkg ) ) {
      D__ << "Don't download" << endl;
      continue;
    }


    if(_usedeltas)
    {
      bool havefullrpm = false;
      // check if full or patch rpm is already downloaded (#47807)
      if(!_settings->reloadPatches())
      {
	bool repeat_with_patchrpm = false;
	do
	{
	  Pathname rpmPath = patch->product()->rpmPath( *itPkg, repeat_with_patchrpm );

	  if(_media.provideFile( rpmPath, true, true) == MediaError::E_ok)
	  {
	    havefullrpm = true;
	    DBG << (*itPkg)->name() << " already on disk, not using delta" << endl;
	  }

	  if(!havefullrpm && repeat_with_patchrpm == false)
	  {
	    repeat_with_patchrpm = hasPatchRpm( *itPkg );
	  }
	  else
	  {
	    repeat_with_patchrpm = false;
	  }
	} while(repeat_with_patchrpm);
      }

      if(!havefullrpm)
      {
	DeltaToApply* delta = FetchSuitableDelta(patch, *itPkg);
	if(delta)
	{
	  _deltastoapply.push_back(delta);
	  continue;
	}
      }
    }

    error = retrievePackage( *itPkg, patch->product() );
    if ( error ) {
      MediaCurl::setCallbacks( 0 );
      if ( error == MediaError::E_user_abort ) {
        return YouError::E_user_abort;
      } else {
        error.addDetails( "Patch: " + patch->name().asString() );
        return error;
      }
    }
    if ( _settings->checkSignatures() ) {
      string localRpm;
      string externalUrl = (*itPkg)->externalUrl();
      if ( externalUrl.empty() ) {
        localRpm = _media.localPath( (*itPkg)->location() ).asString();
      } else {
        localRpm = (*itPkg)->location();
      }
      unsigned result = rpm.checkPackage( localRpm );
      if ( result != 0 ) {
        MediaCurl::setCallbacks( 0 );
        ERR << "Signature check failed for " << localRpm << endl;
        return PMError( YouError::E_bad_sig_rpm );
      }
    }
  }

  list<PMYouFile>::const_iterator itFile;
  for( itFile = files.begin(); itFile != files.end(); ++itFile ) {
    callbacks.setBaseProgress( progressCurrent );
    PMError error = patchProgress( progressCurrent * 100 / progressTotal );
    if ( error ) {
      MediaCurl::setCallbacks( 0 );
      return error;
    }
    progressCurrent += 100;

    error = retrieveFile( *itFile );
    if ( error ) {
      MediaCurl::setCallbacks( 0 );
      ERR << "Error retrieving file." << endl;
      return error;
    }
  }

  MediaCurl::setCallbacks( 0 );

  PMError error = patchProgress( 98 );
  if ( error ) return error;

  Pathname scriptPath;
  if ( !patch->preScript().empty() ) {
    PMError error = retrieveScript( patch->preScript(), patch->product() );
    if ( error ) {
      ERR << "Error retrieving preScript." << endl;
      return error;
    }
  }

  error = patchProgress( 99 );
  if ( error ) return error;

  if ( !patch->postScript().empty() ) {
    PMError error = retrieveScript( patch->postScript(), patch->product() );
    if ( error ) {
      ERR << "Error retrieving postScript." << endl;
      return error;
    }
  }

  error = patchProgress( 100 );
  if ( error ) return error;

  return PMError();
}

PMError InstYou::retrieveScript( const string &script,
                                 const PMYouProductPtr &product )
{
  DBG << "Retrieve script '" << script << "'" << endl;

  Pathname scriptPath = product->scriptPath( script );

  PMError error = _media.provideFile( scriptPath, !_settings->reloadPatches() );

  if ( error ) {
    ERR << "Error downloading script from '"
        << _settings->patchUrl() << "/" << scriptPath << "'" << endl;
    return error;
  }

  int e = PathInfo::assert_dir( product->localScriptPath( "" ) );
  if ( e ) {
    ERR << "Can't create " << product->localScriptPath( "" ) << " (errno: "
        << e << ")" << endl;
    return PMError( InstSrcError::E_error );
  }

  string sourceScript = _media.localPath( scriptPath ).asString();
  string destScript = product->localScriptPath( script ).asString();

  GPGCheck gpg;
  bool ok = gpg.check_file( sourceScript, destScript );

  if ( !ok ) {
    if ( _settings->checkSignatures() ) {
      ERR << "Signature check failed for script " << sourceScript << endl;
      return PMError( YouError::E_bad_sig_file, sourceScript );
    } else {
      WAR << "gpg call failed" << endl;
      PathInfo pi( destScript );
      if ( !pi.isExist() ) {
        ERR << "Missing signature." << endl;
        return PMError( YouError::E_missing_sig_file, sourceScript );
      }
    }
  }

  return PMError();
}

PMError InstYou::retrievePackage( const PMPackagePtr &pkg,
                                  const PMYouProductPtr &product )
{
  DBG << "InstYou::retrievePackage: '" << pkg->name() << "'" << endl;

  string externalUrl = pkg->externalUrl();

  if ( !externalUrl.empty() ) {
    Url url( externalUrl );
    if ( !url.isValid() ) {
      ERR << "Invalid URL: '" << url.asString() << "'" << endl;
      return InstSrcError::E_bad_url;
    }

    int e = PathInfo::assert_dir( _settings->externalRpmDir() );
    if ( e ) {
      ERR << "Can't create " << _settings->externalRpmDir() << " (errno: "
          << e << ")" << endl;
      return PMError( InstSrcError::E_error );
    }

    Pathname path = url.path();
    url.setPath( "" );

    MediaAccess media;

    PMError err = media.open( url, _settings->externalRpmDir() );
    if ( err ) return err;

    if ( !_settings->noExternalPackages() ) {
      err = media.attach();
      if ( err ) return err;

      err = media.provideFile( path, !_settings->reloadPatches() );
      if ( err ) return err;
    }

    Pathname localpath = media.localPath( path );
    _info->packageDataProvider()->setLocation( pkg, localpath.asString() );

    return check_md5( pkg->md5sum(), localpath );
  }

  bool patchRpm = hasPatchRpm( pkg );

  PMError error( YouError::E_error );
  Pathname rpmPath = product->rpmPath( pkg, false );

  bool gotRpm = false;
  string md5;

  // no need to download patch rpm if full rpm is already there (#47807)
  if(_media.provideFile( rpmPath, !_settings->reloadPatches(), true ) == MediaError::E_ok)
  {
    patchRpm = false;
  }

  if ( patchRpm ) {
    // If the package has a version installed, try to get patch RPM first.
    Pathname patchRpmPath = product->rpmPath( pkg, patchRpm );
    D__ << "Trying retrieving '" << _settings->patchUrl() << "/" << patchRpmPath
        << "'" << endl;
    error = _media.provideFile( patchRpmPath, !_settings->reloadPatches() );
    if ( error ) {
      WAR << "Retrieving RPM '" << pkg->name() << "' failed: " << error
          << endl;
      if ( error == MediaError::E_user_abort ) return error;
    } else {
      gotRpm = true;
      rpmPath = patchRpmPath;
    }
    md5 = pkg->patchRpmMD5();
  }
  else
  {
    md5 = pkg->md5sum();
  }

  if ( !gotRpm || _settings->getAll() ) {
    D__ << "Retrieving '" << _settings->patchUrl() << "/" << rpmPath
        << "'" << endl;
    error = _media.provideFile( rpmPath, !_settings->reloadPatches() );
    if ( error ) {
      D__ << "Retrieval failed: " << error << endl;
      if ( error == MediaError::E_user_abort ) return error;
    }
  }

  // If download was successful store path to RPM and return.
  if ( !error ) {
    _info->packageDataProvider()->setLocation( pkg, rpmPath.asString() );
    DBG << "RPM: " << pkg->name() << ": " << pkg->location() << endl;
    return check_md5(md5, _media.localPath(rpmPath));
  }

  ERR << "Error retrieving RPM " << pkg->name() << endl;

  error.addDetails( "Package: " + pkg->nameEd() );

  return error;
}

PMError InstYou::retrieveFile( const PMYouFile &file )
{
  DBG << "InstYou::retrieveFile: '" << file.name() << "'" << endl;

  Url url( file.name() );

  if ( !url.isValid() ) {
    ERR << "Invalid URL: '" << url.asString() << "'" << endl;
    return InstSrcError::E_bad_url;
  }

  int e = PathInfo::assert_dir( _settings->filesDir() );
  if ( e ) {
    ERR << "Can't create " << _settings->filesDir() << " (errno: "
        << e << ")" << endl;
    return PMError( InstSrcError::E_error );
  }

  Pathname path = url.path();
  url.setPath( "" );

  MediaAccess media;

  PMError err = media.open( url, _settings->filesDir() );
  if ( err ) return err;

  err = media.attach();
  if ( err ) return err;

  err = media.provideFile( path, !_settings->reloadPatches() );
  if ( err ) return err;

  return PMError();
}

PMError InstYou::retrieveDelta( const std::string& name, const PMYouProductPtr& product, const std::string& md5sum)
{

  Pathname path = product->deltaPath( name );

  PMError err = _media.provideFile( path, !_settings->reloadPatches() );
  if ( err )
  {
    ERR << err << endl;
    return err;
  }

  string fn = _media.localPath( path ).asString();

  ifstream file(fn.c_str());
  string digest = Digest::digest("MD5", file);

  if(digest.empty() || digest != md5sum)
  {
    // Translator: filename, md5sum, md5sum
    string msg = stringutil::form(_("file '%s' has wrong MD5 checksum: '%s' != '%s'"),
	path.asString().c_str(), digest.c_str(), md5sum.c_str());
    return PMError(YouError::E_md5sum_mismatch, msg);
  }


  return err;
}

PMError InstYou::disconnect()
{
  return _media.disconnect();
}

PMError InstYou::removePackages()
{
  PMError error;
  PMPackagePtr nullpkg(NULL);
  const string empty;
  Pathname path;
  list<PMYouProductPtr> products;
  list<PMYouProductPtr>::iterator prodit;

  products = _settings->products();

  for(prodit = products.begin(); prodit != products.end(); ++prodit)
  {
    path = (*prodit)->scriptPath(empty);
    error = _media.releaseFile(path);
    if ( error ) ERR << "Can't release " << path.asString() << endl;

    path = (*prodit)->localScriptPath(empty);
    PathInfo::recursive_rmdir(path);

    path = (*prodit)->rpmPath(nullpkg, false);
    error = _media.releaseFile(path);
    if ( error ) ERR << "Can't release " << path.asString() << endl;

    path = (*prodit)->deltaPath(empty);
    error = _media.releaseFile(path);
    if ( error ) ERR << "Can't release " << path.asString() << endl;
  }

  return error;
}

void InstYou::showPatches( bool verbose )
{
  vector<PMYouPatchPtr>::const_iterator it;
  for( it = _patches.begin(); it != _patches.end(); ++it ) {
    PMSelectablePtr sel = (*it)->getSelectable();
    if ( sel ) {
      if ( sel->has_installed() ) cout << "I";
      else cout << " ";
      if ( sel->to_install() ) cout << "S";
      else cout << " ";
      cout << " ";
    }
    cout << (*it)->name() << " (" << (*it)->kindLabel( (*it)->kind() ) << "): "
         << (*it)->shortDescription() << endl;
    if ( verbose ) {
    list<PMPackagePtr> packages = (*it)->packages();
    list<PMPackagePtr>::const_iterator itPkg;
      for ( itPkg = packages.begin(); itPkg != packages.end(); ++itPkg ) {
        cout << "     RPM: " << (*itPkg)->name() << " " << (*itPkg)->edition()
             << " " << (*itPkg)->arch()
             << " (";
        if ( (*itPkg)->hasInstalledObj() ) {
          PkgEdition instEd = (*itPkg)->getInstalledObj()->edition();
          cout << "installed: " << instEd << " ";
          if ( instEd < (*itPkg)->edition() ) cout << "[older]";
          else if ( instEd > (*itPkg)->edition() ) cout << "[newer]";
          else cout << "[same]";
        } else {
          cout << "not installed";
        }
        cout << ")" << endl;

	std::list<PMPackageDelta> d = (*itPkg)->deltas();
	if(!d.empty())
	{
	  cout << "Deltas:" << endl;
	  std::list<PMPackageDelta>::iterator it = d.begin();
	  std::list<PMPackageDelta>::iterator end = d.end();
	  for(; it != end; ++it)
	  {
	    cout << "  " << *it;
	  }
	}
      }
    }
  }
}

bool InstYou::hasOnlyNewPackages( const PMYouPatchPtr &patch )
{
  bool install = false;

  // Check, if patch contains at least one package which is newer than the
  // correpsonding package on the system. If yes, trigger install.
  // If a package from the patch is older than the installed package. don't
  // trigger install.
  list<PMPackagePtr> packages = patch->packages();
  list<PMPackagePtr>::const_iterator itPkg;
  for ( itPkg = packages.begin(); itPkg != packages.end(); ++itPkg ) {
    PkgEdition candEd = (*itPkg)->edition();
    D__ << "  PKG-CAND: " << (*itPkg)->name() << "-" << candEd << endl;
    if ( (*itPkg)->hasInstalledObj() ) {
      PMPackagePtr installed = (*itPkg)->getInstalledObj();
      PkgEdition instEd = installed->edition();
      D__ << "  PKG-INST: " << installed->name() << "-" << instEd << endl;
      if ( instEd > candEd ) {
        install = false;
        break;
      } else if ( instEd < candEd ) {
        install = true;
      }
    }
  }

  D__ << "hasOnlyNewPackages: " << patch->fullName() << " "
      << ( install ? "yes" : "no" ) << endl;

  return install;
}

bool InstYou::hasNewPackages( const PMYouPatchPtr &patch )
{
  bool install = false;

  // Check, if patch contains at least one package which is newer than the
  // correpsonding package on the system. If yes, trigger install.
  list<PMPackagePtr> packages = patch->packages();
  list<PMPackagePtr>::const_iterator itPkg;
  for ( itPkg = packages.begin(); itPkg != packages.end(); ++itPkg ) {
    if ( isNewerPackage( *itPkg ) ) {
      install = true;
      break;
    }
  }

  D__ << "hasNewPackages: " << patch->fullName() << " "
      << ( install ? "yes" : "no" ) << endl;

  return install;
}

bool InstYou::isNewerPackage( const PMPackagePtr &pkg )
{
  if ( !pkg->hasInstalledObj() ) return false;

  PkgEdition candEd = pkg->edition();
  D__ << "  PKG-CAND: " << pkg->name() << "-" << candEd << endl;
  PMPackagePtr installed = pkg->getInstalledObj();
  if ( !installed ) {
    INT << "No installed package" << endl;
    return false;
  }
  PkgEdition instEd = installed->edition();
  D__ << "  PKG-INST: " << installed->name() << "-" << instEd << endl;
  return candEd > instEd;
}

bool InstYou::firesPackageTrigger( const PMYouPatchPtr &patch )
{
  return false;
}

bool InstYou::firesScriptTrigger( const PMYouPatchPtr &patch )
{
  return false;
}

bool InstYou::hasPatchRpm( const PMPackagePtr &pkg )
{
  bool pkgHasInstalledObj = pkg->hasInstalledObj();

  if ( pkgHasInstalledObj ) {
    PkgEdition installedVersion = pkg->getInstalledObj()->edition();
    D__ << "Installed: " << installedVersion.asString() << endl;
    list<PkgEdition> baseVersions = pkg->patchRpmBaseVersions();
    list<PkgEdition>::const_iterator it2;
    for( it2 = baseVersions.begin(); it2 != baseVersions.end(); ++it2 ) {
      D__ << " Base version: " << (*it2).asString() << endl;
      if ( *it2 == installedVersion ) {
        D__ << "return true" << endl;
        return true;
      }
    }
  }

  D__ << "return false" << endl;
  return false;
}

void InstYou::setCallbacks( InstYou::Callbacks *callbacks )
{
  _callbacks = callbacks;
}

PMError InstYou::progress( int i )
{
  bool ret = true;

  if ( _callbacks ) {
    ret = _callbacks->progress( i );
  }

  if ( ret ) return PMError();
  else return YouError::E_user_abort;
}

PMError InstYou::patchProgress( int i, const string &pkg )
{
  bool ret = true;

  if ( _callbacks ) {
    ret = _callbacks->patchProgress( i, pkg );
  }

  if ( ret ) return PMError();
  else return YouError::E_user_abort;
}

PMError InstYou::showError( const string &type, const string &text,
                            const string &details )
{
  if ( _callbacks ) {
    return _callbacks->showError( type, text, details );
  } else {
    return YouError::E_callback_missing;
  }
}

PMError InstYou::showError( const PMError &error )
{
  return showError( "error", error.errstr(), error.details() );
}

PMError InstYou::showMessage( const string &type, list<PMYouPatchPtr> &patches )
{
  if ( _callbacks ) {
    return _callbacks->showMessage( type, patches );
  } else {
    return YouError::E_callback_missing;
  }
}

void InstYou::log( const string &text )
{
  MIL << text << endl;

  if ( _callbacks ) {
    _callbacks->log( text );
  } else {
    D__ << "No callback set" << endl;
  }

  if ( _settings->isLogEnabled() && !text.empty() && text != "\n" ) {
    time_t timestamp = time( 0 );
    struct tm *brokentime = localtime( &timestamp );
    char date[ 50 ];
    strftime ( date, sizeof( date ), "%F %H:%M:%S", brokentime );

    pid_t pid = getpid();

    string logFile = _settings->logFile().asString();
    ofstream out( logFile.c_str(), ios::app );
    if ( out.fail() ) {
      ERR << "Unable to save '" << logFile << "'" << endl;
    }
    out << date << " (" << pid << "): " << text.c_str();
    if ( *(text.rbegin() ) != '\n' ) out << endl;
  }
}

int InstYou::lastUpdate()
{
  string date = _settings->config()->readEntry( "LastUpdate" );

  D__ << "LastUpdate: '" << date << "'" << endl;

  if ( date.empty() ) return -1;

  Date lastUpdate( date );
  DBG << "LastUpdate: " << lastUpdate << endl;

  int diff = Date::now() - lastUpdate;

  DBG << diff << endl;

  return diff / 60 / 60 / 24;
}

PMError InstYou::writeLastUpdate()
{
  MIL << "writeLastUpdate" << endl;

  _settings->config()->writeEntry( "LastUpdate", Date::toSECONDS( Date::now() ) );
  _settings->config()->save();

  return PMError();
}

int InstYou::quickCheckUpdates()
{
  PMYouServer server = _settings->patchServer();

  Url url = server.url();

  D__ << "InstYou::quickCheckUpdates(): " << url << endl;

  _info->processMediaDir();

  // FIXME: Iterate through all directories

  Pathname path = url.path();
  path += _settings->primaryProduct()->patchPath() + _settings->directoryFileName();
  url.setPath( path.asString() );

  Pathname dest = _settings->localWriteDir() + "quickcheck";

  PMError error = MediaAccess::getFile( url, dest );
  if ( error ) {
    ERR << "Quick check Updates: " << error << endl;
    return -1;
  }

  list<string> newPatchFiles;
  _info->readDirectoryFile( dest, newPatchFiles );

  Pathname dirFile = _settings->rootAttachPoint() +
                     _settings->primaryProduct()->patchPath() +
                     _settings->directoryFileName();

  DBG << "Old directory file: " << dirFile << endl;

  list<string> oldPatchFiles;
  _info->readDirectoryFile( dirFile, oldPatchFiles );

  int patchCount = 0;
  list<string>::const_iterator newIt;
  for( newIt = newPatchFiles.begin(); newIt != newPatchFiles.end(); ++newIt ) {
    D__ << "newIt: " << *newIt << endl;
    list<string>::const_iterator oldIt;
    for( oldIt = oldPatchFiles.begin(); oldIt != oldPatchFiles.end(); ++oldIt ) {
      D__ << "oldIt: " << *oldIt << endl;
      if ( *newIt == *oldIt ) {
        break;
      }
    }
    if ( oldIt == oldPatchFiles.end() ) patchCount++;
  }

  return patchCount;
}

void InstYou::filterArchitectures( PMYouPatchPtr &patch )
{
  D__ << "filter " << patch->name() << endl;

  list<PMPackagePtr> packages = patch->packages();
  list<PMPackagePtr>::iterator it1 = packages.begin();
  while( it1 != packages.end() ) {
    D__ << "  PKG: " << (*it1)->nameEdArch() << endl;

    PMSelectablePtr s = Y2PM::packageManager().getItem( (*it1)->name() );

    // Handle installed packages. If the package is already installed, all
    // instances of the packages in the patch which have a different
    // architecture are discarded.
    if ( s ) {
      PMPackagePtr installedPkg = s->installedObj();
      if ( installedPkg ) {
        D__ << "    Package is installed" << endl;
        if ( installedPkg->arch() == (*it1)->arch() ) {
          D__ << "    Same arch" << endl;
          ++it1;
        } else {
          D__ << "    Different arch" << endl;
          it1 = packages.erase( it1 );
        }
        continue;
      }
    }

    D__ << "    Package isn't installed." << endl;

    std::list<PkgArch> archs = _settings->primaryProduct()->archs();

    // Check if package instance has an allowed arch. If not, discard it.
    std::list<PkgArch>::const_iterator archIt;
    for( archIt = archs.begin(); archIt != archs.end(); ++archIt ) {
      if ( *archIt == (*it1)->arch() ) break;
    }
    if ( archIt == archs.end() ) {
      D__ << "    Discard package because of unknown arch." << endl;
      it1 = packages.erase( it1 );
      continue;
    }

    // Handle uninstalled packages. The instance of the package which has the
    // best architecture is used. All other instances are discarded.
    std::list<PMPackagePtr>::iterator bestArch = it1;
    list<PMPackagePtr>::iterator it2 = it1;
    ++it2;
    while( it2 != packages.end() ) {
      D__ << "      Iterating " << (*it2)->nameEdArch() << endl;
      if ( (*bestArch)->name() == (*it2)->name() ) {
        if ( (*bestArch)->arch() == (*it2)->arch() ) {
          ERR << "Two packages '" << (*bestArch)->name() << "' with same "
              << "architecture in the same patch is illegal." << endl;
          ++it2;
        } else {
          std::list<PkgArch>::const_iterator archIt;
          for( archIt = archs.begin(); archIt != archs.end(); ++archIt ) {
            if ( *archIt == (*bestArch)->arch() ) {
              D__ << "        already have best arch" << endl;
              it2 = packages.erase( it2 );
              break;
            } else if ( *archIt == (*it2)->arch() ) {
              D__ << "        better arch." << endl;
              if ( bestArch == it1 ) ++it1;
              packages.erase( bestArch );
              bestArch = it2;
              ++it2;
              break;
            }
          }
          if ( archIt == archs.end() ) {
            D__ << "        arch no allowed." << endl;
            if ( bestArch == it1 ) ++it1;
            packages.erase( bestArch );
            break;
          }
        }
      } else {
        ++it2;
      }
    }

    ++it1;
  }

  patch->setPackages( packages );
}

PMError InstYou::verifyMediaNumber( int number, int lastNumber )
{
  D__ << "verifyMediaNumber: " << number << " last: " << lastNumber << endl;

  PMYouMediaPtr masterMedia = _settings->masterMedia();

  if ( !masterMedia ) return PMError::E_ok;

  PMError error = attachSource();
  if ( error ) return error;

  PMYouMediaPtr mediaInstance = new PMYouMedia( _media );
  error = mediaInstance->readInfo( number );
  if ( error ) {
    WAR << "Unable to read media info: " << error << endl;
  }

  while ( !mediaInstance->isPartOf( masterMedia ) ) {
    InstSrcManagerCallbacks::MediaChangeReport::Send
        callback( InstSrcManagerCallbacks::mediaChangeReport );

    log( stringutil::form( _("Request media %d."), number ) );
    log( " " );

    if ( callback->isSet() ) {
      PMError error = releaseSource();
      if ( error ) {
        ERR << "Release source failed." << endl;
        return error;
      }

      string errorStr = _("Media not found");
      string productStr = _("YOU Patch CD");
      string urlStr = _settings->patchUrl().asString();
      string result = callback->changeMedia( errorStr, urlStr, productStr,
                                             lastNumber, "",
					     number, "",
                                             masterMedia->doubleSided() );
      if ( result == "" ) {
        log( _("Ok.\n") );
        // retry
      } else if ( result == "I" ) {
        log( _("Ignore.\n") );
        // Ignore bad media id
        break;
      } else if ( result == "S" ) {
        log( _("Skip.\n") );
        return YouError::E_user_skip;
      } else if ( result == "C" ) {
        log( _("Abort.\n") );
        return YouError::E_user_abort;
      } else if ( result == "E" ) {
        log( _("Eject media.\n") );
        _media.release( true );
        continue;
      }
    } else {
      log( _("Failed.\n") );
      return YouError::E_wrong_media;
    }

    PMError error = attachSource();
    if ( error ) {
      ERR << "Attach source failed." << endl;
      return error;
    }

    error = mediaInstance->readInfo( number );
    if ( error ) {
      WAR << "Unable to read media info: " << error << endl;
    }
  }

  return PMError::E_ok;
}

void InstYou::withdrawCandidates(void)
{
  if(!_patches.empty())
  {
      Y2PM::youPatchManager().poolRemoveCandidates( _patches );

      vector<PMYouPatchPtr>::iterator itPatch;
      for( itPatch = _patches.begin(); itPatch != _patches.end(); ++itPatch ) {
         if ( !_settings->getAll() ) filterArchitectures( *itPatch );
         Y2PM::packageManager().poolRemoveCandidates( (*itPatch)->packages() );
      }
      Y2PM::packageManager().setNothingSelected();
  }
}
