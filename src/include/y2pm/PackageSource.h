#ifndef _PackageSource_h
#define _PackageSource_h

//#include <DistTag.h>
#include <string>
#include <y2util/hash.h>
#include <y2util/RefObject.h>
#include <y2pm/PkgName.h>
#include <y2pm/PkgDb.h>

class PkgDbRep;
typedef class RefObject<PkgDbRep> PkgDb;

// todo
#define PkgIdent PkgNameEd

typedef size_t Size;

/*
class Size
{
    private:
	size_t _size;
    public:
	Size() : _size(0) {};
	Size( size_t s ) : _size(s) {};
	Size( const Size& s ) : _size(s._size) {};
	size_t value() { return _size; };
};
*/

//****************** move to own header

// class for transparent handling of files on various media
class MediaAccessRep
{
    private:
	MediaAccessRep(const MediaAccessRep&);
    protected:
	std::string _urlprefix;
    public:

	MediaAccessRep(std::string urlprefix = "file");
	virtual ~MediaAccessRep();
	virtual std::string getFile(const std::string& file, bool purge = false)=0;
};

typedef RefObject<MediaAccessRep> MediaAccess;

class MediaAccess_FileRep : public MediaAccessRep
{
    public:

	MediaAccess_FileRep();
	~MediaAccess_FileRep();
	// purge means remember the file and delete it on destruction of the
	// object, e.g for temporary ftp downloads
	std::string getFile(const std::string& file, bool purge=false);
};

typedef DerRefObject<MediaAccess_FileRep,MediaAccessRep> MediaAccess_File;

extern hash<std::string,MediaAccess> media_access_handlers;

// *********************************


class Url
{
    private:
//	Url(const Url&);
	MediaAccess _access;
	std::string _path;
    public:
	Url(const std::string& url);
	MediaAccess getAccess();
	const std::string& getPath() const;
};

// Base class for all data providers
class PackageDataProvider
{
    public:
	typedef RefObject<PackageDataProvider> Ref;

	PackageDataProvider(const PackageDataProvider&); // no copy

	enum stringAttributes { SIZE, BUILDTIME, LABEL, LICENSE};
    protected:
	PkgDb _pool;

    public:
	PackageDataProvider(PkgDb pool) : _pool(pool) {};
	virtual ~PackageDataProvider() {};

	// create Package objects for all known Packages on this source
	virtual void addAllPackages() = 0;

	// access functions
//	virtual Size getSize(const PkgIdent& id) = 0;

	virtual std::string getStringAttribute( stringAttributes attr ) = 0;
};

class SuSEClassicDataProvider : public PackageDataProvider
{
    protected:
	Url _url;

    private:
	void CommonPkdLookup(unsigned offset) { /* TODO */ } ;
	void ParseCommonPkd();

	static const char* const _commonpkd="/suse/setup/descr/common.pkd";

    public:

	SuSEClassicDataProvider(PkgDb pool, const Url& url)
	    : PackageDataProvider(pool), _url(url) {};
	~SuSEClassicDataProvider() {};
	
	void addAllPackages()
	{
	    ParseCommonPkd();
	};
	
	std::string getStringAttribute( stringAttributes attr )
	{
	    return "nix";
	}

};

#endif

// vim:sw=4
