#include <y2pm/MediaAccess.h>
#include <y2util/Y2SLog.h>
#include <y2util/Url.h>

#include <iostream>

using namespace std;

int main(int argc, char* argv[])
{
	MediaAccessPtr m(new MediaAccess());

	if(argc<3)
	{
		cerr << "Usage: " << argv[0] << " <base url> <destdir> <path to file on medium>" << endl;
		cerr << endl;
		return 1;
	}

	PMError result;
	//result = m->open("ftp:hewitt:/suse-STABLE");
	Url url(argv[1]);
	result = m->open(url);
	DBG << result << endl;
	result = m->attach(argv[2]);
	DBG << result << endl;
	//result = m->provideFile("/apt/SuSE/STABLE-i386/examples/sources.list.FTP");
	result = m->provideDir(argv[3]);
	DBG << result << endl;
	m->release(true);
}
