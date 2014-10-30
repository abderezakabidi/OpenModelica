/*
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-CurrentYear, Open Source Modelica Consortium (OSMC),
 * c/o Linköpings universitet, Department of Computer and Information Science,
 * SE-58183 Linköping, Sweden.
 *
 * All rights reserved.
 *
 * THIS PROGRAM IS PROVIDED UNDER THE TERMS OF THE BSD NEW LICENSE OR THE
 * GPL VERSION 3 LICENSE OR THE OSMC PUBLIC LICENSE (OSMC-PL) VERSION 1.2.
 * ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS PROGRAM CONSTITUTES
 * RECIPIENT'S ACCEPTANCE OF THE OSMC PUBLIC LICENSE OR THE GPL VERSION 3,
 * ACCORDING TO RECIPIENTS CHOICE.
 *
 * The OpenModelica software and the OSMC (Open Source Modelica Consortium)
 * Public License (OSMC-PL) are obtained from OSMC, either from the above
 * address, from the URLs: http://www.openmodelica.org or
 * http://www.ida.liu.se/projects/OpenModelica, and in the OpenModelica
 * distribution. GNU version 3 is obtained from:
 * http://www.gnu.org/copyleft/gpl.html. The New BSD License is obtained from:
 * http://www.opensource.org/licenses/BSD-3-Clause.
 *
 * This program is distributed WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE, EXCEPT AS
 * EXPRESSLY SET FORTH IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE
 * CONDITIONS OF OSMC-PL.
 *
 */

/* File: meta_modelica_builtin.h
 * Description: This is the C header file for the new builtin
 * functions existing in MetaModelica.
 */

#ifndef META_MODELICA_BUILTIN_H_
#define META_MODELICA_BUILTIN_H_

#include "openmodelica.h"

#if defined(__cplusplus)
extern "C" {
#endif

#include "meta_modelica_builtin_boxptr.h"

typedef modelica_metatype metamodelica_string;
typedef const modelica_metatype metamodelica_string_const;

extern metamodelica_string intString(modelica_integer);
extern modelica_string realString(modelica_real);

/* String Character Conversion */

#define stringCharInt(X) nobox_stringCharInt(threadData,X)
extern modelica_integer nobox_stringCharInt(threadData_t *threadData,metamodelica_string chr);
#define intStringChar(X) nobox_intStringChar(threadData,X)
extern metamodelica_string nobox_intStringChar(threadData_t *threadData,modelica_integer ix);

/* String Operations */
#define stringInt(x) nobox_stringInt(threadData,x)
#define stringReal(x) nobox_stringReal(threadData,x)
extern modelica_integer nobox_stringInt(threadData_t*,metamodelica_string s);
extern modelica_real nobox_stringReal(threadData_t*,metamodelica_string s);
extern modelica_metatype stringListStringChar(metamodelica_string s);
extern metamodelica_string_const stringAppend(metamodelica_string_const s1,metamodelica_string_const s2);
extern metamodelica_string stringAppendList(modelica_metatype lst);
extern metamodelica_string stringDelimitList(modelica_metatype lst,metamodelica_string_const delimiter);
#define stringLength(x) MMC_STRLEN(x)
extern modelica_integer mmc_stringCompare(const void * str1,const void * str2);
#define stringGetStringChar(X,Y) boxptr_stringGetStringChar(threadData,X,mmc_mk_icon(Y))
extern modelica_metatype boxptr_stringGetStringChar(threadData_t*,metamodelica_string str,modelica_metatype ix);
#define stringGet(X,Y) nobox_stringGet(threadData,X,Y)
extern modelica_integer nobox_stringGet(threadData_t *threadData,metamodelica_string str, modelica_integer ix);
#define stringGetNoBoundsChecking(str,ix) MMC_STRINGDATA((str))[(ix)-1]
#define stringUpdateStringChar(X,Y,Z) boxptr_stringUpdateStringChar(threadData,X,Y,mmc_mk_icon(Z))
extern modelica_metatype boxptr_stringUpdateStringChar(threadData_t *,metamodelica_string str, metamodelica_string c, modelica_metatype ix);
extern modelica_integer stringHash(metamodelica_string_const);
extern modelica_integer stringHashDjb2(metamodelica_string_const s);
extern modelica_integer stringHashDjb2Mod(metamodelica_string_const s,modelica_integer mod);
extern modelica_integer stringHashSdbm(metamodelica_string_const str);
#define substring(X,Y,Z) boxptr_substring(threadData,X,mmc_mk_icon(Y),mmc_mk_icon(Z))
extern modelica_metatype boxptr_substring(threadData_t *,metamodelica_string_const str, modelica_metatype start, modelica_metatype stop);

#define System_stringHashDjb2Mod stringHashDjb2Mod
#define boxptr_System_stringHashDjb2Mod boxptr_stringHashDjb2Mod
#define boxvar_System_stringHashDjb2Mod boxvar_stringHashDjb2Mod

extern modelica_metatype boxptr_stringHashDjb2Mod(threadData_t*,modelica_metatype v,modelica_metatype mod);

/* List Operations */
extern modelica_metatype listReverse(modelica_metatype);
extern modelica_metatype listReverseInPlace(modelica_metatype);
extern modelica_boolean listMember(modelica_metatype, modelica_metatype);
extern modelica_metatype listAppend(modelica_metatype,modelica_metatype);
extern modelica_integer listLength(modelica_metatype);
#define listGet(X,Y) boxptr_listGet(threadData,X,mmc_mk_icon(Y))
#define listEmpty(LST) MMC_NILTEST(LST)
#define listDelete(X,Y) boxptr_listDelete(threadData,X,mmc_mk_icon(Y))
#define listRest(X) boxptr_listRest(threadData,X)
#define listHead(X) boxptr_listHead(threadData,X)

#define isNone(X) MMC_OPTIONNONE(X)
#define isSome(X) MMC_OPTIONSOME(X)

extern modelica_metatype boxptr_listNth(threadData_t*,modelica_metatype,modelica_metatype);
extern modelica_metatype boxptr_listGet(threadData_t*,modelica_metatype,modelica_metatype);
extern modelica_metatype boxptr_listDelete(threadData_t*,modelica_metatype,modelica_metatype);
extern modelica_metatype boxptr_listRest(threadData_t*,modelica_metatype);
extern modelica_metatype boxptr_listHead(threadData_t*,modelica_metatype);

/* Option Operations */
#define optionNone(x) (0==MMC_HDRSLOTS(MMC_GETHDR(x)) ? 1 : 0)

/* Array Operations */
static inline modelica_integer arrayLength(modelica_metatype arr)
{
  return MMC_HDRSLOTS(MMC_GETHDR(arr));
}

extern modelica_metatype arrayList(modelica_metatype);
extern modelica_metatype listArray(modelica_metatype);
extern modelica_metatype arrayCopy(modelica_metatype);
#define arrayGet(X,Y) nobox_arrayGet(threadData,X,Y)
static inline modelica_metatype nobox_arrayGet(threadData_t *threadData,modelica_metatype arr,modelica_integer ix)
{
  int nelts = MMC_HDRSLOTS(MMC_GETHDR(arr));
  if (ix < 1 || ix > nelts)
    MMC_THROW_INTERNAL();
  return MMC_STRUCTDATA(arr)[ix-1];
}
static inline modelica_metatype arrayCreate(modelica_integer nelts, modelica_metatype val)
{
  if (nelts < 0) {
    MMC_THROW();
  } else {
    void* arr = (struct mmc_struct*)mmc_mk_box_no_assign(nelts, MMC_ARRAY_TAG);
    void **arrp = MMC_STRUCTDATA(arr);
    int i = 0;
    for(i=0; i<nelts; i++)
      arrp[i] = val;
    return arr;
  }
}

#define arrayGetNoBoundsChecking(arr,ix) (MMC_STRUCTDATA((arr))[(ix)-1])
#define arrayUpdate(X,Y,Z) boxptr_arrayUpdate(threadData,X,mmc_mk_icon(Y),Z)
extern modelica_metatype arrayAdd(modelica_metatype, modelica_metatype);

extern modelica_metatype boxptr_arrayNth(threadData_t *threadData,modelica_metatype,modelica_metatype);
extern modelica_metatype boxptr_arrayGet(threadData_t *threadData,modelica_metatype,modelica_metatype);
static inline modelica_metatype boxptr_arrayUpdate(threadData_t *threadData,modelica_metatype arr, modelica_metatype i, modelica_metatype val)
{
  int ix = mmc_unbox_integer(i);
  int nelts = MMC_HDRSLOTS(MMC_GETHDR(arr));
  if (ix < 1 || ix > nelts)
    MMC_THROW_INTERNAL();
  MMC_STRUCTDATA(arr)[ix-1] = val;
  return arr;
}

extern modelica_metatype boxptr_arrayUpdate(threadData_t *threadData,modelica_metatype, modelica_metatype, modelica_metatype);

/* Misc Operations */
#define print(X) boxptr_print(NULL,X)
extern void boxptr_print(threadData_t*,modelica_metatype);
extern modelica_integer tick(void);
extern modelica_real mmc_clock(void);
#define equality(X,Y) boxptr_equality(threadData,X,Y)
extern void boxptr_equality(threadData_t *,modelica_metatype, modelica_metatype);

/* Weird RML stuff */
#define getGlobalRoot(X) nobox_getGlobalRoot(threadData,X)
#define setGlobalRoot(X,V) boxptr_setGlobalRoot(threadData,mmc_mk_icon(X),V)
#define valueConstructor(val) MMC_HDRCTOR(MMC_GETHDR(val))

extern modelica_metatype nobox_getGlobalRoot(threadData_t*,modelica_integer);
extern void boxptr_setGlobalRoot(threadData_t*,modelica_metatype, modelica_metatype);
extern modelica_metatype boxptr_valueConstructor(threadData_t*,modelica_metatype);
#define referenceEq(X,Y) ((X) == (Y))

extern modelica_real realMaxLit(void);
extern modelica_integer intMaxLit(void);

extern modelica_boolean setStackOverflowSignal(modelica_boolean);
extern metamodelica_string referenceDebugString(modelica_metatype fnptr);

#include "meta_modelica_builtin_boxvar.h"

#if defined(__cplusplus)
}
#endif

#endif /* META_MODELICA_BUILTIN_H_ */
