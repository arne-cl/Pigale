/****************************************************************************
**
** Copyright (C) 2001 Hubert de Fraysseix, Patrice Ossona de Mendez.
** All rights reserved.
** This file is part of the PIGALE Toolkit.
**
** This file may be distributed under the terms of the GNU Public License
** appearing in the file LICENSE.HTML included in the packaging of this file.
**
*****************************************************************************/

#ifndef __MYDEBUG_H__
#define __MYDEBUG_H__

bool & debug();
int& Error();
void myabort();
void DebugPuts(char *str);
void DebugPrintf(char *fmt,...);
void LogPrintf(char *fmt,...);
void DebugIndent(int i);

#ifdef TDEBUG
  #include  <stdlib.h>

  #define DOPEN()   { DebugIndent(1); }
  #define DCLOSE()  { DebugIndent(-1); }

  #define DPRINTF(arg) { \
     DebugPrintf("%s(%d): ", __FILE__, __LINE__); \
     DebugPrintf arg; \
  }
  #define PRINTF(arg) { \
     DebugPrintf arg; \
  }
  #define DPUTS(str) { \
     DebugPrintf("%s(%d): %s", __FILE__, __LINE__, str); \
  }
  #define CHKNEW(p) { \
     if (p == 0) { \
        DebugPrintf("%s(%d): Short of memory.\n", __FILE__, __LINE__); \
        exit(1); \
     } \
  }
#else
  #define DOPEN()
  #define DCLOSE()
  #define DPRINTF(arg) {}
  #define DPUTS(str) {}
  #define CHKNEW(p)
#endif


#endif

