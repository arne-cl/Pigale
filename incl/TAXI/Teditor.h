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
#include <stdio.h>       //vsprintf provisoire
#include  <math.h>
#include  <LEDA/panel.h>
#include  <TAXI/Tbase.h>
#include  <TAXI/graphs.h>
#include  <TAXI/color.h>

// Some related functions
int  ShowEdgeInfo(int info = -1);
int  ShowVertexInfo(int info = -1);
leda_color & GetEColor();
leda_color & GetVColor();
int & GetWidth();



struct _Editor {
  int save_but[8];
  drawing_mode save_mode;
  double xyorigin;
  bool DrawAxes;
  double node_radius;
  window &W;

  GeometricGraph G;
  Prop<bool> eoriented;
  int slabel_num;
  Prop<long> slabel;
  
  void Draw_node(tvertex v)
      {if(ShowVertexInfo() == 1)
          W.draw_int_node(G.vcoord[v].x(),G.vcoord[v].y(),v(),G.vcolor[v]);
      else if(ShowVertexInfo() == 2)
          W.draw_int_node(G.vcoord[v].x(),G.vcoord[v].y()
                          ,G.vlabel[v],G.vcolor[v]);
      else if(ShowVertexInfo() == 3)
          W.draw_int_node(G.vcoord[v].x(),G.vcoord[v].y()
                          ,slabel[v],G.vcolor[v]);
      else
          W.draw_filled_node(G.vcoord[v].x(),G.vcoord[v].y(),G.vcolor[v]);
      }
  void Draw_edge(tedge e)
      {Draw_brin(e.firsttbrin());
      }
  void Draw_brin(tbrin e, bool reorient = false)
      {if(e < 0) e=-e;
      tvertex v = G.vin[e];
      point ps(G.vcoord[v].x(),G.vcoord[v].y());
      v = G.vin[-e];
      point pt(G.vcoord[v].x(),G.vcoord[v].y());
      int old_line_width = W.set_line_width(G.ewidth[e]);

      if (!eoriented[e]) W.draw_edge(ps, pt, G.ecolor[e]);
      else if (!reorient) W.draw_edge_arrow(ps, pt, G.ecolor[e]);
      else W.draw_edge_arrow(pt, ps, G.ecolor[e]);

      if(ShowEdgeInfo()  == 1)
          W.draw_text(point((ps.to_vector()+pt.to_vector())/2.0)
                      ,string("%d", e()), G.ecolor[e]);
      else if(ShowEdgeInfo()  == 2)
          W.draw_text(point((ps.to_vector()+pt.to_vector())/2.0)
                      ,string("%d", G.elabel[e]), G.ecolor[e]);

      W.set_line_width(old_line_width);
      }
  void Draw_node_with_edges(tvertex v)
      {tbrin b0 = G.pbrin[v];
      if (b0!=0)
          {tbrin b = b0;
          do {Draw_brin(b);
          b = G.cir[b];
          }while (b!=b0);
          }
      Draw_node(v);
      }
  void Draw_graph ()
      {
      W.set_node_width(8);
      if(DrawAxes)
          {W.draw_segment(W.xmin(),xyorigin,W.xmax(),xyorigin,grey1);
          W.draw_segment(xyorigin,W.ymin(),xyorigin,W.ymax(),grey1);
          }
      for (tvertex v=1; v<=G.nv(); v++) Draw_node(v);
      if (G.PE().exist(PROP_REORIENTED))
          {Prop<bool> reorient(G.PE(),PROP_REORIENTED);
          for (tbrin b=1; b<=G.ne(); b++)
              Draw_brin(b, reorient[b.GetEdge()]);
          }
      else for (tbrin b=1; b<=G.ne(); b++)Draw_brin(b);
      }
  void ScrollGraph(const Tpoint &p);
  Tpoint & pos(tvertex v)
      { return G.vcoord[v];}

  bool nearto(const Tpoint &p0, const Tpoint &p)
      {return Distance(p,p0) <  node_radius/2;}
  tedge find_edge(const Tpoint &p)
      {return G.FindEdge(p,node_radius);}
  tvertex find_node(const Tpoint &p) 
      {return G.FindVertex(p,node_radius);}

  _Editor(window &WW, Graph &GG,bool draw_axes)  : W(WW), G(GG)
  ,eoriented(G.PE(),PROP_ORIENTED,false), 
  slabel_num(G.Set().exist(PROP_SYM) ? PROP_SYMLABEL : PROP_LABEL)
  ,slabel(G.PV(),slabel_num,0)
      {
      save_mode = W.get_mode();
      W.std_buttons(save_but);
      node_radius = W.get_node_width()/W.scale();
      DrawAxes = draw_axes;
      xyorigin = .5*Min(W.xmax()- W.xmin(),W.ymax()- W.ymin());
      W.clear();
      Draw_graph();
      eoriented.SetName("eoriented");
      }
  ~_Editor()
      {W.set_redraw();
      W.set_mode(save_mode);
      W.set_buttons(save_but);
      }
};
