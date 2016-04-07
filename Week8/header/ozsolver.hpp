#pragma once
#include "shared.h"

/*

 This is a discrete fast Fourier sine transform (DFFT) from Numerical
 Recipes (Numerical Recipes in C, 2nd Ed., by W.H. Press,
 S.A. Teukolsky, W.T. Vettering and B.P. Flannery).  I modified it to
 1) use double precision; 2) use arrays who's indices start from 0,
 not 1.

It computes:

                            N - 1
                            ====
                            \
                             >    y(j) SIN(%PI j k / N )
                            /
                            ====
                            j = 0


where k=0,1,...,N-1, and where N must be a power of 2.  You use it by
initializing the data in y and simply call sinft(y,N).  When it's
done, y contains the DFFT.

Note that the grid spacings in a sine transform are given by

                                          %PI
                                  dk dr = ---
                                           N

without an extra factor of 2 on the right hand side.  Also note that
the y[0] is zero.

*/

void realft(double *data,unsigned long n, int isign);
void four1(double *data,unsigned long nn,int isign);

void sinft(double *y, unsigned long n)
{
        unsigned long  j,n2=n+2;
        double sum,y1,y2;
        double theta,wi=0.0,wr=1.0,wpi,wpr,wtemp;

        theta=3.14159265358979/(double) n;
        wtemp=sin(0.5*theta);
        wpr = -2.0*wtemp*wtemp;
        wpi=sin(theta);
        y--;
        y[1]=0.0;
        for (j=2;j<=(n>>1)+1;j++) {
                wr=(wtemp=wr)*wpr-wi*wpi+wr;
                wi=wi*wpr+wtemp*wpi+wi;
                y1=wi*(y[j]+y[n2-j]);
                y2=0.5*(y[j]-y[n2-j]);
                y[j]=y1+y2;
                y[n2-j]=y1-y2;
        }
        realft(y,n,1);
        y[1]*=0.5;
        sum=y[2]=0.0;
        for (j=1;j<=n-1;j+=2) {
                sum += y[j];
                y[j]=y[j+1];
                y[j+1]=sum;
        }
}

void realft(double *data,unsigned long n, int isign)
{
        unsigned long i,i1,i2,i3,i4,np3;
        double c1=0.5,c2,h1r,h1i,h2r,h2i;
        double wr,wi,wpr,wpi,wtemp,theta;

        theta=3.141592653589793/(double) (n>>1);
        if (isign == 1) {
                c2 = -0.5;
                four1(data,n>>1,1);
        } else {
                c2=0.5;
                theta = -theta;
        }
        wtemp=sin(0.5*theta);
        wpr = -2.0*wtemp*wtemp;
        wpi=sin(theta);
        wr=1.0+wpr;
        wi=wpi;
        np3=n+3;
        for (i=2;i<=(n>>2);i++) {
                i4=1+(i3=np3-(i2=1+(i1=i+i-1)));
                h1r=c1*(data[i1]+data[i3]);
                h1i=c1*(data[i2]-data[i4]);
                h2r = -c2*(data[i2]+data[i4]);
                h2i=c2*(data[i1]-data[i3]);
                data[i1]=h1r+wr*h2r-wi*h2i;
                data[i2]=h1i+wr*h2i+wi*h2r;
                data[i3]=h1r-wr*h2r+wi*h2i;
                data[i4] = -h1i+wr*h2i+wi*h2r;
                wr=(wtemp=wr)*wpr-wi*wpi+wr;
                wi=wi*wpr+wtemp*wpi+wi;
        }
        if (isign == 1) {
                data[1] = (h1r=data[1])+data[2];
                data[2] = h1r-data[2];
        } else {
                data[1]=c1*((h1r=data[1])+data[2]);
                data[2]=c1*(h1r-data[2]);
                four1(data,n>>1,-1);
        }
}

#define SWAP(a,b) tempr=(a);(a)=(b);(b)=tempr

void four1(double *data,unsigned long nn,int isign)
{
        unsigned long n,mmax,m,j,istep,i;
        double wtemp,wr,wpr,wpi,wi,theta;
        double tempr,tempi;

        n=nn << 1;
        j=1;
        for (i=1;i<n;i+=2) {
                if (j > i) {
                        SWAP(data[j],data[i]);
                        SWAP(data[j+1],data[i+1]);
                }
                m=n >> 1;
                while (m >= 2 && j > m) {
                        j -= m;
                        m >>= 1;
                }
                j += m;
        }
        mmax=2;
        while (n > mmax) {
                istep=mmax << 1;
                theta=isign*(6.28318530717959/mmax);
                wtemp=sin(0.5*theta);
                wpr = -2.0*wtemp*wtemp;
                wpi=sin(theta);
                wr=1.0;
                wi=0.0;
                for (m=1;m<mmax;m+=2) {
                        for (i=m;i<=n;i+=istep) {
                                j=i+mmax;
                                tempr=wr*data[j]-wi*data[j+1];
                                tempi=wr*data[j+1]+wi*data[j];
                                data[j]=data[i]-tempr;
                                data[j+1]=data[i+1]-tempi;
                                data[i] += tempr;
                                data[i+1] += tempi;
                        }
                        wr=(wtemp=wr)*wpr-wi*wpi+wr;
                        wi=wi*wpr+wtemp*wpi+wi;
                }
                mmax=istep;
        }
}
#undef SWAP

double c_theo(double eta, double r) {
  return r<1.0 ? (-std::pow(1+2*eta,2) + 6*eta*std::pow(1+0.5*eta,2)*r-0.5*eta*std::pow(1+2*eta,2)*std::pow(r,3))/std::pow(1.0-eta,4) : 0.0;
}

class OZSolverHardSpheres
{
private:
  double
    * Gamma_r = nullptr,
    * C_r = nullptr,
    * Gamma_k = nullptr,
    * C_k = nullptr,
    Density = 0.0,
    DeltaR = 0.0,
    Tolerance = 0.0;

  uint32_t GridPoints = 0, MaxIterations = 1000000;

  void __ComputeNewGamma();
  void __ComputeClosure();
  double __ComputeError(double * new_gamma_r);

public:
  OZSolverHardSpheres(double x_max, double eta, uint32_t n_points, double tol)
   : Density(6.0/pi*eta),
     DeltaR(x_max/n_points),
     Tolerance(tol),
     GridPoints(n_points)
  {
    Gamma_r = new double[GridPoints];
    C_r = new double[GridPoints];
    Gamma_k = new double[GridPoints];
    C_k = new double[GridPoints];
  }

  ~OZSolverHardSpheres() {
    delete[] Gamma_r;
    delete[] Gamma_k;
    delete[] C_r;
    delete[] C_k;
  }

  void ComputeOutputAndSave(const char * filename);
};

void OZSolverHardSpheres::__ComputeClosure()
{
  for(uint32_t i = 0; i < GridPoints; i++) {
    C_r[i] = i * DeltaR < 1.0 ? -Gamma_r[i] - 1.0 : 0.0;
  }
}

double OZSolverHardSpheres::__ComputeError(double * new_gamma_r)
{
  double err = 0.0;
  for(uint32_t i = 1; i < GridPoints; i++) {
    err += std::fabs(new_gamma_r[i]-Gamma_r[i]);
  }
  return err;
}

void OZSolverHardSpheres::__ComputeNewGamma()
{
  double alpha = 0.5, delta_q = pi/GridPoints/DeltaR;
  double * new_gamma_r = new double[GridPoints];
  for (uint32_t i = 0; i < GridPoints; i++) { Gamma_r[i] = 0.0; }

  printf("rho: %lf, gp: %d, dr: %lf\n", Density, GridPoints, DeltaR);

  for (uint32_t i = 0; i < MaxIterations; i++) {
    __ComputeClosure();

    for (uint32_t j = 1; j < GridPoints; j++) { C_k[j] = j*4*pi*DeltaR*DeltaR/delta_q*C_r[j]; }
    sinft(C_k, GridPoints);

    for (uint32_t j = 1; j < GridPoints; j++) {
      new_gamma_r[j] = delta_q*delta_q/(2.0*pi*pi*DeltaR)/j*Density*C_k[j]*C_k[j]/(1.0-Density*C_k[j]/j);
    }

    sinft(new_gamma_r, GridPoints);
    for (uint32_t j = 1; j < GridPoints; j++) { new_gamma_r[j] = new_gamma_r[j]/j; }

    if (__ComputeError(new_gamma_r) < Tolerance) {
      for (uint32_t j = 0; j < GridPoints; j++) { Gamma_r[j] = new_gamma_r[j]; }
      printf("Iterations: %d\n", i);
      break;
    } else {
      for (uint32_t j = 0; j < GridPoints; j++) { Gamma_r[j] = alpha*new_gamma_r[j] + (1-alpha)*Gamma_r[j]; }
    }
  }
}

void OZSolverHardSpheres::ComputeOutputAndSave(const char * filename)
{
  __ComputeNewGamma();
  __ComputeClosure();

  double delta_q = pi/GridPoints/DeltaR;

  for (uint32_t j = 1; j < GridPoints; j++) { C_k[j] = j*4*pi*DeltaR*DeltaR/delta_q*C_r[j]; }
  sinft(C_k, GridPoints);

  double * g_r = new double[GridPoints];
  double * s_k = new double[GridPoints];
  double * c_tr = new double[GridPoints];
  double * c_tk = new double[GridPoints];
  double * g_t = new double[GridPoints];
  double * s_t = new double[GridPoints];

  for (uint32_t i = 1; i < GridPoints; i++) {
    s_k[i] = 1.0 / (1.0 - Density * C_k[i]/i);
    g_r[i] = delta_q*delta_q/(2.0*pi*pi*DeltaR)*i*(s_k[i]-1)/Density;
    c_tr[i] = c_theo(Density * pi / 6.0, i * DeltaR);
    c_tk[i] = i*4*pi*DeltaR*DeltaR/delta_q*c_tr[i];
  }

  sinft(c_tk, GridPoints);

  for (uint32_t i = 1; i < GridPoints; i++) {
    s_t[i] = 1.0 / (1.0 - Density * c_tk[i]/i);
    g_t[i] = delta_q*delta_q/(2.0*pi*pi*DeltaR)*i*(s_t[i]-1)/Density;
  }

  sinft(g_r, GridPoints);
  sinft(g_t, GridPoints);

  for (uint32_t i = 1; i < GridPoints; i++) { g_t[i] = g_t[i]/i+1; g_r[i] = g_r[i]/i+1; }

  Json::Value Root;

  Root["GridPoints"] = GridPoints;
  Root["Density"] = Density;
  Root["DeltaR"] = DeltaR;

  for (uint32_t i = 0; i < GridPoints; i++) {
    Root["c_r"][i] = C_r[i];
    Root["g_r"][i] = g_r[i];
    Root["s_k"][i] = s_k[i];
    Root["c_t"][i] = c_tr[i];
    Root["g_t"][i] = g_t[i];
    Root["s_t"][i] = s_t[i];
  }

  delete[] g_r;
  delete[] s_k;
  delete[] c_tr;
  delete[] c_tk;
  delete[] g_t;
  delete[] s_t;

  Json::FastWriter writer;
  std::ofstream savefile(filename);
  savefile << writer.write(Root);
  savefile.close();
}
