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
#include <iosfwd>

#include <y2pm/PMSolvablePtr.h>
#include <y2pm/PkgSet.h>


/**
 * compute Installation order.<br>
 *
 * There are two Interfaces:<br>
 * - getTopSorted: return flat list of packages in proper order<br>
 * - computeNextSet: return only packages without requirements, see comment below<br>
 * */
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
	    int order; // number of incoming edges in reverse graph

	    PMSolvablePtr solvable;

	    NodeInfo() : begintime(0), endtime(0), visited(false), order(0) {}
	    NodeInfo(PMSolvablePtr ptr) : begintime(0), endtime(0), visited(false), order(0), solvable(ptr) {}
	};
	
	typedef std::map<constPMSolvablePtr,NodeInfo> Nodes;

	Nodes _nodes;

	unsigned _rdfstime;

	SolvableList _topsorted;

	bool _dirty;

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
	 * Compute a list of Solvables which have no requirements and can be
	 * installed in parallel without conflicts. Use setInstalled to make
	 * computation of a different set possible */
	SolvableList computeNextSet();

	/**
	 * set a Solvable as installed, computeNextSet is able to compute a new
	 * set then
	 * */
	void setInstalled( constPMSolvablePtr ptr );
	
	/**
	 * like above, for convenience
	 * */
	void setInstalled( const SolvableList& list );


	/**
	 * recoursive depth first search, build internal trees
	 * */
	void startrdfs();

	/**
	 * Initialize data structures. Must be called before any other
	 * function.
	 * */
	void init() { startrdfs(); }

	/**
	 * compute topological sorted list
	 *
	 * @return list of solvables in an installable order
	 * */
	const SolvableList& getTopSorted() const;

	const void printAdj(std::ostream& os, bool reversed = false) const;
};
