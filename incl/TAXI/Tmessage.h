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

#ifndef _TMESSAGE_H_INCLUDED_
#define _TMESSAGE_H_INCLUDED_
void Tmessage(const char *tstring);
void Tclear();
void Twait(const char *txt);
void Tprintf(char *fmt,...);
#endif
