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
#include <stdarg.h>
#include <math.h>
#include <stdio.h>    //provisoire pour debug

#include <TAXI/Tbase.h>
#include <TAXI/Tdebug.h>
#include <TAXI/Tmessage.h>
#include <TAXI/Tsvector.h>
#include <TAXI/netcut.h>


static void CalcNorm(double p,double q,double &norm,double &c,double &s);
static int ComputeBilinearForm(double **dis,double ** Distances,int ni);
static int symqr(double **dis,int ni,int nf,double& trace,svector<double>& EigenValues);


const double  epsilon = 1.E-12;      //bon ave 1.E-100


int diag(double **dis,int nb_vertex,double **Distances,svector<double>& EigenValues,bool project)
  {int i,j;

  int ni = nb_vertex;
  int nf = ni - 1;
  double Trace;

  // copy the matrix Distances -> dis
  for(i = 1; i <= ni; i++)
      for(j = 1; j <= i; j++)
          dis[i][j] = dis[j][i] = (double)Distances[i][j];

  // Compute dis, the projection matrix to be diagonalized (sum of elements in a line == 0)
  if(project && ComputeBilinearForm(dis,Distances,ni) != 0) return 1;

  // Compute the trace
  double TraceInit = .0;
  for(i = 1;i <= ni;i++)TraceInit += dis[i][i];	  
  if(TraceInit < epsilon){Tprintf("Trace is null !"); /* return 2;*/}

  if(debug())
      {LogPrintf("\nDiagonalisation(trace_init=%f)",TraceInit);
      int imax = Min(ni,20);
      LogPrintf("\nScalar products (*10*n)");
      LogPrintf("\n     *01*");
      for(i = 1; i <= ni; i++)
          {LogPrintf("\n%2.2d  ",i);
          for(j = 1; j <= Min(i,imax); j++)
              LogPrintf("%+0.2f ",ni*10*dis[i][j]);
	  LogPrintf(" *%2.2d*",j);
	  }
      }

  if(symqr(dis,ni,nf,Trace,EigenValues))
      {Tprintf("DIAG.C:symqr error");
      return 1;
      }

  // Normalization: dis[i][j] coordonne j du sommet i (i=1,ni j=1,nf)
  double a;
  for(j = 1; j <= nf; j++)
      {if(Abs(EigenValues[j]) < 1.E-10)
	  for (i = 1; i <= ni; i++)  dis[i][j] = .0;
      else
	  {a = sqrt(Abs(EigenValues[j]*ni));
	  for (i = 1; i <= ni; i++)  dis[i][j] *= a;
	  }
      }
  if(debug()) // Impression des coordonnees
      {int imax = Min(ni,20);
      LogPrintf("\nDistance");
      LogPrintf("\n     *01*");
      for(i = 2; i <= ni; i++)
          {LogPrintf("\n%2.2d  ",i);
          for(j = 1; j < Min(i,imax); j++)
              LogPrintf("%+0.2f ",Distances[i][j]);
	  LogPrintf(" *%2.2d*",j);
	  }

      LogPrintf("\nCoords");
      for(i = 1; i <= ni; i++)
          {LogPrintf("\n%2.2d  ",i);
          for(j = 1; j < imax; j++)
              LogPrintf("%+0.3f ",dis[i][j]);
          }
      }
  // Calcul multiplicites
  int *Multiplicite = new int[ni+1];
  int NumEqualEigenvalues = 0;
  int NumNullEigenvalues = 0;
  int i1,i2,mul;
  i1 = 1;
  while(i1 <= nf)
      {mul = 1;
      i2 = i1 + 1;
      while(i2 <= nf && fabs(EigenValues[i2] - EigenValues[i1]) < 1.E-10)
          {++mul;++i2;}
      for(i = i1;i < i2;i++)
          Multiplicite[i] = mul;
      if(mul != 1)++NumEqualEigenvalues;
      i1 = (mul == 1) ? i1 + 1 : i2;
      }
  for(i = 1;i <= nf;i++)
      if(EigenValues[i] < 1.E-10 && EigenValues[i] > -1.E-10)++NumNullEigenvalues;

  double Sum = .0;
  Tprintf("Eigenvalues:%d",nf);    
  for(i1 = 1;i1 <= Min(nf,4);i1++)
      {Sum += (EigenValues[i1]/TraceInit * 100.);
      Tprintf("%lf M:%d P:%d%%",EigenValues[i1],Multiplicite[i1],(int)(Sum+.5));
      }
  Tprintf("----");
  
  int MaxMul = Multiplicite[1];
  for(i1 = 2;i1 <= nf;i1++)
      MaxMul = Max(MaxMul,Multiplicite[i1]);
  int Mult1 = 0;
  int val1,val2,val3;
  val1 = val2 = val3 = 0;
  int negative = 0;
  for(i1 = 1;i1 <= nf;i1++)
      {if(Multiplicite[i1] == 1  && EigenValues[i1] > 1.E-10)
	  {++Mult1;
	  if(Mult1 == 1)val1 = i1;
	  else if(Mult1 == 2)val2 = i1;
	  else if(Mult1 == 3)val3 = i1;
	  }
      if(EigenValues[i1] < -1.E-10)++negative;
      }
  Tprintf("Equal:%d MaxMul:%d Null:%d Neg:%d",NumEqualEigenvalues,MaxMul,NumNullEigenvalues,negative);
  Tprintf("%d simple eigenvalues (%d,%d,%d,...)",Mult1,val1,val2,val3);

  if(debug())
      {LogPrintf("\nEigenvalues equal:%d null:%d"
               ,NumEqualEigenvalues,NumNullEigenvalues);
      LogPrintf("\nTableau des valeurs propres:Trace:%f",Trace);
      double Sum = .0;
      for(i = 1; i < ni; i++)     //ni
          {Sum += EigenValues[i];
          LogPrintf("\n%d %0.4f (%%:%3.1f Mul:%d)"
                  ,i,EigenValues[i],100.*Sum/Trace,Multiplicite[i]);
          }
      LogPrintf("\n");
      }

  delete [] Multiplicite;
  return 0;
  }
int ComputeBilinearForm(double **dis,double **Distances,int ni)
// Form the distance matrix Distances, computes the bilinear form to diagonalise
  {int i,j;
  double dd;

  double *dj = new double[ni + 1];
  if(dj == NULL)return 1;

  // dis: matrice a diagonaliser (ni X ni)
  for(i = 1; i <= ni; i++)dis[i][i] = (double)Distances[i][i];

  // Calcul dj[i] = poids de l'element i / poids total (=ni)
  for(i = 1; i <= ni; i++)
      {dj[i] = .0;
      for(j = 1; j <= ni; j++) dj[i] += dis[i][j];
      dj[i] /= ni;
      }

  // Calcul de dd: somme des tous les poids
  dd = 0.;
  for(i = 1;i <= ni;i++)
      dd += dj[i];
  dd /= ni;

  for(i = 1; i <= ni; i++)
      for(j = 1; j <= ni; j++)
          dis[i][j] = -0.5 * (dis[i][j] - dj[i] - dj[j] + dd)/ni;

  delete [] dj;
  return 0;
  }

int symqr(double **dis,int ni,int nf,double& trace,svector<double>& EigenValues)
// Compute the eigenvalues od the matrix dis
  {// Définition des constantes
  double const k0=5.0;
  double const titter=50.0;
  double const eps = 1.E-12;

  // Définition des variables
  double k1,k2,k,norm,max0;
  int l,l1,i,j,ll,ll1,nu,num1,nnl,nl,jj;
  double sum,sum1,p,q,c,s,temp,vmax,r;
  int shft;
  nl = jj = 0;
  int   *ik = new int[ni + 1];
  double *d = new double[ni + 1];
  double *e = new double[ni + 1];
  if((ik==NULL) || (d==NULL) || (e==NULL))
      {if(ik)delete [] ik;
      if(d)delete [] d;
      return 1;
      }

  // initialisation
  for(l = 1; l <= ni; l++)
      {d[l] = dis[l][l];
      dis[l][l] = 1.;
      e[l] = 0.;
      if(l > ni - 2) continue;
      max0 = 0.;
      l1 = l + 1;
      for(i = l1; i <= ni; i++)
          max0 = Max(max0,Abs(dis[i][l]));
      if(max0 < epsilon) continue;
      sum = 0.;
      for(i = l1; i <= ni; i++)
          {dis[i][l] = dis[i][l]/max0;
          sum = sum + (dis[i][l] * dis[i][l]);
          }
      sum = sqrt(sum);
      if(dis[l1][l] < 0.) sum = -sum;
      e[l] = -sum * max0;
      dis[l1][l] = dis[l1][l] + sum;
      dis[l][l] = sum * dis[l1][l];
      sum1 = 0.;
      for(i = l1; i <= ni; i++)
          {sum = 0.;
          for(j = l1; j <= i; j++)
              sum = sum + (dis[i][j] * dis[j][l]);
          if(i != ni)
              {for(j = i+1; j <= ni; j++)
                  sum = sum + (dis[j][l] * dis[j][i]);
              }
          e[i] = sum / dis[l][l];
          sum1 = sum1 + (dis[i][l] * e[i]);
          }
      sum = 0.5 * sum1 /dis[l][l];
      for(i = l1; i <= ni; i++)
          {e[i] = e[i] - sum * dis[i][l];
          for(j = l1; j <= i; j++)
              dis[i][j] = dis[i][j] - dis[i][l] * e[j] - dis[j][l] * e[i];
          }
      }
  e[ni-1] = dis[ni][ni-1];
  dis[ni-1][ni] = 0.0;
  dis[ni][ni-1] = 0.0;
  if(ni != 2) // toujours le cas !!!
      {for(l = 1; l<= ni-2; l++)
          {ll = ni - 2 - l + 1;
          ll1 = ll + 1;
          for(i = ll1; i <= ni; i++)
              {sum = 0.0;
              for(j = ll1; j <= ni; j++)
                  sum = sum + (dis[j][ll] * dis[j][i]);
              dis[ll][i] = sum / dis[ll][ll];
              }
          for(i = ll1; i <= ni; i++)
              {for(j = ll1; j <= ni; j++)
                  dis[i][j] = dis[i][j] - dis[i][ll] * dis[ll][j];
              }
          for(i =ll1; i <= ni; i++)
              {dis[i][ll] = 0.0;
              dis[ll][i] = 0.0;
              }
          dis[ll][ll] = 1.0;
          }
      }
  nu = ni;
  num1 = ni - 1;
  shft = 1;
  k1 = k0;
  // qu'as t'on fait !!
  again :
      for(nnl = 1; nnl <= num1; nnl++)
          {nl = num1 - nnl + 1;
          sum = eps * Min(Abs(d[nl]), Abs(d[nl + 1]));
          if(Abs(e[nl]) <= sum)
              {e[nl] = 0.0;
              nl = nl + 1;
              if(nl != nu) break;
              if(num1 == 1)    // All eigen values have been computed
                  {for(l = 1 ; l <= ni; l ++)                           //h400
                      {// Sorting the eigenvalues
                      vmax = -1.;
                      for(j = 1; j <= ni; j++)
                          {if(d[j] <= vmax) continue;
                          vmax = d[j]; jj =j;
                          }
                      ik[l] = jj; EigenValues[l] = vmax; d[jj] = -1.;
                      // e[ik[i]] = # iterations
                      }
                  trace = .0;
                  for(i = 1; i <= ni; i++)                              //h422
                      {trace = trace + EigenValues[i];
                      for(l = 1; l <= ni; l++)
                          d[l] = dis[i][l];
                      for(j = 1; j <= nf ; j++)
                          {jj = ik[j]; dis[i][j] = d[jj];}
                      }
                  delete [] e; delete [] d; delete [] ik;
                  return 0;
                  }
              nu = num1;
              num1 = nu - 1;
              goto again;
              }
          }
  e[nu] = e[nu] + 1.;
  if(e[nu] > titter)
      {delete [] e; delete [] d; delete [] ik;
      return 1;
      }

  sum = (d[num1] - d[nu]) / 2.;
  sum1 = sqrt(sum * sum + e[num1] * e[num1]);
  if(sum < 0.)sum1 = - sum1;
  k2 = d[nu] - e[num1] * e[num1] / (sum +sum1);
  if(shft == 1)
      {
      if(Abs(k2- k1) < (0.5 * Abs(k2)))
          {shft = 0;k = k2;}
      else
          {k1 = k2; k = k0;}
      }
  else
      k = k2;

  p = d[nl] - k;
  q =e[nl];
  CalcNorm(p,q,norm,c,s);
  for(i = nl;i <= num1;i++)
      {
      for(j = 1; j <= ni; j++)
          {// Attention aux UNDERFLOW
          if(Abs(dis[j][i])   < epsilon)dis[j][i]   = .0;
          if(Abs(dis[j][i+1]) < epsilon)dis[j][i+1] = .0;
          temp = c * dis[j][i] + s * dis[j][i+1];
          dis[j][i+1] = -s * dis[j][i] + c * dis[j][i+1];
          dis[j][i] = temp;
          }
      d[i] = c * d[i] * c + (2. * c * e[i] + s * d[i+1]) * s;
      d[i+1] = -s * e[i] + c * d[i+1];
      e[i] = -s * k;
      if(i != num1)
          {if(Abs(s) > Abs(c))
              {p = c * e[i] + s * d[i+1];
              q = s * e[i+1];
              e[i+1] = c * e[i+1];
              d[i+1] = c * p / s + k;
              r = 1.0;
              }
          else
              {r = s / c;
              d[i+1] = -s * e[i] + c * d[i+1];
              p = d[i+1] - k;
              e[i+1] = c * e[i+1];
              q = e[i+1];
              }
          CalcNorm(p,q,norm,c,s);
          e[i] = r * norm;
          }
      }

  temp = c * e[num1] + s * d[nu];
  d[nu] = -s * e[num1] + c * d[nu];
  e[num1] = temp;
  goto again;
  }

void CalcNorm(double p, double q,double &norm, double &c, double &s)
  {double pp,qq;

  pp = Abs(p); qq = Abs(q);

  if(qq <= pp)
      norm = pp * sqrt(1.0 + (qq / pp) * (qq / pp));
  else
      {if(qq < epsilon)
          norm = 0.0;
      else
          norm = qq * sqrt(1.0 + (pp / qq) * (pp /qq));
      }

  if(norm > epsilon)
      {c = p / norm; s = q / norm;}
  else
      {c = 1.0; s = 0.0; }
  if(Abs(s) < epsilon)s = .0;
  if(Abs(p) < epsilon)p = .0;
  }

  
