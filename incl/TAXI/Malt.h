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
#ifndef _MALT_H
#define _MALT_H
class MondecaArea {
 private:
  long Offset[256];
  long Size[256];
  long *Buffer;
  long sizeP;
  long size_free;
 
  void init(long sP)
      {
      Buffer = (long *)::operator new((unsigned int)sP*sizeof(long));
      for (int i=0; i<256;i++) // clear
          Offset[i]=Size[i]=0;
      
      Offset[MONDECA_JAVA_DATA]=0;
      Size[MONDECA_JAVA_DATA]=0;
      Offset[MONDECA_PIGALE_DATA]=0;
      Size[MONDECA_PIGALE_DATA]=0;
      size_free=sizeP=sP; 
    }
  void grow(long adds)
    {
    long *nbuff = (long *)::operator new((unsigned int)(sizeP+adds)*sizeof(long));
    memcpy((void *)nbuff, Buffer, sizeP);
    delete Buffer;
    Buffer=nbuff;
    sizeP += adds;
    size_free+= adds;
    }
 public:
  MondecaArea(long sp)
    { init(sp);}
  MondecaArea() {init(4096);}
  long size(int id) const { return Size[id]-1;}
  long * operator[] (int id) const { return Buffer+Offset[id];}
  long * operator() (int id, long s)
    {s+=1;
    if (Size[id]!=0)
      {if (Size[id]==s) // ok
	return (*this)[id];
      else 
	; // erreur
      }
    if (size_free < s)
      grow(s);
    Size[id]=s;
    Offset[id]=Offset[MONDECA_PIGALE_DATA]+Size[MONDECA_PIGALE_DATA];
    Size[MONDECA_PIGALE_DATA]+=s;
    size_free -= s;
    return (*this)[id];
    }
};
#endif
