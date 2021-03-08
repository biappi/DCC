/****************************************************************************
 *  dcc project disassembler header
 * (C) Mike van Emmerik
 ****************************************************************************/

/* Definitions for extended keys (first key is zero) */

#define EXT 0x100 /* "Extended" flag */

#ifdef __MSDOS__
#define KEY_DOWN EXT + 'P'
#define KEY_LEFT EXT + 'K'
#define KEY_UP EXT + 'H'
#define KEY_RIGHT EXT + 'M'
#define KEY_NPAGE EXT + 'Q'
#define KEY_PPAGE EXT + 'I'
#endif

#ifdef _CONSOLE
#define KEY_DOWN 0x50 /* Same as keypad scancodes */
#define KEY_LEFT 0x4B
#define KEY_UP 0x48
#define KEY_RIGHT 0x4D
#define KEY_NPAGE 0x51 /* Enter correct value! */
#define KEY_PPAGE 0x49 /* Another guess! */
#endif

/* "Attributes" */
#define A_NORMAL 'N' /* For Dos/Unix */
#define A_REVERSE 'I'
#define A_BOLD 'B'

#define LINES 24
#define COLS 80
