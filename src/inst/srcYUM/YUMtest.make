
CXXFLAGS = -ggdb -Wall -I. -I/usr/include/libxml2 -I/usr/include/YaST2 -l stdc++ -l xml2

# All:	YUMRepomdDataIterator.o YUMRepomdData.o LibXMLHelper.o test-YUMRepomdData.o

All: YUMtest

YUMtest:	YUMtest.o YUMData.o YUMRepomdParser.o YUMPrimaryParser.o YUMGroupParser.o YUMFileListParser.o YUMOtherParser.o LibXMLHelper.o
			gcc -ggdb -Wall -I/usr/include/libxml2 -l stdc++ -l xml2 -l y2util YUMtest.o YUMData.o YUMRepomdParser.o YUMPrimaryParser.o YUMGroupParser.o YUMFileListParser.o YUMOtherParser.o LibXMLHelper.o -o YUMtest


YUMtest.o:	YUMtest.cc YUMData.h YUMRepomdParser.h YUMPrimaryParser.h YUMGroupParser.h YUMFileListParser.h YUMOtherParser.h

YUMData.o:	YUMData.cc YUMData.h

YUMRepomdParser.o:	YUMRepomdParser.cc YUMRepomdParser.h YUMData.h LibXMLHelper.h

YUMPrimaryParser.o:	YUMPrimaryParser.cc YUMPrimaryParser.h YUMData.h  LibXMLHelper.h

YUMGroupParser.o:	YUMGroupParser.cc YUMGroupParser.h YUMData.h  LibXMLHelper.h

YUMFileListParser.o:	YUMFileListParser.cc YUMFileListParser.h YUMData.h LibXMLHelper.h

YUMOtherParser.o:	YUMOtherParser.cc YUMOtherParser.h YUMData.h LibXMLHelper.h

LibXMLHelper.o:		LibXMLHelper.cc LibXMLHelper.h

