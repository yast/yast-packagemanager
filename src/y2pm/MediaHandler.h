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

   File:	MediaHandler.h

   Author:	Klaus Kaempf <kkaempf@suse.de>
   Maintainer:	Klaus Kaempf <kkaempf@suse.de>

   Purpose:	Abstract base class for 'physical' MediaHandler
		like MediaCD, MediaDIR, ...

/-*/
#ifndef MediaHandler_h
#define MediaHandler_h

#include <iosfwd>
#include <string>
#include <list>

#include <y2util/Pathname.h>
#include <y2util/PathInfo.h>
#include <y2util/Url.h>

#include <y2pm/MediaAccess.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : MediaHandler
/**
 * @short Abstract base class for 'physical' MediaHandler like MediaCD, etc.
 *
 * Handles the requests forwarded by @ref MediaAccess. The public interface
 * contains nonvirtual methods, which should do common sanitychecks and
 * logging. For the real action they call virtual methods overloaded by the
 * concrete handler.
 **/
class MediaHandler {
    friend std::ostream & operator<<( std::ostream & str, const MediaHandler & obj );

    public:

        /**
	 * default error class
	 **/
        typedef MediaError Error;

    private:

	/**
	 * this is where the media will be actually "mounted"
	 * all files are provided 'below' this directory.
	 **/
	Pathname _attachPoint;

	/**
	 * If a default attach point was created, it has to be
	 * removed on destuction.
	 **/
	bool _tmp_attachPoint;

	/**
	 * The local directory that corresponds to the media url.
	 * With NFS it's the '_attachPoint', as the directory on the
	 * server is mounted. With CD/DVD it's 'attach point+_url.path()'
	 * because the CDs root directory is mounted. And with CIFS
	 * it's '_url.path() without the shares name'.
	 **/
	Pathname _localRoot;

	/**
	 * True if concrete handler downloads files to the local
	 * filesystem. If true releaseFile/Dir will delete them.
	 **/
	bool _does_download;

        /**
	 * True, if media is attached.
	 **/
	bool _isAttached;

    protected:

        /**
	 * Url to handle
	 **/
	const Url _url;

        /**
	 * Attachpoint to use
	 **/
	const Pathname & attachPoint() const { return _attachPoint; }

    protected:

        ///////////////////////////////////////////////////////////////////
        //
        // Real action interface to be overloaded by concrete handler.
        //
        ///////////////////////////////////////////////////////////////////

	/**
	 * Call concrete handler to attach the media.
	 *
	 * Asserted that not already attached, and attachPoint is a directory.
	 *
	 * @param next try next available device in turn until end of device
	 * list is reached (for media which are accessible through multiple
	 * devices like cdroms).
	 **/
	virtual PMError attachTo(bool next = false) = 0;

        /**
         * Call concrete handler to disconnect media.
	 *
	 * Asserted that media is attached.
	 *
         * This is useful for media which e.g. holds open a connection to a
         * server like FTP. After calling disconnect() the media object still is
         * valid and files are present.
	 *
         * After calling disconnect() it's not possible to call provideFile() or
         * provideDir() anymore.
	 **/
        virtual PMError disconnectFrom() { return Error::E_ok; }

	/**
	 * Call concrete handler to release the media.
	 * If eject is true, physically eject the media (i.e. CD-ROM).
	 *
	 * Asserted that media is attached.
	 **/
	virtual PMError releaseFrom( bool eject ) = 0;

	/**
	 * Call concrete handler to physically eject the media (i.e. CD-ROM)
	 * in case the media is not attached..
	 *
	 * Asserted that media is not attached.
	 **/
	virtual void forceEject() {}

	/**
	 * Call concrete handler to provide file below attach point.
	 *
	 * Default implementation provided, that returns whether a file
	 * is located at '_localRoot + filename'.
	 *
	 * Asserted that media is attached.
	 **/
	virtual PMError getFile( const Pathname & filename ) const = 0;

	/**
	 * Call concrete handler to provide directory content (not recursive!)
	 * below attach point.
	 *
	 * Return E_not_supported_by_media if media does not support retrieval of
	 * directory content.
	 *
	 * Default implementation provided, that returns whether a directory
	 * is located at '_localRoot + dirname'.
	 *
	 * Asserted that media is attached.
	 **/
	virtual PMError getDir( const Pathname & dirname, bool recurse_r ) const = 0;

	/**
	 * Call concrete handler to provide a content list of directory on media
	 * via retlist. If dots is false entries starting with '.' are not reported.
	 *
	 * Return E_not_supported_by_media if media does not support retrieval of
	 * directory content.
	 *
	 * Default implementation provided, that returns the content of a
	 * directory at '_localRoot + dirnname' retrieved via 'readdir'.
	 *
	 * Asserted that media is attached and retlist is empty.
	 **/
        virtual PMError getDirInfo( std::list<std::string> & retlist,
			            const Pathname & dirname, bool dots = true ) const = 0;

	/**
	 * Basically the same as getDirInfo above. The content list is returned as
	 * PathInfo::dircontent, which includes name and filetype of each directory
	 * entry. Retrieving the filetype usg. requires an additional ::stat call for
	 * each entry, thus it's more expensive than a simple readdir.
	 *
	 * Asserted that media is attached and retlist is empty.
	 **/
        virtual PMError getDirInfo( PathInfo::dircontent & retlist,
			            const Pathname & dirname, bool dots = true ) const = 0;

  protected:

        /**
	 * Retrieve and if available scan dirname/directory.yast.
	 *
	 * Asserted that media is attached.
	 **/
        PMError getDirectoryYast( std::list<std::string> & retlist,
			          const Pathname & dirname, bool dots = true ) const;

        /**
	 * Retrieve and if available scan dirname/directory.yast.
	 *
	 * Asserted that media is attached.
	 **/
        PMError getDirectoryYast( PathInfo::dircontent & retlist,
			          const Pathname & dirname, bool dots = true ) const;

  public:

	/**
	 * If the concrete media handler provides a nonempty
	 * attach_point, it must be an existing directory.
	 *
	 * On an empty attach_point, MediaHandler will create
	 * a temporay directory, which will be erased from
	 * destructor.
	 *
	 * On any error, the attach_point is set to an empty Pathname,
	 * which should lead to E_bad_attachpoint.
	 **/
	MediaHandler ( const Url&       url_r,
		       const Pathname & attach_point_r,
		       const Pathname & urlpath_below_attachpoint_r,
		       const bool       does_download_r );

	/**
	 * Contolling MediaAccess takes care, that attached media is released
	 * prior to deleting this.
	 **/
	virtual ~MediaHandler();

    public:

        ///////////////////////////////////////////////////////////////////
        //
        // MediaAccess interface. Does common checks and logging.
        // Invokes real action if necessary.
        //
        ///////////////////////////////////////////////////////////////////

        /**
	 * Protocol hint for MediaAccess.
	 **/
        Url::Protocol protocol() const { return _url.protocol(); }

	/**
	 * Url used.
	 **/
        Url url() const { return _url; }

	/**
	 * Use concrete handler to attach the media.
	 *
	 * @param next try next available device in turn until end of device
	 * list is reached (for media which are accessible through multiple
	 * devices like cdroms).
	 **/
	PMError attach(bool next);

	/**
	 * True if media is attached.
	 **/
	bool isAttached() const { return _isAttached; }

	/**
	 * Return the local directory that corresponds to medias url,
	 * no matter if media isAttached or not. Files requested will
	 * be available at 'localRoot() + filename' or better
	 * 'localPath( filename )'.
	 *
	 * Returns empty pathname if E_bad_attachpoint
	 **/
	const Pathname & localRoot() const { return _localRoot; }

	/**
	 * Files provided will be available at 'localPath(filename)'.
	 *
	 * Returns empty pathname if E_bad_attachpoint
	 **/
         Pathname localPath( const Pathname & pathname ) const;

        /**
	 * Use concrete handler to isconnect media.
	 *
	 * This is useful for media which e.g. holds open a connection to a
	 * server like FTP. After calling disconnect() the media object still is
	 * valid and files are present.
	 *
	 * After calling disconnect() it's not possible to call provideFile() or
	 * provideDir() anymore.
	 **/
        PMError disconnect();

	/**
	 * Use concrete handler to release the media.
	 * @param eject if true, physically eject the media * (i.e. CD-ROM)
	 **/
	PMError release( bool eject = false );

	/**
	 * Use concrete handler to provide file denoted by path below
	 * 'localRoot'. Filename is interpreted relative to the
	 * attached url and a path prefix is preserved.
	 **/
	PMError provideFile( Pathname filename ) const;

	/**
	 * Use concrete handler to provide directory denoted
	 * by path below 'localRoot' (not recursive!).
	 * dirname is interpreted relative to the
	 * attached url and a path prefix is preserved.
	 **/
	PMError provideDir( Pathname dirname ) const;

	/**
	 * Use concrete handler to provide directory tree denoted
	 * by path below 'localRoot' (recursive!!).
	 * dirname is interpreted relative to the
	 * attached url and a path prefix is preserved.
	 **/
	PMError provideDirTree( Pathname dirname ) const;

	/**
	 * Remove filename below localRoot IFF handler downloads files
	 * to the local filesystem. Never remove anything from media.
	 **/
	PMError releaseFile( const Pathname & filename ) const { return releasePath( filename ); }

	/**
	 * Remove directory tree below localRoot IFF handler downloads files
	 * to the local filesystem. Never remove anything from media.
	 **/
	PMError releaseDir( const Pathname & dirname ) const { return releasePath( dirname ); }

	/**
	 * Remove pathname below localRoot IFF handler downloads files
	 * to the local filesystem. Never remove anything from media.
	 *
	 * If pathname denotes a directory it is recursively removed.
	 * If pathname is empty or '/' everything below the localRoot
	 * is recursively removed.
	 * If pathname denotes a file it is unlinked.
	 **/
	PMError releasePath( Pathname pathname ) const;

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
};

///////////////////////////////////////////////////////////////////

#define	MEDIA_HANDLER_API						\
    protected:								\
	virtual PMError attachTo (bool next = false);			\
	virtual PMError releaseFrom( bool eject );			\
	virtual PMError getFile( const Pathname & filename ) const;	\
	virtual PMError getDir( const Pathname & dirname, bool recurse_r ) const;	\
        virtual PMError getDirInfo( std::list<std::string> & retlist,	\
			            const Pathname & dirname, bool dots = true ) const;	\
        virtual PMError getDirInfo( PathInfo::dircontent & retlist,	\
			            const Pathname & dirname, bool dots = true ) const;


#endif // MediaHandler_h

