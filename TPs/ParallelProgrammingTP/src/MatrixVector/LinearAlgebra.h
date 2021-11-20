/*
 * LinearAlgebra.h
 *
 *  Created on: Sep 20, 2018
 *      Author: gratienj
 */

#ifndef SRC_MATRIXVECTOR_LINEARALGEBRA_H_
#define SRC_MATRIXVECTOR_LINEARALGEBRA_H_

namespace PPTP
{
  double norm2(std::vector<double> const& vector) {
    double value = 0 ;
    for( auto const & x : vector)
      value += x*x ;
    return std::sqrt(value) ;
  }

  double norm2(Eigen::VectorXd const& x) {
    return std::sqrt(x.dot(x)) ;
  }

  double dot(std::vector<double> const& x,std::vector<double> const& y)
  {
    double value = 0 ;
    for(std::size_t i=0;i<x.size();++i)
    {
      value += x[i]*y[i] ;
    }
    return value ;
  }

  void axpy(double a,std::vector<double> const& x,std::vector<double>& y)
  {
    for(std::size_t i=0;i<x.size();++i)
    {
      y[i] += a*x[i] ;
    }
  }

} /* namespace PPTP */

#endif /* SRC_MATRIXVECTOR_LINEARALGEBRA_H_ */
