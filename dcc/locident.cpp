/*
 * File: locIdent.c
 * Purpose: support routines for high-level local identifier definitions.
 * Date: October 1993
 * (C) Cristina Cifuentes
 */

#include "dcc.h"
#include <string.h>

#define LOCAL_ID_DELTA 25
#define IDX_ARRAY_DELTA 5

void insertIdx(IDX_ARRAY *list, Int idx)
/* Inserts the new index at the end of the list.  If there is need to
 * allocate extra storage, it does so.  */
{
    if (list->csym == list->alloc) {
        list->alloc += IDX_ARRAY_DELTA;
        list->idx = (Int *)reallocVar(list->idx, list->alloc * sizeof(Int));
        memset(&list->idx[list->csym], 0, IDX_ARRAY_DELTA * sizeof(Int));
    }
    list->idx[list->csym] = idx;
    list->csym++;
}

void LOCAL_ID::insertIdx(Int i, Int idx) { ::insertIdx(&id[i].idx, idx); }

static boolT inList(IDX_ARRAY *list, Int idx)
/* Returns whether idx is in the list array or not */
{
    Int i;

    for (i = 0; i < list->csym; i++)
        if (list->idx[i] == idx)
            return (TRUE);
    return (FALSE);
}

/* Creates a new identifier node of type t and returns it.
 * Arguments: locSym : local long symbol table
 *            t : type of LONG identifier
 *            f : frame where this variable is located
 */
void LOCAL_ID::newIdent(hlType t, frameType f)
{
    if (csym == alloc) {
        alloc += LOCAL_ID_DELTA;
        id = (ID *)reallocVar(id, alloc * sizeof(ID));
        memset(&id[csym], 0, LOCAL_ID_DELTA * sizeof(ID));
    }

    id[csym].type = t;
    id[csym].loc = f;
    csym++;
}

/* Creates a new register identifier node of TYPE_BYTE_(UN)SIGN or
 * TYPE_WORD_(UN)SIGN type.  Returns the index to this new entry
 */
Int LOCAL_ID::newByteWordRegId(hlType t, byte regi)
{
    Int idx;

    /* Check for entry in the table */
    for (idx = 0; idx < csym; idx++) {
        if ((id[idx].type == t) && (id[idx].id.regi == regi))
            return (idx);
    }

    /* Not in table, create new identifier */
    newIdent(t, REG_FRAME);
    idx = csym - 1;
    id[idx].id.regi = regi;
    return (idx);
}

/* Flags the entry associated with the offset off to illegal, as this
 * offset is part of a long stack variable.
 * Note: it is easier enough to remove this entry by moving the rest of
 *       the array 1 position.  The problem is that indexes into this
 *       array have already been saved in several positions; therefore,
 *       flagging this entry as illegal is all that can be done.
 */
void LOCAL_ID::flagByteWordId(Int off)
{
    Int idx;

    for (idx = 0; idx < csym; idx++) {
        if (((id[idx].type == TYPE_WORD_SIGN) ||
             (id[idx].type == TYPE_BYTE_SIGN)) &&
            (id[idx].id.bwId.off == off) && (id[idx].id.bwId.regOff == 0)) {
            id[idx].illegal = TRUE;
            break;
        }
    }
}

/* Creates a new stack identifier node of TYPE_BYTE_(UN)SIGN or
 * TYPE_WORD_(UN)SIGN type.  Returns the index to this new entry.
 */
Int LOCAL_ID::newByteWordStkId(hlType t, Int off, byte regOff)
{
    Int idx;

    /* Check for entry in the table */
    for (idx = 0; idx < csym; idx++) {
        if ((id[idx].id.bwId.off == off) && (id[idx].id.bwId.regOff == regOff))
            return (idx);
    }

    /* Not in table, create new identifier */
    newIdent(t, STK_FRAME);
    idx = csym - 1;
    id[idx].id.bwId.regOff = regOff;
    id[idx].id.bwId.off = off;
    return (idx);
}

/* Checks if the entry exists in the locSym, if so, returns the idx to this
 * entry; otherwise creates a new global identifier node of type
 * TYPE_WORD_(UN)SIGN and returns the index to this new entry.
 * Arguments: locSym : ptr to the local symbol table
 *            seg: segment value for global variable
 *            off: offset from segment
 *            regi: indexed register into global variable
 *            ix: index into icode array
 *            t: HIGH_LEVEL type
 */
Int LOCAL_ID::newIntIdxId(int16 seg, int16 off, byte regi, Int ix, hlType t)
{
    Int idx;

    /* Check for entry in the table */
    for (idx = 0; idx < csym; idx++) {
        if (/*(locSym->id[idx].type == t) &&   Not checking type */
            (id[idx].id.bwGlb.seg == seg) && (id[idx].id.bwGlb.off == off) &&
            (id[idx].id.bwGlb.regi == regi))
            return (idx);
    }

    /* Not in the table, create new identifier */
    newIdent(t, GLB_FRAME);
    idx = csym - 1;
    id[idx].id.bwGlb.seg = seg;
    id[idx].id.bwGlb.off = off;
    id[idx].id.bwGlb.regi = regi;
    return (idx);
}

/* Checks if the entry exists in the locSym, if so, returns the idx to this
 * entry; otherwise creates a new register identifier node of type
 * TYPE_LONG_(UN)SIGN and returns the index to this new entry.
 */
Int LOCAL_ID::newLongRegId(hlType t, byte regH, byte regL, Int ix)
{
    Int idx;

    /* Check for entry in the table */
    for (idx = 0; idx < csym; idx++) {
        if (/*(locSym->id[idx].type == t) &&   Not checking type */
            (id[idx].id.longId.h == regH) && (id[idx].id.longId.l == regL)) {
            /* Check for occurrence in the list */
            if (inList(&id[idx].idx, ix))
                return (idx);
            else {
                /* Insert icode index in list */
                ::insertIdx(&id[idx].idx, ix);
                return (idx);
            }
        }
    }

    /* Not in the table, create new identifier */
    newIdent(t, REG_FRAME);
    ::insertIdx(&id[csym - 1].idx, ix);
    idx = csym - 1;
    id[idx].id.longId.h = regH;
    id[idx].id.longId.l = regL;
    return (idx);
}

/* Checks if the entry exists in the locSym, if so, returns the idx to this
 * entry; otherwise creates a new global identifier node of type
 * TYPE_LONG_(UN)SIGN and returns the index to this new entry.
 */
Int LOCAL_ID::newLongGlbId(int16 seg, int16 offH, int16 offL, Int ix, hlType t)
{
    Int idx;

    /* Check for entry in the table */
    for (idx = 0; idx < csym; idx++) {
        if (/*(locSym->id[idx].type == t) &&   Not checking type */
            (id[idx].id.longGlb.seg == seg) &&
            (id[idx].id.longGlb.offH == offH) &&
            (id[idx].id.longGlb.offL == offL))
            return (idx);
    }

    /* Not in the table, create new identifier */
    newIdent(t, GLB_FRAME);
    idx = csym - 1;
    id[idx].id.longGlb.seg = seg;
    id[idx].id.longGlb.offH = offH;
    id[idx].id.longGlb.offL = offL;
    return (idx);
}

/* Checks if the entry exists in the locSym, if so, returns the idx to this
 * entry; otherwise creates a new global identifier node of type
 * TYPE_LONG_(UN)SIGN and returns the index to this new entry.
 */
Int LOCAL_ID::newLongIdxId(int16 seg, int16 offH, int16 offL, byte regi, Int ix,
                           hlType t)
{
    Int idx;

    /* Check for entry in the table */
    for (idx = 0; idx < csym; idx++) {
        if (/*(id[idx].type == t) &&   Not checking type */
            (id[idx].id.longGlb.seg == seg) &&
            (id[idx].id.longGlb.offH == offH) &&
            (id[idx].id.longGlb.offL == offL) &&
            (id[idx].id.longGlb.regi == regi))
            return (idx);
    }

    /* Not in the table, create new identifier */
    newIdent(t, GLB_FRAME);
    idx = csym - 1;
    id[idx].id.longGlb.seg = seg;
    id[idx].id.longGlb.offH = offH;
    id[idx].id.longGlb.offL = offL;
    id[idx].id.longGlb.regi = regi;
    return (idx);
}

/* Creates a new stack identifier node of type TYPE_LONG_(UN)SIGN.
 * Returns the index to this entry. */
Int LOCAL_ID::newLongStkId(hlType t, Int offH, Int offL)
{
    Int idx;

    /* Check for entry in the table */
    for (idx = 0; idx < csym; idx++) {
        if ((id[idx].type == t) && (id[idx].id.longStkId.offH == offH) &&
            (id[idx].id.longStkId.offL == offL))
            return (idx);
    }

    /* Not in the table; flag as invalid offH and offL */
    flagByteWordId(offH);
    flagByteWordId(offL);

    /* Create new identifier */
    newIdent(t, STK_FRAME);
    idx = csym - 1;
    id[idx].id.longStkId.offH = offH;
    id[idx].id.longStkId.offL = offL;
    return (idx);
}

/* Returns the index to an appropriate long identifier.
 * Note: long constants should be checked first and stored as a long integer
 *       number in an expression record.
 */
Int LOCAL_ID::newLongId(opLoc sd, PICODE pIcode, hlFirst f, Int ix, operDu du,
                        Int off)
{
    Int idx = 0;
    PMEM pmH, pmL;

    if (f == LOW_FIRST) {
        pmL = (sd == SRC) ? &pIcode->ic.ll.src : &pIcode->ic.ll.dst;
        pmH = (sd == SRC) ? &(pIcode + off)->ic.ll.src
                          : &(pIcode + off)->ic.ll.dst;
    }
    else /* HIGH_FIRST */
    {
        pmH = (sd == SRC) ? &pIcode->ic.ll.src : &pIcode->ic.ll.dst;
        pmL = (sd == SRC) ? &(pIcode + off)->ic.ll.src
                          : &(pIcode + off)->ic.ll.dst;
    }

    if (pmL->regi == 0) /* global variable */
        idx =
            newLongGlbId(pmH->segValue, pmH->off, pmL->off, ix, TYPE_LONG_SIGN);

    else if (pmL->regi < INDEXBASE) /* register */
    {
        idx = newLongRegId(TYPE_LONG_SIGN, pmH->regi, pmL->regi, ix);
        if (f == HIGH_FIRST)
            setRegDU(pIcode, pmL->regi, du); /* low part */
        else
            setRegDU(pIcode, pmH->regi, du); /* high part */
    }

    else if (pmL->off) {                                       /* offset */
        if ((pmL->seg == rSS) && (pmL->regi == INDEXBASE + 6)) /* idx on bp */
            idx = newLongStkId(TYPE_LONG_SIGN, pmH->off, pmL->off);
        else if ((pmL->seg == rDS) && (pmL->regi == INDEXBASE + 7)) /* bx */
        { /* glb var indexed on bx */
            idx = newLongIdxId(pmH->segValue, pmH->off, pmL->off, rBX, ix,
                               TYPE_LONG_SIGN);
            setRegDU(pIcode, rBX, OPerDu_USE);
        }
        else /* idx <> bp, bx */
            printf("long not supported, idx <> bp\n");
    }

    else /* (pm->regi >= INDEXBASE && pm->off = 0) => indexed && no off */
        printf("long not supported, idx && no off\n");

    return (idx);
}

boolT checkLongEq(LONG_STKID_TYPE longId, PICODE pIcode, Int i, Int idx,
                  PPROC pProc, COND_EXPR **rhs, COND_EXPR **lhs, Int off)
/* Checks whether the long stack identifier is equivalent to the source or
 * destination operands of pIcode and pIcode+1 (ie. these are LOW_LEVEL
 * icodes at present).  If so, returns the rhs and lhs of this instruction.
 * Arguments: longId    : long stack identifier
 *            pIcode    : ptr to first LOW_LEVEL icode instruction
 *            i         : idx into local identifier table for longId
 *            idx       : idx into icode array
 *            pProc     : ptr to current procedure record
 *            rhs, lhs  : return expressions if successful. */
{
    PMEM pmHdst, pmLdst, pmHsrc, pmLsrc; /* pointers to LOW_LEVEL icodes */

    pmHdst = &pIcode->ic.ll.dst;
    pmLdst = &(pIcode + off)->ic.ll.dst;
    pmHsrc = &pIcode->ic.ll.src;
    pmLsrc = &(pIcode + off)->ic.ll.src;

    if ((longId.offH == pmHdst->off) && (longId.offL == pmLdst->off)) {
        *lhs = idCondExpLongIdx(i);
        if ((pIcode->ic.ll.flg & NO_SRC) != NO_SRC)
            *rhs = idCondExpLong(&pProc->localId, SRC, pIcode, HIGH_FIRST, idx,
                                 OPerDu_USE, off);
        return (TRUE);
    }
    else if ((longId.offH == pmHsrc->off) && (longId.offL == pmLsrc->off)) {
        *lhs = idCondExpLong(&pProc->localId, DST, pIcode, HIGH_FIRST, idx,
                             OPerDu_DEF, off);
        *rhs = idCondExpLongIdx(i);
        return (TRUE);
    }
    return (FALSE);
}

boolT checkLongRegEq(LONGID_TYPE longId, PICODE pIcode, Int i, Int idx,
                     PPROC pProc, COND_EXPR **rhs, COND_EXPR **lhs, Int off)
/* Checks whether the long stack identifier is equivalent to the source or
 * destination operands of pIcode and pIcode+1 (ie. these are LOW_LEVEL
 * icodes at present).  If so, returns the rhs and lhs of this instruction.
 * Arguments: longId    : long stack identifier
 *            pIcode    : ptr to first LOW_LEVEL icode instruction
 *            i         : idx into local identifier table for longId
 *            idx       : idx into icode array
 *            pProc     : ptr to current procedure record
 *            rhs, lhs  : return expressions if successful. */
{
    PMEM pmHdst, pmLdst, pmHsrc, pmLsrc; /* pointers to LOW_LEVEL icodes */

    pmHdst = &pIcode->ic.ll.dst;
    pmLdst = &(pIcode + off)->ic.ll.dst;
    pmHsrc = &pIcode->ic.ll.src;
    pmLsrc = &(pIcode + off)->ic.ll.src;

    if ((longId.h == pmHdst->regi) && (longId.l == pmLdst->regi)) {
        *lhs = idCondExpLongIdx(i);
        if ((pIcode->ic.ll.flg & NO_SRC) != NO_SRC)
            *rhs = idCondExpLong(&pProc->localId, SRC, pIcode, HIGH_FIRST, idx,
                                 OPerDu_USE, off);
        return (TRUE);
    }
    else if ((longId.h == pmHsrc->regi) && (longId.l == pmLsrc->regi)) {
        *lhs = idCondExpLong(&pProc->localId, DST, pIcode, HIGH_FIRST, idx,
                             OPerDu_DEF, off);
        *rhs = idCondExpLongIdx(i);
        return (TRUE);
    }
    return (FALSE);
}

/* Given an index into the local identifier table for a long register
 * variable, determines whether regi is the high or low part, and returns
 * the other part
 */
byte LOCAL_ID::otherLongRegi(byte regi, Int idx)
{
    ID *id;

    id = &this->id[idx];
    if ((id->loc == REG_FRAME) &&
        ((id->type == TYPE_LONG_SIGN) || (id->type == TYPE_LONG_UNSIGN))) {
        if (id->id.longId.h == regi)
            return (id->id.longId.l);
        else if (id->id.longId.l == regi)
            return (id->id.longId.h);
    }
    return 0; // Cristina: please check this!
}

/* Checks if the registers regL and regH have been used independently in
 * the local identifier table.  If so, macros for these registers are
 * placed in the local identifier table, as these registers belong to a
 * long register identifier.
 */
void LOCAL_ID::propLongId(byte regL, byte regH, const char *name)
{
    Int i;
    ID *id;

    for (i = 0; i < csym; i++) {
        id = &this->id[i];
        if ((id->type == TYPE_WORD_SIGN) || (id->type == TYPE_WORD_UNSIGN)) {
            if (id->id.regi == regL) {
                strcpy(id->name, name);
                strcpy(id->macro, "LO");
                id->hasMacro = TRUE;
                id->illegal = TRUE;
            }
            else if (id->id.regi == regH) {
                strcpy(id->name, name);
                strcpy(id->macro, "HI");
                id->hasMacro = TRUE;
                id->illegal = TRUE;
            }
        }
    }
}
