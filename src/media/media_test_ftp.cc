#include <y2pm/MediaAccess.h>
#include <y2util/Y2SLog.h>

#include <iostream>

using namespace std;

int main(void)
{
	MediaAccessPtr m(new MediaAccess());
	
	unsigned result;
	result = m->open("ftp:hewitt:/suse-STABLE");
	DBG << result << endl;
	result = m->attachTo("/tmp");
	DBG << result << endl;
	result = m->provideFile("mount--bind");
	DBG << result << endl;
}
