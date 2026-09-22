// Positive multilinear interpolation and its exact sampled-domain integral.
#ifndef BOONE_TABLE_INTERPOLATION_HH
#define BOONE_TABLE_INTERPOLATION_HH
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <vector>
namespace boone_table {
struct Interval { int lo; int hi; double fraction; };
inline Interval interval(const double* axis,int n,double x) {
  if(n<1 || !std::isfinite(x)) throw std::runtime_error("Invalid table coordinate");
  if(n==1 || x<=axis[0]) return {0,0,0.};
  if(x>=axis[n-1]) return {n-1,n-1,0.};
  int hi=int(std::upper_bound(axis,axis+n,x)-axis),lo=hi-1;
  return {lo,hi,(x-axis[lo])/(axis[hi]-axis[lo])};
}
inline double linear(const double* values,const double* axis,int n,double x) {
  const auto a=interval(axis,n,x);
  return (1-a.fraction)*values[a.lo]+a.fraction*values[a.hi];
}
template<int NY,int NZ>
double interpolate(const double (*table)[NY][NZ],const double* ax,int nx,
                   const double* ay,const double* az,double x,double y,double z) {
  const auto a=interval(ax,nx,x),b=interval(ay,NY,y),c=interval(az,NZ,z);
  double value=0.;
  for(int i=0;i<2;++i) for(int j=0;j<2;++j) for(int k=0;k<2;++k) {
    const double node=table[i?a.hi:a.lo][j?b.hi:b.lo][k?c.hi:c.lo];
    if(!std::isfinite(node) || node<0.) throw std::runtime_error("Invalid production table node");
    value+=node*(i?a.fraction:1-a.fraction)*(j?b.fraction:1-b.fraction)*(k?c.fraction:1-c.fraction);
  }
  return value;
}
inline std::vector<double> integration_weights(const double* axis,int n,double lower,double upper) {
  std::vector<double> weights(n,0.),edges{lower,upper};
  for(int i=0;i<n;++i) if(axis[i]>lower && axis[i]<upper) edges.push_back(axis[i]);
  std::sort(edges.begin(),edges.end());
  for(unsigned i=1;i<edges.size();++i) {
    const double dx=(edges[i]-edges[i-1])/2.;
    for(double x:{edges[i-1],edges[i]}) {
      const auto a=interval(axis,n,x);
      weights[a.lo]+=dx*(1-a.fraction);weights[a.hi]+=dx*a.fraction;
    }
  }
  return weights;
}
template<int NY,int NZ>
double integral(const double (*table)[NY][NZ],int ix,const double* ay,const double* az,
                double ylo,double yhi,double zlo,double zhi) {
  const auto wy=integration_weights(ay,NY,ylo,yhi),wz=integration_weights(az,NZ,zlo,zhi);
  double sum=0.;
  for(int j=0;j<NY;++j) for(int k=0;k<NZ;++k) sum+=table[ix][j][k]*wy[j]*wz[k];
  return sum;
}
}
#endif
