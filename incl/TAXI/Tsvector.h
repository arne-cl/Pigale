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


#ifndef TSVECTOR_H
#define TSVECTOR_H

#include <string.h>
#include <stdlib.h>
#include <TAXI/Tbase.h>
#include <TAXI/Tdebug.h>

#define TSVECTOR_INCREMENT (Max((finish-start)>>1,32))

/* Vecteur simple non type : _svector
 *
 * Cette classe contient un grand nombre de fonctionalites
 *
*/
class _svector
{
 protected:
    int start;
    int finish;        
    void *ptr0;
    void *buff;
    int _start;
    int _finish;
    size_t size_elmt;
    void *init;
    char name[16];

    friend void Destroy(_svector &x);
    int InBuffRange(int index) const 
        {return ((index >= _start+2) && (index < _finish));}
    void * tmpptr() 
        {return (void*) ((char *)buff+size_elmt);}
    const void * tmpptr() const 
        {return (const void*) ((const char *)buff+size_elmt);}
    void * ptr(int i) 
        {return (void*) ((char *)ptr0+i*size_elmt);}
    const void * ptr(int i) const 
        {return (const void*) ((const char *)ptr0+i*size_elmt);}
    void ReserveGrow(int a, int b)
        {if (InBuffRange(a) && InBuffRange(b)) return;
        int inc = Max(Max(b-finish+1,start-a+2),TSVECTOR_INCREMENT);
        if ((a-2 < _start) || (_start < -2)) _start -= inc;
        if ((_finish < b+1) || (_start < -2)) _finish += inc;
        realloc_buffer();
        }
    void realloc_buffer()
        {unsigned int nsize = (_finish - _start)*size_elmt;
        char *nbuff = (char *)::operator new (nsize);
        if (start!=finish)
            {memcpy((void *)(nbuff+(start-_start)*size_elmt),
                    begin(),getsize());
            }
	if (init)
            {memcpy((void *)nbuff,buff,size_elmt);
	    init=nbuff;
	    }
        delete (char *)buff;
        buff = (void *)nbuff;
        ptr0 = (void *)(nbuff - _start*size_elmt);
        }
    void reserve(int a, int b)
        {if (InBuffRange(a) && InBuffRange(b)) return;
        if (a-2 < _start) _start = a-2;
        if (_finish < b+1) _finish = b-1;
        realloc_buffer();
        }
    void vreserve(int s, int f, size_t nsize_elmt)
        {unsigned int oldsize = (unsigned int)(_finish-_start)*size_elmt;
        unsigned int nsize = (unsigned int)(f-s)*nsize_elmt;
        if (nsize >oldsize)
            {char *nbuff = (char *)::operator new(nsize);
            if (init)
                if (nsize_elmt==size_elmt) 
                    {memcpy((void *)nbuff,buff,size_elmt);
		    init=nbuff;
		    }
                else init=0;
            delete (char *)buff;
            buff = (void *)nbuff;
            }
        ptr0 = (void *)((char *)buff-s*nsize_elmt);
        _start = s;
        _finish = f;
        size_elmt = nsize_elmt;
        }
  
 public:
    _svector(int a, int b, int size_elt,const void *p=0) :
        start(a), finish(b+1),size_elmt(size_elt),init(0)
        {unsigned int sizebuff = getsize()+2*size_elmt;
        if (sizebuff) buff = ::operator new (sizebuff);
        else buff = 0;
        _start = start-2;
        _finish = finish;
        ptr0 = (void *)((char *)buff-start*size_elmt);
        definit(p);
        strcpy(name,"*");
        name[sizeof(name)-1]='\0';
        reset();
        }
    _svector(const _svector &s) : start(0), finish(0), ptr0(0), buff(0),
        _start(0), _finish(0), size_elmt(0),init(0)
        {
            strcpy(name,"*");
            name[sizeof(name)-1]='\0';
            copy(s); 
        }
    _svector() : start(0), finish(0), ptr0(0), buff(0), 
        _start(0), _finish(0), size_elmt(0), init(0)
        {strcpy(name,"*"); name[sizeof(name)-1]='\0'; }
    ~_svector() {delete (char *)buff;}

    int empty() const {return start==finish;}
    int InRange(int index) const 
        {return ((index >= start) && (index < finish));}
    int getsize() const {return size_elmt*(finish-start);}
    int n() const {return finish-start;}
    int SizeElmt() {return size_elmt;}
    int starti() const {return start;}
    int stopi() const {return finish;}
    void * begin() {return ptr(start);}
    const void * begin() const {return ptr(start);}
    void * origin() {return ptr0;}
    const void * origin() const {return ptr0;}
    void * end() {return ptr(finish);}
    const void * end() const {return ptr(finish);}

    void resize(int a, int b)
        {ReserveGrow(a,b);
        int tmp_start = start;
        int tmp_finish = finish;
        start = a;
        finish = b+1;
        if (a < tmp_start) reset(a,tmp_start-1);
        if (b >= tmp_finish) reset(tmp_finish,b);
        }

    void clear()
        { int s=getsize(); if (s!=0) memset(begin(),0,s);}
    void fill(int from, int to, char byte=0)
        { check(from);
        check(to);
        if (from>to) return;
        memset(ptr(from),byte,(to-from+1)*size_elmt);
        }
    void reset(int a, int b)
        {if (!init) return;
        check(a);
        check(b);
        char *p = (char *)ptr(a);
        for (int i=a; i<=b; i++)
            {memcpy(p,init,(unsigned int) size_elmt);
            p += size_elmt;
            }
        }
    void reset() {reset(start,finish-1);}
    void swap(_svector &s)
        {int tmp=start; start=s.start; s.start=tmp;
        tmp = finish; finish=s.finish; s.finish=tmp;
        tmp=_start; _start=s._start; s._start=tmp;
        tmp = _finish; _finish=s._finish; s._finish=tmp;
        void *ptr=buff; buff=s.buff; s.buff=ptr;
        ptr=ptr0; ptr0=s.ptr0; s.ptr0=ptr;
        ptr=init; init=s.init; s.init=ptr;
        tmp = size_elmt; size_elmt=s.size_elmt; s.size_elmt=tmp;
        }
    _svector &operator=(const _svector &s)
        {return copy(s);
        }
    _svector &copy(const _svector &s)
        {if (&s==this) return *this;
        vreserve(s.start-2,s.finish,s.size_elmt);
        start = s.start;
        finish = s.finish;
        memcpy(begin(),s.begin(),getsize());
        return *this;
        }
    const void * pinit() const {return init;}
    void * pinit() {return init;}
    void definit(const void *p)
        {if(p)
            {if (!init) 
		{if (!buff) reserve(0,-1);
		init=buff;
		}
            memcpy(init,p,size_elmt);
            }
        else if (init)
            init = 0;
        }
    void SetName(const char *txt)
        {strncpy(name,txt,sizeof(name)-1);}
    const char *GetName() const {return name;}

#ifdef DEBUG
    void check(int i) const
        {if (!InRange(i))
            {if (empty()) 
                DPRINTF(("%s indice %d for empty vector",name,i))//;
            else 
                DPRINTF(("%s indice %d out of [%d %d[",name,i,start,finish))//;
            myabort();
            }
        }
#else
    void check(int) const {}
#endif

    void SwapIndex(int a, int b)
        { check(a); check(b);
        memcpy(tmpptr(),ptr(a),size_elmt);
        memcpy(ptr(a),ptr(b),size_elmt);
        memcpy(ptr(b),tmpptr(),size_elmt);
        }
    void CopyIndex(int a,int b)
        { check(a);
        memcpy(ptr(a),ptr(b),size_elmt);
        }
    void qsort(int (*compare)(const void *, const void *))
        { ::qsort(begin(),n(),SizeElmt(),compare);}

    };
#include <TAXI/elmt.h>
template <class T>
class svector : public _svector
    { _svector & me() { return *this;}
    const _svector & me() const { return *this;}
    public :
    svector() : _svector(0,-1,sizeof(T)) {}
    svector(int a, int b) : _svector(a,b,sizeof(T)) {}
    svector(int a, int b, const T& value) : _svector(a,b,sizeof(T),(const void *)&value)
        { for (T * p=begin(); p!=end(); p++) *p=value;}
    svector(int n) : _svector(0,n-1,sizeof(T)) {}
    svector(const svector<T> &v) : _svector(v) {}
    ~svector() {}
    svector<T> &operator =(const svector<T> &v)
        { me() = v; return *this;}
    T& operator[] (int i) {check(i); return ((T *)ptr0)[i];}
    const T& operator[] (int i) const {check(i); return ((T *)ptr0)[i];}
    T& operator() (int i)
        {if (i<start) resize(i,finish-1);
        else if (i>=finish) resize(start,i);
        return ((T *)ptr0)[i];
        }
    T& operator[] (tbrin i) {check(i()); return ((T *)ptr0)[i()];}
    const T& operator[] (tbrin i) const {check(i()); return ((T *)ptr0)[i()];}
    T& operator[] (tedge i) {check(i()); return ((T *)ptr0)[i()];}
    const T& operator[] (tedge i) const {check(i()); return ((T *)ptr0)[i()];}
    T& operator[] (tvertex i) {check(i()); return ((T *)ptr0)[i()];}
    const T& operator[] (tvertex i) const {check(i()); return ((T *)ptr0)[i()];}
    T & operator() (tedge i) { return (*this)(i());}
    T & operator() (tvertex i) { return (*this)(i());}
    T & operator() (tbrin i) { return (*this)(i());}
    T * begin() {return (T *)(me().begin());}
    T * end() {return (T *)(me().end());}
    T * origin() {return (T *)(me().origin());}
    const T * begin() const {return (const T *)(me().begin());}
    const T * end() const {return (const T *)(me().end());}
    const T * origin() const {return (const T *)(me().origin());}
    static svector<T> & cast(_svector &v) { return *(svector<T> *)&v;}
    static const svector<T> & cast(const _svector &v) { return *(const svector<T> *)&v;}
    const T * pinit() const { return (T *) (me().pinit());}
    void definit(const T &value) { me().definit((const void *)&value);}
    void getinit(T & value) const { const T* p = pinit(); if (p) value=*p;}
    void DeletePack(int i)
        {check(i);
        ((T *)ptr0)[--finish]= ((T *)ptr0)[i];
        }
    void SwapIndex(int a, int b) 
        { T &ra = (*this)[a]; T &rb = (*this)[b]; T tmp=ra; ra=rb; rb=tmp;}
    svector<T> & vector() {return *this;}
    };
template <class T>
class rsvector
    {
    protected:
        _svector &v;
    public:
        rsvector(_svector &s) : v(s) {}
        ~rsvector() {}
        T& operator[] (int i) {v.check(i); return origin()[i];}
        const T& operator[] (int i) const {v.check(i); return origin()[i];}
        T &operator() (int i) {return svector<T>::cast(v)(i);}
        T& operator[] (tbrin i) {v.check(i()); return origin()[i()];}
        const T& operator[] (tbrin i) const {v.check(i()); return origin()[i()];}
        T& operator[] (tedge i) {v.check(i()); return origin()[i()];}
        const T& operator[] (tedge i) const {v.check(i()); return origin()[i()];}
        T& operator[] (tvertex i) {v.check(i()); return origin()[i()];}
        const T& operator[] (tvertex i) const {v.check(i()); return origin()[i()];}
        T & operator() (tedge i) { return (*this)(i());}
        T & operator() (tvertex i) { return (*this)(i());}
        T & operator() (tbrin i) { return (*this)(i());}
        T * begin() {return (T *)v.begin();}
        T * origin() {return (T *)v.origin();}
        T * end() {return (T *)v.end();}
        const T * begin() const {return (T *)v.begin();}
        const T * origin() const {return (T *)v.origin();}
        const T * end() const {return (T *)v.end();}
        void clear() {v.clear();}
        void fill(int from, int to, char byte=0) {v.fill(from,to,byte);}
        void swap(_svector &s) {v.swap(s);}
        rsvector &operator=(const _svector &s) {v=s; return *this;}
        rsvector &operator=(const rsvector<T> &s) {v=s.v; return *this;}
        rsvector &operator=(const svector<T> &s) {v=s; return *this;}
        int starti() const {return v.starti();}
        int stopi() const {return v.stopi();}
        operator svector<T> &() { return svector<T>::cast(v);}
        void resize(int a, int b) {v.resize(a,b);}
        const T * pinit() const { return (T *) (v.pinit());}
        void definit(const T &value) { v.definit((const void *)&value);}
        void getinit(T & value) { svector<T>::cast(v).getinit(value);}
        int empty() const {return v.empty();}
        int InRange(int index){return v.InRange(index);}
        void SetName(const char *txt){v.SetName(txt);}
        const char *GetName() const {return v.GetName();}
        int n() const {return v.n();}
        void DeletePack(int i)  { svector<T>::cast(v).DeletePack(i);}
        void SwapIndex(int a, int b) { svector<T>::cast(v).SwapIndex(a,b);}
        svector<T> & vector() {return svector<T>::cast(v);}
    };

template <class T>
T MaxElement (const svector<T> &v)
{
 const T *p = v.begin();
 T maxe = *p;
 for (; p!= v.end(); ++p)
   if (*p > maxe) maxe=*p;
 return maxe;
}
template <class T>
void Fill (svector<T> &v, const T &x)
{
  for (T* p=v.begin(); p!=v.end();++p)
    *p = x;
}
#endif
