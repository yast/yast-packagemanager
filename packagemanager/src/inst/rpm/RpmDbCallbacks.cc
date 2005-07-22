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

  File:       RpmDbCallbacks.cc

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Callbacks triggered by RpmDb

/-*/

#include <iostream>

#include <y2pm/RpmDbCallbacks.h>

using namespace std;

///////////////////////////////////////////////////////////////////
namespace RpmDbCallbacks {
///////////////////////////////////////////////////////////////////

  ///////////////////////////////////////////////////////////////////
  // Reporting progress of rpm database conversion.
  ///////////////////////////////////////////////////////////////////
  ConvertDbReport convertDbReport;

  void ConvertDbCallback::start( const Pathname & v3db ) {
  }
  void ConvertDbCallback::progress( const ProgressData & prg,
				    unsigned failed, unsigned ignored, unsigned alreadyInV4 ) {
  }
  void ConvertDbCallback::dbInV4( const string & pkg ) {
  }
  CBSuggest ConvertDbCallback::dbReadError( int offset ) {
    return CBSuggest::PROCEED;
  }
  void ConvertDbCallback::dbWriteError( const string & pkg ) {
  }
  void ConvertDbCallback::stop( PMError error ) {
  }

  ///////////////////////////////////////////////////////////////////
  // Reporting progress of rpm database rebuild.
  ///////////////////////////////////////////////////////////////////
  RebuildDbReport rebuildDbReport;

  void RebuildDbCallback::start() {
  }
  void RebuildDbCallback::progress( const ProgressData & prg ) {
  }
  void RebuildDbCallback::notify( const string & msg ) {
  }
  void RebuildDbCallback::stop( PMError error ) {
  }

  ///////////////////////////////////////////////////////////////////
  // Reporting progress reading the rpm database
  ///////////////////////////////////////////////////////////////////
  ScanDbReport scanDbReport;

  void ScanDbCallback::start() {
  }
  void ScanDbCallback::progress( const ProgressData & prg ) {
  }
  void ScanDbCallback::stop( PMError error ) {
  }

  ///////////////////////////////////////////////////////////////////
  // Reporting progress of rpm package installation.
  ///////////////////////////////////////////////////////////////////
  InstallPkgReport installPkgReport;

  void InstallPkgCallback::start( const Pathname & filename ) {
  }
  void InstallPkgCallback::progress( const ProgressData & prg ) {
  }
  void InstallPkgCallback::stop( PMError error ) {
  }

  ///////////////////////////////////////////////////////////////////
  // Reporting progress of rpm package deletion.
  ///////////////////////////////////////////////////////////////////
  RemovePkgReport removePkgReport;

  void RemovePkgCallback::start( const string & label ) {
  }
  void RemovePkgCallback::progress( const ProgressData & prg ) {
  }
  void RemovePkgCallback::stop( PMError error ) {
  }

#if 0
  ///////////////////////////////////////////////////////////////////
  // Reporting @
  ///////////////////////////////////////////////////////////////////
  @Report @Report;

  void @Callback::start() {
  }
  void @Callback::progress( const ProgressData & prg ) {
  }
  void @Callback::stop( PMError error ) {
  }
#endif

///////////////////////////////////////////////////////////////////
} // namespace RpmDbCallbacks
///////////////////////////////////////////////////////////////////
