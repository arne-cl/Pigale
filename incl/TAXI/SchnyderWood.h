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

#ifndef __SCHWOOD_H__
#define __SCHWOOD_H__

#include <TAXI/graphs.h>
#include <TAXI/IntList.h>
#include <TAXI/Tdebug.h>
#include "TAXI/color.h"
// The graph must be planar and 3-connected

// The outgoing brins are colored.
// If an edge is single, only one of its brin is colored (Red/Blue/Green). The other is colored black.

class SchnyderWood {
    TopologicalGraph G;
    tbrin FirstBrin;
    tvertex v_1,v_2,v_n;
    svector<tbrin> ParentB; //(1,G.nv(),0);
    svector<tbrin> ParentG; // (1,G.nv(),0);
    svector<tbrin> ParentR; // (1,G.nv(),0);
    svector<short> brin_color; // (1,G.nv(),0);


    public:
    bool isOK;

    // b is the brin indicating the outer face.
    // Typically, b is the brin defined by the vertex v_n and the edge {v_1,v_n}.
    // This brin can be found by the function: GeometricGraph::FindExteriorface().
    SchnyderWood(Graph &G0, tbrin b);

    ~SchnyderWood() {}
    //
    //  tvertex FindVertex(tbrin &left, tbrin &right);
    //    tvertex FindVertex();

    tvertex GetRoot(short c) const;
    bool IsSimple (tedge e) const;
    bool IsBlack (tedge e) const;
    bool IsRoot (tvertex v) const;
    tbrin GetParentBr(tvertex v, short c) const;
    tedge GetParentE (tvertex v, short c) const;
    tvertex GetParentV (tvertex v, short c) const;
    void GetEdgeColors (tbrin b, short &c1, short &c2) const;
    short GetBrinColor (tbrin b) const;
    

    // remove the cw-elbow with  a single colored c
    bool remove_cw_elbow (tvertex v, short c);

    // remove the cw-elbow with  a single colored c
    bool remove_ccw_elbow (tvertex v, short c);

    bool is_cw_elbow(tvertex v, short c) const;
    bool is_ccw_elbow(tvertex v, short c) const;
    bool is_ccw_smooth(tvertex v) const;
    bool is_cw_smooth(tvertex v) const;
    // Check if v is a blue-red corner of a cw_face
    bool is_cw_face(tvertex v) const;
    bool reverse_cw_face(tvertex v);

    // Check if v is a red-blue corner of a ccw_face
    bool is_ccw_face(tvertex v) const;
    bool reverse_ccw_face(tvertex v);

    bool cw_merge (tvertex v, short c);
    bool ccw_merge (tvertex v, short c);

    // Return the color befor and after c
    void CyclicColors(short c, short &c_left, short &c_right) const;
    tvertex GetFirstChildren( tvertex v, short c) const;
    tvertex GetNextChildren (tvertex current_child, short c) const;
    tvertex GetLastChildren( tvertex v, short c) const;
    tvertex GetPreviousChildren (tvertex current_child, short c) const;
    int CountDeltaSharpCW () const;
    int CountDeltaSharpCCW () const;
    int CountDeltaSmoothCW () const;
    int CountDeltaSmoothCCW () const;
};


//void SchnyderDecomp(TopologicalGraph &G, tbrin brin,svector<short> &short);

#endif
