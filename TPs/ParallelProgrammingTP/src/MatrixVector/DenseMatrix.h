/*
 * DenseMatrix.h
 *
 *  Created on: Sep 24, 2018
 *      Author: gratienj
 */

#ifndef SRC_MATRIXVECTOR_DENSEMATRIX_H_
#define SRC_MATRIXVECTOR_DENSEMATRIX_H_

namespace PPTP
{

  class DenseMatrix
  {
    public:
      typedef std::vector<double>         VectorType ;
      typedef std::tuple<int,int,double>  MatrixEntryType ;
      DenseMatrix(std::size_t nrows=0)
      : m_nrows(nrows)
      {
        init(nrows) ;
      }

      virtual ~DenseMatrix() {}
      std::size_t nrows() const {
        return m_nrows ;
      }

      void setChunkSize(int chunk_size)
      {
        m_chunk_size = chunk_size ;
      }

      void init(std::size_t nrows)
      {
        m_nrows = nrows ;
        if(m_nrows>0)
        {
          // m_values.resize(m_nrows*m_nrows) ;
          m_values.assign(m_nrows*m_nrows,0.) ;
        }
      }

      void copy(DenseMatrix const& rhs)
      {
        m_nrows = rhs.nrows() ;
        m_values.resize(m_nrows*m_nrows) ;
        for(std::size_t k=0;k<m_nrows*m_nrows;++k)
          m_values[k] = rhs.m_values[k] ;
      }

      void setFromTriplets(int nrows, std::vector<MatrixEntryType> const& entries)
      {
        init(nrows) ;
        for( const auto& entry : entries )
        {
          int irow = std::get<0>(entry) ;
          int jcol = std::get<1>(entry) ;
          auto const& val = std::get<2>(entry) ;
          m_values[irow*m_nrows+jcol] = val ;
        }
      }

      double& operator()(std::size_t i,std::size_t j)
      {
        assert(m_nrows>0) ;
        assert(i<m_nrows) ;
        assert(j<m_nrows) ;
        return m_values[i*m_nrows+j] ;
      }

      double operator()(std::size_t i,std::size_t j) const
      {
        assert(m_nrows>0) ;
        assert(i<m_nrows) ;
        assert(j<m_nrows) ;
        return m_values[i*m_nrows+j] ;
      }

      void mult(VectorType const& x, VectorType& y) const
      {
        assert(x.size()>=m_nrows) ;
        assert(y.size()>=m_nrows) ;
        double const* matrix_ptr = m_values.data() ;
        for(std::size_t irow =0; irow<m_nrows;irow++)
        {
          double value = 0 ;
          for(std::size_t jcol =0; jcol<m_nrows;jcol++)
          {
            value += matrix_ptr[jcol]*x[jcol] ;
          }
          y[irow] = value ;
          matrix_ptr += m_nrows ;
        }
      }

      void multNcols(VectorType const& x, VectorType& y, int cols) const
      {
        double const* matrix_ptr = m_values.data() ;
        for(std::size_t irow =0; irow<m_nrows;irow++)
        {
          double value = 0 ;
          for(std::size_t jcol =0; jcol<cols;jcol++)
          {
            value += matrix_ptr[jcol]*x[jcol] ;
          }
          y[irow] = value ;
          matrix_ptr += cols ;
        }
      }

      void ompmult(VectorType const& x, VectorType& y) const
      {
        assert(x.size()>=m_nrows) ;
        assert(y.size()>=m_nrows) ;

        #pragma omp parallel for
        for (std::size_t irow = 0; irow < m_nrows; ++irow) {
          double const* matrix_ptr = m_values.data() + irow * m_nrows;
          double value = 0;
          for(std::size_t jcol = 0; jcol < m_nrows; ++jcol) {
            value += matrix_ptr[jcol]*x[jcol];
          }
          y[irow] = value;
        }
      }

      void omptaskmult(VectorType const& x, VectorType& y) const
      {
        assert(x.size()>=m_nrows) ;
        assert(y.size()>=m_nrows) ;
        // uncomment below functions lines to see ordered and critical access demo
        // int task_count = 0;

        std::size_t nb_task = (m_nrows+m_chunk_size-1)/m_chunk_size ;
        #pragma omp parallel
        {
          #pragma omp single
          {
            for (std::size_t tsk = 0 ; tsk < nb_task ; tsk++) {
              #pragma omp task
              {
                  std::size_t first = tsk * m_chunk_size;
                  std::size_t end = std::min((tsk + 1) * m_chunk_size, m_nrows);
                for (std::size_t irow = first; irow < end; ++irow) {
                  double const* matrix_ptr = m_values.data() + irow * m_nrows;
                  double value = 0;
                  for(std::size_t jcol = 0; jcol < m_nrows; ++jcol) {
                    value += matrix_ptr[jcol]*x[jcol];
                  }
                  y[irow] = value;
                  /*
                  #pragma omp critical
                  {
                    task_count += 1;
                    std::cout << "RUNNING TASK" << task_count << std::endl;
                  }
                  */
                }
              }
            }
            // std::cout << "SINGLE SECTION" << std::endl;
          }
        }
      }

      void omptilemult(VectorType const& x, VectorType& y) const
      {
        assert(x.size()>=m_nrows) ;
        assert(y.size()>=m_nrows) ;

        std::size_t nb_task = (m_nrows+m_chunk_size-1)/m_chunk_size ;
        {

            {
              // TODO TASK OPENMP 2D
            }
        }
      }

      void tbbmult(VectorType const& x, VectorType& y) const
      {
        assert(x.size()>=m_nrows) ;
        assert(y.size()>=m_nrows) ;

        parallel_for(size_t(0), m_nrows,
                     [&] (size_t irow) {
                          double const* matrix_ptr = m_values.data() + irow * m_nrows;
                          double value = 0;
                          for(std::size_t jcol = 0; jcol < m_nrows; ++jcol) {
                            value += matrix_ptr[jcol]*x[jcol];
                          }
                          y[irow] = value;
                     });
      }


      void tbbrangemult(VectorType const& x, VectorType& y) const
      {
        assert(x.size()>=m_nrows) ;
        assert(y.size()>=m_nrows) ;
        {
            // TODO TBB WITH RANGE
        }
      }

      void tbbrange2dmult(VectorType const& x, VectorType& y) const
      {
        assert(x.size()>=m_nrows) ;
        assert(y.size()>=m_nrows) ;
        {
                // TODO TBB RANGE 2D
        }
      }

      // nrows mutators
      std::size_t getNrows() { return m_nrows; }
      void setNrows(std::size_t nrows) { m_nrows = nrows; }

      // m_values mutators
      std::vector<double>* getValues() { return &m_values; }
      void setValues(std::vector<double>& vals) { m_values = vals; }

    private:
      // number of lines
      std::size_t         m_nrows = 0;

      // matrix values
      std::vector<double> m_values ;

      int m_chunk_size = 1 ;

  };

} /* namespace PPTP */

#endif /* SRC_MATRIXVECTOR_DENSEMATRIX_H_ */
