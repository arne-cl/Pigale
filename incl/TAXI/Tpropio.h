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

#ifndef TPROPIO_H
#define TPROPIO_H

#include <TAXI/Tsvector.h>
#include <TAXI/Tbase.h>
#include <TAXI/Tsvector.h>
#include <TAXI/Tstring.h>


// Par defaut, export/import par copie
//

/*
 *  Principe de l'import/export
 *
 * Export prend un pointeur sur un objet du type et renvoie un _svector *
 * obtenu par un new. Seul le buffer du _svector est signifiant.
 *
 * Import recoit un _svector * et l'utilise pour initialiser un objet.
 * Seul le buffer du _svector est signifiant.
 */

//#ifndef _MSC_VER
//    inline void* operator new(size_t, void* p) {return p;}
//#else
//#include <new.h>
//#endif

template <class T>
struct TypeHandler
{
    static void Import(T& obj, const _svector *p) {obj = *(T *)(p->begin());}
    static _svector *Export(const void *pobj)
        {svector<T> *v = new svector<T>(1);
        new (v->begin()) T(*(const T *)pobj);
        return v;
        }
};

template <>
struct TypeHandler<tstring>
{
    static void Import(tstring& obj, const _svector *v)
        {obj = (char *)(v->begin());}
    static _svector *Export(const void *pobj)
        {
        const tstring & obj = *(const tstring *)pobj;
        int l = obj.length();
        svector<char> *v = new svector<char>(l+1);
        
        memcpy(v->begin(), (void *)~obj,l);
        (*v)[l] = '\0';
        return v;
        }
};

#endif
