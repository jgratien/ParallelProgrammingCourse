/*
 * LUSolver.h
 *
 *  Created on: Sep 24, 2018
 *      Author: gratienj
 */

#ifndef SRC_SOLVERS_LUSOLVER_H_
#define SRC_SOLVERS_LUSOLVER_H_

#include <assert.h>

namespace PPTP
{

  template<typename MatrixT>
  class LUSolver ;

  template<>
  class LUSolver<DenseMatrix>
  {
    public:
      typedef DenseMatrix            MatrixType ;
      typedef MatrixType::VectorType VectorType ;

      LUSolver(){}
      virtual ~LUSolver(){}

      void init(DenseMatrix const& matrix)
      {
        /*
         *
         *  DO k = 1, n-1
              DO i = k+1, n                    ! Column
                A(i, k) = A(i, k) / A(k, k)    ! Normalization
              END DO                           !
              DO i = k+1, n                           !
                DO j = k+1, n                         ! Submatrix
                  A(i, j) = A(i, j) - A(i, k)*A(k, j) ! Modification
                END DO                                !
              END DO                                  !
            END DO
         *
         */
        m_nrows = matrix.nrows() ;
        m_LU.copy(matrix) ;
        for(std::size_t k=0;k<m_nrows;++k)
        {
          for(std::size_t i=k+1;i<m_nrows;++i)
            m_LU(i,k) = m_LU(i,k) / m_LU(k,k) ;
          for(std::size_t i=k+1;i<m_nrows;++i)
          {
            for(std::size_t j=k+1;j<m_nrows;++j)
            {
              m_LU(i,j) = m_LU(i,j) - m_LU(i,k)*m_LU(k,j) ;
            }
          }
        }
        m_ready = true ;
      }

      void solve(VectorType const& y, VectorType& x) const
      {
        assert(m_ready) ;
        // solve L.U x = y
        VectorType ux(m_nrows) ;
        _solveL(y,ux) ;
        _solveU(ux,x) ;
      }
    private :
      void _solveL(VectorType const& y, VectorType& x) const
      {
        /*
         *
         *  DO i = 1, n
              X[i] = Y[i]
              DO j = 1, i-1
                X(i) -= L(i, j) * X(j)
              END DO
            ENDDO
         *
         */
        for(std::size_t i=0;i<m_nrows;++i)
        {
          x[i] = y[i] ;
          for(std::size_t j=0;j<i;++j)
          {
            x[i] -= m_LU(i,j)*x[j] ;
          }
        }
      }

      void _solveU(VectorType const& y, VectorType& x) const
      {
        /*
         *
         *  DO i = n, 1
              X(i) = Y(i)
              DO j = i+1, n
                X(i) -= U(i, j) * X(j)
              END DO
              X(i) /= U(i,i)
            ENDDO
         *
         */
        for(int i=m_nrows-1;i>-1;--i)
        {
          x[i] = y[i] ;
          for(std::size_t j=i+1;j<m_nrows;++j)
            x[i] -= m_LU(i,j)*x[j] ;
          assert(m_LU(i,i)!= 0.) ;
          x[i] /= m_LU(i,i) ;
        }
      }
      DenseMatrix m_LU ;
      std::size_t m_nrows = 0 ;
      bool        m_ready = false ;
  };

} /* namespace PPTP */

#endif /* SRC_SOLVERS_LUSOLVER_H_ */
