#include "BooNETableInterpolation.hh"
#include <cassert>
#include <cmath>
#include <iostream>
int main() {
  double x[2]={1.,2.},y[2]={0.,1.},z[2]={0.,1.};
  double table[2][2][2]={{{1.,.1},{.1,.1}},{{2.,.2},{.2,.2}}};
  const double value=boone_table::interpolate<2,2>(table,x,2,y,z,1.,.9,.9);
  assert(std::abs(value-.109)<1e-14); // Old additive Taylor interpolation was negative.
  assert(std::abs(boone_table::integral<2,2>(table,0,y,z,0.,1.,0.,1.)-.325)<1e-14);
  double numeric=0.;const int n=200;
  for(int i=0;i<n;++i) for(int j=0;j<n;++j)
    numeric+=boone_table::interpolate<2,2>(table,x,2,y,z,1.,(i+.5)/n,(j+.5)/n)/(n*n);
  assert(std::abs(numeric-.325)<1e-11);
  assert((boone_table::interpolate<2,2>(table,x,2,y,z,9.,9.,9.)==.2));
  std::cout << "positive interpolation, exact normalization and boundaries: passed\n";
}
