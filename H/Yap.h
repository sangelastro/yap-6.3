/*************************************************************************
*									 *
*	 YAP Prolog 	%W% %G% 					 *
*	Yap Prolog was developed at NCCUP - Universidade do Porto	 *
*									 *
* Copyright L.Damas, V.S.Costa and Universidade do Porto 1985-1997	 *
*									 *
**************************************************************************
*									 *
* File:		Yap.h   						 *
* mods:									 *
* comments:	main header file for YAP				 *
* version:      $Id: Yap.h,v 1.38 2008-06-18 10:02:27 vsc Exp $	 *
*************************************************************************/

#ifndef YAP_H
#define YAP_H 1

#if defined(YAPOR)
#error Do not explicitly define YAPOR
#endif /* YAPOR */

#if (defined(YAPOR_COPY) && (defined(YAPOR_COW) || defined(YAPOR_SBA) || defined(YAPOR_THREADS))) || (defined(YAPOR_COW) && (defined(YAPOR_SBA) || defined(YAPOR_THREADS))) || (defined(YAPOR_SBA) && defined(YAPOR_THREADS))
#error Do not define multiple or-parallel models
#endif /* (YAPOR_COPY && (YAPOR_COW || YAPOR_SBA || YAPOR_THREADS)) || (YAPOR_COW && (YAPOR_SBA || YAPOR_THREADS)) || (YAPOR_SBA || YAPOR_THREADS) */

#if defined(YAPOR_COPY) || defined(YAPOR_COW) || defined(YAPOR_SBA) || defined(YAPOR_THREADS)
#define YAPOR 1
#define FIXED_STACKS 1
#endif /* YAPOR_COPY || YAPOR_COW || YAPOR_SBA || YAPOR_THREADS */

#if defined(TABLING) && (defined(YAPOR_COW) || defined(YAPOR_SBA) || defined(YAPOR_THREADS))
#error TABLING only works with YAPOR_COPY
#endif /* TABLING && (YAPOR_COW || YAPOR_SBA || YAPOR_THREADS) */

#if defined(THREADS) && (defined(YAPOR_COW) || defined(YAPOR_SBA) || defined(YAPOR_COPY))
#error THREADS only works with YAPOR_THREADS
#endif /* THREADS && (YAPOR_COW || YAPOR_SBA || YAPOR_COPY) */

// Bad export from Python
#ifdef HAVE_STAT
#undef HAVE_STAT
#endif
#include "config.h"

#define FunAdr(X) X

#include "inline-only.h"
#if defined(YAPOR) || defined(TABLING)
#include "opt.config.h"
#endif /* YAPOR || TABLING */
#if HAVE_STRINGS_H
/* bzero */
#include <strings.h>
#endif
#if HAVE_STDINT_H
#include <stdint.h>
#endif
#if HAVE_INTTYPES_H
#include <inttypes.h>
#endif


/*

#define RATIONAL_TREES 1
#define DEPTH_LIMIT 1
#define COROUTINING 1
#define ANALYST 1

*/

#define MULTI_ASSIGNMENT_VARIABLES 1

#if defined(YAPOR) || defined(THREADS)
#define MULTIPLE_STACKS 1
#define PARALLEL_YAP 1
#endif /* YAPOR || THREADS */

#if defined(YAPOR) || defined(TABLING)
#undef TRAILING_REQUIRES_BRANCH
#endif /* YAPOR || TABLING */

#if defined(TABLING) || defined(YAPOR_SBA)
#define FROZEN_STACKS 1
#endif /* TABLING || YAPOR_SBA */

#if defined(THREADS) || defined(SUPPORT_CONDOR)
#define USE_SYSTEM_MALLOC 1
#endif /* THREADS ||  SUPPORT_CONDOR */

#if defined(ANALYST) && defined(USE_THREADED_CODE)
#undef USE_THREADED_CODE
#endif /* ANALYST && USE_THREADED_CODE */

#if defined(COROUTINING) && !defined(TERM_EXTENSIONS)
#define TERM_EXTENSIONS 1
#endif /* COROUTINING && !TERM_EXTENSIONS */

/* truth-values */
#if HAVE_STDBOOL_H
#include <stdbool.h>
#else

typedef int _Bool;

#define bool _Bool;

#define false 0
#define true 1
#endif

#ifndef TRUE
#define	 TRUE	true
#endif
#ifndef FALSE
#define	 FALSE	false
#endif

/**
 * Stolen from Mozzila, this code should deal with bad implementations of stdc++.
 *
 * Use C++11 nullptr if available; otherwise use a C++ typesafe template; and
 * for C, fall back to longs.  See bugs 547964 and 626472.
 */
#ifndef HAVE_NULLPTR
#ifndef __cplusplus
# define nullptr ((void*)0)
#elif defined(__GNUC__)
# define nullptr __null
#elif defined(_WIN64)
# define nullptr 0LL
#else
# define nullptr 0L
#endif
#endif /* defined(HAVE_NULLPTR) */


/* Microsoft's Visual C++ Compiler */
#ifdef _MSC_VER   /* adjust a config.h from mingw32 to work with vc++ */
#ifdef HAVE_GCC
#undef HAVE_GCC
#endif /* HAVE_GCC */
#ifdef  USE_THREADED_CODE
#undef  USE_THREADED_CODE
#endif /* USE_THREADED_CODE */
#define inline __inline
#define YAP_VERSION "YAP-6.3.4"
#define BIN_DIR "c:\\Yap\\bin"
#define LIB_DIR "c:\\Yap\\lib\\Yap"
#define SHARE_DIR "c:\\Yap\\share\\Yap"
#ifdef HOST_ALIAS
#undef HOST_ALIAS
#endif /* HOST_ALIAS */
#define HOST_ALIAS "i386-pc-win32"
#ifdef HAVE_IEEEFP_H
#undef HAVE_IEEEFP_H
#endif /* HAVE_IEEEFP_H */
#ifdef HAVE_UNISTD_H
#undef HAVE_UNISTD_H
#endif /* HAVE_UNISTD_H */
#ifdef HAVE_SYS_TIME_H
#undef HAVE_SYS_TIME_H
#endif /* HAVE_SYS_TIME_H */
#endif /* _MSC_VER */

#if HAVE_TIME_H
#include <time.h>
#endif

#ifdef __MINGW32__
#ifndef _WIN32
#define _WIN32 1
#endif /* _WIN32 */
#endif /* __MINGW32__ */

#if HAVE_GCC
#define MIN_ARRAY 0
#define DUMMY_FILLER_FOR_ABS_TYPE
#else
#define MIN_ARRAY 1
#define DUMMY_FILLER_FOR_ABS_TYPE int dummy;
#endif /* HAVE_GCC */

#ifdef HAVE___BUILTIN_EXPECT
#define likely(x)       __builtin_expect((x), 1)
#define unlikely(x)     __builtin_expect((x), 0)
#else
#define likely(x)       (x)
#define unlikely(x)     (x)
#endif

#ifdef THREADS
#if USE_PTHREAD_LOCKING
#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 600
#endif  /* !_XOPEN_SOURCE */
#endif /* USE_PTHREAD_LOCKING */
#include <pthread.h>
#endif /* THREADS */

#ifndef ADTDEFS_C
#define EXTERN  static
#else
#define EXTERN
#endif /* ADTDEFS_C */


/* null pointer	*/
#define	 NIL	0

/* Basic types */

#include "YapTerm.h"

#if HAVE_SIGPROF && (defined(__linux__)  || defined(__APPLE__))
#define LOW_PROF 1
#endif


/* #define FORCE_SECOND_QUADRANT 1 */

#if defined(FORCE_SECOND_QUADRANT)
#define IN_SECOND_QUADRANT 1
#define MMAP_ADDR 0x42000000
#endif /* FORCE_SECOND_QUADRANT */

#if !defined(IN_SECOND_QUADRANT)
#if defined(__linux__) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(mips) || (__DragonFly__)
#if defined(YAPOR) && defined(__alpha)

#define MMAP_ADDR 0x40000000
#elif defined(mips)
#define MMAP_ADDR 0x02000000
#elif defined(__powerpc__)
#define MMAP_ADDR 0x20000000
#else
#define MMAP_ADDR 0x10000000
#endif /* YAPOR && __alpha */
#elif __svr4__ || defined(__SVR4)
#define MMAP_ADDR 0x02000000
#endif /* __linux__ || __FreeBSD__ || __NetBSD__ || mips || __APPLE__ || __DragonFly__ */
#endif /* !IN_SECOND_QUADRANT */

/* #define RANDOMIZE_START_ADDRESS 1 */

#ifdef USE_SYSTEM_MALLOC
#define HEAP_INIT_BASE  0L
#define AtomBase        NULL
#else /* !USE_SYSTEM_MALLOC */
#if defined(MMAP_ADDR) && (defined(USE_MMAP) || USE_SHMAT) && !defined(__simplescalar__) && !defined(RANDOMIZE_START_ADDRESS)
#define HEAP_INIT_BASE  (MMAP_ADDR)
#define AtomBase        ((char *)MMAP_ADDR)
#else /*! (MMAP_ADDR && (USE_MMAP || USE_SHMAT) && !__simplescalar__ && !RANDOMIZE_START_ADDRESS) */
#define HEAP_INIT_BASE  ((CELL)Yap_HeapBase)
#define AtomBase        (Yap_HeapBase)
#endif /* MMAP_ADDR && (USE_MMAP || USE_SHMAT) && !__simplescalar__ && !RANDOMIZE_START_ADDRESS */
#endif /* USE_SYSTEM_MALLOC */



#ifndef ALIGN_LONGS
#define ALIGN_LONGS 1
#endif

#define K1   ((CELL)1024)
#define K16  ((CELL)(1024*64))
#define K64  ((CELL)(1024*64))
#define M1   ((CELL)(1024*1024))
#define M2   ((CELL)(2048*1024))

#if ALIGN_LONGS
typedef CELL SFLAGS;
#else
typedef BITS16 SFLAGS;
#endif

typedef char *ADDR;
typedef CELL OFFSET;
typedef unsigned char *CODEADDR;


#define TermPtr(V)	((Term *) (V))
#define	Addr(V)		((ADDR) (V))

#define CodePtr(V)	((CODEADDR)(V))
#define	CellPtr(V) 	((CELL *)(V))
#define OpCodePtr(V)	((OPCODE *)(V))
#define OpRegPtr(V)	((OPREG *)(V))
#define SmallPtr(V)	((SMALLUNSGN *)(V))
#define	WordPtr(V)	((BITS16 *)(V))
#define DisplPtr(V)	((DISPREG *)(V))

#if !defined(YAPOR) && !defined(THREADS)
#include <nolocks.h>
#elif USE_PTHREAD_LOCKING || defined(__CYGWIN__)

#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 600
#endif

#include <locks_pthread.h>
typedef pthread_mutex_t lockvar;
typedef pthread_rwlock_t rwlock_t;

#elif defined(i386)|| defined(__x86_64__)
typedef volatile int lockvar;
#include <locks_x86.h>
#elif defined(sparc) || defined(__sparc)
typedef volatile int lockvar;
#include <locks_sparc.h>
#elif defined(mips)
typedef volatile int lockvar;
#include <locks_mips.h>
#elif defined(__alpha)
typedef volatile int lockvar;
#include <locks_alpha.h>
#else

#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 600
#endif

typedef pthread_mutex_t lockvar;
typedef pthread_rwlock_t rwlock_t;
#include <locks_pthread.h>
#endif

#define FUNC_READ_LOCK(X) READ_LOCK((X)->FRWLock)
#define FUNC_READ_UNLOCK(X) READ_UNLOCK((X)->FRWLock)
#define FUNC_WRITE_LOCK(X) WRITE_LOCK((X)->FRWLock)
#define FUNC_WRITE_UNLOCK(X) WRITE_UNLOCK((X)->FRWLock)


/*************************************************************************************************
                              use an auxiliary function for ranges	
*************************************************************************************************/

#ifdef __GNUC__
#define IN_BETWEEN(MIN,X,MAX) (Unsigned((Int)(X)-(Int)(MIN)) <=  \
 			    Unsigned((Int)(MAX)-(Int)(MIN)) )

#define OUTSIDE(MIN,X,MAX) (Unsigned((Int)(X)-(Int)(MIN)) >  \
 			    Unsigned((Int)(MAX)-(Int)(MIN)) )
#else
#define IN_BETWEEN(MIN,X,MAX) ((void *)(X) >= (void *)(MIN) && (void *)(X) <= (void *)(MAX))

#define OUTSIDE(MIN,X,MAX) ((void *)(X) < (void *)(MIN) || (void *)(X) > (void *)(MAX))
#endif

/*************************************************************************************************
                              main exports in YapInterface.h
*************************************************************************************************/

/* Basic exports */

#include "YapDefs.h"

/*************************************************************************************************
                                             Atoms	
*************************************************************************************************/

#include "Atoms.h"

/*************************************************************************************************
                                             Coroutining	
*************************************************************************************************/


#ifdef COROUTINING
/* Support for co-routining */
#include "corout.h"
#endif

/*************************************************************************************************
                                      abstract machine registers	
*************************************************************************************************/

#include "amidefs.h"

#include "Regs.h"

#if defined(YAPOR) ||defined(THREADS)
#ifdef mips
#include <locks_mips_funcs.h>
#endif
#ifdef __alpha
#include <locks_alpha_funcs.h>
#endif
#endif

/*************************************************************************************************
                              variables concerned with Error Handling	
*************************************************************************************************/

#include <setjmp.h>

#if defined(SIMICS) || !HAVE_SIGSETJMP
#define sigjmp_buf jmp_buf
#define sigsetjmp(Env, Arg) setjmp(Env)
#define siglongjmp(Env, Arg) longjmp(Env, Arg)
#endif

/* Support for arrays */
#include "arrays.h"

#include "YapError.h"

typedef enum
{
  LANGUAGE_MODE_FLAG = 8,
  SOURCE_MODE_FLAG = 11,
  FLOATING_POINT_EXCEPTION_MODE_FLAG = 12,
  WRITE_QUOTED_STRING_FLAG = 13,
  ALLOW_ASSERTING_STATIC_FLAG = 14,
  HALT_AFTER_CONSULT_FLAG = 15,
  FAST_BOOT_FLAG = 16,
  STACK_DUMP_ON_ERROR_FLAG = 17,
  INDEXING_MODE_FLAG = 19,
  TABLING_MODE_FLAG = 20,
  VARS_CAN_HAVE_QUOTE_FLAG = 21,
  QUIET_MODE_FLAG = 22,
  INDEXING_TERM_DEPTH_FLAG = 23,
  /* let this be the last one */
  NUMBER_OF_YAP_FLAGS = 24
} yap_flags;

#define STRING_AS_CHARS		0
#define STRING_AS_ATOM		2

#define QUINTUS_TO_CHARS	0
#define ISO_TO_CHARS		1

typedef enum
{
  INDEX_MODE_OFF = 0,
  INDEX_MODE_SINGLE = 1,
  INDEX_MODE_COMPACT = 2,
  INDEX_MODE_MULTI = 3,
  INDEX_MODE_MAX = 4
} index_mode_options;

typedef enum
{
  GPROF_NO_EVENT,
  GPROF_NEW_PRED_FUNC,
  GPROF_NEW_PRED_THREAD,
  GPROF_NEW_PRED_ATOM,
  GPROF_INDEX,
  GPROF_INDEX_EXPAND,
  GPROF_CLAUSE,
  GPROF_MEGA,
  GPROF_LU_INDEX,
  GPROF_STATIC_INDEX,
  GPROF_INIT_OPCODE,
  GPROF_INIT_SYSTEM_CODE,
  GPROF_INIT_EXPAND,
  GPROF_INIT_LOG_UPD_CLAUSE,
  GPROF_INIT_DYNAMIC_CLAUSE,
  GPROF_INIT_STATIC_CLAUSE,
  GPROF_INIT_COMMA,
  GPROF_INIT_FAIL,
  GPROF_NEW_LU_CLAUSE,
  GPROF_NEW_LU_SWITCH,
  GPROF_NEW_STATIC_SWITCH,
  GPROF_NEW_EXPAND_BLOCK
} gprof_info;

#define MAX_EMPTY_WAKEUPS 16


/*************************************************************************************************
                                           prototypes
*************************************************************************************************/

#include "Yapproto.h"

#include "YapTags.h"

#define	TermSize    sizeof(Term)

/*************************************************************************************************
                           variables related to memory allocation 
*************************************************************************************************/
/* must be before TermExt.h */

extern ADDR Yap_HeapBase;

/* This is ok for Linux, should be ok for everyone */
#define YAP_FILENAME_MAX 1024

#define MAX_ERROR_MSG_SIZE YAP_FILENAME_MAX

/*************************************************************************************************
                                    Debugging Support
*************************************************************************************************/

#ifdef DEBUG
extern int Yap_output_msg;
#endif


#if __ANDROID__
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <android/log.h>
#else
static inline char * __android_log_print(int i,const char *loc,const char *msg,...) {
  return NULL;
}
#define ANDROID_LOG_INFO 0
#define ANDROID_LOG_ERROR 0
#define ANDROID_LOG_DEBUG 0
#endif


/*************************************************************************************************
                                variables concerned with atoms table
*************************************************************************************************/

#define	MaxHash	    3333
#define	MaxWideHash (MaxHash/10+1)

#define FAIL_RESTORE  0
#define DO_EVERYTHING 1
#define DO_ONLY_CODE  2

/*************************************************************************************************
                                   common instructions codes
*************************************************************************************************/

#define MAX_PROMPT  256

#if USE_THREADED_CODE

/*************************************************************************************************
                                   reverse lookup of instructions
*************************************************************************************************/
typedef struct opcode_tab_entry
{
  OPCODE opc;
  op_numbers opnum;
} opentry;

#endif

/*************************************************************************************************
                                   Prolog may be in several modes 
*************************************************************************************************/

typedef enum
{
  BootMode = 0x1,		/* if booting or restoring */
  UserMode = 0x2,		/* Normal mode */
  CritMode = 0x4,		/* If we are meddling with the heap */
  AbortMode = 0x8,		/* expecting to abort */
  InterruptMode = 0x10,		/* under an interrupt */
  InErrorMode = 0x20,		/* under an interrupt */
  ConsoleGetcMode = 0x40,	/* blocked reading from console */
  ExtendStackMode = 0x80,	/* trying to extend stack */
  GrowHeapMode = 0x100,		/* extending Heap  */
  GrowStackMode = 0x200,	/* extending Stack */
  GCMode = 0x400,		/* doing Garbage Collecting */
  ErrorHandlingMode = 0x800,	/* doing error handling */
  CCallMode = 0x1000,		/* In c Call */
  UnifyMode = 0x2000,		/* In Unify Code */
  UserCCallMode = 0x4000,	/* In User C-call Code */
  MallocMode = 0x8000,		/* Doing malloc, realloc, free */
  SystemMode = 0x10000,		/* in system mode */
  AsyncIntMode = 0x20000,	/* YAP has just been interrupted from the outside */
  InReadlineMode = 0x40000	/* YAP has just been interrupted from the outside */
} prolog_exec_mode;


/*************************************************************************************************
                                     number of modules
*************************************************************************************************/

#define DefaultMaxModules	256

/*************************************************************************************************
                                       Critical sections
*************************************************************************************************/
#ifdef YAPOR
#define YAPEnterCriticalSection()                                        \
	{                                                                \
          if (worker_id != GLOBAL_locks_who_locked_heap) {               \
	    LOCK(GLOBAL_locks_heap_access);                              \
	    GLOBAL_locks_who_locked_heap = worker_id;                    \
	  }                                                              \
          LOCAL_PrologMode |= CritMode;                                   \
          LOCAL_CritLocks++;                                              \
        }
#define YAPLeaveCriticalSection()                                        \
	{                                                                \
          LOCAL_CritLocks--;                                              \
          if (!LOCAL_CritLocks) {                                         \
            LOCAL_PrologMode &= ~CritMode;                                \
            if (LOCAL_PrologMode & AbortMode) {                           \
	      LOCAL_PrologMode &= ~AbortMode;                             \
	      Yap_Error(PURE_ABORT, 0, "");                             \
            }                                                            \
	    GLOBAL_locks_who_locked_heap = MAX_WORKERS;                  \
            UNLOCK(GLOBAL_locks_heap_access);                            \
          }                                                              \
        }
#elif defined(THREADS)
#define YAPEnterCriticalSection()                                        \
	{                                                                \
          /* LOCK(BGL); */                                              \
          LOCAL_PrologMode |= CritMode;                                   \
        }
#define YAPLeaveCriticalSection()                                        \
	{                                                                \
          LOCAL_PrologMode &= ~CritMode;                                \
          if (LOCAL_PrologMode & AbortMode) {                           \
	    LOCAL_PrologMode &= ~AbortMode;                             \
	    Yap_Error(PURE_ABORT, 0, "");                             \
            Yap_RestartYap( 1 );                                      \
          }                                                            \
          /* UNLOCK(BGL); */                                           \
        }
#else
#define YAPEnterCriticalSection()                                        \
	{                                                                \
          LOCAL_PrologMode |= CritMode;                                   \
          LOCAL_CritLocks++;                                              \
        }
#define YAPLeaveCriticalSection()                                        \
	{                                                                \
          LOCAL_CritLocks--;                                              \
          if (!LOCAL_CritLocks) {                                         \
            LOCAL_PrologMode &= ~CritMode;                                \
            if (LOCAL_PrologMode & AbortMode) {                           \
	      LOCAL_PrologMode &= ~AbortMode;                             \
              Yap_RestartYap( 1 );                                      \
            }                                                            \
          }                                                              \
        }
#endif /* YAPOR */

/* when we are calling the InitStaff procedures */
#define AT_BOOT      0
#define AT_RESTORE   1


/*************************************************************************************************
                                       mutable variables
*************************************************************************************************/

/* I assume that the size of this structure is a multiple of the size
   of CELL!!! */
typedef struct TIMED_MAVAR
{
  CELL value;
  CELL clock;
} timed_var;


/*************************************************************************************************
                                       execution mode
*************************************************************************************************/

typedef enum
  {
    INTERPRETED,         /* interpreted */
    MIXED_MODE_USER,       /* mixed mode only for user predicates */
    MIXED_MODE_ALL,        /* mixed mode for all predicates */
    COMPILE_USER,          /* compile all user predicates*/
    COMPILE_ALL          /* compile all predicates */
  } yap_exec_mode;


/************************/
  // queues are an example of collections of DB objects
  typedef struct queue_entry {
    struct queue_entry *next;
    struct DB_TERM *DBT;
  } QueueEntry;

  typedef struct idb_queue
  {
    struct FunctorEntryStruct *id;		/* identify this as being pointed to by a DBRef */
    SMALLUNSGN    Flags;  /* always required */
  #if PARALLEL_YAP
    rwlock_t    QRWLock;         /* a simple lock to protect this entry */
  #endif
    QueueEntry *FirstInQueue, *LastInQueue;
  }  db_queue;

void Yap_init_tqueue( db_queue *dbq );
void Yap_destroy_tqueue( db_queue *dbq  USES_REGS);
bool Yap_enqueue_tqueue(db_queue *father_key, Term t USES_REGS);
bool Yap_dequeue_tqueue(db_queue *father_key, Term t, bool first, bool release USES_REGS);

#ifdef THREADS


typedef struct thread_mbox {
    Term name;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
   struct idb_queue msgs;
    int  nmsgs, nclients;  // if nclients < 0 mailbox has been closed.
    bool open;
    struct thread_mbox *next;
} mbox_t;

typedef struct thandle {
  int in_use;
  int zombie;
  UInt ssize;
  UInt tsize;
  UInt sysize;
  void *stack_address;
  Term tdetach;
  Term  cmod, texit_mod;
  struct DB_TERM *tgoal, *texit;
  int id;
  int ret;
  REGSTORE *default_yaam_regs;
  REGSTORE *current_yaam_regs;
  struct pred_entry *local_preds;
  pthread_t pthread_handle;
  mbox_t    mbox_handle;
  int ref_count;
#ifdef LOW_LEVEL_TRACER
  long long int thread_inst_count;
  int been_here1;
  int been_here2;
#endif
#ifdef DEBUG
  int been_here;
#endif
  pthread_mutex_t tlock;
  pthread_mutex_t tlock_status;
#if HAVE_GETHRTIME
  hrtime_t start_of_w_times;
  hrtime_t last_w_time;
#endif
#if HAVE_GETRUSAGE
  struct timeval *start_of_timesp;
  struct timeval *last_timep;
  struct timeval *start_of_times_sysp;
  struct timeval *last_time_sysp;
#elif _WIN32
  struct _FILETIME *start_of_timesp;
  struct _FILETIME *last_timep;
  struct _FILETIME *start_of_times_sysp;
  struct _FILETIME *last_time_sysp;
#endif
} yap_thandle;
#endif /* THREADS */

#define GC_MAVARS_HASH_SIZE 512
typedef struct gc_ma_hash_entry_struct {
  UInt timestmp;
#ifdef TABLING
  tr_fr_ptr loc;
  struct gc_ma_hash_entry_struct *more;
#endif
  CELL* addr;
  struct gc_ma_hash_entry_struct *next;
} gc_ma_hash_entry;

typedef int   (*Agc_hook)(Atom);

typedef struct scratch_block_struct {
  char *ptr;
  UInt sz, msz;
} scratch_block;


/* scanner types */
#include "ScannerTypes.h"

/*************************************************************************************************
                                       OPTYAP includes
*************************************************************************************************/

#if defined(YAPOR) || defined(TABLING)
#include "opt.structs.h"
#include "opt.proto.h"
#include "opt.macros.h"
#endif /* YAPOR || TABLING */


/*************************************************************************************************
                                  GLOBAL and LOCAL variables
*************************************************************************************************/

#if defined(YAPOR_COPY) || defined(YAPOR_COW) || defined(YAPOR_SBA)
extern struct global_data *Yap_global;
extern long Yap_worker_area_size;
#else
extern struct global_data Yap_Global;
#define Yap_global (&Yap_Global)
#endif

#if defined(THREADS)
extern struct worker_local *Yap_local[MAX_THREADS];
#define REMOTE(wid)        (Yap_local[wid])
#elif defined(YAPOR)
extern struct worker_local *Yap_local;
#define REMOTE(wid)        (Yap_local + wid)
#else /* !THREADS && !YAPOR */
extern struct worker_local Yap_local;
#define REMOTE(wid)        (&Yap_local)
#endif

#include <stdio.h>
#define YP_FILE		FILE
#include "hglobals.h"
#include "dglobals.h"
#include "hlocals.h"
#include "dlocals.h"


/*************************************************************************************************
                                       unification routines
*************************************************************************************************/

#ifdef YAPOR_SBA
#include "or.sba_amiops.h"
#include "or.sba_unify.h"
#else
#include "amiops.h"
#endif /* YAPOR_SBA */

/*************************************************************************************************
                                       unification support
*************************************************************************************************/

#include "YapCompoundTerm.h"

#include "YapHandles.h"

// take care of signal handling within YAP

#include "YapSignals.h"


#endif /* YAP_H */

