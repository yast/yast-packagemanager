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
	
	unsigned result;
	//result = m->open("ftp:hewitt:/suse-STABLE");
	Url url(argv[1]);
	result = m->open(url);
	DBG << media_result_strings[result] << endl;
	result = m->attachTo(argv[2]);
	DBG << media_result_strings[result] << endl;
	//result = m->provideFile("/apt/SuSE/STABLE-i386/examples/sources.list.FTP");
	result = m->provideFile(argv[3]);
	DBG << media_result_strings[result] << endl;
}
