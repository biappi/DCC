/****************************************************************************
 *          dcc project general header
 * (C) Cristina Cifuentes, Mike van Emmerik
 ****************************************************************************/

/**** Common definitions and macros ****/
typedef int Int;              /* Int: 0x80000000..0x7FFFFFFF  */
typedef unsigned int flags32; /* 32 bits  */
typedef unsigned int dword;   /* 32 bits  */
#define MAX 0x7FFFFFFF
/* Type definitions used in the program */
typedef unsigned char byte;  /* 8 bits   */
typedef unsigned short word; /* 16 bits  */
typedef short int16;         /* 16 bits  */
typedef unsigned char boolT; /* 8 bits   */

#define TRUE 1
#define FALSE 0

#define SYNTHESIZED_MIN 0x100000 /* Synthesized labs use bits 21..32 */

/* These are for C library signature detection */
#define SYMLEN 16 /* Length of proc symbols, incl null */
#define PATLEN 23 /* Length of proc patterns  */
#define WILD 0xF4 /* The wild byte            */

/****** MACROS *******/

/* Macro to allocate a node of size sizeof(structType). */
#define allocStruc(structType) (structType *)allocMem(sizeof(structType))

/* Macro reads a LH word from the image regardless of host convention */
/* Returns a 16 bit quantity, e.g. C000 is read into an Int as C000 */
//#define LH(p)  ((int16)((byte *)(p))[0] + ((int16)((byte *)(p))[1] << 8))
#define LH(p) ((word)((byte *)(p))[0] + ((word)((byte *)(p))[1] << 8))

/* Macro reads a LH word from the image regardless of host convention */
/* Returns a signed quantity, e.g. C000 is read into an Int as FFFFC000 */
#define LHS(p) (((byte *)(p))[0] + (((char *)(p))[1] << 8))

/* Macro tests bit b for type t in prog.map */
#define BITMAP(b, t) (prog.map[(b) >> 2] & ((t) << (((b)&3) << 1)))

/* Macro to convert a segment, offset definition into a 20 bit address */
#define opAdr(seg, off) ((seg << 4) + off)
