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
#ifndef _SPLIT_H_INCLUDED_
#define _SPLIT_H_INCLUDED_
#include <LEDA/basic.h>
#include <LEDA/window.h>
#include <TAXI/graphs.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <stdio.h>
#include <TAXI/Tbase.h>
#include <TAXI/Tpoint.h>
#include <TAXI/netcut.h>



typedef void FUNC(void);
typedef FUNC *PFUNC;

class Project3d
    {private:
        double alpha,theta;
        // projection angles:
        // 0 <= theta < 2*PI, -PI/2 <= alpha <= PI/2
        double s_a,c_a,s_t,c_t;      //sinus and cosinus

    public:
        //double alpha,theta;
        double mul;
        Tpoint3 tr;                  // translation
        Tpoint3 center;              // coords de l'origine 
        
    public:
        Project3d() : alpha(.0),theta(.0),mul(1.),tr(.0,.0,.0) {}
        ~Project3d(){}

        void SetProjectParameters(double a,double b)
            {alpha = a; theta = b;
            s_a = sin(alpha);  c_a = cos(alpha);
            s_t = sin(theta);  c_t = cos(theta);
            }
        void SetTranslateParameters(double _mul,Tpoint3 _tr)
            {mul = _mul; tr = _tr;
            }
        Tpoint3 Project(Tpoint3 p3)            // project p3 on p2
            {Tpoint3 p2;
            p2.x() = (p3.x()*c_t - p3.y()*s_t)*c_a - p3.z()*s_a;
            p2.y() = p3.x()*s_t + p3.y()*c_t;
            p2.z() = (p3.x()*c_t - p3.y()*s_t)*s_a + p3.z()*c_a;
            p2 = p2*mul + tr;
            return p2;
            }
    };



class Project3dWindow : public Project3d
    {private:
        int save_but[8];
        int node_width;
        drawing_mode save_mode;
    public:
        window& WindowProject;
        SplitGraph& G;
        double node_radius;
        bool DrawBarycenters;

    public:
        Project3dWindow(window& W,SplitGraph& GG,PFUNC redraw_func,bool draw) :
            WindowProject(W), G(GG), DrawBarycenters(draw)
            {init(redraw_func);}
        ~Project3dWindow()
            {WindowProject.set_node_width(node_width);
            WindowProject.set_redraw();
            WindowProject.set_mode(save_mode);
            WindowProject.set_buttons(save_but);
            WindowProject.set_show_coordinates(false);
            }
        void LoopEvents();
        void DrawFactors();
    private:
        void init(PFUNC redraw_func);
    };

#endif
