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



#include  <TAXI/KantShel.h>
#include  <TAXI/color.h>
#include  <TAXI/Tmessage.h>


//void DrawGraph();
 

int KantShelling::FindNext(tbrin &left, tbrin &right)
// return = 0: if there is no vertex nor face to shell.
//        = 1: if a vertex v has been shelled. left (resp. right)
//             will be the leftmost (resp. rightmost) brin
//             incident to v.
//        = k (>1): a face has been shelled. k is the length
//             of the shelled chain {v_1,...,v_p}.
//             left=(v_1,v_2), right=(v_{p-1},v_p).
// MarkedV indique si un sommet est sature -> Red
// MarkedE indique si une are a ete packer -> Red
  {int i,count;
  tbrin b,b0,LastB,StopB;
  tvertex v;

  Prop<short> ecolor(G.Set(tedge()),PROP_COLOR);
  Prop<short> vcolor(G.Set(tvertex()),PROP_COLOR);
  Prop<int> ewidth(G.Set(tedge()),PROP_WIDTH);

  do
      {if(Candidates.empty()) // shell the last face.
          {if(!MarkedV[G.vin[-G.cir[FirstBrin]]()])  
              {count=0;
              left=G.cir[FirstBrin];
              right=left;
              do {count++;right=G.cir[-right];} while (right!=-FirstBrin);
              right=-G.acir[right];
              MarkedV[G.vin[-G.cir[FirstBrin]]()]=1;
              /*
              if(debug())
                  {for(tedge e = 1;e <= G.ne();e++){ecolor[e]=Black;ewidth[e] = 1;}
                  for(tedge e = 1;e <= G.ne();e++)
                      {if(IsOuterE[e])ecolor[e]=Green; // exterior face
                      if(MarkedE[e]){ecolor[e]=Red;ewidth[e] = 2;}
                      }
                  for(tvertex w=1;w < G.nv();w++)
                      {if(MarkedV[w])vcolor[w] = Red;}
                  ecolor[FirstBrin.GetEdge()()] = Yellow;
                  ecolor[left] = Blue;
                  ecolor[right] = Orange;
                  
                  DrawGraph();// red: treated green: exterior face
                  }
              */
              return count;
              }
          // return FirstBrin at the last time.
          left = FirstBrin;
          return 0; // no more vertex or face to shell.
          }
      i = Candidates.pop();
      }while (i > 0 && 
              (MarkedF[i] || Brin2Face[-FirstBrin]==i) || (i<0 && MarkedV[-i]));
  // skip marked faces, the face incident to v_1 and v_2, and marked vertices
  // list of the faces which were not separation faces before the shelling.  
  IntList NonSepFaces; 
  // list of the faces which become separating faces by the shelling.
  IntList NewSepFaces; 
  // list of the vertices which become incident to F_out by the shelling.
  IntList NewOuterVertices;
  /*
   if(debug())
        {for(tedge e = 1;e <= G.ne();e++){ecolor[e]=Black;ewidth[e] = 1;}
        for(tedge e = 1;e <= G.ne();e++)
            {if(IsOuterE[e])ecolor[e]=Green; // exterior face
            if(MarkedE[e]){ecolor[e]=Red;ewidth[e] = 2;}
            }
        for(tvertex w=1;w < G.nv();w++)
            {if(MarkedV[w])vcolor[w] = Red;}
        ecolor[FirstBrin.GetEdge()()] = Yellow;
        DrawGraph();// red: treated green: exterior face
        }
 
  */
  if (i > 0) // shell a face ======================================= 
      {int f=i; 
         
      MarkedF[f]=1;
      // Let (v_1,...,v_p) the chain to shell of length count
      // vin[left] = v_1  vin[-right] = v_p
      right = Face2Brin[f];
      while (!IsOuterE[right.GetEdge()()]) right = G.cir[-right];
      while ( IsOuterE[right.GetEdge()()]) right = G.cir[-right];
      right = -G.acir[right];
      left = G.cir[-right];
      while(!IsOuterE[left.GetEdge()()])
          left = G.cir[-left];
      count = 1; // length of the chain
      b = left;
      do
          count++;
      while((b = G.cir[-b]) != right);
      /*
      if(debug())
          {ecolor[right.GetEdge()()] = Orange;ewidth[right.GetEdge()()] = 2; 
          ecolor[left.GetEdge()()] = Blue; ewidth[right.GetEdge()()] = 2; 
          }
      */
      // update IsOuterV, IsOuterE.
      // update new face 
      b=-G.acir[left];
      LastB=G.cir[-right];
      while (1) 
          //          {IsOuterE[abs(b())]=1;
          {IsOuterE[b.GetEdge()]=1;
          if (b==LastB) break;
          IsOuterV[G.vin[b]()] = 1;
          b=-G.acir[b];
          }
      //if(debug()){DrawGraph();Twait("Packingface: left right green");}   
      // update MarkedV[v], MarkedE[e].
      // TsuTsu     b=right;   while (b!=left) 
      b = right; LastB =-G.acir[left];
      while (b != left) 
          {MarkedV[G.vin[b]()] = 1;
          /*
          if(debug())
              {vcolor[G.vin[b]()] = Red;
              ecolor[b.GetEdge()()] = Red;ewidth[b.GetEdge()()] = 2;
              }
          */
          MarkedE[b.GetEdge()()] = 1;
          b = -G.acir[b];
          }
      MarkedE[left.GetEdge()()] = 1;
      //if(debug()){ecolor[b.GetEdge()()] = Red;ewidth[b.GetEdge()()] = 2;}
      
      // update visited[v].
      _visited(G.vin[left](),1);    _visited(G.vin[-right](),1);

      // set NewOuterVertices, NonSepFaces.
      // check faces which have a vertex on the new face
      b=-G.acir[left];
      LastB = G.cir[-right];
      while (b != LastB)
          {NewOuterVertices.push(G.vin[b]());
          StopB = G.acir[b];
          b = G.cir[b]; // orbit of belongs to another face
          do
              {f = Brin2Face[b];
              if(outv[f] < 3 && (outv[f] != 2 || oute[f] != 0)) 
                  {if(!NonSepFaces.InList(f))
                      NonSepFaces.push(f);
                  }
              b = G.cir[b];
              } while (b != StopB);
          b = -StopB;
          }

      // for all v newly incident to F_out, 
      // for all f incident to v, outv[f]++.
      // update oute[f].
      b=-G.acir[left];
      LastB=G.cir[-right];
      while (b!=LastB)
          {_oute(Brin2Face[-b],1);
          b0=b;
          do
              {f=Brin2Face[b]; 
              if (!MarkedF[f]) _outv(f,1);
              b=G.cir[b];
              } while (b != b0);
          b=-G.acir[b];  // to the next brin...
          }       
      _oute(Brin2Face[-b],1);

      // set NewSepFaces.
      NonSepFaces.ToHead();
      while ((f=~NonSepFaces) != 0)
          {if (outv[f] >= 3 || (outv[f] == 2 && oute[f] == 0))
              NewSepFaces.push(f);
          ++NonSepFaces;
          }         
      // for all f \in NewSepFaces, for all v incident to f, sepf[v]++;
      UpdateSepf1(NewSepFaces);

      // update sepf[v] for v=v_1,v_p.
      _sepf(G.vin[left](),-1);      _sepf(G.vin[-right](),-1);
      
      if(-G.acir[left] == G.cir[-right]()) // if {v_1,v_p} \in E...
          {int f=Brin2Face[G.acir[left]()];  
          // f is the face (!= F_out) incident to the edge {v_1,v_p}.
          if (outv[f] == 2 && oute[f] == 1)
              {_sepf(G.vin[left](),-1);_sepf(G.vin[-right](),-1);}
          }

      // for all v \in NewOuterVertices, update sepf[v].
      UpdateSepf2(NewOuterVertices);
      return count;
      }
  else   // shell a vertex ========================================
      {v = -i;
      MarkedV[v]=1;
      
      // set leftmost & rightmost brins incident to v.
      // G.vin[left]() = G.vin[right]() = v()
      if (v == v_n) 
          {left=-G.acir[FirstBrin];  right=G.acir[left];}
      else
          {left = G.FirstBrin(v);
          while (!MarkedV[G.vin[-left]()]) left = G.cir[left];
          while ( MarkedV[G.vin[-left]()]) left = G.cir[left];
          right  = G.acir[left];
          while (MarkedV[G.vin[-right]()]) right = G.acir[right];
          }
      /*
      if(debug())
          {vcolor[v] = Red;
          ecolor[left.GetEdge()()] = Blue;
          ecolor[right.GetEdge()()] = Orange; 
          DrawGraph();Twait("vertex left right");
          }
      */
      // MarkedE[e] -> 1 for all incident edges of v
      b=left;
      while(!MarkedE[b.GetEdge()()])
          {MarkedE[b.GetEdge()()]=1;
          b=G.cir[b];
          }

      // update MarkedF[f], IsOuterV[v], IsOuterE[e].
      // visited : vertices adjacent to v => _visited has at least a  MarkedE
      // Mark edges of the new face under the v umbrella
      _visited(G.vin[-left](),1);
      _visited(G.vin[-right](),1);
      b=-G.acir[-left];
      int f;
      while (1)
          {f=Brin2Face[b()];
          MarkedF[f]=1;
          IsOuterV[G.vin[b]()] = 1;
          IsOuterE[b.GetEdge()()] = 1;
          //tsu _visited(G.vin[b](),1);
          b = G.acir[b];
          if(-b == right())break;
          if(G.vin[-b] == v)
              {_visited(G.vin[b](),1); //hub
              b = G.acir[b];
              }
          b = -b;
          }
      
      // set NewOuterVertices, NonSepFaces.
      b = -G.acir[-left];
      LastB = G.cir[-right];
      while (b != LastB)  //id precedent
          {NewOuterVertices.push(G.vin[b]());
          StopB = b;
          do
              {StopB = G.acir[StopB];}
          while (G.vin[-StopB]==v);
          while(b != StopB)
              {b = G.cir[b];
              f = Brin2Face[b];
              if(outv[f]<3 && (outv[f]!=2 || oute[f]!=0)) 
                  if(!NonSepFaces.InList(f)) 
                      NonSepFaces.push(f);
              }
          b=-StopB;
          }
      // for all v newly incident to F_out, 
      //   for all f incident to v, outv[f]++.
      // update oute[f].
      b=-G.acir[-left];
      LastB=G.cir[-right];
      while (b!=LastB)
          { _oute(Brin2Face[-b],1);
          b0=b;
          do
              {f=Brin2Face[b]; 
              if (!MarkedF[f]) _outv(f,1);
              }while((b = G.cir[b]) != b0);
          b = G.acir[b];
          if(G.vin[-b] == v)b = G.acir[b];
          b = -b;
          }       
      _oute(Brin2Face[-b],1);
		
      // set NewSepFaces.
      NonSepFaces.ToHead();
      while ((f = ~NonSepFaces) != 0)
          {if(outv[f] >=3 || (outv[f] == 2 && oute[f] == 0)) 
              NewSepFaces.push(f);
          ++NonSepFaces;
          }         
      // for all f \in NewSepFaces, for all v incident to f, sepf[v]++;
      UpdateSepf1(NewSepFaces);
      // for all v \in NewOuterVertices, update sepf[v].
      UpdateSepf2(NewOuterVertices);
      return 1;
      }
  }




