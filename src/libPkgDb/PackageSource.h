#ifndef _PackageSource_h
#define _PackageSource_h

//#include <DistTag.h>
#include <hash.h>
#include <RefObject.h>
#include <PkgName.h>
#include <Exception.h>

class MediaAccess;

// todo
#define PkgIdent PkgNameEd

extern hash<string,MediaAccess*> media_access_handlers;

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
class Url
{
    private:
//	Url(const Url&);
	MediaAccess* _access;
	string _path;
    public:
	Url(const string& url);
	MediaAccess* getAccess();
	const string& getPath() const;
};

class PackageInfo
{
    public:
	RefObject<class Package> _package;
	typedef RefObject<PackageInfo> Ref;
	Size _size;
	string _label;
	string _rpmgroup;

	PackageInfo(){};
};

class CommonPkdPackageInfo : public PackageInfo
{
    public:
	typedef RefObject<CommonPkdPackageInfo> Ref;
	int offset;
	CommonPkdPackageInfo() : PackageInfo()
	{
	    offset = -1;
	};
};

// Base class for all data providers
class PackageDataProvider
{
    protected:
	Url _url;

    public:
	typedef RefObject<PackageDataProvider> Ref;

	PackageDataProvider(const PackageDataProvider&); // no copy

    public:
	PackageDataProvider(const Url& url) : _url(url) {};
	virtual ~PackageDataProvider() {};

	// create Package objects for all known Packages on this source
	virtual void addAllPackages() = 0;

	// access functions
//	virtual Size getSize(const PkgIdent& id) = 0;
};

class SuSEClassicDataProvider : public PackageDataProvider
{
    private:
//	hash<PkgIdent,CommonPkdPackageInfo::Ref> _packages;
	void CommonPkdLookup(unsigned offset) { /* TODO */ } ;
	void ParseCommonPkd();

	static const char* const _commonpkd="/suse/setup/descr/common.pkd";

    public:

	SuSEClassicDataProvider(const Url& url):PackageDataProvider(url) {};
	~SuSEClassicDataProvider() {};
	
	void addAllPackages()
	{
	    ParseCommonPkd();
	};

	    /*
	Size getSize(const PkgIdent& id)
	{
	    CommonPkdPackageInfo::Ref info(_packages[id]);
	    if (info.null())
	    {
		throw PkgDbExcp("package not known to this source");
	    }
	    else if(info->_size == 0)
	    {
		CommonPkdLookup(info->offset);
	    }
	    return info->_size;
	    return 12;
	};
	    */
};

// class for transparent handling of files on various media
class MediaAccess
{
    private:
	MediaAccess(const MediaAccess&);
    protected:
	string _urlprefix;
    public:
	MediaAccess(string urlprefix = "file");
	virtual ~MediaAccess();
	virtual string getFile(const string& file, bool purge = false)=0;
};

class MediaAccess_File : public MediaAccess
{
    public:
	MediaAccess_File();
	~MediaAccess_File();
	// purge means remember the file and delete it on destruction of the
	// object, e.g for temporary ftp downloads
	string getFile(const string& file, bool purge=false);
};

#endif

// vim:sw=4
