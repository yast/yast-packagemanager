/*
   test_updateinf.cc

*/

#include <iostream>
#include <y2pm/UpdateInfParser.h>

int
main (int argc, char **argv)
{
    UpdateInfParser parser;

    if (parser.fromPath (Pathname ("/var/lib/YaST/update.inf")))
    {
	std::cerr << "Failed" << std::endl;
	return 1;
    }
    std::cout << "Basesystem " << parser.basesystem() << std::endl;
    std::cout << "Distname " << parser.distname() << std::endl;
    std::cout << "Distversion " << parser.distversion() << std::endl;
    std::cout << "Distrelease " << parser.distrelease() << std::endl;
    std::cout << "FTP Patches " << parser.ftppatch() << std::endl;
    std::list<std::string> sources = parser.ftpsources();
    std::cout << "FTP Sources:" << std::endl;
    for (std::list<std::string>::iterator it = sources.begin();
	 it != sources.end(); ++it)
    {
	std::cout << *it << std::endl;
    }

    return 0;
}
