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
* File:		iopreds.c						 *
* Last rev:	5/2/88							 *
* mods:									 *
* comments:	Input/Output C implemented predicates			 *
*									 *
*************************************************************************/
#ifdef SCCS
static char SccsId[] = "%W% %G%";
#endif

/*
 * This file includes the definition of a miscellania of standard predicates
 * for yap refering to: Files and GLOBAL_Streams, Simple Input/Output,
 *
 */

#include "Yap.h"
#if HAVE_FCNTL_H
/* for O_BINARY and O_TEXT in WIN32 */
#include <fcntl.h>
#endif
#include "Yatom.h"
#include "YapHeap.h"
#include "yapio.h"
#include "eval.h"
#include "YapText.h"
#include <stdlib.h>
#if HAVE_STDARG_H
#include <stdarg.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#if HAVE_CTYPE_H
#include <ctype.h>
#endif
#if HAVE_WCTYPE_H
#include <wctype.h>
#endif
#if HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif
#if HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#if HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#if HAVE_SYS_SELECT_H && !_MSC_VER && !defined(__MINGW32__)
#include <sys/select.h>
#endif
#if HAVE_STRING_H
#include <string.h>
#endif
#if HAVE_SIGNAL_H
#include <signal.h>
#endif
#ifdef _WIN32
#if HAVE_IO_H
/* Windows */
#include <io.h>
#endif
#endif
#if !HAVE_STRNCAT
#define strncat(X, Y, Z) strcat(X, Y)
#endif
#if !HAVE_STRNCPY
#define strncpy(X, Y, Z) strcpy(X, Y)
#endif
#if _MSC_VER || defined(__MINGW32__)
#if HAVE_SOCKET
#include <winsock2.h>
#endif
#include <windows.h>
#ifndef S_ISDIR
#define S_ISDIR(x) (((x)&_S_IFDIR) == _S_IFDIR)
#endif
#endif
#include "iopreds.h"

#if _MSC_VER || defined(__MINGW32__)
#define SYSTEM_STAT _stat
#else
#define SYSTEM_STAT stat
#endif

static void CloseStream(int sno);

FILE *Yap_GetInputStream(Term t, const char *msg) {
  int sno = Yap_CheckStream(t, Input_Stream_f, msg);
  FILE *rc;

  if (!(GLOBAL_Stream[sno].status &
        (Tty_Stream_f | Socket_Stream_f | Pipe_Stream_f)))
    rc = GLOBAL_Stream[sno].file;
  else
    rc = NULL;
  UNLOCK(GLOBAL_Stream[sno].streamlock);
  return rc;
}

FILE *Yap_GetOutputStream(Term t, const char *msg) {
  int sno = Yap_CheckStream(t, Output_Stream_f, msg);
  FILE *rc;

  if (!(GLOBAL_Stream[sno].status & (Tty_Stream_f | Socket_Stream_f)))
    rc = GLOBAL_Stream[sno].file;
  else
    rc = NULL;
  UNLOCK(GLOBAL_Stream[sno].streamlock);
  return rc;
}

int GetFreeStreamD(void) {
  CACHE_REGS
  LOCK(GLOBAL_StreamDescLock);
  int sno;
  for (sno = 0; sno < MaxStreams; ++sno) {
    if (GLOBAL_Stream[sno].status & Free_Stream_f) {
      break;
    }
  }
  if (sno == MaxStreams) {
    UNLOCK(GLOBAL_StreamDescLock);
    return -1;
  }
  LOCK(GLOBAL_Stream[sno].streamlock);
  UNLOCK(GLOBAL_StreamDescLock);
  GLOBAL_Stream[sno].encoding = LOCAL_encoding;
  return sno;
}

int Yap_GetFreeStreamD(void) { return GetFreeStreamD(); }

/* used from C-interface */
int Yap_GetFreeStreamDForReading(void) {
  CACHE_REGS
  int sno = GetFreeStreamD();
  StreamDesc *s;

  if (sno < 0)
    return sno;
  s = GLOBAL_Stream + sno;
  s->status |= User_Stream_f | Input_Stream_f;
  s->charcount = 0;
  s->linecount = 1;
  s->linepos = 0;
  s->stream_wgetc = get_wchar;
  s->stream_wputc = put_wchar;
  s->encoding = LOCAL_encoding;
  if (GLOBAL_CharConversionTable != NULL)
    s->stream_wgetc_for_read = ISOWGetc;
  else
    s->stream_wgetc_for_read = s->stream_wgetc;
  if (s->encoding == ENC_ISO_UTF8)
    s->stream_getc_for_utf8 = s->stream_getc;
  else
    s->stream_getc_for_utf8 = GetUTF8;
  UNLOCK(s->streamlock);
  return sno;
}

static Int p_stream_flags(USES_REGS1) { /* '$stream_flags'(+N,-Flags) */
  Term trm;
  trm = Deref(ARG1);
  if (IsVarTerm(trm) || !IsIntTerm(trm))
    return (FALSE);
  return (Yap_unify_constant(ARG2,
                             MkIntTerm(GLOBAL_Stream[IntOfTerm(trm)].status)));
}

static Int p_check_stream(USES_REGS1) { /* '$check_stream'(Stream,Mode) */
  Term mode = Deref(ARG2);
  int sno = Yap_CheckStream(
      ARG1, AtomOfTerm(mode) == AtomRead ? Input_Stream_f : Output_Stream_f,
      "check_stream/2");
  if (sno != -1)
    UNLOCK(GLOBAL_Stream[sno].streamlock);
  return sno != -1;
}

static Int p_check_if_stream(USES_REGS1) { /* '$check_stream'(Stream) */
  int sno = Yap_CheckStream(ARG1, Input_Stream_f | Output_Stream_f |
                                      Append_Stream_f | Socket_Stream_f,
                            "check_stream/1");
  if (sno != -1)
    UNLOCK(GLOBAL_Stream[sno].streamlock);
  return sno != -1;
}

static Int is_input(int sno
                        USES_REGS) { /* '$set_output'(+Stream,-ErrorMessage)  */
  bool rc = GLOBAL_Stream[sno].status & Input_Stream_f;
  return rc;
}

static Int is_output(int sno
                         USES_REGS) { /* '$set_output'(+Stream,-ErrorMessage) */
  bool rc = GLOBAL_Stream[sno].status & (Output_Stream_f | Append_Stream_f);
  return rc;
}

static Int
has_bom(int sno, Term t2 USES_REGS) { /* '$set_output'(+Stream,-ErrorMessage) */
  bool rc = GLOBAL_Stream[sno].status & Seekable_Stream_f;
  if (!IsVarTerm(t2) && !boolean(t2)) {
    return FALSE;
  }
  if (rc) {
    return Yap_unify_constant(t2, TermTrue);
  } else {
    return Yap_unify_constant(t2, TermFalse);
  }
}

static Int
has_reposition(int sno,
               Term t2 USES_REGS) { /* '$set_output'(+Stream,-ErrorMessage)  */
  bool rc = GLOBAL_Stream[sno].status & Seekable_Stream_f;
  if (!IsVarTerm(t2) && !boolean(t2)) {
    return FALSE;
  }
  if (rc) {
    return Yap_unify_constant(t2, TermTrue);
  } else {
    return Yap_unify_constant(t2, TermFalse);
  }
}

char *Yap_guessFileName(int f, int sno, char *nameb, size_t max) {
#if __linux__
  char path[256];
  if (snprintf(path, 255, "/proc/self/fd/~d", f) && readlink(path, nameb, max))
    return nameb;
#elif __APPLE__
  if (fcntl(f, F_GETPATH, nameb) != -1) {
    return nameb;
  }
#elif __WIN32_
  FILE_NAME_INFO *fni = (FILE_NAME_INFO *)malloc(sizeof(FILE_NAME_INFO) +
                                                 sizeof(WCHAR) * MAXPATHLEN);
  HANDLE handle = (HANDLE)_get_osfhandle(f);
  if (GetFileInformationByHandleEx(handle, FileNameInfo, &fni, max)) {
    int i;
    char *ptr = nameb;
    for (i = 0; i < fni->FileNameLength; i++)
      *ptr = _PL__utf8_put_char(ptr, fni->FileName[i]);
    *ptr = '\0';
    return nameb;
  }
#endif
  return RepAtom(AtomOfTerm(StreamName(sno)))->StrOfAE;
}

static Int representation_error(int sno, Term t2 USES_REGS) {
  stream_flags_t flags =
      GLOBAL_Stream[sno].status & (RepError_Xml_f | RepError_Prolog_f);
  /* '$representation_error'(+Stream,-ErrorMessage)  */
  if (!IsVarTerm(t2) && isatom(t2)) {
    return false;
  }
  if (flags & RepError_Xml_f) {
    return Yap_unify(t2, TermXml);
  }
  if (flags & RepError_Prolog_f) {
    return Yap_unify(t2, TermProlog);
  }
  return Yap_unify(t2, TermError);
}

static Int file_name(int sno, Term t2 USES_REGS) {
  char s[MAXPATHLEN + 1];
  int f = Yap_GetStreamFd(sno);
  Term rc;
  char *name = Yap_guessFileName(f, sno, s, MAXPATHLEN);
  if (name)
    rc = MkAtomTerm(Yap_LookupAtom(name));
  else
    return false;
  if (!IsVarTerm(t2) && !isatom(t2)) {
    return FALSE;
  }
  return Yap_unify_constant(t2, rc);
}

static Int file_no(int sno, Term t2 USES_REGS) {
  int f = Yap_GetStreamFd(sno);
  Term rc = MkIntTerm(f);
  if (!IsVarTerm(t2) && !IsIntTerm(t2)) {
    return false;
  }
  return Yap_unify_constant(t2, rc);
}

static bool SetCloseOnAbort(int sno, bool close) {
  if (close) {
    GLOBAL_Stream[sno].status |= DoNotCloseOnAbort_Stream_f;
  } else {
    GLOBAL_Stream[sno].status &= ~DoNotCloseOnAbort_Stream_f;
  }
  return true;
}

static Int has_close_on_abort(
    int sno, Term t2 USES_REGS) { /* '$set_output'(+Stream,-ErrorMessage)  */
  bool rc = GLOBAL_Stream[sno].status & DoNotCloseOnAbort_Stream_f;
  if (!IsVarTerm(t2)) {
    return t2 == (rc ? TermTrue : TermFalse);
  }
  if (rc) {
    return Yap_unify_constant(t2, TermTrue);
  } else {
    return Yap_unify_constant(t2, TermFalse);
  }
}

static bool
has_encoding(int sno,
             Term t2 USES_REGS) { /* '$set_output'(+Stream,-ErrorMessage)  */
  if (!IsVarTerm(t2) && !(isatom(t2))) {
    return FALSE;
  }
  if (0 && IsAtomTerm(t2)) {
    encoding_t e = enc_id(RepAtom(AtomOfTerm(t2))->StrOfAE);
    GLOBAL_Stream[sno].encoding = e;
    return true;
  } else {
    const char *s = enc_name(LOCAL_encoding);
    return Yap_unify(t2, MkAtomTerm(Yap_LookupAtom(s)));
  }
}

static bool
found_eof(int sno,
          Term t2 USES_REGS) { /* '$set_output'(+Stream,-ErrorMessage)  */
  stream_flags_t flags =
      GLOBAL_Stream[sno].status & (Past_Eof_Stream_f | Eof_Stream_f);
  if (!IsVarTerm(t2) && !(isatom(t2))) {
    return FALSE;
  }
  if (flags & Past_Eof_Stream_f)
    return Yap_unify(t2, MkAtomTerm(AtomPast));
  if (flags & Eof_Stream_f)
    return Yap_unify(t2, MkAtomTerm(AtomAt));
  return Yap_unify(t2, MkAtomTerm(AtomNot));
}

static bool
stream_mode(int sno,
            Term t2 USES_REGS) { /* '$set_output'(+Stream,-ErrorMessage)  */
  stream_flags_t flags = GLOBAL_Stream[sno].status &
                         (Input_Stream_f | Output_Stream_f | Append_Stream_f);
  if (!IsVarTerm(t2) && !(isatom(t2))) {
    return FALSE;
  }
  if (flags & Input_Stream_f)
    return Yap_unify(t2, TermRead);
  if (flags & Output_Stream_f)
    return Yap_unify(t2, TermWrite);
  if (flags & Append_Stream_f)
    return Yap_unify(t2, TermAppend);
  return false;
}

static bool
stream_tty(int sno,
           Term t2 USES_REGS) { /* '$set_output'(+Stream,-ErrorMessage)  */
  stream_flags_t flags = GLOBAL_Stream[sno].status & (Tty_Stream_f);
  if (!IsVarTerm(t2) && !(isatom(t2))) {
    return FALSE;
  }
  if (flags & Tty_Stream_f)
    return Yap_unify(t2, TermTrue);
  return Yap_unify(t2, TermFalse);
}

static bool
stream_type(int sno,
            Term t2 USES_REGS) { /* '$set_output'(+Stream,-ErrorMessage)  */
  stream_flags_t flags = GLOBAL_Stream[sno].status & (Binary_Stream_f);
  if (!IsVarTerm(t2) && !(isatom(t2))) {
    return FALSE;
  }
  if (flags & Binary_Stream_f)
    return Yap_unify(t2, TermBinary);
  return Yap_unify(t2, TermText);
}

static bool
stream_position(int sno,
                Term t2 USES_REGS) { /* '$set_output'(+Stream,-ErrorMessage)  */
  Term tout = StreamPosition(sno);
  return Yap_unify(t2, tout);
}

static bool
stream_line_count(int sno,
                Term t2 USES_REGS) { /* '$set_output'(+Stream,-ErrorMessage)  */
  Term tout = StreamPosition(GLOBAL_Stream[sno].linecount);
  return Yap_unify(t2, MkIntegerTerm(tout));
}

static bool
stream_line_number(int sno,
                Term t2 USES_REGS) { /* '$set_output'(+Stream,-ErrorMessage)  */
  Term tout = StreamPosition(GLOBAL_Stream[sno].linecount);
  return Yap_unify(t2, MkIntegerTerm(tout));
}

static bool
SetBuffering(int sno, Atom at) { /* '$set_bufferingt'(+Stream,-ErrorMessage)  */
  if (at == AtomFull) {
    if (setvbuf(GLOBAL_Stream[sno].file, NULL, _IOFBF, 0) < 0)
      return PlIOError(SYSTEM_ERROR_INTERNAL, Yap_MkStream(sno), "could not set buffer");
  } else if (at == AtomLine) {
    if (setvbuf(GLOBAL_Stream[sno].file, NULL, _IOLBF, 0) < 0)
      return PlIOError(SYSTEM_ERROR_INTERNAL, Yap_MkStream(sno),
                       "could not set line buffering");
  } else if (at == AtomFalse) {
    if (setvbuf(GLOBAL_Stream[sno].file, NULL, _IONBF, 0) < 0)
      return PlIOError(SYSTEM_ERROR_INTERNAL, Yap_MkStream(sno),
                       "could not set disable buffering");
  } else {
    CACHE_REGS
    LOCAL_Error_TYPE = DOMAIN_ERROR_OUT_OF_RANGE;
    LOCAL_ErrorMessage = "in set_stream/2:buffer";
    return false;
  }
  return true;
}

static bool SetBuffer(int sno,
                      Int sz) { /* '$set_bufferingt'(+Stream,-ErrorMessage)  */
  if (setvbuf(GLOBAL_Stream[sno].file, NULL, _IOFBF, sz) < 0) {
    return PlIOError(SYSTEM_ERROR_INTERNAL, Yap_MkStream(sno), "could not set buffer");
  }
  return true;
}

static bool
eof_action(int sno,
           Term t2 USES_REGS) { /* '$set_output'(+Stream,-ErrorMessage)  */
  stream_flags_t flags =
      GLOBAL_Stream[sno].status &
      (Eof_Error_Stream_f | Reset_Eof_Stream_f | Push_Eof_Stream_f);
  if (!IsVarTerm(t2) && !(isatom(t2))) {
    return FALSE;
  }
  if (flags & Eof_Error_Stream_f) {
    return Yap_unify(t2, TermError);
  }
  if (flags & Reset_Eof_Stream_f) {
    return Yap_unify(t2, TermReset);
  }
  return Yap_unify(t2, TermEOfCode);
}


#define STREAM_PROPERTY_DEFS()                                                 \
  PAR("alias", filler, STREAM_PROPERTY_ALIAS),                                \
  PAR("bom", filler, STREAM_PROPERTY_BOM),				\
      PAR("close_on_abort", filler, STREAM_PROPERTY_CLOSE_ON_ABORT),           \
      PAR("encoding", filler, STREAM_PROPERTY_ENCODING),                       \
      PAR("end_of_stream", filler, STREAM_PROPERTY_END_OF_STREAM),             \
      PAR("eof_action", filler, STREAM_PROPERTY_EOF_ACTION),                   \
      PAR("file_name", filler, STREAM_PROPERTY_FILE_NAME),                     \
      PAR("file_no", filler, STREAM_PROPERTY_FILE_NO),                         \
      PAR("input", ok, STREAM_PROPERTY_INPUT),                                 \
      PAR("line_count", ok, STREAM_PROPERTY_LINE_COUNT),                       \
      PAR("line_number", ok, STREAM_PROPERTY_LINE_NUMBER),                     \
      PAR("mode", filler, STREAM_PROPERTY_MODE),                               \
      PAR("output", filler, STREAM_PROPERTY_OUTPUT),                           \
      PAR("position", isatom, STREAM_PROPERTY_POSITION),                       \
      PAR("reposition", filler, STREAM_PROPERTY_REPOSITION),                   \
      PAR("representation_errors", filler,                                     \
          STREAM_PROPERTY_REPRESENTATION_ERRORS),                              \
      PAR("type", filler, STREAM_PROPERTY_TYPE),                               \
      PAR("tty", filler, STREAM_PROPERTY_TTY),                                 \
      PAR(NULL, ok, STREAM_PROPERTY_END)

#define PAR(x, y, z) z

typedef enum stream_property_enum_choices {
  STREAM_PROPERTY_DEFS()
} stream_property_choices_t;

#undef PAR

#define PAR(x, y, z)                                                           \
  { x, y, z }

static const param_t stream_property_defs[] = {STREAM_PROPERTY_DEFS()};
#undef PAR

static bool do_stream_property(int sno,
                               xarg *args USES_REGS) { /* Init current_stream */
  stream_property_choices_t i;
  bool rc = true;

  for (i = 0; i < STREAM_PROPERTY_END; i++) {
    if (args[i].used) {
      switch (i) {
      case STREAM_PROPERTY_ALIAS:
        rc = rc & Yap_FetchStreamAlias(
                      sno, args[STREAM_PROPERTY_ALIAS].tvalue PASS_REGS);
        break;
      case STREAM_PROPERTY_BOM:
        rc = rc && has_bom(sno, args[STREAM_PROPERTY_BOM].tvalue PASS_REGS);
        break;
      case STREAM_PROPERTY_CLOSE_ON_ABORT:
        rc = rc &&
             has_close_on_abort(
                 sno, args[STREAM_PROPERTY_CLOSE_ON_ABORT].tvalue PASS_REGS);
        break;
      case STREAM_PROPERTY_ENCODING:
        rc = rc &&
             has_encoding(sno, args[STREAM_PROPERTY_ENCODING].tvalue PASS_REGS);
        break;
      case STREAM_PROPERTY_END_OF_STREAM:
        rc = rc &&
             found_eof(sno,
                       args[STREAM_PROPERTY_END_OF_STREAM].tvalue PASS_REGS);
        break;
      case STREAM_PROPERTY_EOF_ACTION:
        rc = rc &&
             eof_action(sno, args[STREAM_PROPERTY_EOF_ACTION].tvalue PASS_REGS);
        break;
      case STREAM_PROPERTY_FILE_NAME:
        rc = rc &&
             file_name(sno, args[STREAM_PROPERTY_FILE_NAME].tvalue PASS_REGS);
        break;
      case STREAM_PROPERTY_FILE_NO:
        rc = rc && file_no(sno, args[STREAM_PROPERTY_FILE_NO].tvalue PASS_REGS);
        break;
      case STREAM_PROPERTY_INPUT:
        rc = rc && is_input(sno PASS_REGS);
        break;
      case STREAM_PROPERTY_LINE_NUMBER:
        rc = rc && stream_line_number(sno, args[STREAM_PROPERTY_LINE_NUMBER].tvalue PASS_REGS);
        break;
      case STREAM_PROPERTY_LINE_COUNT:
        rc = rc && stream_line_count(sno, args[STREAM_PROPERTY_LINE_COUNT].tvalue PASS_REGS);
        break;
      case STREAM_PROPERTY_MODE:
        rc =
            rc && stream_mode(sno, args[STREAM_PROPERTY_MODE].tvalue PASS_REGS);
        break;
      case STREAM_PROPERTY_OUTPUT:
        rc = rc && is_output(sno PASS_REGS);
        break;
      case STREAM_PROPERTY_POSITION:
        rc = rc && stream_position(
                       sno, args[STREAM_PROPERTY_POSITION].tvalue PASS_REGS);
        break;
      case STREAM_PROPERTY_REPOSITION:
        rc = rc &&
             has_reposition(sno, args[STREAM_PROPERTY_MODE].tvalue PASS_REGS);
        break;
      case STREAM_PROPERTY_REPRESENTATION_ERRORS:
        rc = rc &&
             representation_error(
                 sno,
                 args[STREAM_PROPERTY_REPRESENTATION_ERRORS].tvalue PASS_REGS);
        break;
      case STREAM_PROPERTY_TYPE:
        rc =
            rc && stream_type(sno, args[STREAM_PROPERTY_TYPE].tvalue PASS_REGS);
        break;
      case STREAM_PROPERTY_TTY:
        rc = rc && stream_tty(sno, args[STREAM_PROPERTY_TTY].tvalue PASS_REGS);
        break;
      case STREAM_PROPERTY_END:
        rc = false;
        break;
      }
    }
  }
  UNLOCK(GLOBAL_Stream[sno].streamlock);
  return rc;
}

static Int cont_stream_property(USES_REGS1) { /* current_stream */
  bool det;
  xarg *args;
  int i = IntOfTerm(EXTRA_CBACK_ARG(2, 1));
  bool rc;

  args = Yap_ArgListToVector(Deref(ARG2), stream_property_defs,
                             STREAM_PROPERTY_END);
  if (args == NULL) {
    cut_fail();
  }
  LOCK(GLOBAL_StreamDescLock);
  if (args[STREAM_PROPERTY_ALIAS].tvalue &&
      IsAtomTerm(args[STREAM_PROPERTY_ALIAS].tvalue)) {
    // one solution only
    det = true;
    i = Yap_CheckAlias(AtomOfTerm(args[STREAM_PROPERTY_ALIAS].tvalue));
    UNLOCK(GLOBAL_StreamDescLock);
    if (i < 0) {
      cut_fail();
    }
    rc = true;
  } else {
    while (GLOBAL_Stream[i].status & Free_Stream_f) {
      ++i;
      if (i == MaxStreams) {
	UNLOCK(GLOBAL_StreamDescLock);
       cut_fail();
      }
    }
    LOCK(GLOBAL_Stream[i].streamlock);
    UNLOCK(GLOBAL_StreamDescLock);
    rc = do_stream_property(i, args PASS_REGS);
    UNLOCK(GLOBAL_Stream[i].streamlock);
  }
  if (rc)
    rc = Yap_unify(ARG1, Yap_MkStream(i));
  if (rc) {
    if (det)
      cut_succeed();
    else {
      EXTRA_CBACK_ARG(2, 1) = MkIntTerm(i + 1);
      return true;
    }
  } else if (det)
    cut_fail();
  else {
    EXTRA_CBACK_ARG(2, 1) = MkIntTerm(i + 1);
    return false;
  }
}

static Int stream_property(USES_REGS1) { /* Init current_stream */
  Term t1 = Deref(ARG1);
  // Yap_DebugPlWrite(ARG1);fprintf(stderr,", ");
  // Yap_DebugPlWrite(ARG2);fprintf(stderr,"\n");

  /* make valgrind happy by always filling in memory */
  EXTRA_CBACK_ARG(2, 1) = MkIntTerm(0);
  if (!IsVarTerm(t1)) {
    Int i;
    xarg *args;

    i = Yap_CheckStream(t1, Input_Stream_f | Output_Stream_f | Append_Stream_f,
                        "current_stream/3");
    if (i < 0) {
      UNLOCK(GLOBAL_Stream[i].streamlock);
     cut_fail();
    }
    args = Yap_ArgListToVector(Deref(ARG2), stream_property_defs,
                               STREAM_PROPERTY_END);
    if (args == NULL) {
      UNLOCK(GLOBAL_Stream[i].streamlock);
      cut_fail();
    }
    if (do_stream_property(i, args PASS_REGS)) {
      UNLOCK(GLOBAL_Stream[i].streamlock);
      cut_succeed();
    } else {
      UNLOCK(GLOBAL_Stream[i].streamlock);
      cut_fail();
    }
  } else {
    return cont_stream_property(PASS_REGS1);
  }
}

#define SET_STREAM_DEFS()                                                      \
  PAR("alias", isatom, SET_STREAM_ALIAS),                                      \
      PAR("buffer", boolean, SET_STREAM_BUFFER),                               \
      PAR("buffer_size", nat, SET_STREAM_BUFFER_SIZE),                         \
      PAR("close_on_abort", boolean, SET_STREAM_CLOSE_ON_ABORT),               \
      PAR("encoding", isatom, SET_STREAM_ENCODING),                            \
      PAR("eof_action", isatom, SET_STREAM_EOF_ACTION),                        \
      PAR("file_name", isatom, SET_STREAM_FILE_NAME),                          \
      PAR("line_position", nat, SET_STREAM_LINE_POSITION),                     \
      PAR("newline", filler, SET_STREAM_NEWLINE),                              \
      PAR("record_position", isatom, SET_STREAM_RECORD_POSITION),              \
      PAR("representation_errors", isatom, SET_STREAM_REPRESENTATION_ERRORS),  \
      PAR("type", isatom, SET_STREAM_TYPE),                                    \
      PAR("tty", filler, SET_STREAM_TTY), PAR(NULL, ok, SET_STREAM_END)

#define PAR(x, y, z) z

typedef enum set_stream_enum_choices {
  SET_STREAM_DEFS()
} set_stream_enum_choices_t;

#undef PAR

#define PAR(x, y, z)                                                           \
  { x, y, z }

static const param_t set_stream_defs[] = {SET_STREAM_DEFS()};
#undef PAR

static bool do_set_stream(int sno,
                          Term opts USES_REGS) { /* Init current_stream */
  xarg *args;
  set_stream_enum_choices_t i;
  bool rc = true;

  args = Yap_ArgListToVector(opts, set_stream_defs, SET_STREAM_END);
  if (args == NULL) {
    UNLOCK(GLOBAL_Stream[sno].streamlock);
    return false;
  }
  for (i = 0; i < SET_STREAM_END; i++) {
    if (args[i].used) {
      Term t = args[i].tvalue;
      switch (i) {
      case SET_STREAM_ALIAS:
        rc = rc && Yap_AddAlias(AtomOfTerm(t), sno);
        break;
      case SET_STREAM_BUFFER:
        rc = rc && SetBuffering(sno, AtomOfTerm(t));
        break;
      case SET_STREAM_BUFFER_SIZE:
        rc = rc && SetBuffer(sno, IntegerOfTerm(t));
        break;
      case SET_STREAM_CLOSE_ON_ABORT:
        rc = rc &&
             SetCloseOnAbort(
                 sno, (args[SET_STREAM_CLOSE_ON_ABORT].tvalue == TermTrue));
        break;
      case SET_STREAM_ENCODING:
        GLOBAL_Stream[sno].encoding =
            enc_id(AtomOfTerm(args[SET_STREAM_ENCODING].tvalue)->StrOfAE);
        has_encoding(sno, args[SET_STREAM_ENCODING].tvalue PASS_REGS);
        break;
      case SET_STREAM_EOF_ACTION: {
        Term t2 = args[SET_STREAM_EOF_ACTION].tvalue;
        if (t2 == TermError) {
          GLOBAL_Stream[sno].status |= Eof_Error_Stream_f;
          GLOBAL_Stream[sno].status &= ~Reset_Eof_Stream_f;
        } else if (t2 == TermReset) {
          GLOBAL_Stream[sno].status |= GLOBAL_Stream[sno].status &=
              ~Eof_Error_Stream_f;
          GLOBAL_Stream[sno].status |= Reset_Eof_Stream_f;
        } else if (t2 == TermEOfCode) {
          GLOBAL_Stream[sno].status &= ~Eof_Error_Stream_f;
          GLOBAL_Stream[sno].status &= ~Reset_Eof_Stream_f;
        } else {
          LOCAL_Error_TYPE = DOMAIN_ERROR_OUT_OF_RANGE;
          LOCAL_ErrorMessage = "in set_stream/2:eof_action";
          rc = false;
        }
        break;
      case SET_STREAM_FILE_NAME:
        GLOBAL_Stream[sno].user_name = args[SET_STREAM_FILE_NAME].tvalue;
        break;
      case SET_STREAM_LINE_POSITION:
        GLOBAL_Stream[sno].linepos =
            IntegerOfTerm(args[SET_STREAM_FILE_NAME].tvalue);
        break;
      case SET_STREAM_NEWLINE:
        printf("not yet\n");
        break;
      case SET_STREAM_RECORD_POSITION:
        if (args[SET_STREAM_RECORD_POSITION].tvalue == TermTrue)
          GLOBAL_Stream[sno].status |= Seekable_Stream_f;
        else
          GLOBAL_Stream[sno].status &= ~Seekable_Stream_f;
        break;
      case SET_STREAM_REPRESENTATION_ERRORS: {
        Term t2 = args[SET_STREAM_EOF_ACTION].tvalue;
        if (t2 == TermXml) {
          GLOBAL_Stream[sno].status |= RepError_Xml_f;
          GLOBAL_Stream[sno].status &= ~RepError_Prolog_f;
        } else if (t2 == TermError) {
          GLOBAL_Stream[sno].status |= GLOBAL_Stream[sno].status &=
              ~RepError_Xml_f;
          GLOBAL_Stream[sno].status |= RepError_Prolog_f;
        } else if (t2 == TermEOfCode) {
          GLOBAL_Stream[sno].status &= ~RepError_Xml_f;
          GLOBAL_Stream[sno].status |= RepError_Prolog_f;
        } else {
          LOCAL_Error_TYPE = DOMAIN_ERROR_OUT_OF_RANGE;
          LOCAL_ErrorMessage = "in set_stream/2:eof_action";
          rc = false;
        }
      } break;
      case SET_STREAM_TYPE:
        rc &= stream_type(sno, args[SET_STREAM_TYPE].tvalue PASS_REGS);
        break;
      case SET_STREAM_TTY:
        rc &= stream_tty(sno, args[SET_STREAM_TTY].tvalue PASS_REGS);
        break;
      case SET_STREAM_END:
        rc = false;
        break;
      }
      }
    }
  }
  UNLOCK(GLOBAL_Stream[sno].streamlock);
  return rc;
}

static Int set_stream(USES_REGS1) { /* Init current_stream */
  int sno =
      Yap_CheckStream(ARG1, Input_Stream_f | Output_Stream_f | Append_Stream_f,
                      "set_stream_position/2");
  if (sno < 0) {
    return (FALSE);
  }
  return do_set_stream(sno, Deref(ARG2) PASS_REGS);
}

/*
 * Called when you want to close all open streams, except for stdin, stdout
 * and stderr
 */
void Yap_CloseStreams(int loud) {
  CACHE_REGS
  int sno;
  for (sno = 3; sno < MaxStreams; ++sno) {
    if (GLOBAL_Stream[sno].status & Free_Stream_f)
      continue;
    if ((GLOBAL_Stream[sno].status & Popen_Stream_f))
      pclose(GLOBAL_Stream[sno].file);
#if _MSC_VER || defined(__MINGW32__)
    if (GLOBAL_Stream[sno].status & Pipe_Stream_f)
      CloseHandle(GLOBAL_Stream[sno].u.pipe.hdl);
#else
    if (GLOBAL_Stream[sno].status & (Pipe_Stream_f | Socket_Stream_f))
      close(GLOBAL_Stream[sno].u.pipe.fd);
#endif
    else if (GLOBAL_Stream[sno].status & (Socket_Stream_f)) {
      Yap_CloseSocket(GLOBAL_Stream[sno].u.socket.fd,
                      GLOBAL_Stream[sno].u.socket.flags,
                      GLOBAL_Stream[sno].u.socket.domain);
    } else if (GLOBAL_Stream[sno].status & InMemory_Stream_f) {
      if (GLOBAL_Stream[sno].u.mem_string.src == MEM_BUF_CODE) {
        Yap_FreeAtomSpace(GLOBAL_Stream[sno].u.mem_string.buf);
      } else if (GLOBAL_Stream[sno].u.mem_string.src == MEM_BUF_MALLOC) {
        free(GLOBAL_Stream[sno].u.mem_string.buf);
      }
    } else if (!(GLOBAL_Stream[sno].status & Null_Stream_f)) {
      fclose(GLOBAL_Stream[sno].file);
    } else {
      if (loud)
        fprintf(Yap_stderr, "%% YAP Error: while closing stream: %s\n",
                RepAtom(GLOBAL_Stream[sno].name)->StrOfAE);
    }
    if (LOCAL_c_input_stream == sno) {
      LOCAL_c_input_stream = StdInStream;
    } else if (LOCAL_c_output_stream == sno) {
      LOCAL_c_output_stream = StdOutStream;
    }
    GLOBAL_Stream[sno].status = Free_Stream_f;
  }
}

static void
CloseStream(int sno) {
  CACHE_REGS
  if (!(GLOBAL_Stream[sno].status &
        (Null_Stream_f | Socket_Stream_f | InMemory_Stream_f | Pipe_Stream_f)))
    fclose(GLOBAL_Stream[sno].file);
#if HAVE_SOCKET
  else if (GLOBAL_Stream[sno].status & (Socket_Stream_f)) {
    Yap_CloseSocket(GLOBAL_Stream[sno].u.socket.fd,
                    GLOBAL_Stream[sno].u.socket.flags,
                    GLOBAL_Stream[sno].u.socket.domain);
  }
#endif
  else if (GLOBAL_Stream[sno].status & Pipe_Stream_f) {
#if _MSC_VER || defined(__MINGW32__)
    CloseHandle(GLOBAL_Stream[sno].u.pipe.hdl);
#else
    close(GLOBAL_Stream[sno].u.pipe.fd);
#endif
  } else if (GLOBAL_Stream[sno].status & (InMemory_Stream_f)) {
    if (GLOBAL_Stream[sno].u.mem_string.src == MEM_BUF_CODE)
      Yap_FreeAtomSpace(GLOBAL_Stream[sno].u.mem_string.buf);
    else if (GLOBAL_Stream[sno].u.mem_string.src == MEM_BUF_MALLOC) {
      free(GLOBAL_Stream[sno].u.mem_string.buf);
    }
  }
  GLOBAL_Stream[sno].status = Free_Stream_f;
  Yap_DeleteAliases(sno);
  if (LOCAL_c_input_stream == sno) {
    LOCAL_c_input_stream = StdInStream;
  } else if (LOCAL_c_output_stream == sno) {
    LOCAL_c_output_stream = StdOutStream;
  } else if (LOCAL_c_error_stream == sno) {
    LOCAL_c_error_stream = StdErrStream;
  }
  /*  if (st->status == Socket_Stream_f|Input_Stream_f|Output_Stream_f) {
    Yap_CloseSocket();
  }
  */
}

void Yap_CloseStream(int sno) { CloseStream(sno); }

void Yap_ReleaseStream(int sno) {
  CACHE_REGS
  GLOBAL_Stream[sno].status = Free_Stream_f;
  Yap_DeleteAliases(sno);
  if (LOCAL_c_input_stream == sno) {
    LOCAL_c_input_stream = StdInStream;
  } else if (LOCAL_c_output_stream == sno) {
    LOCAL_c_output_stream = StdOutStream;
  } else if (LOCAL_c_error_stream == sno) {
    LOCAL_c_error_stream = StdErrStream;
  }
  /*  if (st->status == Socket_Stream_f|Input_Stream_f|Output_Stream_f) {
    Yap_CloseSocket();
  }
  */
}

static Int current_input(USES_REGS1) { /* current_input(?Stream) */
  Term t1 = Deref(ARG1);
  if (IsVarTerm(t1)) {
    Term t = Yap_MkStream(LOCAL_c_input_stream);
    YapBind(VarOfTerm(t1), t);
    return TRUE;
  } else if (!IsApplTerm(t1) || FunctorOfTerm(t1) != FunctorStream ||
             !IsIntTerm((t1 = ArgOfTerm(1, t1)))) {
    Yap_Error(DOMAIN_ERROR_STREAM, t1, "current_input/1");
    return FALSE;
  } else {
    return LOCAL_c_input_stream == IntOfTerm(t1);
  }
}

/** @pred  set_input(+ _S_) is iso
 * Set stream  _S_ as the current input stream. Predicates like read/1
 * and get/1 will start using stream  _S_ by default.
 *
 *
 * @param Input-mode stream
 *
 */
static Int set_input(USES_REGS1) { /* '$show_stream_position'(+Stream,Pos) */
  int sno = Yap_CheckStream(ARG1, Input_Stream_f, "set_input/1");
  if (sno < 0)
    return false;
  LOCAL_c_input_stream = sno;
  UNLOCK(GLOBAL_Stream[sno].streamlock);
  return true;
}

static Int current_output(USES_REGS1) { /* current_output(?Stream) */
  Term t1 = Deref(ARG1);
  if (IsVarTerm(t1)) {
    Term t = Yap_MkStream(LOCAL_c_output_stream);
    YapBind(VarOfTerm(t1), t);
    return TRUE;
  } else if (!IsApplTerm(t1) || FunctorOfTerm(t1) != FunctorStream ||
             !IsIntTerm((t1 = ArgOfTerm(1, t1)))) {
    Yap_Error(DOMAIN_ERROR_STREAM, t1, "current_output/1");
    return FALSE;
  } else {
    return (LOCAL_c_output_stream == IntOfTerm(t1));
  }
}

/** @pred  set_input(+ _S_) is iso
 * Set stream  _S_ as the current input stream. Predicates like read/1
 * and get/1 will start using stream  _S_ by default.
 *
 *
 * @param Output-mode stream
 *
 */
static Int set_output(USES_REGS1) { /* '$show_stream_position'(+Stream,Pos) */
  int sno =
      Yap_CheckStream(ARG1, Output_Stream_f | Append_Stream_f, "set_output/2");
  if (sno < 0)
    return false;
  LOCAL_c_output_stream = sno;
  UNLOCK(GLOBAL_Stream[sno].streamlock);
  return true;
}

static Int p_user_file_name(USES_REGS1) {
  Term tout;
  int sno =
      Yap_CheckStream(ARG1, Input_Stream_f | Output_Stream_f | Append_Stream_f,
                      "user_file_name/2");
  if (sno < 0)
    return (FALSE);
  tout = Yap_StreamUserName(sno);
  UNLOCK(GLOBAL_Stream[sno].streamlock);
  return (Yap_unify_constant(ARG2, tout));
}

static Int p_file_name(USES_REGS1) {
  Term tout;
  int sno = Yap_CheckStream(
      ARG1, Input_Stream_f | Output_Stream_f | Append_Stream_f, "file_name/2");
  if (sno < 0)
    return (FALSE);
#if HAVE_SOCKET
  if (GLOBAL_Stream[sno].status & Socket_Stream_f)
    tout = MkAtomTerm(AtomSocket);
  else
#endif
      if (GLOBAL_Stream[sno].status & Pipe_Stream_f)
    tout = MkAtomTerm(AtomPipe);
  else if (GLOBAL_Stream[sno].status & InMemory_Stream_f)
    tout = MkAtomTerm(AtomCharsio);
  else
    tout = MkAtomTerm(GLOBAL_Stream[sno].name);
  UNLOCK(GLOBAL_Stream[sno].streamlock);
  return Yap_unify_constant(ARG2, tout);
}

static Int p_cur_line_no(USES_REGS1) { /* '$current_line_number'(+Stream,-N) */
  Term tout;
  int sno =
      Yap_CheckStream(ARG1, Input_Stream_f | Output_Stream_f | Append_Stream_f,
                      "current_line_number/2");
  if (sno < 0)
    return (FALSE);
  /* one has to be somewhat more careful because of terminals */
  if (GLOBAL_Stream[sno].status & Tty_Stream_f) {
    Int no = 1;
    int i;
    Atom my_stream;
#if HAVE_SOCKET
    if (GLOBAL_Stream[sno].status & Socket_Stream_f)
      my_stream = AtomSocket;
    else
#endif
        if (GLOBAL_Stream[sno].status & Pipe_Stream_f)
      my_stream = AtomPipe;
    else if (GLOBAL_Stream[sno].status & InMemory_Stream_f)
      my_stream = AtomCharsio;
    else
      my_stream = GLOBAL_Stream[sno].name;
    for (i = 0; i < MaxStreams; i++) {
      if (!(GLOBAL_Stream[i].status & (Free_Stream_f | Socket_Stream_f |
                                       Pipe_Stream_f | InMemory_Stream_f)) &&
          GLOBAL_Stream[i].name == my_stream)
        no += GLOBAL_Stream[i].linecount - 1;
    }
    tout = MkIntTerm(no);
  } else
    tout = MkIntTerm(GLOBAL_Stream[sno].linecount);
  UNLOCK(GLOBAL_Stream[sno].streamlock);
  return (Yap_unify_constant(ARG2, tout));
}

static Int p_line_position(USES_REGS1) { /* '$line_position'(+Stream,-N) */
  Term tout;
  int sno =
      Yap_CheckStream(ARG1, Input_Stream_f | Output_Stream_f | Append_Stream_f,
                      "line_position/2");
  if (sno < 0)
    return (FALSE);
  if (GLOBAL_Stream[sno].status & Tty_Stream_f) {
    Int no = 0;
    int i;
    Atom my_stream = GLOBAL_Stream[sno].name;
    for (i = 0; i < MaxStreams; i++) {
      if (!(GLOBAL_Stream[i].status & Free_Stream_f) &&
          GLOBAL_Stream[i].name == my_stream)
        no += GLOBAL_Stream[i].linepos;
    }
    tout = MkIntTerm(no);
  } else
    tout = MkIntTerm(GLOBAL_Stream[sno].linepos);
  UNLOCK(GLOBAL_Stream[sno].streamlock);
  return (Yap_unify_constant(ARG2, tout));
}

static Int p_character_count(USES_REGS1) { /* '$character_count'(+Stream,-N) */
  Term tout;
  int sno =
      Yap_CheckStream(ARG1, Input_Stream_f | Output_Stream_f | Append_Stream_f,
                      "character_count/2");
  if (sno < 0)
    return (FALSE);
  if (GLOBAL_Stream[sno].status & Tty_Stream_f) {
    Int no = 0;
    int i;
    Atom my_stream = GLOBAL_Stream[sno].name;
    for (i = 0; i < MaxStreams; i++) {
      if (!(GLOBAL_Stream[i].status & Free_Stream_f) &&
          GLOBAL_Stream[i].name == my_stream)
        no += GLOBAL_Stream[i].charcount;
    }
    tout = MkIntTerm(no);
  } else if (GLOBAL_Stream[sno].status & Null_Stream_f)
    tout = MkIntTerm(GLOBAL_Stream[sno].charcount);
  else
    tout = MkIntTerm(ftell(GLOBAL_Stream[sno].file));
  UNLOCK(GLOBAL_Stream[sno].streamlock);
  return (Yap_unify_constant(ARG2, tout));
}

static Int
    p_show_stream_flags(USES_REGS1) { /* '$show_stream_flags'(+Stream,Pos) */
  Term tout;
  int sno =
      Yap_CheckStream(ARG1, Input_Stream_f | Output_Stream_f | Append_Stream_f,
                      "stream_property/2");
  if (sno < 0)
    return (FALSE);
  tout = MkIntTerm(GLOBAL_Stream[sno].status);
  UNLOCK(GLOBAL_Stream[sno].streamlock);
  return (Yap_unify(ARG2, tout));
}

Term Yap_StreamPosition(int sno) { return StreamPosition(sno); }

static Int p_show_stream_position(
    USES_REGS1) { /* '$show_stream_position'(+Stream,Pos) */
  Term tout;
  int sno =
      Yap_CheckStream(ARG1, Input_Stream_f | Output_Stream_f | Append_Stream_f,
                      "stream_position/2");
  if (sno < 0)
    return (FALSE);
  tout = StreamPosition(sno);
  UNLOCK(GLOBAL_Stream[sno].streamlock);
  return Yap_unify(ARG2, tout);
}

static Int
    set_stream_position(USES_REGS1) { /* '$set_stream_position'(+Stream,Pos) */
  Term tin, tp;
  Int char_pos;
  int sno =
      Yap_CheckStream(ARG1, Input_Stream_f | Output_Stream_f | Append_Stream_f,
                      "set_stream_position/2");
  if (sno < 0) {
    return (FALSE);
  }
  tin = Deref(ARG2);
  if (IsVarTerm(tin)) {
    UNLOCK(GLOBAL_Stream[sno].streamlock);
    Yap_Error(INSTANTIATION_ERROR, tin, "set_stream_position/2");
    return (FALSE);
  } else if (!(IsApplTerm(tin))) {
    UNLOCK(GLOBAL_Stream[sno].streamlock);
    Yap_Error(DOMAIN_ERROR_STREAM_POSITION, tin, "set_stream_position/2");
    return (FALSE);
  }
  if (FunctorOfTerm(tin) == FunctorStreamPos) {
    if (IsVarTerm(tp = ArgOfTerm(1, tin))) {
      UNLOCK(GLOBAL_Stream[sno].streamlock);
      Yap_Error(INSTANTIATION_ERROR, tp, "set_stream_position/2");
      return (FALSE);
    } else if (!IsIntTerm(tp)) {
      UNLOCK(GLOBAL_Stream[sno].streamlock);
      Yap_Error(DOMAIN_ERROR_STREAM_POSITION, tin, "set_stream_position/2");
      return (FALSE);
    }
    if (!(GLOBAL_Stream[sno].status & Seekable_Stream_f)) {
      UNLOCK(GLOBAL_Stream[sno].streamlock);
      Yap_Error(PERMISSION_ERROR_REPOSITION_STREAM, ARG1,
                "set_stream_position/2");
      return (FALSE);
    }
    char_pos = IntOfTerm(tp);
    if (IsVarTerm(tp = ArgOfTerm(2, tin))) {
      UNLOCK(GLOBAL_Stream[sno].streamlock);
      Yap_Error(INSTANTIATION_ERROR, tp, "set_stream_position/2");
      return (FALSE);
    } else if (!IsIntTerm(tp)) {
      UNLOCK(GLOBAL_Stream[sno].streamlock);
      Yap_Error(DOMAIN_ERROR_STREAM_POSITION, tin, "set_stream_position/2");
      return (FALSE);
    }
    GLOBAL_Stream[sno].charcount = char_pos;
    GLOBAL_Stream[sno].linecount = IntOfTerm(tp);
    if (IsVarTerm(tp = ArgOfTerm(3, tin))) {
      UNLOCK(GLOBAL_Stream[sno].streamlock);
      Yap_Error(INSTANTIATION_ERROR, tp, "set_stream_position/2");
      return (FALSE);
    } else if (!IsIntTerm(tp)) {
      UNLOCK(GLOBAL_Stream[sno].streamlock);
      Yap_Error(DOMAIN_ERROR_STREAM_POSITION, tin, "set_stream_position/2");
      return (FALSE);
    }
    GLOBAL_Stream[sno].linepos = IntOfTerm(tp);
    if (fseek(GLOBAL_Stream[sno].file, (long)(char_pos), 0) == -1) {
      UNLOCK(GLOBAL_Stream[sno].streamlock);
      Yap_Error(SYSTEM_ERROR_INTERNAL, tp, "fseek failed for set_stream_position/2");
      return (FALSE);
    }
    GLOBAL_Stream[sno].stream_getc = PlGetc;
    GLOBAL_Stream[sno].stream_gets = PlGetsFunc();
  } else if (FunctorOfTerm(tin) == FunctorStreamEOS) {
    if (IsVarTerm(tp = ArgOfTerm(1, tin))) {
      UNLOCK(GLOBAL_Stream[sno].streamlock);
      Yap_Error(INSTANTIATION_ERROR, tp, "set_stream_position/2");
      return (FALSE);
    } else if (tp != MkAtomTerm(AtomAt)) {
      UNLOCK(GLOBAL_Stream[sno].streamlock);
      Yap_Error(DOMAIN_ERROR_STREAM_POSITION, tin, "set_stream_position/2");
      return (FALSE);
    }
    if (!(GLOBAL_Stream[sno].status & Seekable_Stream_f)) {
      UNLOCK(GLOBAL_Stream[sno].streamlock);
      PlIOError(PERMISSION_ERROR_REPOSITION_STREAM, ARG1,
                "set_stream_position/2");
      return (FALSE);
    }
    if (fseek(GLOBAL_Stream[sno].file, 0L, SEEK_END) == -1) {
      UNLOCK(GLOBAL_Stream[sno].streamlock);
      PlIOError(SYSTEM_ERROR_INTERNAL, tp, "fseek failed for set_stream_position/2: %s",
                strerror(errno));
      return (FALSE);
    }
    GLOBAL_Stream[sno].stream_getc = PlGetc;
    GLOBAL_Stream[sno].stream_gets = PlGetsFunc();
    /* reset the counters */
    GLOBAL_Stream[sno].linepos = 0;
    GLOBAL_Stream[sno].linecount = 1;
    GLOBAL_Stream[sno].charcount = 0;
  }
  UNLOCK(GLOBAL_Stream[sno].streamlock);
  return (TRUE);
}

#if HAVE_SELECT
/* stream_select(+Streams,+TimeOut,-Result)      */
static Int p_stream_select(USES_REGS1) {
  Term t1 = Deref(ARG1), t2;
  fd_set readfds, writefds, exceptfds;
  struct timeval timeout, *ptime;

#if _MSC_VER
  u_int fdmax = 0;
#else
  int fdmax = 0;
#endif
  Term tout = TermNil, ti, Head;

  if (IsVarTerm(t1)) {
    Yap_Error(INSTANTIATION_ERROR, t1, "stream_select/3");
    return FALSE;
  }
  if (!IsPairTerm(t1)) {
    Yap_Error(TYPE_ERROR_LIST, t1, "stream_select/3");
    return (FALSE);
  }
  FD_ZERO(&readfds);
  FD_ZERO(&writefds);
  FD_ZERO(&exceptfds);
  ti = t1;
  while (ti != TermNil) {
#if _MSC_VER
    u_int fd;
#else
    int fd;
#endif
    int sno;

    Head = HeadOfTerm(ti);
    sno = Yap_CheckStream(Head, Input_Stream_f, "stream_select/3");
    if (sno < 0)
      return (FALSE);
    fd = GetStreamFd(sno);
    FD_SET(fd, &readfds);
    UNLOCK(GLOBAL_Stream[sno].streamlock);
    if (fd > fdmax)
      fdmax = fd;
    ti = TailOfTerm(ti);
  }
  t2 = Deref(ARG2);
  if (IsVarTerm(t2)) {
    Yap_Error(INSTANTIATION_ERROR, t2, "stream_select/3");
    return (FALSE);
  }
  if (IsAtomTerm(t2)) {
    if (t2 == MkAtomTerm(AtomOff)) {
      /* wait indefinitely */
      ptime = NULL;
    } else {
      Yap_Error(DOMAIN_ERROR_TIMEOUT_SPEC, t1, "stream_select/3");
      return (FALSE);
    }
  } else {
    Term t21, t22;

    if (!IsApplTerm(t2) || FunctorOfTerm(t2) != FunctorModule) {
      Yap_Error(DOMAIN_ERROR_TIMEOUT_SPEC, t2, "stream_select/3");
      return (FALSE);
    }
    t21 = ArgOfTerm(1, t2);
    if (IsVarTerm(t21)) {
      Yap_Error(INSTANTIATION_ERROR, t2, "stream_select/3");
      return (FALSE);
    }
    if (!IsIntegerTerm(t21)) {
      Yap_Error(DOMAIN_ERROR_TIMEOUT_SPEC, t2, "stream_select/3");
      return (FALSE);
    }
    timeout.tv_sec = IntegerOfTerm(t21);
    if (timeout.tv_sec < 0) {
      Yap_Error(DOMAIN_ERROR_TIMEOUT_SPEC, t2, "stream_select/3");
      return (FALSE);
    }
    t22 = ArgOfTerm(2, t2);
    if (IsVarTerm(t22)) {
      Yap_Error(INSTANTIATION_ERROR, t2, "stream_select/3");
      return (FALSE);
    }
    if (!IsIntegerTerm(t22)) {
      Yap_Error(DOMAIN_ERROR_TIMEOUT_SPEC, t2, "stream_select/3");
      return (FALSE);
    }
    timeout.tv_usec = IntegerOfTerm(t22);
    if (timeout.tv_usec < 0) {
      Yap_Error(DOMAIN_ERROR_TIMEOUT_SPEC, t2, "stream_select/3");
      return (FALSE);
    }
    ptime = &timeout;
  }
  /* do the real work */
  if (select(fdmax + 1, &readfds, &writefds, &exceptfds, ptime) < 0) {
#if HAVE_STRERROR
    Yap_Error(SYSTEM_ERROR_INTERNAL, TermNil, "stream_select/3 (select: %s)",
              strerror(errno));
#else
    Yap_Error(SYSTEM_ERROR_INTERNAL, TermNil, "stream_select/3 (select)");
#endif
  }
  while (t1 != TermNil) {
    int fd;
    int sno;

    Head = HeadOfTerm(t1);
    sno = Yap_CheckStream(Head, Input_Stream_f, "stream_select/3");
    fd = GetStreamFd(sno);
    if (FD_ISSET(fd, &readfds))
      tout = MkPairTerm(Head, tout);
    else
      tout = MkPairTerm(TermNil, tout);
    UNLOCK(GLOBAL_Stream[sno].streamlock);
    t1 = TailOfTerm(t1);
  }
  /* we're done, just pass the info back */
  return (Yap_unify(ARG3, tout));
}
#endif

Int Yap_StreamToFileNo(Term t) {
  int sno =
      Yap_CheckStream(t, (Input_Stream_f | Output_Stream_f), "StreamToFileNo");
  if (GLOBAL_Stream[sno].status & Pipe_Stream_f) {
    UNLOCK(GLOBAL_Stream[sno].streamlock);
#if _MSC_VER || defined(__MINGW32__)
    return ((Int)(GLOBAL_Stream[sno].u.pipe.hdl));
#else
    return (GLOBAL_Stream[sno].u.pipe.fd);
#endif
#if HAVE_SOCKET
  } else if (GLOBAL_Stream[sno].status & Socket_Stream_f) {
    UNLOCK(GLOBAL_Stream[sno].streamlock);
    return (GLOBAL_Stream[sno].u.socket.fd);
#endif
  } else if (GLOBAL_Stream[sno].status & (Null_Stream_f)) {
    UNLOCK(GLOBAL_Stream[sno].streamlock);
    return (-1);
  } else {
    UNLOCK(GLOBAL_Stream[sno].streamlock);
    return (fileno(GLOBAL_Stream[sno].file));
  }
}

static Int p_stream(USES_REGS1) {
  Term in = Deref(ARG1);
  if (IsVarTerm(in))
    return (FALSE);
  if (IsAtomTerm(in))
    return (Yap_CheckAlias(AtomOfTerm(in)) >= 0);
  if (IsApplTerm(in))
    return (FunctorOfTerm(in) == FunctorStream);
  return (FALSE);
}

FILE *Yap_FileDescriptorFromStream(Term t) {
  int sno = Yap_CheckStream(t, Input_Stream_f | Output_Stream_f,
                            "FileDescriptorFromStream");
  FILE *rc;
  if (sno < 0)
    return NULL;
  if (GLOBAL_Stream[sno].status &
      (Null_Stream_f | Socket_Stream_f | Pipe_Stream_f | Free_Stream_f))
    rc = NULL;
  else
    rc = GLOBAL_Stream[sno].file;
  UNLOCK(GLOBAL_Stream[sno].streamlock);
  return rc;
}

void

Yap_InitBackIO (void)
{
  Yap_InitCPredBack("stream_property", 2, 1, stream_property,
                    cont_stream_property, SafePredFlag | SyncPredFlag);
}

void Yap_InitIOStreams(void) {
  Yap_InitCPred("$stream_flags", 2, p_stream_flags,
                SafePredFlag | SyncPredFlag | HiddenPredFlag);
  Yap_InitCPred("$check_stream", 2, p_check_stream,
                SafePredFlag | SyncPredFlag | HiddenPredFlag);
  Yap_InitCPred("$check_stream", 1, p_check_if_stream,
                SafePredFlag | SyncPredFlag | HiddenPredFlag | HiddenPredFlag);
  Yap_InitCPred("$current_line_number", 2, p_cur_line_no,
                SafePredFlag | SyncPredFlag | HiddenPredFlag);
  Yap_InitCPred("$line_position", 2, p_line_position,
                SafePredFlag | SyncPredFlag | HiddenPredFlag);
  Yap_InitCPred("$character_count", 2, p_character_count,
                SafePredFlag | SyncPredFlag | HiddenPredFlag);
  Yap_InitCPred("$show_stream_flags", 2, p_show_stream_flags,
                SafePredFlag | SyncPredFlag | HiddenPredFlag);
  Yap_InitCPred("$user_file_name", 2, p_user_file_name,
                SafePredFlag | SyncPredFlag),
      Yap_InitCPred("$file_name", 2, p_file_name, SafePredFlag | SyncPredFlag),
      Yap_InitCPred("current_input", 1, current_input,
                    SafePredFlag | SyncPredFlag);
  Yap_InitCPred("current_output", 1, current_output,
                SafePredFlag | SyncPredFlag);
  Yap_InitCPred("set_input", 1, set_input, SafePredFlag | SyncPredFlag);
  Yap_InitCPred("set_output", 1, set_output, SafePredFlag | SyncPredFlag);
  Yap_InitCPred("$stream", 1, p_stream, SafePredFlag | TestPredFlag);
  Yap_InitCPred("$stream", 1, p_stream, SafePredFlag | TestPredFlag);
#if HAVE_SELECT
  Yap_InitCPred("stream_select", 3, p_stream_select,
                SafePredFlag | SyncPredFlag);
#endif
  Yap_InitCPred("$show_stream_position", 2, p_show_stream_position,
                SafePredFlag | SyncPredFlag | HiddenPredFlag);
  Yap_InitCPred("set_stream_position", 2, set_stream_position,
                SafePredFlag | SyncPredFlag);
  Yap_InitCPred("set_stream", 2, set_stream, SafePredFlag | SyncPredFlag);
}
