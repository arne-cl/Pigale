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

#ifndef __TSTRING_H__
#define __TSTRING_H__

#include  <iostream.h>
#include  <string.h>

class tstring
  {
  friend void Build(tstring &);
  friend void Destroy(tstring &);
  friend void Cast(const char *const str, tstring &);
  struct srep 
      {char *s;
        int n;    // nombre de references
        srep ()  {n = 1;}
      };
  
  srep *p;

  void delref()
    {if(--p->n == 0){delete[] p->s; delete p;}}
  void addref(srep *pp)
    {pp->n++;}
  void newref()
    {if(p->n>1)
      {delete[] p->s; delete p; p=new srep;}
    else 
      { delete[] p->s;}
    }

  public:
  tstring()
      {p = new srep;
      p->s = new char[1];
      p->s[0] = '\0';
      }
  tstring(const tstring &x)
      {x.p->n++; p = x.p;}
  tstring(const char * const str)
      {p = new srep;
      p->s = new char[strlen(str) + 1];
      strcpy(p->s,str);
      }
  tstring(char c, int l=1)
      {p = new srep;
      p->s = new char[l + 1];
      memset(p->s,c,l);
      p->s[l] = '\0';
      }
  ~tstring()
      {delref();
      }
  tstring & operator =(const tstring &x)
      {addref(x.p);
      delref();
      p = x.p;
      return *this;
      }
  tstring & operator =(const char * const str)
      {newref();
      p->s = new char[strlen(str) + 1];
      strcpy(p->s,str);
      return *this;
      }
  
  friend tstring operator + (tstring const &x, tstring const &y)
      {tstring tmp(x); tmp += y; return tmp;}
  friend tstring operator + (tstring const &x, char const *y)
      {tstring tmp(x); tmp += y; return tmp;}
  friend tstring operator + (tstring const &x, char c)
      {tstring tmp(x); tmp += c; return tmp;}
  friend tstring operator + (char const *x,tstring const &y)
      {tstring tmp(x); tmp += y; return tmp;}
  friend tstring operator + (char c,tstring const &y)
      {tstring tmp(c); tmp += y; return tmp;}
  
  tstring & operator +=(const tstring &x)
      {char *s = new char[length() + x.length()+1];
      strcpy(s,p->s);
      strcat(s,x.p->s);
      newref();
      p->s = s;
      return *this;
      }
  tstring & operator += (char c)
      {operator +=(tstring(c)); return *this;}
  
  operator       char * const ()       { return p->s;}
  operator const char * const () const { return p->s;}
  char * operator ~()       { return p->s;}
  const char * const operator ~() const { return p->s;}

  char  operator[] (int i) const
      {if(i > length())return '\0';
      return p->s[i];
      }
  char & operator[] (int i)  
      {if(i > length())return p ->s[0];  // ????
      if(p->n > 1)
          {srep* np = new srep;
          np->s = new char[strlen(p->s)+1];
          strcpy(np->s,p->s);
          p->n--;
          p = np;
          }
      return p->s[i];
      }

  const int length() const {return (int)strlen(p->s);}
    
  friend int operator == (tstring const &x, tstring const &y)
      {return strcmp(x.p->s, y.p->s) == 0;}
  friend int operator == (tstring const &x, char const *y)
      {return strcmp(x.p->s, y) == 0;}
  friend int operator == (char const *x, tstring const &y)
      {return strcmp(x, y.p->s) == 0;}
  friend int operator != (tstring const &x, tstring const &y)
      {return strcmp(x.p->s, y.p->s);}
  friend int operator != (tstring const &x, char const *y)
      {return strcmp(x.p->s, y);}
  friend int operator != (char const *x, tstring const &y)
      {return strcmp(x, y.p->s);}

  friend ostream& operator <<(ostream &os,const tstring &x)
      {return os << x.p->s << endl;}
  friend istream& operator >> (istream &is, tstring & x)
      {char buff[256];
      is >> buff;
      x = buff;
      return is;
      }
  
  //substrings
  tstring operator () (int pos) const
    {tstring tmp;
    if(pos < length()) tmp = p->s + pos;
    return tmp;
    }
  tstring operator () (int pos, int l) const
      {tstring tmp;
        if(pos < length())
            {tmp = p->s + pos;
            if(l < tmp.length())tmp.p->s[l+1] = '\0';
            }
        return tmp;
      }
 
  // Others
  int match(tstring const &x, int pos) const
      {tstring tmp;
      if(pos < length()) tmp = p->s + pos;
      else return 0;
      if(strcmp(x.p->s,tmp.p->s) <= 0)return 1;
      return 0;
      }
  int match(tstring const &x) const
      {int end = length() - x.length();
      
      for(int pos = 0;pos <= end;++pos)
          if(match(x,pos))return pos;
      return -1;
      }
  
  
  };
#endif

