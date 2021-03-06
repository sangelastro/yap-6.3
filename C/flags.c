/*************************************************************************
*									 *
*	 YAP Prolog 							 *
*									 *
*	Yap Prolog was developed at NCCUP - Universidade do Porto	 *
*									 *
* Copyright L.Damas, V.S.Costa and Universidade do Porto 2015-		 *
*									 *
**************************************************************************
*									 *
* File:		flags.c							 *
* Last rev:								 *
* mods:									 *
* comments:	abstract machine definitions				 *
*									 *
*************************************************************************/

/** @file C/flags.c

@ingroup Flags
@{
*/

// this is where  we define flags
#define INIT_FLAGS 1

#include "Yap.h"

static bool ro(Term inp);
static bool nat(Term inp);
static bool isatom(Term inp);
static bool booleanFlag(Term inp);
// static bool string( Term inp );
// static bool  list_atom( Term inp );
static bool list_option(Term inp);
static bool argv(Term inp);
static bool os_argv(Term inp);
static bool agc_threshold(Term inp);
static bool gc_margin(Term inp);
static bool executable(Term inp);
static bool sys_thread_id(Term inp);
static bool mkprompt(Term inp);
static bool synerr(Term inp);
static bool indexer(Term inp);
static bool getenc(Term inp);
static bool typein(Term inp);
static bool dqf(Term t2);

static void newFlag(Term fl, Term val);
static Int current_prolog_flag(USES_REGS1);
static Int set_prolog_flag(USES_REGS1);

#include "Yatom.h"
#include "yapio.h"
#include "eval.h"

#define YAP_FLAG(ID, NAME, WRITABLE, DEF, INIT, HELPER)                        \
  { NAME, WRITABLE, DEF, INIT, HELPER }

#define GZERO_FLAG                                                             \
  { NULL, false, NULL, NULL, NULL }
#define LZERO_FLAG                                                             \
  { NULL, false, NULL, NULL, NULL }

static flag_info global_flags_setup[] = {
#include "YapGFlagInfo.h"
    GZERO_FLAG};

static flag_info local_flags_setup[] = {
#include "YapLFlagInfo.h"
    LZERO_FLAG};

static bool indexer(Term inp) {
  if (inp == TermOff || inp == TermSingle || inp == TermCompact ||
      inp == TermMulti || inp == TermOn || inp == TermMax)
    return true;

  if (IsAtomTerm(inp)) {
    Yap_Error(DOMAIN_ERROR_OUT_OF_RANGE, inp,
              "set_prolog_flag index in {off,single,compact,multi,on,max}");
    return false;
  }
  Yap_Error(TYPE_ERROR_ATOM, inp,
            "set_prolog_flag in {dec10,error,fail,quiet}");
  return false;
}

static bool dqf1(ModEntry *new, Term t2 USES_REGS) {
  new->flags &= ~(DBLQ_CHARS | DBLQ_CODES | DBLQ_ATOM | DBLQ_STRING);
  if (IsAtomTerm(t2)) {
    if (t2 == TermString) {
      new->flags |= DBLQ_STRING;
      return true;
    } else if (t2 == TermAtom) {
      new->flags |= DBLQ_ATOM;
      return true;
    } else if (t2 == TermCodes) {
      new->flags |= DBLQ_CODES;
      return true;
    } else if (t2 == TermChars) {
      new->flags |= DBLQ_CHARS;
      return true;
    }
    /* bad argument, but still an atom */
    Yap_Error(DOMAIN_ERROR_OUT_OF_RANGE, t2, "bad option %s for backquoted "
                                             "string flag, use one string, "
                                             "atom, codes or chars",
              RepAtom(AtomOfTerm(t2))->StrOfAE);
    return false;
  } else {
    Yap_Error(TYPE_ERROR_ATOM, t2, "set_prolog_flag(double_quotes, %s), should "
                                   "be {string,atom,codes,chars}",
              RepAtom(AtomOfTerm(t2))->StrOfAE);
    return false;
  }
}

static bool dqf(Term t2) {
  CACHE_REGS
  ModEntry *new = Yap_GetModuleEntry(CurrentModule);
  return dqf1(new, t2 PASS_REGS);
}

static bool bqf1(ModEntry *new, Term t2 USES_REGS) {
  new->flags &= ~(BCKQ_CHARS | BCKQ_CODES | BCKQ_ATOM | BCKQ_STRING);
  if (IsAtomTerm(t2)) {
    if (t2 == TermString) {
      new->flags |= BCKQ_STRING;
      return true;
    } else if (t2 == TermAtom) {
      new->flags |= BCKQ_ATOM;
      return true;
    } else if (t2 == TermCodes) {
      new->flags |= BCKQ_CODES;
      return true;
    } else if (t2 == TermChars) {
      new->flags |= BCKQ_CHARS;
      return true;
    }
    Yap_Error(DOMAIN_ERROR_OUT_OF_RANGE, t2, "bad option %s for backquoted "
                                             "string flag, use one string, "
                                             "atom, codes or chars",
              RepAtom(AtomOfTerm(t2))->StrOfAE);
    return false;
  } else {
    Yap_Error(TYPE_ERROR_ATOM, t2, "flag  %s is not module-scoped",
              RepAtom(AtomOfTerm(t2))->StrOfAE);
    return false;
  }
}

static bool isaccess(Term inp) {
  if (inp == TermReadWrite || inp == TermReadOnly)
    return true;

  if (IsAtomTerm(inp)) {
    Yap_Error(DOMAIN_ERROR_OUT_OF_RANGE, inp,
              "set_prolog_flag access in {read_write,read_only}");
    return false;
  }
  Yap_Error(TYPE_ERROR_ATOM, inp,
            "set_prolog_flag access in {read_write,read_only}");
  return false;
}

static bool isground(Term inp) { return Yap_IsGroundTerm(inp); }

static bool flagscope(Term inp) {
  if (inp == TermGlobal || inp == TermThread || inp == TermModule)
    return true;

  if (IsAtomTerm(inp)) {
    Yap_Error(DOMAIN_ERROR_OUT_OF_RANGE, inp,
              "set_prolog_flag access in {global,module,thread}");
    return false;
  }
  Yap_Error(TYPE_ERROR_ATOM, inp,
            "set_prolog_flag access in {global,module,thread}");
  return false;
}

static bool mkprompt(Term inp) {
  CACHE_REGS
  if (IsVarTerm(inp)) {
    return Yap_unify(inp, MkAtomTerm(Yap_LookupAtom(LOCAL_Prompt)));
  }
  if (!IsAtomTerm(inp)) {
    Yap_Error(TYPE_ERROR_ATOM, inp, "set_prolog_flag");
    return false;
  }
  strncpy(LOCAL_Prompt, (const char *)RepAtom(AtomOfTerm(inp))->StrOfAE,
          MAX_PROMPT);
  return true;
}

static bool getenc(Term inp) {
  CACHE_REGS
  if (!IsVarTerm(inp) && !IsAtomTerm(inp)) {
    Yap_Error(TYPE_ERROR_ATOM, inp, "get_encoding");
    return false;
  }
     return Yap_unify(inp, MkAtomTerm(Yap_LookupAtom(enc_name(LOCAL_encoding))));
}

/*
static bool enablerl( Term inp ) {
CACHE_REGS
if (IsVarTerm(inp)) {
return Yap_unify( inp, MkAtomTerm( Yap_LookupAtom( enc_name(LOCAL_encoding)
)) );
}
if (!IsAtomTerm(inp) ) {
Yap_Error(TYPE_ERROR_ATOM, inp, "set_prolog_flag");
return false;
}
enc_id( RepAtom( AtomOfTerm( inp ) )->StrOfAE, ENC_OCTET );
return true;
}
*/

static bool typein(Term inp) {
  CACHE_REGS
  if (IsVarTerm(inp)) {
    Term tin = CurrentModule;
    if (tin == PROLOG_MODULE)
      tin = TermProlog;
    return Yap_unify(inp, tin);
  }
  if (!IsAtomTerm(inp)) {
    Yap_Error(TYPE_ERROR_ATOM, inp, "set_prolog_flag");
    return false;
  }
  CurrentModule = inp;
  if (inp == TermProlog)
    CurrentModule = PROLOG_MODULE;
  return true;
}

#if 0

                    static Int p_has_yap_or(USES_REGS1) {
#ifdef YAPOR
                        return (TRUE);
#else
                        return (FALSE);
#endif
                    }

                    static Int p_has_eam(USES_REGS1) {

#ifdef BEAM
                        return (TRUE);
#else
                        return (FALSE);
#endif
                    }

                    static Int p_has_jit(USES_REGS1) {
#ifdef HAS_JIT
                        return (TRUE);
#else
                        return (FALSE);
#endif
                    }

                    static bool tabling( Term inp ) {
                        if (value == 0) { /* default */
                            tab_ent_ptr tab_ent = GLOBAL_root_tab_ent;
                            while (tab_ent) {
                                TabEnt_mode(tab_ent) = TabEnt_flags(tab_ent);
                                tab_ent = TabEnt_next(tab_ent);
                            }
                            yap_flags[TA BLING_MODE_FLAG] = 0;
                        } else if (value == 1) { /* batched */
                            tab_ent_ptr tab_ent = GLOBAL_root_tab_ent;
                            while (tab_ent) {
                                SetMode_Batched(TabEnt_mode(tab_ent));
                                tab_ent = TabEnt_next(tab_ent);
                            }
                            SetMode_Batched(yap_flags[TABLING_MODE_FLAG]);
                        } else if (value == 2) { /* local */
                            tab_ent_ptr tab_ent = GLOBAL_root_tab_ent;
                            while (tab_ent) {
                                SetMode_Local(TabEnt_mode(tab_ent));
                                tab_ent = TabEnt_next(tab_ent);
                            }
                            SetMode_Local(yap_flags[TABLING_MODE_FLAG]);
                        } else if (value == 3) { /* exec_answers */
                            tab_ent_ptr tab_ent = GLOBAL_root_tab_ent;
                            while (tab_ent) {
                                SetMode_ExecAnswers(TabEnt_mode(tab_ent));
                                tab_ent = TabEnt_next(tab_ent);
                            }
                            SetMode_ExecAnswers(yap_flags[TABLING_MODE_FLAG]);
                        } else if (value == 4) { /* load_answers */
                            tab_ent_ptr tab_ent = GLOBAL_root_tab_ent;
                            while (tab_ent) {
                                SetMode_LoadAnswers(TabEnt_mode(tab_ent));
                                tab_ent = TabEnt_next(tab_ent);
                            }
                            SetMode_LoadAnswers(yap_flags[TABLING_MODE_FLAG]);
                        } else if (value == 5) { /* local_trie */
                            tab_ent_ptr tab_ent = GLOBAL_root_tab_ent;
                            while (tab_ent) {
                                SetMode_LocalTrie(TabEnt_mode(tab_ent));
                                tab_ent = TabEnt_next(tab_ent);
                            }
                            SetMode_LocalTrie(yap_flags[TABLING_MODE_FLAG]);
                        } else if (value == 6) { /* global_trie */
                            tab_ent_ptr tab_ent = GLOBAL_root_tab_ent;
                            while (tab_ent) {
                                SetMode_GlobalTrie(TabEnt_mode(tab_ent));
                                tab_ent = TabEnt_next(tab_ent);
                            }
                            SetMode_GlobalTrie(yap_flags[TABLING_MODE_FLAG]);
                        } else if (value == 7) { /* CoInductive */
                            tab_ent_ptr tab_ent = GLOBAL_root_tab_ent;
                            while (tab_ent) {
                                SetMode_CoInductive(TabEnt_mode(tab_ent));
                                tab_ent = TabEnt_next(tab_ent);
                            }
                            SetMode_CoInductive(yap_flags[TABLING_MODE_FLAG]);
                        }
                    }

                    static bool string( Term inp ) {
                        if (IsVarTerm(inp)) {
                            Yap_Error(INSTANTIATION_ERROR, inp, "set_prolog_flag in \"...\"");
                            return false;
                        }
                        if (IsStringTerm( inp ))
                        return true;
                        Term inp0  = inp;
                        if (IsPairTerm(inp)) {
                            Term hd = HeadOfTerm(inp);
                            if (IsAtomTerm(hd)) {
                                do {
                                    Term hd = HeadOfTerm(inp);
                                    if (!IsAtomTerm(hd)) {
                                        Yap_Error(TYPE_ERROR_TEXT, inp0, "set_prolog_flag in \"...\"");
                                        return false;
                                    }
                                } while (IsPairTerm( inp ) );
                            } else if (IsIntTerm(hd)) {
                                do {
                                    Term hd = HeadOfTerm(inp);
                                    if (!IsIntTerm(hd)) {
                                        Yap_Error(TYPE_ERROR_TEXT, inp0, "set_prolog_flag in \"...\"");
                                        return false;
                                    }
                                    if (IntOfTerm(hd) < 0) {
                                        Yap_Error(DOMAIN_ERROR_NOT_LESS_THAN_ZERO, inp0, "set_prolog_flag in 0...");
                                        return false;
                                    }
                                } while (IsPairTerm( inp ) );
                            } else {
                                Yap_Error(TYPE_ERROR_TEXT, inp0, "set_prolog_flag in \"...\"");
                                return false;
                            }
                        }
                        if ( inp != TermNil ) {
                            Yap_Error(TYPE_ERROR_TEXT, inp0, "set_prolog_flag in \"...\"");
                            return false;
                        }
                        return true;
                    }

                    static bool list_atom( Term inp ) {
                        if (IsVarTerm(inp)) {
                            Yap_Error(INSTANTIATION_ERROR, inp, "set_prolog_flag in \"...\"");
                            return false;
                        }
                        Term inp0  = inp;
                        if (IsPairTerm(inp)) {
                            Term hd = HeadOfTerm(inp);
                            do {
                                if (!IsAtomTerm(hd)) {
                                    Yap_Error(TYPE_ERROR_ATOM, inp0, "set_prolog_flag in \"...\"");
                                    return false;
                                }
                            } while (IsPairTerm( inp ) );
                        }
                        if ( inp != TermNil ) {
                            Yap_Error(TYPE_ERROR_LIST, inp0, "set_prolog_flag in [...]");
                            return false;
                        }
                        return true;
                    }
#endif

static bool list_option(Term inp) {
  if (IsVarTerm(inp)) {
    Yap_Error(INSTANTIATION_ERROR, inp, "set_prolog_flag in \"...\"");
    return false;
  }
  Term inp0 = inp;
  if (IsPairTerm(inp)) {
    do {
      Term hd = HeadOfTerm(inp);
      inp = TailOfTerm(inp);
      if (IsAtomTerm(hd)) {
        continue;
      }
      if (IsApplTerm(hd)) {
        Functor f = FunctorOfTerm(hd);
        if (!IsExtensionFunctor(f) && ArityOfFunctor(f) == 1 &&
            Yap_IsGroundTerm(hd)) {
          continue;
        }
        if (!Yap_IsGroundTerm(hd))
          Yap_Error(INSTANTIATION_ERROR, hd, "set_prolog_flag in \"...\"");
        return false;
      }
    } while (IsPairTerm(inp));
    if (inp == TermNil) {
      return true;
    }
    Yap_Error(TYPE_ERROR_LIST, inp0, "set_prolog_flag in [...]");
    return false;
  } else /* lone option */ {
    if (IsAtomTerm(inp)) {
      return true;
    } else if (IsApplTerm(inp)) {
      Functor f = FunctorOfTerm(inp);
      if (!IsExtensionFunctor(f) && ArityOfFunctor(f) == 1 &&
          Yap_IsGroundTerm(ArgOfTerm(1, inp))) {
        return true;
      }
    }
  }
  return false;
}

static bool agc_threshold(Term t) {
  t = Deref(t);
  if (IsVarTerm(t)) {
    CACHE_REGS
    return Yap_unify(t, MkIntegerTerm(GLOBAL_AGcThreshold));
  } else if (!IsIntegerTerm(t)) {
    Yap_Error(TYPE_ERROR_INTEGER, t, "prolog_flag/2 agc_margin");
    return FALSE;
  } else {
    Int i = IntegerOfTerm(t);
    if (i < 0) {
      Yap_Error(DOMAIN_ERROR_NOT_LESS_THAN_ZERO, t, "prolog_flag/2 agc_margin");
      return FALSE;
    } else {
      GLOBAL_AGcThreshold = i;
      return TRUE;
    }
  }
}

static bool gc_margin(Term t) {
  t = Deref(t);
  if (IsVarTerm(t)) {
    return Yap_unify(t, Yap_GetValue(AtomGcMargin));
  } else if (!IsIntegerTerm(t)) {
    Yap_Error(TYPE_ERROR_INTEGER, t, "prolog_flag/2 agc_margin");
    return FALSE;
  } else {
    Int i = IntegerOfTerm(t);
    if (i < 0) {
      Yap_Error(DOMAIN_ERROR_NOT_LESS_THAN_ZERO, t, "prolog_flag/2 gc_margin");
      return FALSE;
    } else {
      CACHE_REGS
      Yap_PutValue(AtomGcMargin, MkIntegerTerm(i));
      return TRUE;
    }
  }
}

static Term mk_argc_list(USES_REGS1) {
  int i = 1;
  Term t = TermNil;
  while (i < GLOBAL_argc) {
    char *arg = GLOBAL_argv[i];
    /* check for -L -- */
    if (arg[0] == '-' && arg[1] == 'L') {
      arg += 2;
      while (*arg != '\0' && (*arg == ' ' || *arg == '\t'))
        arg++;
      if (*arg == '-' && arg[1] == '-' && arg[2] == '\0') {
        /* we found the separator */
        int j;
        for (j = GLOBAL_argc - 1; j > i + 1; --j) {
          t = MkPairTerm(MkAtomTerm(Yap_LookupAtom(GLOBAL_argv[j])), t);
        }
        return t;
      } else if (GLOBAL_argv[i + 1] && GLOBAL_argv[i + 1][0] == '-' &&
                 GLOBAL_argv[i + 1][1] == '-' &&
                 GLOBAL_argv[i + 1][2] == '\0') {
        /* we found the separator */
        int j;
        for (j = GLOBAL_argc - 1; j > i + 2; --j) {
          t = MkPairTerm(MkAtomTerm(Yap_LookupAtom(GLOBAL_argv[j])), t);
        }
        return t;
      }
    }
    if (arg[0] == '-' && arg[1] == '-' && arg[2] == '\0') {
      /* we found the separator */
      int j;
      for (j = GLOBAL_argc - 1; j > i; --j) {
        t = MkPairTerm(MkAtomTerm(Yap_LookupAtom(GLOBAL_argv[j])), t);
      }
      return (t);
    }
    i++;
  }
  return (t);
}

static Term mk_os_argc_list(USES_REGS1) {
  int i = 0;
  Term t = TermNil;
  for (i = 0; i < GLOBAL_argc; i++) {
    char *arg = GLOBAL_argv[i];
    t = MkPairTerm(MkAtomTerm(Yap_LookupAtom(arg)), t);
  }
  return (t);
}

static bool argv(Term inp) {
  CACHE_REGS
  Term t = mk_argc_list(PASS_REGS1);
  if (IsAtomOrIntTerm(t))
    GLOBAL_Flags[ARGV_FLAG].at = t;
  else {
    GLOBAL_Flags[ARGV_FLAG].DBT = Yap_StoreTermInDB(t, 2);
  }
  return false;
}

static bool os_argv(Term inp) {
  CACHE_REGS
  Term t = mk_os_argc_list(PASS_REGS1);
  if (IsAtomOrIntTerm(t))
    GLOBAL_Flags[OS_ARGV_FLAG].at = t;
  else {
    GLOBAL_Flags[OS_ARGV_FLAG].DBT = Yap_StoreTermInDB(t, 2);
  }
  return false;
}

static FlagEntry *
GetFlagProp(Atom a) { /* look property list of atom a for kind  */
  AtomEntry *ae = RepAtom(a);
  FlagEntry *pp;

  READ_LOCK(ae->ARWLock);

  pp = RepFlagProp(ae->PropsOfAE);
  while (!EndOfPAEntr(pp) && pp->KindOfPE != FlagProperty)
    pp = RepFlagProp(pp->NextOfPE);
  READ_UNLOCK(ae->ARWLock);

  return pp;
}

static void initFlag(flag_info *f, int fnum, bool global) {
  Atom name = Yap_LookupAtom(f->name);
  AtomEntry *ae = RepAtom(name);
  WRITE_LOCK(ae->ARWLock);
  FlagEntry *fprop = RepFlagProp(Yap_GetAPropHavingLock(name, FlagProperty));
  if (fprop == NULL) {
    fprop = (FlagEntry *)Yap_AllocAtomSpace(sizeof(FlagEntry));
    if (fprop == NULL) {
      WRITE_UNLOCK(ae->ARWLock);
      Yap_Error(RESOURCE_ERROR_HEAP, TermNil,
                "not enough space for new Flag %s", ae->StrOfAE);
      return;
    }
    fprop->KindOfPE = FlagProperty;
    fprop->FlagOfVE = fnum;
    fprop->rw = f->writable;
    fprop->global = global;
    fprop->type = f->def;
    fprop->helper = f->helper;
    AddPropToAtom(ae, AbsFlagProp(fprop));
  }
  WRITE_UNLOCK(ae->ARWLock);
}

static bool executable(Term inp) {
  CACHE_REGS
    if (GLOBAL_argv && GLOBAL_argv[0]) {
      if (!Yap_AbsoluteFileInBuffer(GLOBAL_argv[0], LOCAL_FileNameBuf, YAP_FILENAME_MAX - 1,true))
	return false;
    } else
    strncpy(LOCAL_FileNameBuf, Yap_FindExecutable(), YAP_FILENAME_MAX - 1);

  return Yap_unify(MkAtomTerm(Yap_LookupAtom(LOCAL_FileNameBuf)), inp);
}

static bool sys_thread_id(Term inp) {
  CACHE_REGS
  int pid;
#ifdef HAVE_GETTID_SYSCALL
  pid = syscall(__NR_gettid);
#elif defined(HAVE_GETTID_MACRO)
  pid = gettid();
#elif defined(__WINDOWS__)
  pid = GetCurrentThreadId();
#else
  pid = 0;
#endif

  return Yap_unify(MkIntegerTerm(pid), inp);
}

static bool setYapFlagInModule(Term tflag, Term t2, Term mod) {
  CACHE_REGS
  FlagEntry *fv;
  ModEntry *me = Yap_GetModuleEntry(mod);
  if (!me)
    return false;
  fv = GetFlagProp(AtomOfTerm(tflag));
  if (!fv && !fv->global) {
    Yap_Error(DOMAIN_ERROR_PROLOG_FLAG, tflag,
              "trying to set unknown module flag");
    return false;
  }

  if (mod == USER_MODULE) {
    flag_term *tarr = GLOBAL_Flags;
    if (!(fv->type(t2)))
      return false;
    if (fv->helper && !(fv->helper(t2)))
      return false;
    Term tout = tarr[fv->FlagOfVE].at;
    if (IsVarTerm(tout))
      Yap_PopTermFromDB(tarr[fv->FlagOfVE].DBT);
    if (IsAtomOrIntTerm(t2))
      tarr[fv->FlagOfVE].at = t2;
    else {
      tarr[fv->FlagOfVE].DBT = Yap_StoreTermInDB(t2, 2);
    }
  }
  // module specific stuff now

  if (fv->FlagOfVE == UNKNOWN_FLAG) {
    me->flags &= ~(UNKNOWN_MASK);
    if (t2 == TermError) {
      me->flags |= (UNKNOWN_ERROR);
      return true;
    } else if (t2 == TermFail) {
      me->flags |= (UNKNOWN_FAIL);
      return true;
    } else if (t2 == TermWarning) {
      me->flags |= (UNKNOWN_WARNING);
      return true;
    } else if (t2 == TermFastFail) {
      me->flags |= (UNKNOWN_FAST_FAIL);
      return true;
    }
    Yap_Error(
        DOMAIN_ERROR_OUT_OF_RANGE, t2,
        "bad option  %s  for unknown flag, use one of error, fail or warning.",
        RepAtom(AtomOfTerm(tflag))->StrOfAE);
    return false;
  } else if (fv->FlagOfVE == DOUBLE_QUOTES_FLAG) {
    return dqf1(me, t2 PASS_REGS);
  } else if (fv->FlagOfVE == CHARACTER_ESCAPES_FLAG) {
    if (t2 == TermTrue) {
      me->flags |= M_CHARESCAPE;
      return true;
    } else if (t2 == TermFalse) {
      me->flags &= ~(M_CHARESCAPE);
      return true;
    }
    Yap_Error(DOMAIN_ERROR_OUT_OF_RANGE, t2,
              "bad option %s for character_escapes flag, use true or false",
              RepAtom(AtomOfTerm(tflag))->StrOfAE);
    return false;
  } else if (fv->FlagOfVE == BACKQUOTED_STRING_FLAG) {
    return bqf1(me, t2 PASS_REGS);
    ;
  }
  // bad key?
  return false;
}

static Term getYapFlagInModule(Term tflag, Term mod) {
  FlagEntry *fv;
  ModEntry *me = Yap_GetModuleEntry(mod);
  if (!mod)
    return false;
  fv = GetFlagProp(AtomOfTerm(tflag));
  if (!fv && !fv->global) {
    Yap_Error(DOMAIN_ERROR_OUT_OF_RANGE, tflag, "trying to set unknown flag");
    return 0L;
  }
  // module specific stuff now

  if (fv->FlagOfVE == UNKNOWN_FLAG) {
    if (me->flags & UNKNOWN_ERROR)
      return TermError;
    if (me->flags & UNKNOWN_WARNING)
      return TermWarning;
    return TermFail;
  } else if (fv->FlagOfVE == CHARACTER_ESCAPES_FLAG) {
    if (me->flags & M_CHARESCAPE)
      return TermTrue;
  } else if (fv->FlagOfVE == BACKQUOTED_STRING_FLAG) {
    if (me->flags & BCKQ_CHARS)
      return TermChars;
    if (me->flags & BCKQ_CODES)
      return TermCodes;
    if (me->flags & BCKQ_ATOM)
      return TermAtom;
    return TermString;
  } else if (fv->FlagOfVE == DOUBLE_QUOTES_FLAG) {
    if (me->flags & DBLQ_CHARS)
      return TermChars;
    if (me->flags & DBLQ_CODES)
      return TermCodes;
    if (me->flags & DBLQ_ATOM)
      return TermAtom;
    return TermString;
  }
  Yap_Error(DOMAIN_ERROR_OUT_OF_RANGE, tflag, "flag  %s is not module-scoped",
            RepAtom(AtomOfTerm(tflag))->StrOfAE);
  return 0L;
}

static Int cont_yap_flag(USES_REGS1) {
  int i = IntOfTerm(EXTRA_CBACK_ARG(2, 1));
  int gmax = GLOBAL_flagCount;
  int lmax = LOCAL_flagCount;
  Term tflag = Deref(ARG1);
  EXTRA_CBACK_ARG(2, 1) = MkIntTerm(i + 1);

  if (IsApplTerm(tflag) && FunctorOfTerm(tflag) == FunctorModule) {
    Term modt = CurrentModule;
    tflag = Yap_StripModule(tflag, &modt);
    while (i != gmax && i != UNKNOWN_FLAG && i != CHARACTER_ESCAPES_FLAG &&
           i != BACKQUOTED_STRING_FLAG)
      i++;
    if (i == gmax)
      cut_fail();
    EXTRA_CBACK_ARG(2, 1) = MkIntTerm(i + 1);
    {
      Term lab = MkAtomTerm(Yap_LookupAtom(global_flags_setup[i].name));
      Term val = Deref(ARG2);

      if (!Yap_unify(tflag, lab))
        return false;
      if (IsVarTerm(val)) {
        Term oval = getYapFlagInModule(lab, modt);
        if (oval == 0)
          return false;
        return Yap_unify(oval, val);
      } else {
        return setYapFlagInModule(tflag, val, modt);
      }
    }
    return false;
  }
  if (i >= gmax) {
    Yap_unify(ARG1,
              MkAtomTerm(Yap_LookupAtom(local_flags_setup[i - gmax].name)));
    if (i == gmax + lmax - 1)
      do_cut(0);
  } else {
    Yap_unify(ARG1, MkAtomTerm(Yap_LookupAtom(global_flags_setup[i].name)));
  }
  Term flag = getYapFlag(Deref(ARG1));
  return Yap_unify(flag, ARG2);
}

static Int yap_flag(USES_REGS1) {
  Term tflag = Deref(ARG1);
  if (IsVarTerm(tflag)) {
    EXTRA_CBACK_ARG(2, 1) = MkIntTerm(0);
    return cont_yap_flag(PASS_REGS1);
  }
  if (IsApplTerm(tflag) && FunctorOfTerm(tflag) == FunctorModule) {
    Term modt;
    tflag = Yap_StripModule(tflag, &modt);
    if (IsVarTerm(tflag)) {
      EXTRA_CBACK_ARG(2, 1) = MkIntTerm(0);
      return cont_yap_flag(PASS_REGS1);
    }
    do_cut(0);

    if (!isatom(tflag))
      return false;
    if (!isatom(modt))
      return false;
    if (IsVarTerm(Deref(ARG2))) {
      Term flag = getYapFlagInModule(tflag, modt);
      if (flag == 0)
        return false;
      return Yap_unify(flag, ARG2);
    } else {
      return setYapFlagInModule(tflag, Deref(ARG2), modt);
    }
  }

  do_cut(0);

  if (IsVarTerm(Deref(ARG2))) {
    Term flag = getYapFlag(Deref(ARG1));
    if (flag == 0)
      return false;
    return Yap_unify(flag, ARG2);
  }
  return set_prolog_flag(PASS_REGS1);
}

static Int cont_prolog_flag(USES_REGS1) {
  int i = IntOfTerm(EXTRA_CBACK_ARG(3, 1));
  while (i < GLOBAL_flagCount + LOCAL_flagCount) {
    int gmax = GLOBAL_flagCount;
    int lmax = LOCAL_flagCount;
    Term flag, f;

    if (i >= gmax + lmax) {
      cut_fail();
    } else if (i >= gmax) {
      Yap_unify(ARG1, (f = MkAtomTerm(
                           Yap_LookupAtom(local_flags_setup[i - gmax].name))));
    } else {
      Yap_unify(ARG1,
                (f = MkAtomTerm(Yap_LookupAtom(global_flags_setup[i].name))));
    }
    EXTRA_CBACK_ARG(3, 1) = MkIntTerm(++i);
    flag = getYapFlag(f);
    if (!Yap_unify(f, ARG2))
      return false;
    return setYapFlag(f, Deref(ARG3));
  }
  cut_fail();
}

/** @pred prolog_flag(? _Flag_,- _OldValue_,+ _NewValue_)

Obtain the value for a YAP Prolog flag and then set it to a new
value. Equivalent to first calling current_prolog_flag/2 with the
second argument  _OldValue_ unbound and then calling
set_prolog_flag/2 with the third argument  _NewValue_.


*/
static Int prolog_flag(USES_REGS1) {
  if (IsVarTerm(Deref(ARG1))) {
    EXTRA_CBACK_ARG(3, 1) = MkIntTerm(0);
    return cont_prolog_flag(PASS_REGS1);
  }
  do_cut(0);
  if (IsVarTerm(Deref(ARG3))) {
    Term flag = getYapFlag(Deref(ARG1));
    if (flag == 0)
      return false;
    return Yap_unify(flag, ARG2);
  }
  return setYapFlag(Deref(ARG1), Deref(ARG3));
}

static Int cont_current_prolog_flag(USES_REGS1) {
  int i = IntOfTerm(EXTRA_CBACK_ARG(2, 1));
  while (i < GLOBAL_flagCount + LOCAL_flagCount) {
    int gmax = GLOBAL_flagCount;
    int lmax = LOCAL_flagCount;
    Term flag, f;

    if (i >= gmax + lmax) {
      cut_fail();
    } else if (i >= gmax) {
      Yap_unify(ARG1, (f = MkAtomTerm(
                           Yap_LookupAtom(local_flags_setup[i - gmax].name))));
    } else {
      Yap_unify(ARG1,
                (f = MkAtomTerm(Yap_LookupAtom(global_flags_setup[i].name))));
    }
    EXTRA_CBACK_ARG(2, 1) = MkIntTerm(++i);
    flag = getYapFlag(f);
    return Yap_unify(flag, ARG2);
  }
  cut_fail();
}

static Int current_prolog_flag(USES_REGS1) {
  if (IsVarTerm(Deref(ARG1))) {
    EXTRA_CBACK_ARG(3, 1) = MkIntTerm(0);
    return cont_current_prolog_flag(PASS_REGS1);
  }
  do_cut(0);
  Term flag = getYapFlag(Deref(ARG1));
  if (flag == 0)
    return false;
  return Yap_unify(flag, ARG2);
}

/** @pred current_prolog_flag(? _Flag_,- _Value_) is iso

Obtain the value for a YAP Prolog flag. Equivalent to calling
yap_flag/2 with the second argument unbound, and unifying the
returned second argument with  _Value_.

*/
static Int current_prolog_flag2(USES_REGS1) {
  Term tflag = Deref(ARG1);
  Term tout = 0;
  FlagEntry *fv;
  flag_term *tarr;

  if (IsVarTerm(tflag)) {
    EXTRA_CBACK_ARG(2, 1) = MkIntTerm(0);
    return cont_yap_flag(PASS_REGS1);
  }
  do_cut(0);
  if (!IsAtomTerm(tflag)) {
    Yap_Error(TYPE_ERROR_ATOM, tflag, "current_prolog_flag/3");
    return (FALSE);
  }
  fv = GetFlagProp(AtomOfTerm(tflag));
  if (!fv) {
    // should itself depend on a flag
    return FALSE;
  }
  if (fv->global)
    tarr = GLOBAL_Flags;
  else
    tarr = LOCAL_Flags;
  tout = tarr[fv->FlagOfVE].at;
  if (IsVarTerm(tout))
    tout = Yap_FetchTermFromDB(tarr[fv->FlagOfVE].DBT);
  return (Yap_unify(ARG2, tout));
}

void Yap_setModuleFlags(ModEntry *new, ModEntry *cme) {
  CACHE_REGS

  Atom at = new->AtomOfME;
  if (at == AtomProlog || CurrentModule == PROLOG_MODULE) {
    new->flags =
        M_SYSTEM | UNKNOWN_ERROR | M_CHARESCAPE | DBLQ_CODES | BCKQ_STRING;
    if (at == AtomUser)
      new->flags = UNKNOWN_ERROR | M_CHARESCAPE | DBLQ_CODES | BCKQ_STRING;
  } else if (cme && cme->flags && cme != new) {
    new->flags = cme->flags;
  } else {
    new->flags = (UNKNOWN_ERROR | M_CHARESCAPE | DBLQ_CODES | BCKQ_STRING);
  }
  // printf("cme=%s new=%s flags=%x\n",cme,at->StrOfAE,new->flags);
}

bool setYapFlag(Term tflag, Term t2) {
  FlagEntry *fv;
  flag_term *tarr;
  if (IsVarTerm(tflag)) {
    Yap_Error(INSTANTIATION_ERROR, tflag, "yap_flag/2");
    return (FALSE);
  }
  if (IsApplTerm(tflag) && FunctorOfTerm(tflag) == FunctorModule) {
    Term modt;
    tflag = Yap_StripModule(tflag, &modt);
    if (!isatom(tflag))
      return false;
    if (!isatom(modt))
      return false;
    return setYapFlagInModule(tflag, t2, modt);
  }
  if (!IsAtomTerm(tflag)) {
    Yap_Error(TYPE_ERROR_ATOM, tflag, "yap_flag/2");
    return (FALSE);
  }
  fv = GetFlagProp(AtomOfTerm(tflag));
  if (!fv) {
    Term fl = GLOBAL_Flags[USER_FLAGS_FLAG].at;
    if (fl == TermSilent) {
      CACHE_REGS
      Term t2 = Deref(ARG2);
      newFlag(tflag, t2);
    } else if (fl == TermWarning) {
      Yap_Warning("Flag %s does not exist", RepAtom(AtomOfTerm(fl))->StrOfAE);
    } else {
      Yap_Error(DOMAIN_ERROR_PROLOG_FLAG, fl, "trying to set unknown flag ~s",
                AtomName(AtomOfTerm(fl)));
    }
    return FALSE;
  }
  if (fv->global) {
    CACHE_REGS
    switch (fv->FlagOfVE) {
    case UNKNOWN_FLAG:
    case CHARACTER_ESCAPES_FLAG:
    case BACKQUOTED_STRING_FLAG:
      return setYapFlagInModule(tflag, t2, CurrentModule);
    default:
      tarr = GLOBAL_Flags;
    }
  } else {
    CACHE_REGS
    tarr = LOCAL_Flags;
  }
  if (!(fv->type(t2)))
    return false;
  if (fv->helper && !(fv->helper(t2)))
    return false;
  Term tout = tarr[fv->FlagOfVE].at;
  if (IsVarTerm(tout))
    Yap_PopTermFromDB(tarr[fv->FlagOfVE].DBT);
  if (IsAtomOrIntTerm(t2))
    tarr[fv->FlagOfVE].at = t2;
  else {
    tarr[fv->FlagOfVE].DBT = Yap_StoreTermInDB(t2, 2);
  }
  return true;
}


Term Yap_UnknownFlag(Term mod) {
     if (mod == PROLOG_MODULE)
       mod = TermProlog;

      ModEntry *fv = Yap_GetModuleEntry(mod);
      if (fv == NULL)
        fv = Yap_GetModuleEntry(TermUser);
      if (fv->flags & UNKNOWN_ERROR)
        return TermError;
      if (fv->flags & UNKNOWN_WARNING)
        return TermWarning;
      return TermFail;
}

Term getYapFlag(Term tflag) {
  FlagEntry *fv;
  flag_term *tarr;
  if (IsVarTerm(tflag)) {
    Yap_Error(INSTANTIATION_ERROR, tflag, "yap_flag/2");
    return (FALSE);
  }
  if (IsApplTerm(tflag) && FunctorOfTerm(tflag) == FunctorModule) {
    Term modt;
    tflag = Yap_StripModule(tflag, &modt);
    if (!isatom(tflag))
      return false;
    if (!isatom(modt))
      return false;
    return getYapFlagInModule(tflag, modt);
  }
  if (!IsAtomTerm(tflag)) {
    Yap_Error(TYPE_ERROR_ATOM, tflag, "yap_flag/2");
    return (FALSE);
  }
  fv = GetFlagProp(AtomOfTerm(tflag));
  if (!fv) {
    Term fl = GLOBAL_Flags[USER_FLAGS_FLAG].at;
    if (fl == TermSilent) {
      return false;
    } else if (fl == TermWarning) {
      Yap_Warning("Flag ~s does not exist", RepAtom(AtomOfTerm(fl))->StrOfAE);
    } else {
      Yap_Error(DOMAIN_ERROR_PROLOG_FLAG, fl, "trying to read unknown flag %s",
                RepAtom(AtomOfTerm(fl))->StrOfAE);
    }
    return false;
  }
  if (fv->global)
    tarr = GLOBAL_Flags;
  else {
    CACHE_REGS
    tarr = LOCAL_Flags;
  }
  Term tout = tarr[fv->FlagOfVE].at;
  if (IsVarTerm(tout))
    return Yap_FetchTermFromDB(tarr[fv->FlagOfVE].DBT);
  else
    return tout;
}

/** @pred set_prolog_flag(+ _Flag_,+ _Value_) is iso

Set the value for YAP Prolog flag `Flag`. Equivalent to
calling yap_flag/2 with both arguments bound.

*/
static Int set_prolog_flag(USES_REGS1) {
  Term tflag = Deref(ARG1), t2 = Deref(ARG2);
  return setYapFlag(tflag, t2);
}

/**   @pred source

After executing this goal, YAP keeps information on the source
of the predicates that will be consulted. This enables the use of
[listing/0](@ref listing), `listing/1` and [clause/2](@ref clause) for those
clauses.

The same as `source_mode(_,on)` or as declaring all newly defined
static procedures as `public`.
*/
static Int source(USES_REGS1) {
  setBooleanGlobalPrologFlag(SOURCE_FLAG, true);
  return true;
}

/** @pred no_source
The opposite to `source`.

The same as `source_mode(_,off)`.

*/
static Int no_source(USES_REGS1) {
  setBooleanGlobalPrologFlag(SOURCE_FLAG, false);
  return true;
}

/**
@pred source_mode(- _O_,+ _N_)

The state of source mode can either be on or off. When the source mode
is on, all clauses are kept both as compiled code and in a "hidden"
database.  _O_ is unified with the previous state and the mode is set
according to  _N_.

*/
static Int source_mode(USES_REGS1) {
  Term targ;
  bool current = trueGlobalPrologFlag(SOURCE_FLAG);
  if (current && !Yap_unify_constant(ARG1, TermTrue))
    return false;
  if (!current && !Yap_unify_constant(ARG1, TermFalse))
    return false;
  targ = Deref(ARG2);
  setYapFlag(TermSource, ARG2);
  return true;
}

static bool setInitialValue(bool bootstrap, flag_func f, const char *s,
                            flag_term *tarr) {
  errno = 0;

  if (f == booleanFlag) {
    if (!bootstrap) {
      return 0;
    }
    if (!strcmp(s, "true")) {
      tarr->at = TermTrue;
      return true;
    }
    if (!strcmp(s, "false")) {
      tarr->at = TermFalse;
      return true;
    }
    if (!strcmp(s, "on")) {
      tarr->at = TermTrue;
      return true;
    }
    if (!strcmp(s, "off")) {
      tarr->at = TermFalse;
      return true;
    }
    Yap_Error(DOMAIN_ERROR_OUT_OF_RANGE, TermNil,
              "~s should be either true (on) or false (off)", s);
    return false;
  } else if (f == nat) {
    if (!bootstrap) {
      return 0;
    }
    UInt r = strtoul(s, NULL, 10);
    Term t;
    if (errno) {
      Yap_Error(DOMAIN_ERROR_OUT_OF_RANGE, TermNil,
                "~s should be a positive integer)", s);
      return false;
    }
    CACHE_REGS
    t = MkIntegerTerm(r);
    if (IsIntTerm(t))
      tarr->at = t;
    else {
      tarr->DBT = Yap_StoreTermInDB(t, 2);
    }
    return true;
  } else if (f == at2n) {
    if (!bootstrap) {
      return 0;
    }
    if (!strcmp(s, "INT_MAX")) {
      tarr->at = MkIntTerm(Int_MAX);
      return true;
    }
    if (!strcmp(s, "MAX_THREADS")) {
      tarr->at = MkIntTerm(MAX_THREADS);
      return true;
    }
    if (!strcmp(s, "MAX_WORKERS")) {
      tarr->at = MkIntTerm(MAX_WORKERS);
      return true;
    }
    if (!strcmp(s, "INT_MIN")) {
      tarr->at = MkIntTerm(Int_MIN);
      return true;
    }
    if (!strcmp(s, "YAP_NUMERIC_VERSION")) {
      tarr->at = MkIntTerm(atol(YAP_NUMERIC_VERSION));
      return true;
    }
    if (!strcmp(s, "YAP_NUMERIC_VERSION")) {
      tarr->at = MkIntTerm(atol(YAP_NUMERIC_VERSION));
      return true;
    }
    Yap_Error(DOMAIN_ERROR_OUT_OF_RANGE, TermNil,
              "~s should be either true (on) or false (off)", s);
    return false;
  } else if (f == isatom) {
    if (!bootstrap) {
      return false;
    }
    Atom r = Yap_LookupAtom(s);
    if (errno) {
      Yap_Error(DOMAIN_ERROR_OUT_OF_RANGE, TermNil,
                "~s should be a positive integer)", s);
      tarr->at = TermNil;
    }
    tarr->at = MkAtomTerm(r);
    return true;
  } else if (f == options) {
    CACHE_REGS
    char tmp[512];
    Term t0;
    if (bootstrap) {
      return false;
    }
    t0 = AbsPair(HR);
    while (true) {
      int i = 0, ch = s[0];
      while (ch != '\0' && ch != ';') {
        if (ch != ' ')
          tmp[i++] = ch;
        s++;
        ch = *s;
      }
      tmp[i] = '\0';
      HR += 2;
      HR[-2] = MkAtomTerm(Yap_LookupAtom(tmp));
      if (ch) {
        HR[-1] = AbsPair(HR);
        s++;
        continue;
      } else {
        HR[-1] = TermNil;
        tarr->DBT = Yap_StoreTermInDB(t0, 2);
        return true;
      }
    }
  } else {
    Term t0;
    if (bootstrap) {
      return false;
    }
    CACHE_REGS
    t0 = Yap_StringToTerm(s, strlen(s) + 1, &LOCAL_encoding, GLOBAL_MaxPriority,
                          NULL);
    if (!t0)
      return false;
    if (IsAtomTerm(t0) || IsIntTerm(t0)) {
      // do yourself flags
      if (t0 == MkAtomTerm(AtomQuery)) {
        f(TermNil);
      } else {
        tarr->at = t0;
      }
    } else {
      tarr->DBT = Yap_StoreTermInDB(t0, 2);
    }
    return true;
  }
}

#define PROLOG_FLAG_PROPERTY_DEFS()                                            \
  PAR("access", isaccess, PROLOG_FLAG_PROPERTY_ACCESS, "read_write"),          \
      PAR("type", isground, PROLOG_FLAG_PROPERTY_TYPE, "term"),                \
      PAR("scope", flagscope, PROLOG_FLAG_PROPERTY_SCOPE, "global"),           \
      PAR("keep", booleanFlag, PROLOG_FLAG_PROPERTY_KEEP, "false"),                \
      PAR(NULL, ok, PROLOG_FLAG_PROPERTY_END, 0)

#define PAR(x, y, z, w) z

typedef enum prolog_flag_property_enum_choices {
  PROLOG_FLAG_PROPERTY_DEFS()
} prolog_flag_property_choices_t;

#undef PAR

#define PAR(x, y, z, w)                                                        \
  { x, y, z, w }

static const param2_t prolog_flag_property_defs[] = {
    PROLOG_FLAG_PROPERTY_DEFS()};
#undef PAR

static Int
do_prolog_flag_property(Term tflag,
                        Term opts USES_REGS) { /* Init current_prolog_flag */
  FlagEntry *fv;
  xarg *args;
  prolog_flag_property_choices_t i;
  bool rc = true;
  args = Yap_ArgList2ToVector(opts, prolog_flag_property_defs,
                              PROLOG_FLAG_PROPERTY_END);
  if (args == NULL) {
    Yap_Error( LOCAL_Error_TYPE, LOCAL_Error_Term, NULL );
    return false;
  }
  if (!IsAtomTerm(tflag)) {
    if (IsApplTerm(tflag) && FunctorOfTerm(tflag) == FunctorModule) {
      Term modt = CurrentModule;
      tflag = Yap_YapStripModule(tflag, &modt);
    } else {
      Yap_Error(TYPE_ERROR_ATOM, tflag, "yap_flag/2");
      return (FALSE);
    }
  }
  fv = GetFlagProp(AtomOfTerm(tflag));
  for (i = 0; i < PROLOG_FLAG_PROPERTY_END; i++) {
    if (args[i].used) {
      switch (i) {
      case PROLOG_FLAG_PROPERTY_ACCESS:
        if (fv->rw)
          rc = rc && Yap_unify(TermReadWrite,
                               args[PROLOG_FLAG_PROPERTY_ACCESS].tvalue);
        else
          rc = rc && Yap_unify(TermReadOnly,
                               args[PROLOG_FLAG_PROPERTY_ACCESS].tvalue);
        break;
      case PROLOG_FLAG_PROPERTY_TYPE:
        if (fv->type == booleanFlag)
          rc = rc &&
               Yap_unify(TermBoolean, args[PROLOG_FLAG_PROPERTY_TYPE].tvalue);
        else if (fv->type == isatom)
          rc =
              rc && Yap_unify(TermAtom, args[PROLOG_FLAG_PROPERTY_TYPE].tvalue);
        else if (fv->type == nat)
          rc = rc &&

               Yap_unify(TermInteger, args[PROLOG_FLAG_PROPERTY_TYPE].tvalue);
        else if (fv->type == isfloat)
          rc = rc &&
               Yap_unify(TermFloat, args[PROLOG_FLAG_PROPERTY_TYPE].tvalue);
        else
          rc =
              rc && Yap_unify(TermTerm, args[PROLOG_FLAG_PROPERTY_TYPE].tvalue);
        break;
      case PROLOG_FLAG_PROPERTY_KEEP:
        rc = rc && false;
        break;
      case PROLOG_FLAG_PROPERTY_SCOPE:
        if (fv->global) {
          if (fv->FlagOfVE == UNKNOWN_FLAG ||
              fv->FlagOfVE == CHARACTER_ESCAPES_FLAG ||
              fv->FlagOfVE == BACKQUOTED_STRING_FLAG)
            Yap_unify(TermModule, args[PROLOG_FLAG_PROPERTY_SCOPE].tvalue);
          rc = rc &&
               Yap_unify(TermGlobal, args[PROLOG_FLAG_PROPERTY_SCOPE].tvalue);
        } else
          rc = rc &&
               Yap_unify(TermThread, args[PROLOG_FLAG_PROPERTY_SCOPE].tvalue);
        break;
      case PROLOG_FLAG_PROPERTY_END:
        /* break; */
        Yap_Error(DOMAIN_ERROR_PROLOG_FLAG, opts, "Flag not supported by YAP");
      }
    }
  }
  // UNLOCK(GLOBAL_Prolog_Flag[sno].prolog_flaglock);
  return rc;
}

static Int cont_prolog_flag_property(USES_REGS1) { /* current_prolog_flag */
  int i = IntOfTerm(EXTRA_CBACK_ARG(2, 1));

  while (i < GLOBAL_flagCount + LOCAL_flagCount) {
    int gmax = GLOBAL_flagCount;
    int lmax = LOCAL_flagCount;
    Term lab;

    if (i >= gmax + lmax) {
      cut_fail();
    } else if (i >= gmax) {
      lab = MkAtomTerm(Yap_LookupAtom(local_flags_setup[i - gmax].name));
    } else {
      if (i == UNKNOWN_FLAG || i == CHARACTER_ESCAPES_FLAG ||
          i == BACKQUOTED_STRING_FLAG) {
        Term labs[2];
        labs[0] = MkVarTerm();
        labs[1] = MkAtomTerm(Yap_LookupAtom(global_flags_setup[i].name));
        lab = Yap_MkApplTerm(FunctorModule, 2, labs);
      } else {
        lab = MkAtomTerm(Yap_LookupAtom(global_flags_setup[i].name));
      }
    }
    EXTRA_CBACK_ARG(2, 1) = MkIntTerm(++i);
    Yap_unify(ARG1, lab);
    return do_prolog_flag_property(lab, Deref(ARG2) PASS_REGS);
  }
  cut_fail();
}

/** @pred prolog_flag_property(+ _Flag_,+ _Prooperties_)

Report a property for a YAP Prolog flag.  _Properties_ include

* `type(+_Type_)` with _Type_ one of `boolean`, `integer`, `float`, `atom`
and `term` (that is, any ground term)

* `access(+_Access_)` with  _Access_ one of `read_only` or `read_write`

* `scope(+_Scope_) the flag aplies to a `thread`, to a `module`, or is
`global` to the system.

*/
static Int prolog_flag_property(USES_REGS1) { /* Init current_prolog_flag */
  Term t1 = Deref(ARG1);
  /* make valgrind happy by always filling in memory */
  EXTRA_CBACK_ARG(2, 1) = MkIntTerm(0);
  if (IsVarTerm(t1)) {
    return (cont_prolog_flag_property(PASS_REGS1));
  } else {
    if (IsApplTerm(t1) && FunctorOfTerm(t1) == FunctorModule) {
      Term modt;
      t1 = Yap_StripModule(t1, &modt);
      if (IsAtomTerm(modt)) {
        Int rc;
        rc = cont_prolog_flag_property(PASS_REGS1);

        return rc;
      }
    } else if (IsAtomTerm(t1)) {
      do_cut(0);
      return do_prolog_flag_property(t1, Deref(ARG2) PASS_REGS);
    } else {
      Yap_Error(TYPE_ERROR_ATOM, t1, "prolog_flag_property/2");
    }
  }
  return false;
}

static void newFlag(Term fl, Term val) {
  flag_info f;
  int i = GLOBAL_flagCount;

  GLOBAL_flagCount++;
  f.name = (char *)RepAtom(AtomOfTerm(fl))->StrOfAE;
  f.writable = true;
  f.helper = 0;
  f.def = ok;
  initFlag(&f, i, true);
  if (IsAtomOrIntTerm(val)) {
    GLOBAL_Flags[i].at = val;
  } else {
    GLOBAL_Flags[i].DBT = Yap_StoreTermInDB(val, 2);
  }
}

static Int do_create_prolog_flag(USES_REGS1) {
  FlagEntry *fv;
  xarg *args;
  prolog_flag_property_choices_t i;
  Term tflag = Deref(ARG1), tval = Deref(ARG2), opts = Deref(ARG3);

  args = Yap_ArgList2ToVector(opts, prolog_flag_property_defs,
                              PROLOG_FLAG_PROPERTY_END);
 if (args == NULL) {
    Yap_Error( LOCAL_Error_TYPE, LOCAL_Error_Term, NULL );
    return false;
  }
  fv = GetFlagProp(AtomOfTerm(tflag));
  if (fv) {
    if (args[PROLOG_FLAG_PROPERTY_KEEP].used &&
        args[PROLOG_FLAG_PROPERTY_KEEP].tvalue == TermTrue)
      return true;
  } else {
    newFlag(tflag, tval);
    fv = GetFlagProp(AtomOfTerm(tflag));
  }
  for (i = 0; i < PROLOG_FLAG_PROPERTY_END; i++) {
    if (args[i].used) {
      switch (i) {
      case PROLOG_FLAG_PROPERTY_KEEP:
        break;
      case PROLOG_FLAG_PROPERTY_ACCESS:
        if (args[PROLOG_FLAG_PROPERTY_ACCESS].tvalue == TermReadWrite)
          fv->rw = true;
        else
          fv->rw = false;
        break;
      case PROLOG_FLAG_PROPERTY_TYPE: {
        Term ttype = args[PROLOG_FLAG_PROPERTY_TYPE].tvalue;
        if (ttype == TermBoolean)
          fv->type = booleanFlag;
        else if (ttype == TermInteger)
          fv->type = isatom;
        else if (ttype == TermFloat)
          fv->type = isfloat;
        else
          fv->type = isground;
      } break;
      case PROLOG_FLAG_PROPERTY_SCOPE:
        return false;
      case PROLOG_FLAG_PROPERTY_END:
        break;
      }
    }
  }
  // UNLOCK(GLOBAL_Prolog_Flag[sno].prolog_flaglock);
  return true;
}

/**
* Init System Prolog flags. This is done in two phases:
*   early on, it takes care of the atomic flags that are required by other
*modules;
* later, it looks at flags that are structured terms
*
* @param bootstrap: wether this is done before stack initialization, or
*afterwards.
* Complex terms can only be built in the second step.
*/

void Yap_InitFlags(bool bootstrap) {
  CACHE_REGS
  tr_fr_ptr tr0 = TR;
  flag_info *f = global_flags_setup;
  GLOBAL_flagCount = 0;
  if (bootstrap) {
    GLOBAL_Flags = (union flagTerm *)Yap_AllocCodeSpace(
        sizeof(union flagTerm) *
        (2 * sizeof(global_flags_setup) / sizeof(flag_info)));
  }
  while (f->name != NULL) {
    bool itf = setInitialValue(bootstrap, f->def, f->init,
                               GLOBAL_Flags + GLOBAL_flagCount);
    if (itf) {
      initFlag(f, GLOBAL_flagCount, true);
    }
    GLOBAL_flagCount++;
    f++;
  }
  LOCAL_flagCount = 0;
  int nflags = sizeof(local_flags_setup) / sizeof(flag_info);
  if (bootstrap)
    LOCAL_Flags =
        (union flagTerm *)Yap_AllocCodeSpace(sizeof(union flagTerm) * nflags);
  f = local_flags_setup;
  while (f->name != NULL) {
    bool itf = setInitialValue(bootstrap, f->def, f->init,
                               LOCAL_Flags + LOCAL_flagCount);
    //    Term itf = Yap_StringToTerm(f->init, strlen(f->init)+1,
    //    LOCAL_encoding, GLOBAL_MaxPriority, &tp);
    if (itf) {
      initFlag(f, LOCAL_flagCount, false);
    }
    LOCAL_flagCount++;
    f++;
  }
  if (!bootstrap) {
    Yap_InitCPredBack("current_prolog_flag", 2, 1, current_prolog_flag,
                      cont_yap_flag, 0);
    TR = tr0;
    /** @pred prolog_flag(? _Flag_,- _Value__)

    Obtain the value for a YAP Prolog flag, same as current_prolog_flag/2_.
    */
    Yap_InitCPredBack("prolog_flag", 3, 1, current_prolog_flag, cont_yap_flag,
                      0);
    Yap_InitCPredBack("yap_flag", 3, 1, prolog_flag, cont_yap_flag, 0);
    Yap_InitCPredBack("prolog_flag", 2, 1, current_prolog_flag2,
                      cont_current_prolog_flag, 0);
    Yap_InitCPredBack("current_prolog_flag", 2, 1, current_prolog_flag2,
                      cont_current_prolog_flag, 0);
    Yap_InitCPred("set_prolog_flag", 2, set_prolog_flag, SyncPredFlag);
    Yap_InitCPred("$create_prolog_flag", 3, do_create_prolog_flag,
                  SyncPredFlag);
    Yap_InitCPredBack("yap_flag", 2, 1, yap_flag, cont_yap_flag, 0);
    Yap_InitCPredBack("prolog_flag_property", 2, 1, prolog_flag_property,
                      cont_prolog_flag_property, 0);
    Yap_InitCPred("source", 0, source, SyncPredFlag);
    Yap_InitCPred("no_source", 0, no_source, SyncPredFlag);
    Yap_InitCPred("source_mode", 2, source_mode, SyncPredFlag);
  }
}

/* Accessing and changing the flags for a predicate */
