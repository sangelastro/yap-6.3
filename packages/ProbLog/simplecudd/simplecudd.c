/******************************************************************************\
*                                                                              *
*    SimpleCUDD library (www.cs.kuleuven.be/~theo/tools/simplecudd.html)       *
*  SimpleCUDD was developed at Katholieke Universiteit Leuven(www.kuleuven.be) *
*                                                                              *
*  Copyright T. Mantadelis and Katholieke Universiteit Leuven 2008             *
*                                                                              *
*  Author: Theofrastos Mantadelis                                              *
*  File: simplecudd.c                                                          *
*                                                                              *
********************************************************************************
*                                                                              *
*        The "Artistic License"                                                *
*                                                                              *
*         Preamble                                                             *
*                                                                              *
* The intent of this document is to state the conditions under which a         *
* Package may be copied, such that the Copyright Holder maintains some         *
* semblance of artistic control over the development of the package,           *
* while giving the users of the package the right to use and distribute        *
* the Package in a more-or-less customary fashion, plus the right to make      *
* reasonable modifications.                                                    *
*                                                                              *
* Definitions:                                                                 *
*                                                                              *
*   "Package" refers to the collection of files distributed by the             *
*   Copyright Holder, and derivatives of that collection of files              *
*   created through textual modification.                                      *
*                                                                              *
*   "Standard Version" refers to such a Package if it has not been             *
*   modified, or has been modified in accordance with the wishes               *
*   of the Copyright Holder as specified below.                                *
*                                                                              *
*   "Copyright Holder" is whoever is named in the copyright or                 *
*   copyrights for the package.                                                *
*                                                                              *
*   "You" is you, if you're thinking about copying or distributing             *
*   this Package.                                                              *
*                                                                              *
*   "Reasonable copying fee" is whatever you can justify on the                *
*   basis of media cost, duplication charges, time of people involved,         *
*   and so on.  (You will not be required to justify it to the                 *
*   Copyright Holder, but only to the computing community at large             *
*   as a market that must bear the fee.)                                       *
*                                                                              *
*   "Freely Available" means that no fee is charged for the item               *
*   itself, though there may be fees involved in handling the item.            *
*   It also means that recipients of the item may redistribute it              *
*   under the same conditions they received it.                                *
*                                                                              *
* 1. You may make and give away verbatim copies of the source form of the      *
* Standard Version of this Package without restriction, provided that you      *
* duplicate all of the original copyright notices and associated disclaimers.  *
*                                                                              *
* 2. You may apply bug fixes, portability fixes and other modifications        *
* derived from the Public Domain or from the Copyright Holder.  A Package      *
* modified in such a way shall still be considered the Standard Version.       *
*                                                                              *
* 3. You may otherwise modify your copy of this Package in any way, provided   *
* that you insert a prominent notice in each changed file stating how and      *
* when you changed that file, and provided that you do at least ONE of the     *
* following:                                                                   *
*                                                                              *
*    a) place your modifications in the Public Domain or otherwise make them   *
*    Freely Available, such as by posting said modifications to Usenet or      *
*    an equivalent medium, or placing the modifications on a major archive     *
*    site such as uunet.uu.net, or by allowing the Copyright Holder to include *
*    your modifications in the Standard Version of the Package.                *
*                                                                              *
*    b) use the modified Package only within your corporation or organization. *
*                                                                              *
*    c) rename any non-standard executables so the names do not conflict       *
*    with standard executables, which must also be provided, and provide       *
*    a separate manual page for each non-standard executable that clearly      *
*    documents how it differs from the Standard Version.                       *
*                                                                              *
*    d) make other distribution arrangements with the Copyright Holder.        *
*                                                                              *
* 4. You may distribute the programs of this Package in object code or         *
* executable form, provided that you do at least ONE of the following:         *
*                                                                              *
*    a) distribute a Standard Version of the executables and library files,    *
*    together with instructions (in the manual page or equivalent) on where    *
*    to get the Standard Version.                                              *
*                                                                              *
*    b) accompany the distribution with the machine-readable source of         *
*    the Package with your modifications.                                      *
*                                                                              *
*    c) give non-standard executables non-standard names, and clearly          *
*    document the differences in manual pages (or equivalent), together        *
*    with instructions on where to get the Standard Version.                   *
*                                                                              *
*    d) make other distribution arrangements with the Copyright Holder.        *
*                                                                              *
* 5. You may charge a reasonable copying fee for any distribution of this      *
* Package.  You may charge any fee you choose for support of this              *
* Package.  You may not charge a fee for this Package itself.  However,        *
* you may distribute this Package in aggregate with other (possibly            *
* commercial) programs as part of a larger (possibly commercial) software      *
* distribution provided that you do not advertise this Package as a            *
* product of your own.  You may embed this Package's interpreter within        *
* an executable of yours (by linking); this shall be construed as a mere       *
* form of aggregation, provided that the complete Standard Version of the      *
* interpreter is so embedded.                                                  *
*                                                                              *
* 6. The scripts and library files supplied as input to or produced as         *
* output from the programs of this Package do not automatically fall           *
* under the copyright of this Package, but belong to whoever generated         *
* them, and may be sold commercially, and may be aggregated with this          *
* Package.  If such scripts or library files are aggregated with this          *
* Package via the so-called "undump" or "unexec" methods of producing a        *
* binary executable image, then distribution of such an image shall            *
* neither be construed as a distribution of this Package nor shall it          *
* fall under the restrictions of Paragraphs 3 and 4, provided that you do      *
* not represent such an executable image as a Standard Version of this         *
* Package.                                                                     *
*                                                                              *
* 7. C subroutines (or comparably compiled subroutines in other                *
* languages) supplied by you and linked into this Package in order to          *
* emulate subroutines and variables of the language defined by this            *
* Package shall not be considered part of this Package, but are the            *
* equivalent of input as in Paragraph 6, provided these subroutines do         *
* not change the language in any way that would cause it to fail the           *
* regression tests for the language.                                           *
*                                                                              *
* 8. Aggregation of this Package with a commercial distribution is always      *
* permitted provided that the use of this Package is embedded; that is,        *
* when no overt attempt is made to make this Package's interfaces visible      *
* to the end user of the commercial distribution.  Such use shall not be       *
* construed as a distribution of this Package.                                 *
*                                                                              *
* 9. The name of the Copyright Holder may not be used to endorse or promote    *
* products derived from this software without specific prior written           *
* permission.                                                                  *
*                                                                              *
* 10. THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR              *
* IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED               *
* WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.          *
*                                                                              *
*         The End                                                              *
*                                                                              *
\******************************************************************************/


#include "simplecudd.h"

/* BDD manager initialization */

int _debug = 0;
int _RapidLoad = 0;
int _maxbufsize = 0;

DdManager* simpleBDDinit(int varcnt) {
  DdManager *temp;
  temp = Cudd_Init(varcnt, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
  Cudd_AutodynEnable(temp, CUDD_REORDER_GROUP_SIFT);
  Cudd_SetMaxCacheHard(temp, 1024*1024*1024);
  Cudd_SetLooseUpTo(temp, 1024*1024*512);
  if (_debug) Cudd_EnableReorderingReporting(temp);
  return temp;
}

/* BDD tree travesrsing */

DdNode* HighNodeOf(DdManager *manager, DdNode *node) {
  DdNode *tmp;
  if (IsHigh(manager, node)) return HIGH(manager);
  if (IsLow(manager, node)) return LOW(manager);
  tmp = Cudd_Regular(node);
  if (Cudd_IsComplement(node)) return NOT(tmp->type.kids.T);
  return tmp->type.kids.T;
}

DdNode* LowNodeOf(DdManager *manager, DdNode *node) {
  DdNode *tmp;
  if (IsHigh(manager, node)) return HIGH(manager);
  if (IsLow(manager, node)) return LOW(manager);
  tmp = Cudd_Regular(node);
  if (Cudd_IsComplement(node)) return NOT(tmp->type.kids.E);
  return tmp->type.kids.E;
}

/* BDD tree generation */

DdNode* D_BDDAnd(DdManager *manager, DdNode *bdd1, DdNode *bdd2) {
  DdNode *tmp;
  tmp = Cudd_bddAnd(manager, bdd1, bdd2);
  Cudd_Ref(tmp);
  Cudd_RecursiveDeref(manager, bdd2);
  return tmp;
}

DdNode* D_BDDNand(DdManager *manager, DdNode *bdd1, DdNode *bdd2) {
  DdNode *tmp;
  tmp = Cudd_bddNand(manager, bdd1, bdd2);
  Cudd_Ref(tmp);
  Cudd_RecursiveDeref(manager, bdd2);
  return tmp;
}

DdNode* D_BDDOr(DdManager *manager, DdNode *bdd1, DdNode *bdd2) {
  DdNode *tmp;
  tmp = Cudd_bddOr(manager, bdd1, bdd2);
  Cudd_Ref(tmp);
  Cudd_RecursiveDeref(manager, bdd2);
  return tmp;
}

DdNode* D_BDDNor(DdManager *manager, DdNode *bdd1, DdNode *bdd2) {
  DdNode *tmp;
  tmp = Cudd_bddNor(manager, bdd1, bdd2);
  Cudd_Ref(tmp);
  Cudd_RecursiveDeref(manager, bdd2);
  return tmp;
}

DdNode* D_BDDXor(DdManager *manager, DdNode *bdd1, DdNode *bdd2) {
  DdNode *tmp;
  tmp = Cudd_bddXor(manager, bdd1, bdd2);
  Cudd_Ref(tmp);
  Cudd_RecursiveDeref(manager, bdd2);
  return tmp;
}

DdNode* D_BDDXnor(DdManager *manager, DdNode *bdd1, DdNode *bdd2) {
  DdNode *tmp;
  tmp = Cudd_bddXnor(manager, bdd1, bdd2);
  Cudd_Ref(tmp);
  Cudd_RecursiveDeref(manager, bdd2);
  return tmp;
}

/* file manipulation */

bddfileheader ReadFileHeader(char *filename) {
  bddfileheader temp;
  char *header;
  temp.inputfile = NULL;
  temp.version = 0;
  temp.varcnt = 0;
  temp.varstart = 0;
  temp.intercnt = 0;
  temp.filetype = BDDFILE_OTHER;
  if ((temp.inputfile = fopen(filename, "r")) == NULL) {
    perror(filename);
    temp.filetype = BDDFILE_ERROR;
    return temp;
  }
  // Read file header
  if (!feof(temp.inputfile)) {
    header = freadline(temp.inputfile);
    temp.version = CheckFileVersion(header);
    if (temp.version > -1) temp.filetype = (strlen(header) == 5) * BDDFILE_SCRIPT + (strlen(header) == 7) * BDDFILE_NODEDUMP;
    free(header);
    switch (temp.filetype) {
      case BDDFILE_SCRIPT:
        switch (temp.version) {
          case 1:
            fscanf(temp.inputfile, "%i\n", &temp.varcnt);
            fscanf(temp.inputfile, "%i\n", &temp.varstart);
            fscanf(temp.inputfile, "%i\n", &temp.intercnt);
            break;
          default:
            fclose(temp.inputfile);
            temp.inputfile = NULL;
            break;
        }
        break;
      case BDDFILE_NODEDUMP:
        switch (temp.version) {
          case 1:
            fscanf(temp.inputfile, "%i\n", &temp.varcnt);
            fscanf(temp.inputfile, "%i\n", &temp.varstart);
            break;
          default:
            fclose(temp.inputfile);
            temp.inputfile = NULL;
            break;
        }
        break;
      case BDDFILE_OTHER:
        fclose(temp.inputfile);
        temp.inputfile = NULL;
        break;
      default:
        fclose(temp.inputfile);
        temp.inputfile = NULL;
        break;
    }
  }
  return temp;
}

int CheckFileVersion(const char *version) {
  if (strlen(version) < 5) return -1;
  if (strlen(version) == 5 && version[0] == '@' && version[1] == 'B' && version[2] == 'D' && version[3] == 'D') return atoi(version + 4);
  if (strlen(version) == 7 && version[0] == '@' && version[1] == 'N' && version[2] == 'O' && version[3] == 'D'
                           && version[4] == 'E' && version[5] == 'S') return atoi(version + 6);
  return -1;
}

int simpleBDDtoDot(DdManager *manager, DdNode *bdd, char *filename) {
  DdNode *f[1];
  int ret;
  FILE *fd;
  f[0] = Cudd_BddToAdd(manager, bdd);
  fd = fopen(filename, "w");
  if (fd == NULL) {
    perror(filename);
    return -1;
  }
  ret = Cudd_DumpDot(manager, 1, f, NULL, NULL, fd);
  fclose(fd);
  return ret;
}

int simpleNamedBDDtoDot(DdManager *manager, namedvars varmap, DdNode *bdd, char *filename) {
  DdNode *f[1];
  int ret;
  FILE *fd;
  f[0] = Cudd_BddToAdd(manager, bdd);
  fd = fopen(filename, "w");
  if (fd == NULL) {
    perror(filename);
    return -1;
  }
  ret = Cudd_DumpDot(manager, 1, f, varmap.vars, NULL, fd);
  fclose(fd);
  return ret;
}

int SaveNodeDump(DdManager *manager, namedvars varmap, DdNode *bdd, char *filename) {
  hisqueue *Nodes;
  FILE *outputfile;
  int i;
  if ((outputfile = fopen(filename, "w")) == NULL) {
    perror(filename);
    return -1;
  }
  fprintf(outputfile, "%s\n%i\n%i\n", "@NODES1", varmap.varcnt, varmap.varstart);
  Nodes = InitHistory(varmap.varcnt);
  for (i = 0; i < varmap.varcnt; i++)
    fprintf(outputfile, "%s\t%i\n", varmap.vars[i], Cudd_ReadPerm(manager, i));
  if (bdd == HIGH(manager)) fprintf(outputfile, "TRUE\t0\tTRUE\t0\tTRUE\t0\n");
  else if (bdd == LOW(manager)) fprintf(outputfile, "FALSE\t0\tFALSE\t0\tFALSE\t0\n");
  else SaveExpand(manager, varmap, Nodes, bdd, outputfile);
  ReInitHistory(Nodes, varmap.varcnt);
  free(Nodes);
  fclose(outputfile);
  return 0;
}

void SaveExpand(DdManager *manager, namedvars varmap, hisqueue *Nodes, DdNode *Current, FILE *outputfile) {
  DdNode *h, *l;
  hisnode *Found;
  char *curnode;
  int inode;
  if (Current != HIGH(manager) && Current != LOW(manager)) {
    if ((Found = GetNode(Nodes, varmap.varstart, Current)) == NULL) {
      AddNode(Nodes, varmap.varstart, Current, 0.0, 0, NULL);
      Found = GetNode(Nodes, varmap.varstart, Current);
    }
    if (!(Found->ivalue)) {
      Found->ivalue = 1;
      curnode = GetNodeVarNameDisp(manager, varmap, Current);
      inode = GetNodeIndex(Nodes, varmap.varstart, Current);
      fprintf(outputfile, "%s\t%i\t", curnode, inode);
      h = HighNodeOf(manager, Current);
      if (h == HIGH(manager)) {
        fprintf(outputfile, "TRUE\t0\t");
      } else if (h == LOW(manager)) {
        fprintf(outputfile, "FALSE\t0\t");
      } else {
        if (GetNode(Nodes, varmap.varstart, h) == NULL) AddNode(Nodes, varmap.varstart, h, 0.0, 0, NULL);
        curnode = GetNodeVarNameDisp(manager, varmap, h);
        inode = GetNodeIndex(Nodes, varmap.varstart, h);
        fprintf(outputfile, "%s\t%i\t", curnode, inode);
      }
      l = LowNodeOf(manager, Current);
      if (l == HIGH(manager)) {
        fprintf(outputfile, "TRUE\t0\n");
      } else if (l == LOW(manager)) {
        fprintf(outputfile, "FALSE\t0\n");
      } else {
        if (GetNode(Nodes, varmap.varstart, l) == NULL) AddNode(Nodes, varmap.varstart, l, 0.0, 0, NULL);
        curnode = GetNodeVarNameDisp(manager, varmap, l);
        inode = GetNodeIndex(Nodes, varmap.varstart, l);
        fprintf(outputfile, "%s\t%i\n", curnode, inode);
      }
      SaveExpand(manager, varmap, Nodes, l, outputfile);
      SaveExpand(manager, varmap, Nodes, h, outputfile);
    }
  }
}

DdNode * LoadNodeDump(DdManager *manager, namedvars varmap, FILE *inputfile) {
  hisqueue *Nodes;
  nodeline temp;
  DdNode *ret;
  int i, pos, *perm;
  char *varnam;
  perm = (int *) malloc(sizeof(int) * varmap.varcnt);
  Nodes = InitHistory(varmap.varcnt);
  for (i = 0; i < varmap.varcnt; i++) {
    varnam = freadstr(inputfile, "\t");
    pos = atoi(freadstr(inputfile, "\n"));
    AddNamedVarAt(varmap, varnam, pos);
    perm[pos] = pos;
  }
  temp.varname = freadstr(inputfile, "\t");
  fscanf(inputfile, "%i\t", &temp.nodenum);
  temp.truevar = freadstr(inputfile, "\t");
  fscanf(inputfile, "%i\t", &temp.truenode);
  temp.falsevar = freadstr(inputfile, "\t");
  fscanf(inputfile, "%i\n", &temp.falsenode);
  ret = LoadNodeRec(manager, varmap, Nodes, inputfile, temp);
  free(temp.varname);
  free(temp.truevar);
  free(temp.falsevar);
  fclose(inputfile);
  ReInitHistory(Nodes, varmap.varcnt);
  free(Nodes);
  Cudd_Ref(ret);
  Cudd_ShuffleHeap(manager, perm);
  for (i = 0; i < varmap.varcnt; i++) varmap.ivalue[i] = 0;
  return ret;
}

DdNode * LoadNodeRec(DdManager *manager, namedvars varmap, hisqueue *Nodes, FILE *inputfile, nodeline current) {
  nodeline temp;
  DdNode *newnode, *truenode, *falsenode;
  int index;
  newnode = GetIfExists(manager, varmap, Nodes, current.varname, current.nodenum);
  if (newnode != NULL) return newnode;
  falsenode = GetIfExists(manager, varmap, Nodes, current.falsevar, current.falsenode);
  if (falsenode == NULL) {
    temp.varname = freadstr(inputfile, "\t");
    fscanf(inputfile, "%i\t", &temp.nodenum);
    temp.truevar = freadstr(inputfile, "\t");
    fscanf(inputfile, "%i\t", &temp.truenode);
    temp.falsevar = freadstr(inputfile, "\t");
    fscanf(inputfile, "%i\n", &temp.falsenode);
    falsenode = LoadNodeRec(manager, varmap, Nodes, inputfile, temp);
    free(temp.varname);
    free(temp.truevar);
    free(temp.falsevar);
  }
  truenode = GetIfExists(manager, varmap, Nodes, current.truevar, current.truenode);
  if (truenode == NULL) {
    temp.varname = freadstr(inputfile, "\t");
    fscanf(inputfile, "%i\t", &temp.nodenum);
    temp.truevar = freadstr(inputfile, "\t");
    fscanf(inputfile, "%i\t", &temp.truenode);
    temp.falsevar = freadstr(inputfile, "\t");
    fscanf(inputfile, "%i\n", &temp.falsenode);
    truenode = LoadNodeRec(manager, varmap, Nodes, inputfile, temp);
    free(temp.varname);
    free(temp.truevar);
    free(temp.falsevar);
  }
  index = GetNamedVarIndex(varmap, current.varname);
  if (!varmap.ivalue[index]) {
    varmap.ivalue[index] = 1;
    newnode = GetVar(manager, varmap.varstart + index);
    //Cudd_RecursiveDeref(manager, newnode->type.kids.T);
    //Cudd_RecursiveDeref(manager, newnode->type.kids.E);
    newnode->type.kids.T = Cudd_NotCond(truenode, Cudd_IsComplement(truenode));
    newnode->type.kids.E = Cudd_NotCond(falsenode, Cudd_IsComplement(truenode));
    Cudd_Ref(newnode->type.kids.T);
    Cudd_Ref(newnode->type.kids.E);
    Cudd_Ref(newnode);
  } else {
    if (_RapidLoad == 1) {
      newnode = cuddAllocNode(manager);
      if (newnode != NULL) {
        newnode->index = varmap.varstart + index;
        newnode->type.kids.T = Cudd_NotCond(truenode, Cudd_IsComplement(truenode));
        newnode->type.kids.E = Cudd_NotCond(falsenode, Cudd_IsComplement(truenode));
        Cudd_Ref(newnode->type.kids.T);
        Cudd_Ref(newnode->type.kids.E);
        Cudd_Ref(newnode);
      }
    } else {
      newnode = cuddUniqueInter(manager, varmap.varstart + index, Cudd_NotCond(truenode, Cudd_IsComplement(truenode)), Cudd_NotCond(falsenode, Cudd_IsComplement(truenode)));
      if (newnode != NULL) {
        Cudd_Ref(newnode);
      } else {
        newnode = cuddAllocNode(manager);
        if (newnode != NULL) {
          newnode->index = varmap.varstart + index;
          newnode->type.kids.T = Cudd_NotCond(truenode, Cudd_IsComplement(truenode));
          newnode->type.kids.E = Cudd_NotCond(falsenode, Cudd_IsComplement(truenode));
          Cudd_Ref(newnode->type.kids.T);
          Cudd_Ref(newnode->type.kids.E);
          Cudd_Ref(newnode);
        }
      }
    }
  }
  if (newnode != NULL) {
    Nodes[index].thenode[current.nodenum].key = Cudd_NotCond(newnode, Cudd_IsComplement(truenode));
    return Cudd_NotCond(newnode, Cudd_IsComplement(truenode));
  }
  return NULL;
}

DdNode * GetIfExists(DdManager *manager, namedvars varmap, hisqueue *Nodes, char *varname, int nodenum) {
  int index;
  if (strcmp(varname, "TRUE") == 0) return HIGH(manager);
  if (strcmp(varname, "FALSE") == 0) return LOW(manager);
  index = GetNamedVarIndex(varmap, varname);
  if (index == -1 * varmap.varcnt) {
    fprintf(stderr, "Error: more variables requested than initialized.\n");
    exit(-1);
  }
  if ((index < 0) || (index == 0 && varmap.vars[0] == NULL)) {
    index = AddNamedVar(varmap, varname);
  }
  ExpandNodes(Nodes, index, nodenum);
  if (Nodes[index].thenode[nodenum].key != NULL) return Nodes[index].thenode[nodenum].key;
  return NULL;
}

void ExpandNodes(hisqueue *Nodes, int index, int nodenum) {
  int i;
  if (Nodes[index].cnt > nodenum) return;
  Nodes[index].thenode = (hisnode *) realloc(Nodes[index].thenode, (nodenum + 1) * sizeof(hisnode));
  for (i = Nodes[index].cnt; i < nodenum + 1; i++) {
    Nodes[index].thenode[i].key = NULL;
    Nodes[index].thenode[i].ivalue = 0;
    Nodes[index].thenode[i].dvalue = 0.0;
    Nodes[index].thenode[i].dynvalue = NULL;
  }
  Nodes[index].cnt = nodenum + 1;
}

int LoadVariableData(namedvars varmap, char *filename) {
  FILE *data;
  char *dataread, buf, *varname, *dynvalue;
  double dvalue = 0.0;
  int icur = 0, maxbufsize = 10, hasvar = 0, index = -1, idat = 0, ivalue = 0;
  dynvalue = NULL;
  if ((data = fopen(filename, "r")) == NULL) {
    perror("fopen");
    return -1;
  }
  dataread = (char *) malloc(sizeof(char) * maxbufsize);
  while(!feof(data)) {
    fread(&buf, 1, 1, data);
    if (buf == '\n') {
      dataread[icur] = '\0';
      icur = 0;
      buf = ' ';
      if (dataread[0] == '@') {
        if (hasvar) {
          for (index = 0; index < varmap.varcnt; index++) {
            if (patternmatch(varname, varmap.vars[index])) {
              varmap.loaded[index] = 1;
              varmap.dvalue[index] = dvalue;
              varmap.ivalue[index] = ivalue;
              if (varmap.dynvalue[index] != NULL) {
                free(varmap.dynvalue[index]);
                varmap.dynvalue[index] = NULL;
              }
              if (dynvalue != NULL) {
                varmap.dynvalue[index] = (void *) malloc(sizeof(char) * (strlen(dynvalue) + 1));
                strcpy(varmap.dynvalue[index], dynvalue);
              }
            }
          }
          dvalue = 0.0;
          ivalue = 0;
          if (dynvalue != NULL) {
            free(dynvalue);
            dynvalue = NULL;
          }
          free(varname);
        }
        varname = (char *) malloc(sizeof(char) * strlen(dataread));
        strcpy(varname, dataread + 1);
        hasvar = 1;
        idat = 0;
      } else {
        if (hasvar >= 0) {
          switch(idat) {
            case 0:
              if (IsRealNumber(dataread)) dvalue = atof(dataread);
              else {
                fprintf(stderr, "Error at file: %s. Variable: %s can't have non real value: %s.\n", filename, varname, dataread);
                fclose(data);
                free(varname);
                free(dataread);
                return -2;
              }
              idat++;
              break;
            case 1:
              if (IsNumber(dataread)) ivalue = atoi(dataread);
              else {
                fprintf(stderr, "Error at file: %s. Variable: %s can't have non integer value: %s.\n", filename, varname, dataread);
                fclose(data);
                free(varname);
                free(dataread);
                return -2;
              }
              idat++;
              break;
            case 2:
              dynvalue = malloc(sizeof(char) * (strlen(dataread) + 1));
              strcpy(dynvalue, dataread);
              break;
          }
        }
      }
    } else {
      dataread[icur] = buf;
      icur++;
      if (icur == _maxbufsize) {
        fprintf(stderr, "Error: Maximum buffer size(%i) exceeded.\n", _maxbufsize);
        fclose(data);
        free(varname);
        free(dataread);
        return -2;
      }
      while (icur > maxbufsize - 1) {
        maxbufsize *= 2;
        dataread = (char *) realloc(dataread, sizeof(char) * maxbufsize);
      }
    }
  }
  if (hasvar) {
    for (index = 0; index < varmap.varcnt; index++) {
      if (patternmatch(varname, varmap.vars[index])) {
        varmap.loaded[index] = 1;
        varmap.dvalue[index] = dvalue;
        varmap.ivalue[index] = ivalue;
        if (dynvalue != NULL) {
          varmap.dynvalue[index] = (void *) malloc(sizeof(char) * (strlen(dynvalue) + 1));
          strcpy(varmap.dynvalue[index], dynvalue);
        }
      }
    }
    if (dynvalue != NULL) {
      free(dynvalue);
      dynvalue = NULL;
    }
    free(varname);
  }
  fclose(data);
  free(dataread);
  return 0;
}

/* Queue for node storing to avoid loops */

hisqueue* InitHistory(int varcnt) {
  int i;
  hisqueue *HisQueue;
  HisQueue = (hisqueue *) malloc(sizeof(hisqueue) * varcnt);
  for (i = 0; i < varcnt; i++) {
    HisQueue[i].thenode = NULL;
    HisQueue[i].cnt = 0;
  }
  return HisQueue;
}

void ReInitHistory(hisqueue *HisQueue, int varcnt) {
  int i, j;
  for (i = 0; i < varcnt; i++) {
    if (HisQueue[i].thenode != NULL) {
      for (j = 0; j < HisQueue[i].cnt; j++)
        if (HisQueue[i].thenode[j].dynvalue != NULL) free(HisQueue[i].thenode[j].dynvalue);
      free(HisQueue[i].thenode);
      HisQueue[i].thenode = NULL;
    }
    HisQueue[i].cnt = 0;
  }
}

void AddNode(hisqueue *HisQueue, int varstart, DdNode *node, double dvalue, int ivalue, void *dynvalue) {
  int index = GetIndex(node) - varstart;
  HisQueue[index].thenode = (hisnode *) realloc(HisQueue[index].thenode, (HisQueue[index].cnt + 1) * sizeof(hisnode));
  HisQueue[index].thenode[HisQueue[index].cnt].key = node;
  HisQueue[index].thenode[HisQueue[index].cnt].dvalue = dvalue;
  HisQueue[index].thenode[HisQueue[index].cnt].ivalue = ivalue;
  HisQueue[index].thenode[HisQueue[index].cnt].dynvalue = dynvalue;
  HisQueue[index].cnt += 1;
}

hisnode* GetNode(hisqueue *HisQueue, int varstart, DdNode *node) {
  int i;
  int index = GetIndex(node) - varstart;
  for(i = 0; i < HisQueue[index].cnt; i++) {
    if (HisQueue[index].thenode[i].key == node) return &(HisQueue[index].thenode[i]);
  }
  return NULL;
}

int GetNodeIndex(hisqueue *HisQueue, int varstart, DdNode *node) {
  int i;
  int index = GetIndex(node) - varstart;
  for(i = 0; i < HisQueue[index].cnt; i++) {
    if (HisQueue[index].thenode[i].key == node) return i;
  }
  return -1;
}

/* Named variables */

namedvars InitNamedVars(int varcnt, int varstart) {
  namedvars temp;
  int i;
  temp.varcnt = varcnt;
  temp.varstart = varstart;
  temp.vars = (char **) malloc(sizeof(char *) * varcnt);
  temp.loaded = (int *) malloc(sizeof(int) * varcnt);
  temp.dvalue = (double *) malloc(sizeof(double) * varcnt);
  temp.ivalue = (int *) malloc(sizeof(int) * varcnt);
  temp.dynvalue = (void **) malloc(sizeof(int) * varcnt);
  for (i = 0; i < varcnt; i++) {
    temp.vars[i] = NULL;
    temp.loaded[i] = 0;
    temp.dvalue[i] = 0.0;
    temp.ivalue[i] = 0;
    temp.dynvalue[i] = NULL;
  }
  return temp;
}

void EnlargeNamedVars(namedvars *varmap, int newvarcnt) {
  int i;
  varmap->vars = (char **) realloc(varmap->vars, sizeof(char *) * newvarcnt);
  varmap->loaded = (int *) realloc(varmap->loaded, sizeof(int) * newvarcnt);
  varmap->dvalue = (double *) realloc(varmap->dvalue, sizeof(double) * newvarcnt);
  varmap->ivalue = (int *) realloc(varmap->ivalue, sizeof(int) * newvarcnt);
  varmap->dynvalue = (void **) realloc(varmap->dynvalue, sizeof(int) * newvarcnt);
  for (i = varmap->varcnt; i < newvarcnt; i++) {
    varmap->vars[i] = NULL;
    varmap->loaded[i] = 0;
    varmap->dvalue[i] = 0.0;
    varmap->ivalue[i] = 0;
    varmap->dynvalue[i] = NULL;
  }
  varmap->varcnt = newvarcnt;
}

int AddNamedVarAt(namedvars varmap, const char *varname, int index) {
  if (varmap.varcnt > index) {
    varmap.vars[index] = (char *) malloc(sizeof(char) * (strlen(varname) + 1));
    strcpy(varmap.vars[index], varname);
    return index;
  }
  return -1;
}

int AddNamedVar(namedvars varmap, const char *varname) {
  int index = GetNamedVarIndex(varmap, varname);
  if (index == -1 * varmap.varcnt) {
    return -1;
  } else if ((index < 0) || (index == 0 && varmap.vars[0] == NULL)) {
    index *= -1;
    varmap.vars[index] = (char *) malloc(sizeof(char) * (strlen(varname) + 1));
    strcpy(varmap.vars[index], varname);
  }
  return index;
}

void SetNamedVarValuesAt(namedvars varmap, int index, double dvalue, int ivalue, void *dynvalue) {
  varmap.dvalue[index] = dvalue;
  varmap.ivalue[index] = ivalue;
  varmap.dynvalue[index] = dynvalue;
}

int SetNamedVarValues(namedvars varmap, const char *varname, double dvalue, int ivalue, void *dynvalue) {
  int index = GetNamedVarIndex(varmap, varname);
  if (index == -1 * varmap.varcnt) {
    return -1;
  } else if ((index < 0) || (index == 0 && varmap.vars[0] == NULL)) {
    index *= -1;
    varmap.vars[index] = (char *) malloc(sizeof(char) * (strlen(varname) + 1));
    strcpy(varmap.vars[index], varname);
    varmap.dvalue[index] = dvalue;
    varmap.ivalue[index] = ivalue;
    varmap.dynvalue[index] = dynvalue;
  } else {
    varmap.dvalue[index] = dvalue;
    varmap.ivalue[index] = ivalue;
    varmap.dynvalue[index] = dynvalue;
  }
  return index;
}

int GetNamedVarIndex(const namedvars varmap, const char *varname) {
  int i;
  for (i = 0; i < varmap.varcnt; i++) {
    if (varmap.vars[i] == NULL) return -1 * i;
    if (strcmp(varmap.vars[i], varname) == 0) return i;
  }
  return -1 * varmap.varcnt;
}

char* GetNodeVarName(DdManager *manager, namedvars varmap, DdNode *node) {
  if (node == NULL) return NULL;
  if (node == HIGH(manager)) return "true";
  if (node == LOW(manager)) return "false";
  return varmap.vars[GetIndex(node) - varmap.varstart];
}

char* GetNodeVarNameDisp(DdManager *manager, namedvars varmap, DdNode *node) {
  if (HIGH(manager) == node) return "TRUE";
  if (LOW(manager) == node) return "FALSE";
  if (NULL == node) return "(null)";
  return varmap.vars[GetIndex(node) - varmap.varstart];
}

int RepairVarcnt(namedvars *varmap) {
    while (varmap->vars[varmap->varcnt - 1] == NULL)
      varmap->varcnt--;
    return varmap->varcnt;
}

int all_loaded(namedvars varmap, int disp) {
  int i, res = 1;
  for (i = 0; i < varmap.varcnt; i++) {
    if (varmap.loaded[i] == 0) {
      res = 0;
      if (disp) fprintf(stderr, "The variable: %s was not loaded with values.\n", varmap.vars[i]); else return 0;
    }
  }
  return res;
}

/* Parser */

DdNode* FileGenerateBDD(DdManager *manager, namedvars varmap, bddfileheader fileheader) {
  int icomment, maxlinesize, icur, iline, curinter, iequal;
  DdNode *Line, **inter;
  char buf, *inputline, *filename;
  bddfileheader interfileheader;
  // Initialization of intermediate steps
  inter = (DdNode **) malloc(sizeof(DdNode *) * fileheader.intercnt);
  for (icur = 0; icur < fileheader.intercnt; icur++) inter[icur] = NULL;
  // Read file data
  interfileheader.inputfile = NULL;
  filename = NULL;  // For nested files
  iequal = 0;       // Flag for encountered = sign
  icur = 0;         // Pointer for inputline buffer location
  iline = 5;        // Current file line (first after header)
  icomment = 0;     // Flag for comments
  maxlinesize = 80; // inputline starting buffer size
  inputline = (char *) malloc(sizeof(char) * maxlinesize);
  while(!feof(fileheader.inputfile)) {
    fread(&buf, 1, 1, fileheader.inputfile);
    if (buf == ';' || buf == '%' || buf == '$') icomment = 1;
    if (buf == '\n') {
      if (icomment) icomment = 0;
      if (iequal > 1) {
        fprintf(stderr, "Error at line: %i. Line contains more than 1 equal(=) signs.\n", iline);
        fclose(fileheader.inputfile);
        free(inter);
        free(inputline);
        return NULL;
      } else iequal = 0;
      if (icur > 0) {
        inputline[icur] = '\0';
        if (inputline[0] != 'L') {
          fprintf(stderr, "Error at line: %i. Intermediate results should start with L.\n", iline);
          fclose(fileheader.inputfile);
          free(inter);
          free(inputline);
          return NULL;
        }
        curinter = getInterBDD(inputline);
        if (curinter == -1) {
          if (inputline[0] == 'L' && IsPosNumber(inputline + 1)) {
            curinter = atoi(inputline + 1) - 1;
            if (curinter > -1 && curinter < fileheader.intercnt && inter[curinter] != NULL) {
              if (_debug) fprintf(stderr, "Returned: %s\n", inputline);
              fclose(fileheader.inputfile);
              Line = inter[curinter];
              free(inter);
              free(inputline);
              return Line;
            } else {
              fprintf(stderr, "Error at line: %i. Return result asked doesn't exist.\n", iline);
              fclose(fileheader.inputfile);
              free(inter);
              free(inputline);
              return NULL;
            }
          } else {
            fprintf(stderr, "Error at line: %i. Invalid intermediate result format.\n", iline);
            fclose(fileheader.inputfile);
            free(inter);
            free(inputline);
            return NULL;
          }
        } else if (curinter > -1 && curinter < fileheader.intercnt && inter[curinter] == NULL) {
          if (_debug) fprintf(stderr, "%i %s\n", curinter, inputline);
          filename = getFileName(inputline);
          if (filename == NULL) {
            Line = LineParser(manager, varmap, inter, fileheader.intercnt, inputline, iline);
          } else {
            interfileheader = ReadFileHeader(filename);
            if (interfileheader.inputfile == NULL) {
              //Line = simpleBDDload(manager, &varmap, filename);
              Line = NULL;
            } else {
              Line = FileGenerateBDD(manager, varmap, interfileheader);
            }
            if (Line == NULL) fprintf(stderr, "Error at line: %i. Error in nested BDD file: %s.\n", iline, filename);
            free(filename);
            filename = NULL;
            interfileheader.inputfile = NULL;
          }
          if (Line == NULL) {
            fclose(fileheader.inputfile);
            free(inter);
            free(inputline);
            return NULL;
          }
          inter[curinter] = Line;
          icur = 0;
        } else if (curinter > -1 && curinter < fileheader.intercnt && inter[curinter] != NULL) {
          fprintf(stderr, "Error at line: %i. Intermediate results can't be overwritten.\n", iline);
          fclose(fileheader.inputfile);
          free(inter);
          free(inputline);
          return NULL;
        } else {
          fprintf(stderr, "Error at line: %i. Intermediate result asked doesn't exist.\n", iline);
          fclose(fileheader.inputfile);
          free(inter);
          free(inputline);
          return NULL;
        }
      }
      iline++;
    } else if (buf != ' ' && buf != '\t' && !icomment) {
      if (buf == '=') iequal++;
      inputline[icur] = buf;
      icur += 1;
      if (icur == _maxbufsize) {
        fprintf(stderr, "Error: Maximum buffer size(%i) exceeded.\n", _maxbufsize);
        fclose(fileheader.inputfile);
        free(inter);
        free(inputline);
        return NULL;
      }
      while (icur > maxlinesize - 1) {
        maxlinesize *= 2;
        inputline = (char *) realloc(inputline, sizeof(char) * maxlinesize);
      }
    }
  }
  fprintf(stderr, "Error, file either doesn't end with a blank line or no return result was asked.\n");
  fclose(fileheader.inputfile);
  free(inter);
  free(inputline);
  return NULL;
}

int getInterBDD(char *function) {
  int i, ret;
  char *inter;
  for (i = 0; i < strlen(function); i++) {
    if (function[i] == '=') {
      inter = (char *) malloc(sizeof(char) * i);
      strncpy(inter, function + 1, i - 1);
      inter[i - 1] = '\0';
      if (IsPosNumber(inter)) {
        ret = atoi(inter) - 1;
        free(inter);
        return ret;
      } else {
        free(inter);
        return -1;
      }
    }
  }
  return -1;
}

char* getFileName(const char *function) {
  int i = 0;
  char *filename;
  while(function[i] != '=' && (i + 1) < strlen(function)) i++;
  if ((i + 1) < strlen(function)) {
    i++;
    if (function[i] == '<' && function[strlen(function) - 1] == '>') {
      filename = (char *) malloc(sizeof(char) * strlen(function) - i);
      strcpy(filename, function + i + 1);
      filename[strlen(function) - i - 2] = '\0';
      return filename;
    }
  }
  return NULL;
}

DdNode* LineParser(DdManager *manager, namedvars varmap, DdNode **inter, int maxinter, char *function, int iline) {
  int istart, iend, ilength, i, symbol, ivar, inegvar, inegoper, iconst;
  long startAt, endAt;
  double secs;
  DdNode *bdd;
  char *term, curoper;
  bdd = HIGH(manager);
  Cudd_Ref(bdd);
  term = NULL;
  ivar = -1;
  curoper = '*';
  istart = -1;
  iend = strlen(function) - 1;
  ilength = -1;
  symbol = -1;
  inegvar = 0;
  inegoper = 0;
  iconst = 0;
  for (i = strlen(function) - 1; i > -1; i--) {
    if (symbol == -1 && isOperator(function[i])) {
      symbol = i;
      istart = i + 1;
      ilength = iend - i;
      iend = i - 1;
      if (ilength > 0 && !(ilength == 1 && function[istart] == '~')) {
        term = (char *) malloc(sizeof(char) * (ilength + 1));
        strncpy(term, function + istart, ilength);
        term[ilength] = '\0';
      } else {
        fprintf(stderr, "Line Parser Error at line: %i. An operator was encounter with no term at its right side.\n", iline);
        free(term);
        return NULL;
      }
    }
    if (symbol != -1) {
      if (term[0] == '~') inegvar = 1; else inegvar = 0;
      if (term[0 + inegvar] != 'L') {
        // Term is a variable
        if (strcmp(term + inegvar, "TRUE") == 0) {
          iconst = 1;
        } else if (strcmp(term + inegvar, "FALSE") == 0) {
          iconst = 1;
          inegvar = 1;
        } else {
          iconst = 0;
          ivar = AddNamedVar(varmap, term + inegvar);
          if (ivar == -1) {
            fprintf(stderr, "Line Parser Error at line: %i. More BDD variables than the reserved term: %s.\n", iline, term);
            free(term);
            return NULL;
          }
        }
        if (_debug) fprintf(stderr, "%s\n", term);
        if (_debug && !iconst) fprintf(stderr, "PNZ1:%.0f P1:%.0f S1:%i PNZ2:%.0f P2:%.0f S2:%i\n",
                                                                            Cudd_CountPathsToNonZero(bdd),
                                                                            Cudd_CountPath(bdd),
                                                                            Cudd_DagSize(bdd),
                                                                            Cudd_CountPathsToNonZero(GetVar(manager, ivar + varmap.varstart)),
                                                                            Cudd_CountPath(GetVar(manager, ivar + varmap.varstart)),
                                                                            Cudd_DagSize(GetVar(manager, ivar + varmap.varstart)) );
        startAt = clock();
        if (!iconst) {
          if (inegvar) bdd = BDD_Operator(manager, NOT(GetVar(manager, ivar + varmap.varstart)), bdd, curoper, inegoper);
          else bdd = BDD_Operator(manager, GetVar(manager, ivar + varmap.varstart), bdd, curoper, inegoper);
        } else {
          switch(curoper) {
            case '+':
              if (inegvar ^ inegoper) ; else {
                bdd = HIGH(manager);
                Cudd_Ref(bdd);
              }
              break;
            case '*':
              if (inegvar ^ inegoper) {
                bdd = LOW(manager);
                Cudd_Ref(bdd);
              }
              break;
            case '#':
              if (inegvar ^ inegoper) ; else bdd = NOT(bdd);
              break;
          }
        }
        endAt = clock();
        secs = ((double) (endAt - startAt)) / ((double) CLOCKS_PER_SEC);
        if (_debug) fprintf(stderr, "term: %s of line: %i took: %i\n", term, iline, endAt - startAt);
        //if ((endAt - startAt) > 10000000) Cudd_AutodynDisable(manager);
        if (bdd == NULL) {
          fprintf(stderr, "Line Parser Error at line: %i. Error using operator %c on term: %s.\n", iline, curoper, term);
          free(term);
          return NULL;
        }
      } else {
        // Term is an intermediate result
        if (IsPosNumber(term + inegvar + 1)) ivar = atoi(term + inegvar + 1) - 1; else {
          fprintf(stderr, "Line Parser Error at line: %i. Invalid intermediate result format term: %s.\n", iline, term);
          free(term);
          return NULL;
        }
        if (ivar < 0 || ivar > maxinter || inter[ivar] == NULL) {
          fprintf(stderr, "Line Parser Error at line: %i. Usage of undeclared intermediate result term: %s.\n", iline, term);
          free(term);
          return NULL;
        }
        if (_debug) fprintf(stderr, "%s\n", term);
        if (_debug) fprintf(stderr, "PNZ1:%.0f P1:%.0f S1:%i PNZ2:%.0f P2:%.0f S2:%i\n",
                                                                            Cudd_CountPathsToNonZero(bdd),
                                                                            Cudd_CountPath(bdd),
                                                                            Cudd_DagSize(bdd),
                                                                            Cudd_CountPathsToNonZero(inter[ivar]),
                                                                            Cudd_CountPath(inter[ivar]),
                                                                            Cudd_DagSize(inter[ivar]) );
        startAt = clock();
        if (inegvar) bdd = BDD_Operator(manager, NOT(inter[ivar]), bdd, curoper, inegoper);
        else bdd = BDD_Operator(manager, inter[ivar], bdd, curoper, inegoper);
        endAt = clock();
        secs = ((double) (endAt - startAt)) / ((double) CLOCKS_PER_SEC);
        if (_debug) fprintf(stderr, "term: %s of line: %i took: %i\n", term, iline, endAt - startAt);
        //if ((endAt - startAt) > 10000000) Cudd_AutodynDisable(manager);
        if (bdd == NULL) {
          fprintf(stderr, "Line Parser Error at line: %i. Error using operator %c on term: %s.\n", iline, curoper, term);
          free(term);
          return NULL;
        }
      }
      free(term);
      term = NULL;
      curoper = function[symbol];
      if (curoper == '=') return bdd;
      if (function[symbol - 1] == '~') {
        inegoper = 1;
        i--;
        iend--;
      } else {
        inegoper = 0;
      }
      symbol = -1;
    }
  }
  return NULL;
}

DdNode* BDD_Operator(DdManager *manager, DdNode *bdd1, DdNode *bdd2, char Operator, int inegoper) {
  switch (Operator) {
    case '+':
      if (inegoper) return D_BDDNor(manager, bdd1, bdd2);
      else return D_BDDOr(manager, bdd1, bdd2);
      break;
    case '*':
      if (inegoper) return D_BDDNand(manager, bdd1, bdd2);
      else return D_BDDAnd(manager, bdd1, bdd2);
      break;
    case '#':
      if (inegoper) return D_BDDXnor(manager, bdd1, bdd2);
      else return D_BDDXor(manager, bdd1, bdd2);
      break;
    default:
      return NULL;
      break;
  }
}

DdNode* OnlineGenerateBDD(DdManager *manager, namedvars *varmap) {
  int icomment, maxlinesize, icur, iline, curinter, iequal, iinters, itmp, i;
  DdNode *Line, **inter;
  char buf, *inputline, *filename;
  bddfileheader interfileheader;
  // Initialization of intermediate steps
  iinters = 1;
  inter = (DdNode **) malloc(sizeof(DdNode *) * iinters);
  for (icur = 0; icur < iinters; icur++) inter[icur] = NULL;
  // Read file data
  interfileheader.inputfile = NULL;
  filename = NULL;  // For nested files
  iequal = 0;       // Flag for encountered = sign
  icur = 0;         // Pointer for inputline buffer location
  iline = 1;        // Current file line (first after header)
  icomment = 0;     // Flag for comments
  maxlinesize = 80; // inputline starting buffer size
  inputline = (char *) malloc(sizeof(char) * maxlinesize);

  do {
    buf = fgetc(stdin);
    if (buf == ';' || buf == '%' || buf == '$') icomment = 1;
    if (buf == '\n') {
      if (icomment) icomment = 0;
      if (iequal > 1) {
        fprintf(stderr, "Error at line: %i. Line contains more than 1 equal(=) signs.\n", iline);
        free(inter);
        free(inputline);
        return NULL;
      } else iequal = 0;
      if (icur > 0) {
        inputline[icur] = '\0';
        if (inputline[0] == '@') {
          if (inputline[1] == 'e') {
            free(inter);
            free(inputline);
            exit(0);
          } else {
            itmp = GetParam(inputline, 1);
            if (itmp > varmap->varcnt)
              EnlargeNamedVars(varmap, itmp);
            itmp = GetParam(inputline, 2);
            if (itmp > iinters) {
              inter = (DdNode **) realloc(inter, sizeof(DdNode *) * itmp);
              for (i = iinters; i < itmp; i++) inter[i] = NULL;
              iinters = itmp;
            }
          }
          icur = 0;
        } else {
          if (inputline[0] != 'L') {
            fprintf(stderr, "Error at line: %i. Intermediate results should start with L.\n", iline);
            free(inter);
            free(inputline);
            return NULL;
          }
          curinter = getInterBDD(inputline);
          if (curinter == -1) {
            if (inputline[0] == 'L' && IsPosNumber(inputline + 1)) {
              curinter = atoi(inputline + 1) - 1;
              if (curinter > -1 && curinter < iinters && inter[curinter] != NULL) {
                if (_debug) fprintf(stderr, "Returned: %s\n", inputline);
                Line = inter[curinter];
                free(inter);
                free(inputline);
                return Line;
              } else {
                fprintf(stderr, "Error at line: %i. Return result asked doesn't exist.\n", iline);
                free(inter);
                free(inputline);
                return NULL;
              }
            } else {
              fprintf(stderr, "Error at line: %i. Invalid intermediate result format.\n", iline);
              free(inter);
              free(inputline);
              return NULL;
            }
          } else if (curinter > -1) {
            if (curinter >= iinters) {
              inter = (DdNode **) realloc(inter, sizeof(DdNode *) * (curinter + 1));
              for (i = iinters; i < curinter + 1; i++) inter[i] = NULL;
              iinters = curinter + 1;
            }
            if (inter[curinter] == NULL) {
              if (_debug) fprintf(stderr, "%i %s\n", curinter, inputline);
              filename = getFileName(inputline);
              if (filename == NULL) {
                Line = OnlineLineParser(manager, varmap, inter, iinters, inputline, iline);
              } else {
                interfileheader = ReadFileHeader(filename);
                if (interfileheader.inputfile == NULL) {
                  //Line = simpleBDDload(manager, varmap, filename);
                  Line = NULL;
                } else {
                  Line = FileGenerateBDD(manager, *varmap, interfileheader);
                }
                if (Line == NULL) fprintf(stderr, "Error at line: %i. Error in nested BDD file: %s.\n", iline, filename);
                free(filename);
                filename = NULL;
                interfileheader.inputfile = NULL;
              }
              if (Line == NULL) {
                free(inter);
                free(inputline);
                return NULL;
              }
              inter[curinter] = Line;
              icur = 0;
            } else if (inter[curinter] != NULL) {
              fprintf(stderr, "Error at line: %i. Intermediate results can't be overwritten.\n", iline);
              free(inter);
              free(inputline);
              return NULL;
            }
          } else {
            fprintf(stderr, "Error at line: %i. Intermediate result asked doesn't exist.\n", iline);
            free(inter);
            free(inputline);
            return NULL;
          }
        }
      }
      iline++;
    } else if (buf != ' ' && buf != '\t' && !icomment) {
      if (buf == '=') iequal++;
      inputline[icur] = buf;
      icur += 1;
      if (icur == _maxbufsize) {
        fprintf(stderr, "Error: Maximum buffer size(%i) exceeded.\n", _maxbufsize);
        free(inter);
        free(inputline);
        return NULL;
      }
      while (icur > maxlinesize - 1) {
        maxlinesize *= 2;
        inputline = (char *) realloc(inputline, sizeof(char) * maxlinesize);
      }
    }
  } while(1);
  fprintf(stderr, "Error, file either doesn't end with a blank line or no return result was asked.\n");
  free(inter);
  free(inputline);
  return NULL;
}

DdNode* OnlineLineParser(DdManager *manager, namedvars *varmap, DdNode **inter, int maxinter, char *function, int iline) {
  int istart, iend, ilength, i, symbol, ivar, inegvar, inegoper, iconst;
  long startAt, endAt;
  double secs;
  DdNode *bdd;
  char *term, curoper;
  bdd = HIGH(manager);
  Cudd_Ref(bdd);
  term = NULL;
  ivar = -1;
  curoper = '*';
  istart = -1;
  iend = strlen(function) - 1;
  ilength = -1;
  symbol = -1;
  inegvar = 0;
  inegoper = 0;
  iconst = 0;
  for (i = strlen(function) - 1; i > -1; i--) {
    if (symbol == -1 && isOperator(function[i])) {
      symbol = i;
      istart = i + 1;
      ilength = iend - i;
      iend = i - 1;
      if (ilength > 0 && !(ilength == 1 && function[istart] == '~')) {
        term = (char *) malloc(sizeof(char) * (ilength + 1));
        strncpy(term, function + istart, ilength);
        term[ilength] = '\0';
      } else {
        fprintf(stderr, "Line Parser Error at line: %i. An operator was encounter with no term at its right side.\n", iline);
        free(term);
        return NULL;
      }
    }
    if (symbol != -1) {
      if (term[0] == '~') inegvar = 1; else inegvar = 0;
      if (term[0 + inegvar] != 'L') {
        // Term is a variable
        if (strcmp(term + inegvar, "TRUE") == 0) {
          iconst = 1;
        } else if (strcmp(term + inegvar, "FALSE") == 0) {
          iconst = 1;
          inegvar = 1;
        } else {
          iconst = 0;
          ivar = AddNamedVar(*varmap, term + inegvar);
          if (ivar == -1) {
            EnlargeNamedVars(varmap, varmap->varcnt + 1);
            ivar = AddNamedVar(*varmap, term + inegvar);
          }
          if (ivar == -1) {
            fprintf(stderr, "Line Parser Error at line: %i. More BDD variables than the reserved term: %s.\n", iline, term);
            free(term);
            return NULL;
          }
        }
        if (_debug) fprintf(stderr, "%s\n", term);
        if (_debug && !iconst) fprintf(stderr, "PNZ1:%.0f P1:%.0f S1:%i PNZ2:%.0f P2:%.0f S2:%i\n",
                                                                            Cudd_CountPathsToNonZero(bdd),
                                                                            Cudd_CountPath(bdd),
                                                                            Cudd_DagSize(bdd),
                                                                            Cudd_CountPathsToNonZero(GetVar(manager, ivar + varmap->varstart)),
                                                                            Cudd_CountPath(GetVar(manager, ivar + varmap->varstart)),
                                                                            Cudd_DagSize(GetVar(manager, ivar + varmap->varstart)) );
        startAt = clock();
        if (!iconst) {
          if (inegvar) bdd = BDD_Operator(manager, NOT(GetVar(manager, ivar + varmap->varstart)), bdd, curoper, inegoper);
          else bdd = BDD_Operator(manager, GetVar(manager, ivar + varmap->varstart), bdd, curoper, inegoper);
        } else {
          switch(curoper) {
            case '+':
              if (inegvar ^ inegoper) ; else {
                bdd = HIGH(manager);
                Cudd_Ref(bdd);
              }
              break;
            case '*':
              if (inegvar ^ inegoper) {
                bdd = LOW(manager);
                Cudd_Ref(bdd);
              }
              break;
            case '#':
              if (inegvar ^ inegoper) ; else bdd = NOT(bdd);
              break;
          }
        }
        endAt = clock();
        secs = ((double) (endAt - startAt)) / ((double) CLOCKS_PER_SEC);
        if (_debug) fprintf(stderr, "term: %s of line: %i took: %i\n", term, iline, endAt - startAt);
        //if ((endAt - startAt) > 10000000) Cudd_AutodynDisable(manager);
        if (bdd == NULL) {
          fprintf(stderr, "Line Parser Error at line: %i. Error using operator %c on term: %s.\n", iline, curoper, term);
          free(term);
          return NULL;
        }
      } else {
        // Term is an intermediate result
        if (IsPosNumber(term + inegvar + 1)) ivar = atoi(term + inegvar + 1) - 1; else {
          fprintf(stderr, "Line Parser Error at line: %i. Invalid intermediate result format term: %s.\n", iline, term);
          free(term);
          return NULL;
        }
        if (ivar < 0 || ivar > maxinter || inter[ivar] == NULL) {
          fprintf(stderr, "Line Parser Error at line: %i. Usage of undeclared intermediate result term: %s.\n", iline, term);
          free(term);
          return NULL;
        }
        if (_debug) fprintf(stderr, "%s\n", term);
        if (_debug) fprintf(stderr, "PNZ1:%.0f P1:%.0f S1:%i PNZ2:%.0f P2:%.0f S2:%i\n",
                                                                            Cudd_CountPathsToNonZero(bdd),
                                                                            Cudd_CountPath(bdd),
                                                                            Cudd_DagSize(bdd),
                                                                            Cudd_CountPathsToNonZero(inter[ivar]),
                                                                            Cudd_CountPath(inter[ivar]),
                                                                            Cudd_DagSize(inter[ivar]) );
        startAt = clock();
        if (inegvar) bdd = BDD_Operator(manager, NOT(inter[ivar]), bdd, curoper, inegoper);
        else bdd = BDD_Operator(manager, inter[ivar], bdd, curoper, inegoper);
        endAt = clock();
        secs = ((double) (endAt - startAt)) / ((double) CLOCKS_PER_SEC);
        if (_debug) fprintf(stderr, "term: %s of line: %i took: %i\n", term, iline, endAt - startAt);
        //if ((endAt - startAt) > 10000000) Cudd_AutodynDisable(manager);
        if (bdd == NULL) {
          fprintf(stderr, "Line Parser Error at line: %i. Error using operator %c on term: %s.\n", iline, curoper, term);
          free(term);
          return NULL;
        }
      }
      free(term);
      term = NULL;
      curoper = function[symbol];
      if (curoper == '=') return bdd;
      if (function[symbol - 1] == '~') {
        inegoper = 1;
        i--;
        iend--;
      } else {
        inegoper = 0;
      }
      symbol = -1;
    }
  }
  return NULL;
}

int GetParam(char *inputline, int iParam) {
  int icoma, istart, iend, ret;
  char *numb;
  istart = 1;
  icoma = istart;
  iend = strlen(inputline);
  while((inputline[icoma] != ',') && (icoma < iend))
    icoma++;
  if (iParam == 1) {
    numb = (char *) malloc(sizeof(char) * icoma);
    strncpy(numb, inputline + 1, icoma - 1);
    numb[icoma - 1] = '\0';
    if (IsPosNumber(numb)) {
      ret = atoi(numb);
      free(numb);
      return ret;
    }
  } else if(iParam == 2) {
    numb = (char *) malloc(sizeof(char) * (iend - icoma + 1));
    strncpy(numb, inputline + icoma + 1, iend - icoma);
    numb[iend - icoma] = '\0';
    if (IsPosNumber(numb)) {
      ret = atoi(numb);
      free(numb);
      return ret;
    }
  }
  return 0;
}

void onlinetraverse(DdManager *manager, namedvars varmap, hisqueue *HisQueue, DdNode *bdd) {
  char buf, *inputline;
  int icur, maxlinesize, iline, index, iloop, ivalue, iQsize, i, inQ;
  double dvalue;
  DdNode **Q, **Q2, *h_node, *l_node, *curnode;
  hisqueue *his;
  hisnode *hnode;
  iloop = 1;
  icur = 0;         // Pointer for inputline buffer location
  iline = 1;        // Current file line (first after header)
  maxlinesize = 80; // inputline starting buffer size
  inputline = (char *) malloc(sizeof(char) * maxlinesize);
  curnode = bdd;
  iQsize = 0;
  Q = (DdNode **) malloc(sizeof(DdNode *) * iQsize);
  Q2 = NULL;
  his = InitHistory(varmap.varcnt);
  do {
    buf = fgetc(stdin);
    if (buf == '\n') {
      inputline[icur] = '\0';
      if ((icur > 0) && (inputline[0] == '@') && (inputline[2] == ',' || inputline[2] == '\0')) {
        switch(inputline[1]) {
          case 'c':
            printf("bdd_temp_value('%s', %i).\n", GetNodeVarNameDisp(manager, varmap, curnode), iQsize);
            break;
          case 'n':
            if (curnode != HIGH(manager) && curnode != LOW(manager) && (hnode = GetNode(his, varmap.varstart, curnode)) == NULL) {
              AddNode(his, varmap.varstart, curnode, 0.0, 0, NULL);
              l_node = LowNodeOf(manager, curnode);
              h_node = HighNodeOf(manager, curnode);
              inQ = 0;
              for(i = 0; (i < iQsize / 2) && (inQ < 3); i++)
                inQ = (Q[i] == l_node) || (Q[iQsize - i] == l_node) + 2 * (Q[i] == h_node) || (Q[iQsize - i] == h_node);
              if (inQ & 1 == 0) inQ = inQ + (GetNode(his, varmap.varstart, l_node) != NULL);
              if (inQ & 2 == 0) inQ = inQ + 2 * (GetNode(his, varmap.varstart, h_node) != NULL);
              if (inQ & 1 == 1) inQ = inQ - (l_node == HIGH(manager) || l_node == LOW(manager));
              if (inQ & 2 == 2) inQ = inQ - 2 * (h_node == HIGH(manager) || h_node == LOW(manager));
              switch(inQ) {
                case 0:
                  iQsize += 2;
                  Q = (DdNode **) realloc(Q, sizeof(DdNode *) * iQsize);
                  Q[iQsize - 2] = l_node;
                  Q[iQsize - 1] = h_node;
                  break;
                case 1:
                  iQsize++;
                  Q = (DdNode **) realloc(Q, sizeof(DdNode *) * iQsize);
                  Q[iQsize - 1] = h_node;
                  break;
                case 2:
                  iQsize++;
                  Q = (DdNode **) realloc(Q, sizeof(DdNode *) * iQsize);
                  Q[iQsize - 1] = l_node;
                  break;
                case 3:
                  break;
                default:
                  break;
              }
            }
            if (inputline[2] == '\0' || strcmp(inputline + 3, "DFS") == 0) {
              if (iQsize > 0) {
                iQsize--;
                curnode = Q[iQsize];
                Q = (DdNode **) realloc(Q, sizeof(DdNode *) * iQsize);
              }
            } else if (strcmp(inputline + 3, "BFS") == 0) {
              if (iQsize > 0) {
                iQsize--;
                curnode = Q[0];
                Q2 = (DdNode **) malloc(sizeof(DdNode *) * iQsize);
                for(i = 0; i < iQsize; i++)
                  Q2[i] = Q[i + 1];
                free(Q);
                Q = Q2;
              }
            } else {
              fprintf(stderr, "Error: Could not find method: %s, Correct syntax @n,[DFS, BFS].\n", inputline + 3);
              free(Q);
              free(inputline);
              exit(-1);
            }
            break;
          case 'h':
            printf("bdd_temp_value('%s').\n", GetNodeVarNameDisp(manager, varmap, HighNodeOf(manager, curnode)));
            break;
          case 'l':
            printf("bdd_temp_value('%s').\n", GetNodeVarNameDisp(manager, varmap, LowNodeOf(manager, curnode)));
            break;
          case 'v':
            index = GetNamedVarIndex(varmap, inputline + 3);
            if (index >= 0) {
              fprintf(stdout, "bdd_temp_value([%f,%i,%s]).\n", varmap.dvalue[index], varmap.ivalue[index], (char *) varmap.dynvalue[index]);
            } else {
              fprintf(stderr, "Error: Could not find variable: %s, Correct syntax @v,[variable name].\n", inputline + 3);
              free(Q);
              free(inputline);
              exit(-1);
            }
            break;
          case 'e':
            iloop = 0;
            break;
          default:
            fprintf(stderr, "Error: Not recognizable instruction: %s.\n", inputline);
            free(Q);
            free(inputline);
            exit(-1);
            break;
        }
        icur = 0;
      } else {
        fprintf(stderr, "Error: Not recognizable instruction: %s.\n", inputline);
        free(Q);
        free(inputline);
        exit(-1);
      }
      iline++;
    } else if (buf != ' ' && buf != '\t') {
      inputline[icur] = buf;
      icur += 1;
      if (icur == _maxbufsize) {
        fprintf(stderr, "Error: Maximum buffer size(%i) exceeded.\n", _maxbufsize);
        free(Q);
        free(inputline);
        exit(-1);
      }
      while (icur > maxlinesize - 1) {
        maxlinesize *= 2;
        inputline = (char *) realloc(inputline, sizeof(char) * maxlinesize);
      }
    }
  } while(iloop);
  free(Q);
  free(inputline);
}
