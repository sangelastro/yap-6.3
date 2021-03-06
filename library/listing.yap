/*************************************************************************
*									 *
*	 YAP Prolog 							 *
*									 *
*	Yap Prolog was developed at NCCUP - Universidade do Porto	 *
*									 *
* Copyright L.Damas, V.S.Costa and Universidade do Porto 1985-1997	 *
*									 *
**************************************************************************
*									 *
* File:		listing.yap						 *
* Last rev:								 *
* mods:									 *
* comments:	listing a prolog program				 *
*									 *
*************************************************************************/

/**
 * @file   listing.yap
 * @author VITOR SANTOS COSTA <vsc@VITORs-MBP.lan>
 * @date   Tue Nov 17 22:03:59 2015
 * 
 * @brief  Emulate SWI Prolog's listing.
 * 
 * 
*/
:- module(swi_listing,
	[ listing/0,
	  listing/1,
	  portray_clause/1,		% +Clause
	  portray_clause/2,		% +Stream, +Clause
	  portray_clause/3		% +Stream, +Clause, +Options
	]).



/* 

 * @defgroup swi_listing SWI Prolog listing emulation
 * @ingroup library

 emulates listing.pl, but just the interface for now.

*/


:- meta_predicate portray_clause( +, + , : ).

portray_clause(Stream, Term, M:Options) :-
    portray_clause( Stream, Term ).
