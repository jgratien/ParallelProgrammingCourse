/*
 * DenseMatrix.h
 *
 *  Created on: Sep 24, 2018
 *      Author: gratienj
 */

#ifndef SRC_MATRIXVECTOR_DENSEMATRIX_H_
#define SRC_MATRIXVECTOR_DENSEMATRIX_H_


#include <tbb/tbb.h>        
#include <tbb/parallel_for.h> 
#include <tbb/blocked_range.h> 
#include <tbb/blocked_range2d.h> 


namespace PPTP
{

  class DenseMatrix
  {
    public:
      typedef std::vector<double>         VectorType ;
      typedef std::tuple<int,int,double>  MatrixEntryType ;
      DenseMatrix(std::size_t nrows=0)
      : m_nrows(nrows)
      , m_ncols(nrows)
      {
        init(nrows) ;
      }

      virtual ~DenseMatrix() {}

      std::size_t nrows() const {
        return m_nrows ;
      }

      std::size_t ncols() const {
        return m_ncols ;
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
          m_values.resize(m_nrows*m_nrows) ;
          m_values.assign(m_nrows*m_nrows,0.) ;
        }
      }

      void init(std::size_t nrows, std::size_t ncols)
      {
        m_nrows = nrows ;
        m_ncols = ncols ;
        if(m_nrows>0)
        {
          m_values.resize(m_nrows*m_ncols) ;
          m_values.assign(m_nrows*m_ncols,0.) ;
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

      double* data() {
        return m_values.data() ;
      }

      void mult(VectorType const& x, VectorType& y) const
      {
        assert(x.size()>=m_nrows) ;
        assert(y.size()>=m_nrows) ;
        double const* matrix_ptr = m_values.data() ;
        for(std::size_t irow =0; irow<m_nrows;++irow)
        {
          double value = 0 ;
          for(std::size_t jcol =0; jcol<m_nrows;++jcol)
          {
            value += matrix_ptr[jcol]*x[jcol] ;
          }
          y[irow] = value ;
          matrix_ptr += m_nrows ;
        }
      }

      void ompmult(VectorType const& x, VectorType& y) const
      {
        assert(x.size()>=m_nrows) ;
        assert(y.size()>=m_nrows) ;
        double const* matrix_ptr = m_values.data() ;
        {
          // OPEN MP
          #pragma omp parallel for
          for(std::size_t irow = 0; irow < m_nrows; ++irow)
          {
            double value = 0 ;
            for(std::size_t jcol = 0; jcol < m_nrows; ++jcol)
            {
              value += matrix_ptr[irow * (m_nrows) + jcol] * x[jcol] ;
            }
            y[irow] = value ;
          }
        }
      }
      // TASK OPENMP
      void omptaskmult(VectorType const& x, VectorType& y) const
      {
          assert(x.size() >= m_nrows);
          assert(y.size() >= m_nrows);

          double const* matrix_ptr = m_values.data();

          std::size_t nb_task = (m_nrows + m_chunk_size - 1) / m_chunk_size;

          #pragma omp parallel
          {
              #pragma omp single
              {
                  for (std::size_t itask = 0; itask < nb_task; ++itask)
                  {
                      std::size_t start = itask * m_chunk_size;
                      std::size_t end = std::min(start + m_chunk_size, m_nrows);
                      
                      // Create a new task for this chunk
                      #pragma omp task firstprivate(start, end) shared(matrix_ptr, x, y)
                      {
                          for (std::size_t irow = start; irow < end; ++irow)
                          {
                              double value = 0.0;
                              const double* row_ptr = matrix_ptr + irow * m_nrows;
                              for (std::size_t jcol = 0; jcol < m_nrows; ++jcol)
                              {
                                  value += row_ptr[jcol] * x[jcol];
                              }
                              y[irow] = value;
                          }
                      }
                  }
              } 
          } 
      }

      void omptilemult(VectorType const& x, VectorType& y) const
      {
          assert(x.size() >= m_nrows);
          assert(y.size() >= m_nrows);

          double const* matrix_ptr = m_values.data();
          for (std::size_t i = 0; i < m_nrows; ++i) {
              y[i] = 0.0;
          }

          std::size_t nb_row_tasks = (m_nrows + m_chunk_size - 1) / m_chunk_size;
          std::size_t nb_col_tasks = (m_nrows + m_chunk_size - 1) / m_chunk_size;

          #pragma omp parallel
          {
              #pragma omp single
              {
                  for (std::size_t it_row = 0; it_row < nb_row_tasks; ++it_row)
                  {
                      std::size_t row_start = it_row * m_chunk_size;
                      std::size_t row_end   = std::min(row_start + m_chunk_size, m_nrows);

                      for (std::size_t it_col = 0; it_col < nb_col_tasks; ++it_col)
                      {
                          std::size_t col_start = it_col * m_chunk_size;
                          std::size_t col_end   = std::min(col_start + m_chunk_size, m_nrows);

                          // Create a task for this tile
                          #pragma omp task firstprivate(row_start, row_end, col_start, col_end) \
                                          shared(matrix_ptr, x, y)
                          {
                              // Compute partial results for this tile
                              for (std::size_t irow = row_start; irow < row_end; ++irow)
                              {
                                  const double* row_ptr = matrix_ptr + irow * m_nrows;
                                  double partial_sum = 0.0;
                                  for (std::size_t jcol = col_start; jcol < col_end; ++jcol)
                                  {
                                      partial_sum += row_ptr[jcol] * x[jcol];
                                  }

                                  // Accumulate partial sum into y[irow] using atomic to avoid race
                                  #pragma omp atomic
                                  y[irow] += partial_sum;
                              }
                          }
                      }
                  }
              } 
          } 
      }


      void tbbmult(VectorType const& x, VectorType& y) const
      {
        assert(x.size()>=m_nrows) ;
        assert(y.size()>=m_nrows) ;

        {
          tbb::parallel_for(std::size_t(0),m_nrows,
          [&](std::size_t irow)
          {
            double const* matrix_ptr = &m_values[irow*m_nrows] ;
            double value = 0 ;
            for(std::size_t jcol =0; jcol<m_nrows;++jcol)
            {
              value += matrix_ptr[jcol]*x[jcol] ;
            }
            y[irow] = value ;
            matrix_ptr += m_nrows ;
          } ) ;
        }
      }



      void tbbrangemult(VectorType const& x, VectorType& y) const
      {
          assert(x.size() >= m_nrows);
          assert(y.size() >= m_nrows);

          const double* values_ptr = m_values.data();

          tbb::parallel_for(
              tbb::blocked_range<std::size_t>(0, m_nrows),
              [&](const tbb::blocked_range<std::size_t>& range) {
                  for (std::size_t irow = range.begin(); irow != range.end(); ++irow) {
                      const double* matrix_ptr = values_ptr + irow * m_nrows;
                      double value = 0.0;
                      for (std::size_t jcol = 0; jcol < m_nrows; ++jcol) {
                          value += matrix_ptr[jcol] * x[jcol];
                      }
                      y[irow] = value;
                  }
              }
          );
      }


      void tbbrange2dmult(VectorType const& x, VectorType& y) const
      {
          assert(x.size() >= m_nrows);
          assert(y.size() >= m_nrows);

          const double* values_ptr = m_values.data();

          // Initialize result vector to zero before accumulation
          for (std::size_t i = 0; i < m_nrows; ++i) {
              y[i] = 0.0;
          }

          // TBB parallel_for with 2D blocked range
          tbb::parallel_for(
              tbb::blocked_range2d<std::size_t>(0, m_nrows, m_chunk_size, 0, m_nrows, m_chunk_size),
              [&](const tbb::blocked_range2d<std::size_t>& range) {
                  // Process rows and columns within this 2D block
                  for (std::size_t irow = range.rows().begin(); irow != range.rows().end(); ++irow) {
                      const double* matrix_ptr = values_ptr + irow * m_nrows;

                      double partial_sum = 0.0;
                      for (std::size_t jcol = range.cols().begin(); jcol != range.cols().end(); ++jcol) {
                          partial_sum += matrix_ptr[jcol] * x[jcol];
                      }

                      // Accumulate the partial result safely
                      #pragma omp atomic
                      y[irow] += partial_sum;
                  }
              }
          );
      }


    private:
      std::size_t         m_nrows = 0;
      std::size_t         m_ncols  = 0 ;

      std::vector<double> m_values ;

      int m_chunk_size = 1 ;

  };

} /* namespace PPTP */

#endif /* SRC_MATRIXVECTOR_DENSEMATRIX_H_ */
