/*---------------------------------------------------------------------\
|                                                                      |
|                      __   __    ____ _____ ____                      |
|                      \ \ / /_ _/ ___|_   _|___ \                     |
|                       \ V / _` \___ \ | |   __) |                    |
|                        | | (_| |___) || |  / __/                     |
|                        |_|\__,_|____/ |_| |_____|                    |
|                                                                      |
|                            Package Management                        |
|                                                     (C) 2002 SuSE AG |
\----------------------------------------------------------------------/

   File:       InstallOrder.h
   Purpose:    Determine order for installing packages
   Author:     Ludwig Nussel <lnussel@suse.de>
   Maintainer: Ludwig Nussel <lnussel@suse.de>

/-*/

#include <map>
#include <list>

#include <y2pm/PMSolvablePtr.h>
#include <y2pm/PkgSet.h>

class InstallOrder
{
    public:
	typedef std::list<constPMSolvablePtr> SolvableList;

    private:
	PkgSet _toinstall;

	/** adjacency list type */
	typedef std::map<constPMSolvablePtr,SolvableList> Graph;

	/** adjacency list, package -> requirements */
	Graph _graph;

	/** reversed graph, package -> referers */
	Graph _rgraph;

	struct NodeInfo
	{
	    unsigned begintime;
	    unsigned endtime;
	    bool visited;
	    unsigned order; // number of incoming edges in reverse graph

	    NodeInfo() : begintime(0), endtime(0), visited(false) {}
	};
	
	typedef std::map<constPMSolvablePtr,NodeInfo> Nodes;

	Nodes _nodes;

	unsigned _rdfstime;

	SolvableList _topsorted;

    private:
	void rdfsvisit(constPMSolvablePtr node);

    public:

	/** 
	 * Constructor
	 *
	 * @param toinstall Set of Solvables that have to be installed
	 * */
	InstallOrder(const PkgSet& toinstall);

	/**
	 * Compute a list of Solvables that can be installed in parallel
	 * without conflicts
	 * */
	SolvableList computeNextSet();

	/**
	 * set a Solvable as installed (i.e. remove it from toinstall)
	 * */
	void setInstalled( constPMSolvablePtr ptr );

	void startrdfs();

	const SolvableList& getTopSorted() const;
};
