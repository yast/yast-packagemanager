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

   File:	MediaAccess.h

   Author:	Klaus Kaempf <kkaempf@suse.de>
   Maintainer:	Klaus Kaempf <kkaempf@suse.de>

   Purpose:	Handle access to a medium

/-*/
#ifndef MediaAccess_h
#define MediaAccess_h

#include <iosfwd>
#include <map>
#include <list>
#include <string>

#include <y2util/Pathname.h>
#include <y2util/PathInfo.h>
#include <y2util/Url.h>

#include <y2pm/MediaAccessPtr.h>
#include <y2pm/MediaError.h>

class MediaHandler;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : MediaAccess
/**
 * @short Handle access to a medium
 *
 * The concrete @ref MediaHandler for a certain url is created
 * on @ref open and deleted on @close.
 *
 * The inteface here basically checks whether the handler exists,
 * then forwards the request to @ref MediaHandler.
 **/
class MediaAccess : public CountedRep {
	REP_BODY(MediaAccess);

    public:

        /**
	 * default error class
	 **/
        typedef MediaError Error;

    private:

	static const Pathname _noPath;

	/**
	 * handler for 'physical' media
	 * == 0 if not open
	 **/
	MediaHandler * _handler;

    public:

       /**
        * constructor
        **/
	MediaAccess();

	/**
	 * open url. If preferred_attach_point is given,
	 * try to use it as attach point.
	 *
	 * <b>Caution:</b> The medium can choose a different attach point.
	 * Only getAttachPoint() knows the real attach point.
	 **/
	PMError open( const Url& url, const Pathname & preferred_attach_point = "" );

	/**
	 * True if media is open.
	 **/
	bool isOpen() const { return( _handler != 0 ); }

	/**
	 * Used Protocol if media is opened, otherwise 'unknown'.
	 **/
        Url::Protocol protocol() const;

	/**
	 * Url if media is opened, otherwise empty.
	 **/
        Url url() const;

	/**
	 * close url
	 **/
	PMError close();

    public:

	/**
	 * Use concrete handler to attach the media.
	 *
	 * @param next try next available device in turn until end of device
	 * list is reached (for media which are accessible through multiple
	 * devices like cdroms).
	 **/
	PMError attach(bool next = false);

	/**
	 * True if media is attached.
	 **/
	bool isAttached() const;

	/**
	 * Return the local directory that corresponds to medias url,
	 * no matter if media isAttached or not. Files requested will
	 * be available at 'localRoot() + filename' or better
	 * 'localPath( filename )'.
	 *
	 * If media is not open an empty pathname is returned.
	 **/
	const Pathname & localRoot() const;

	/**
	 * Short for 'localRoot() + pathname', but returns an empty
	 * pathname if media is not open.
	 *
	 * Files provided will be available at 'localPath(filename)'.
	 **/
	Pathname localPath( const Pathname & pathname ) const;

        /**
          Use concrete handler to disconnect the media.

          This is useful for media which e.g. holds open a connection to a
          server like FTP. After calling disconnect() the media object still is
          valid and files are present.

          After calling disconnect() it's not possible to call provideFile() or
          provideDir() anymore.
        */
        PMError disconnect();

	/**
	 * Use concrete handler to release the media.
	 * @param eject if true, physically eject the media * (i.e. CD-ROM)
	 **/
	PMError release( bool eject = false );

	/**
	 * Use concrete handler to provide file denoted by path below
	 * 'attach point'. Filename is interpreted relative to the
	 * attached url and a path prefix is preserved.
         *
         * @param cached  If cached is set to true, the function checks, if
         *                the file already exists and doesn't download it again
         *                if it does. Currently only the existence is checked,
         *                no other file attributes.
	 * @param checkonly If this and 'cached' are set to true only the
	 *                  existence of the file is checked but it's not
	 *                  downloaded. If 'cached' is unset an errer is
	 *                  returned always.
	 **/
	PMError provideFile( const Pathname & filename, bool cached = false, bool checkonly = false ) const;

	/**
	 * Remove filename below attach point IFF handler downloads files
	 * to the local filesystem. Never remove anything from media.
	 **/
	PMError releaseFile( const Pathname & filename ) const;

	/**
	 * Use concrete handler to provide directory denoted
	 * by path below 'attach point' (not recursive!).
	 * 'dirname' is interpreted relative to the
	 * attached url and a path prefix is preserved.
	 **/
	PMError provideDir( const Pathname & dirname ) const;

	/**
	 * Use concrete handler to provide directory tree denoted
	 * by path below 'attach point' (recursive!!).
	 * 'dirname' is interpreted relative to the
	 * attached url and a path prefix is preserved.
	 **/
	PMError provideDirTree( const Pathname & dirname ) const;

	/**
	 * Remove directory tree below attach point IFF handler downloads files
	 * to the local filesystem. Never remove anything from media.
	 **/
	PMError releaseDir( const Pathname & dirname ) const;

	/**
	 * Remove pathname below attach point IFF handler downloads files
	 * to the local filesystem. Never remove anything from media.
	 *
	 * If pathname denotes a directory it is recursively removed.
	 * If pathname is empty or '/' everything below the attachpoint
	 * is recursively removed.
	 **/
	PMError releasePath( const Pathname & pathname ) const;

    public:

	/**
	 * Return content of directory on media via retlist. If dots is false
	 * entries starting with '.' are not reported.
	 *
	 * The request is forwarded to the concrete handler,
	 * which may atempt to retieve the content e.g. via 'readdir'
	 *
	 * <B>Caution:</B> This is not supported by all media types.
	 * Be prepared to handle E_not_supported_by_media.
	 **/
        PMError dirInfo( std::list<std::string> & retlist,
			 const Pathname & dirname, bool dots = true ) const;

	/**
	 * Basically the same as dirInfo above. The content is returned as
	 * PathInfo::dircontent, which includes name and filetype of each directory
	 * entry. Retrieving the filetype usg. requires an additional ::stat call for
	 * each entry, thus it's more expensive than a simple readdir.
	 *
	 * <B>Caution:</B> This is not supported by all media types.
	 * Be prepared to handle E_not_supported_by_media.
	 **/
	PMError dirInfo( PathInfo::dircontent & retlist,
			 const Pathname & dirname, bool dots = true ) const;

	/**
	 * Destructor
	 **/
	virtual ~MediaAccess();

    public:

	virtual std::ostream & dumpOn( std::ostream & str ) const;

    public:
        /**
         * Get file from location at specified by URL and copy it to
         * destination.
         *
         * @param from Source URL
         * @param to   Destination file name
         **/
        static PMError getFile( const Url &from, const Pathname &to );

    public:

      /**
       * Helper class that provides file on construction
       * and cleans up on destruction.
       *
       * <b>Caution:</b> There's no synchronisation between multiple
       * FileProvider instances, that provide the same file from the
       * same media. If the first one goes out of scope, the file is
       * cleaned. It's just a convenience for 'access and forgett'.
       *
       * <b>Caution:</b> We should either store the reference MediaAccess'
       * MediaHandler here (for this MediaHandler must become a
       * ref counting pointer class), or we need more info from MediaHandler
       * (whether he's downloading to the local fs. If not, no releasefile
       * is necessary).
       * Currently we can not releaseFile after the media was closed
       * (it's passed to the handler, which is deleted on close).
       **/
      class FileProvider {
	FileProvider( const FileProvider & );             // no copy
	FileProvider & operator=( const FileProvider & ); // no assign
	private:
	  constMediaAccessPtr _media;
	  Pathname            _file;
	  PMError             _lasterr;
	public:
	  /**
	   * default error class
	   **/
	  typedef MediaError Error;

	  FileProvider( constMediaAccessPtr media_r, const Pathname & file_r )
	    : _media( media_r )
	    , _file( file_r )
	    , _lasterr( Error::E_error )
	  {
	    if ( _file.empty() ) {
	      _lasterr = Error::E_bad_filename;
	    } else if ( _media ) {
	      _lasterr = _media->provideFile( _file );
	    }
	    if ( _lasterr )
	      _media = 0;
	  }

	  ~FileProvider() {
	    if ( _media )
	      _media->releaseFile( _file );
	  }

	public:

	  /**
	   * If no error, expect operator() to return the local
	   * Pathname of the provided file.
	   **/
	  PMError error() const { return _lasterr; }

	  /**
	   * Return the local Pathname of the provided file or
	   * an empty Pathname on error.
	   **/
	  Pathname operator()() const {
	    if ( _media )
	      return _media->localPath( _file );
	    return Pathname();
	  }
      };
};

///////////////////////////////////////////////////////////////////

#endif // MediaAccess_h

