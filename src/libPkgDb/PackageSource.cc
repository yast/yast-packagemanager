#include <fstream>
#include <sstream>

#include <y2pm/PkgEdition.h>
#include <y2pm/Package.h>
#include <y2pm/PkgDb.h>
#include <y2pm/PackageSource.h>

#include <Y2UTIL.h>
#include <y2util/Pathname.h>
#include <y2util/timeclass.h>

#include <Exception.h>

using namespace std;

hash<string,MediaAccess> media_access_handlers;

#define startswith(Str,With) Str.substr(0,sizeof(With)-1)==With

Url::Url(const string& url)
{
    MediaAccess access;
    string access_str;
    string::size_type pos = url.find("://");
    access_str = url.substr(0, pos);
    if(access_str.length()==0)
	access_str="file";
    access = media_access_handlers[access_str];

    if(access==NULL)
	handled=false;
    else
	handled=true;

    _access = access;
    _path = url.substr(pos+3);
    cerr << _path << endl;
}

const string& Url::getPath() const
{
    return _path;
}

MediaAccess Url::getAccess()
{
    return _access;
}

void SuSEClassicDataProvider::ParseCommonPkd()
{
    Pathname pathtocommonpkd(Pathname::cat(_url.getPath(),_commonpkd));
    string pkdpath(_url.getAccess()->getFile(pathtocommonpkd.asString()));
    debug("trying to open " << pkdpath);
    std::ifstream commonpkd(pkdpath.c_str());
    if (!commonpkd)
    {
	throw PkgDbExcp("commonpkd not found");
    }
    
    time_t starttime;
    std::string line;
    int i=0;
    std::string pkgname;
    std::string pkgversion;
    enum deps {REQUIRES,OBSOLETES,CONFLICTS,PROVIDES};
    Package::PkgRelList_type pkgrelations[PROVIDES+1];
    string::size_type relpos;
    string release;
    cerr << "Reading common.pkd" << endl;
    starttime=time(NULL);
    for(;!commonpkd.eof() && !commonpkd.fail();)
    {
	std::getline(commonpkd, line);
	istringstream words(line);
	if(!words) continue;
	string word;
	words >> word;
	if(word[0]=='#') continue;
	if(word=="RpmName:")
	{
		if(pkgname.length()!=0)
		{
//		    cout << "Paket: " << pkgname << "-" << pkgversion << endl;
//		    cout << "Relation: " << *(pkgrelations[REQUIRES].begin()) << endl;
		    PkgName name(pkgname);

		    relpos=pkgversion.rfind("-");
		    if(relpos!=string::npos)
		    {
			release=pkgversion.substr(relpos+1);
		    }
		    else
			release.erase();
		    pkgversion.assign(pkgversion,0,relpos);

		    PkgEdition edi(pkgversion.c_str(),release.length()>0?release.c_str():NULL);

		    _pool->add_package(
			    new Package(
				name,
				edi,
				pkgrelations[REQUIRES],
				pkgrelations[CONFLICTS],
				pkgrelations[PROVIDES],
				pkgrelations[OBSOLETES]
				),
			    this);
		    pkgrelations[REQUIRES].clear();
		    pkgrelations[CONFLICTS].clear();
		    pkgrelations[PROVIDES].clear();
		    pkgrelations[OBSOLETES].clear();
		    pkgname="";
		    pkgversion="";
		    i++;
		}
		words >> pkgname;
	}
	else if(word=="Version:")
	{
	    words>>pkgversion;
	}
	else if(word=="Requires:"||word=="Obsoletes:"||word=="Conflicts:"||word=="Provides:")
	{
	    string token;
	    enum { TOKEN_REQUIREMENT, TOKEN_VERSION } whattoexpect = TOKEN_REQUIREMENT;
	    rel_op op;
	    string ver;
	    string req;
	    deps dep = PROVIDES;
	    if(word=="Requires:") dep=REQUIRES;
	    else if(word=="Obsoletes:") dep=OBSOLETES;
	    else if(word=="Conflicts:") dep=CONFLICTS;
	    else if(word=="Provides:") dep=PROVIDES;
	    while(words >> token)
	    {
		if(token=="=")
		{
		    whattoexpect = TOKEN_VERSION;
		    op=EQ;
		}
		else if(token=="<")
		{
		    whattoexpect = TOKEN_VERSION;
		    op=LT;
		}
		else if(token=="<=")
		{
		    whattoexpect = TOKEN_VERSION;
		    op=LE;
		}
		else if(token==">")
		{
		    whattoexpect = TOKEN_VERSION;
		    op=GT;
		}
		else if(token==">=")
		{
		    whattoexpect = TOKEN_VERSION;
		    op=GE;
		}
		else if(whattoexpect == TOKEN_VERSION)
		{
		    ver=token;
		    whattoexpect = TOKEN_REQUIREMENT;
		}
		else
		{
		    if(req.length()!=0)
		    {
			relpos=ver.rfind("-");
			if(relpos!=string::npos)
			{
			    release=ver.substr(relpos+1);
			}
			else
			    release.erase();
			ver.assign(ver,0,relpos);

			PkgEdition ed(ver.c_str(),release.length()>0?release.c_str():NULL);

			pkgrelations[dep].push_front(
			    PkgRelation(
				PkgName(req),
				op,
				ed));
//			    cout << pkgname << " requires " << req << op << ver << " " << release << endl;
		    }
		    whattoexpect = TOKEN_REQUIREMENT;
		    req=token;
		    op=NONE;
		    ver="";
		}
	    }

	    // this is to catch the last one too
	    if(req.length()!=0)
	    {
		relpos=ver.rfind("-");
		if(relpos!=string::npos)
		{
		    release=ver.substr(relpos+1);
		}
		else
		    release.erase();
		release=ver.substr(relpos+1);
		ver.assign(ver,0,relpos);
		PkgEdition ed(ver.c_str(),release.length()>0?release.c_str():NULL);
		pkgrelations[dep].push_front(
		    PkgRelation(
			PkgName(req),
			op,
			ed));
	    }
	    whattoexpect = TOKEN_REQUIREMENT;
	    req=token;
	    op=NONE;
	    ver="";
	}
    }
    cerr << "Read " << i << " Packages in "<< time(NULL) - starttime<<" seconds from common.pkd" << endl;
}

MediaAccessRep::MediaAccessRep(string urlprefix)
{
    _urlprefix = urlprefix;
    media_access_handlers[_urlprefix]=MediaAccess(this);
}
MediaAccessRep::~MediaAccessRep()
{
    media_access_handlers.erase(_urlprefix);
}

MediaAccess_FileRep::MediaAccess_FileRep():MediaAccessRep("file")
{
}

MediaAccess_FileRep::~MediaAccess_FileRep()
{
}

string MediaAccess_FileRep::getFile(const string& file, bool purge)
{
    if(purge)
	cerr << "MediaAccess_FileRep::getFile() -- purge not implemented" << endl;
    return file;
}

// vim:sw=4
