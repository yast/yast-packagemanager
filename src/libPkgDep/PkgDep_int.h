#ifndef _PkgDep_int_h
#define _PkgDep_int_h
/*
#ifdef PKGDEP_DEBUG
#define DBG(expr)	do { std::cout << expr; } while(0)
#else
#define DBG(expr)
#endif
*/
#define i_for(iter_type,iter_pfx,iter,base,base_pfx)					\
	for( iter_type iter_pfx##iterator iter = base base_pfx##begin();	\
	     iter != base base_pfx##end();						\
		 ++iter )

#define ci_for(iter_type,iter_pfx,iter,base,base_pfx)							\
	for( iter_type iter_pfx##const_iterator iter = base base_pfx##begin();	\
	     iter != base base_pfx##end();								\
		 ++iter )

#define RevRel_for(revrellist,var)										\
	const PkgSet::RevRelList_type& __revrel_list##var = (revrellist);	\
	ci_for( PkgSet::,RevRelList_, var, __revrel_list##var., )


#endif  /* _PkgDep_int_h */


// Local Variables:
// tab-width: 4
// End:
