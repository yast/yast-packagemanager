#include <y2pm/MediaAccess.h>
#include <y2util/Y2SLog.h>

#include <iostream>

using namespace std;

int main(void)
{
	MediaAccessPtr m(new MediaAccess());
	
	unsigned result;
	//result = m->open("ftp:hewitt:/suse-STABLE");
	result = m->open("ftp:hewitt:/suse-STABLE");
	DBG << result << endl;
	result = m->attachTo("/tmp");
	DBG << result << endl;
	result = m->provideFile("/apt/SuSE/STABLE-i386/examples/sources.list.FTP");
	DBG << result << endl;
}
