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

 

#include <stdlib.h>
#include <stdio.h> 
#include <stdarg.h>
#include <string.h>
#include <TAXI/Tdebug.h>
#include <TAXI/Tmessage.h>



static char LogName[] = "log.txt";
static int Indent = 0;
static bool first = true;
static _Error MainError;

_T_Error &ErrorPositioner(const char *f, int l)
     {MainError.file=f; MainError.line=l; return MainError.error;}

int getError() 
  {return (int) MainError.error;}
const char * getErrorMsg()
  {return MainError.error.msg;}
const char * getErrorFile()
  {return MainError.file;}
int getErrorLine()
  {return MainError.line;}

bool& debug()
  {static bool i = 0;
  return i;
  }
void DebugIndent(int i)
  {Indent += i;} 

void DebugPuts(const char *str)
  {FILE *LogFile;
  if(first)
      {LogFile = fopen(LogName, "w");first = false;}
  else
      LogFile = fopen(LogName, "aw");
  if(LogFile == NULL){setError(-1);return;}
  fprintf(LogFile, "%*s%s\n", Indent*2, "", str);
  fclose(LogFile);
  }

void DebugPrintf(const char *fmt,...)
  {FILE *LogFile;
  va_list arg_ptr;
  char buff[256];
  va_start(arg_ptr,fmt);
  vsprintf(buff,fmt,arg_ptr);
  va_end(arg_ptr);

  if(first)
      {LogFile = fopen(LogName, "w");first = false;}
  else
      LogFile = fopen(LogName, "aw");
  if(LogFile == NULL){setError(-1);return;}
  fprintf(LogFile, "%*s%s\n", Indent*2, "", buff);
  Tprintf("%*s%s", Indent*2, "", buff);
  fclose(LogFile);
  }


void LogPrintf(const char *fmt,...)
  {FILE *LogFile;
  va_list arg_ptr;
  char buff[256];
  va_start(arg_ptr,fmt);
  vsprintf(buff,fmt,arg_ptr);
  va_end(arg_ptr);

  if(first)
      {LogFile = fopen(LogName, "w");first = false;}
  else
      LogFile = fopen(LogName, "aw");
  if(LogFile == NULL){setError(-1);return;}
  fprintf(LogFile,"%s",buff);
  fclose(LogFile);
  }
void myabort() {abort();}

