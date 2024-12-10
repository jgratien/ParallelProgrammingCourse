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

        #pragma omp parallel for schedule(static) default(none) shared(x, y)
        for (std::size_t irow = 0; irow < m_nrows; ++irow)
        {
          double const* matrix_ptr = &m_values[irow * m_nrows];
          double value = 0.0;

          for (std::size_t jcol = 0; jcol < m_nrows; ++jcol)
          {
            value += matrix_ptr[jcol] * x[jcol];
          }

          y[irow] = value;
        }
      }

      void omptaskmult(VectorType const& x, VectorType& y) const
      {
        assert(x.size()>=m_nrows) ;
        assert(y.size()>=m_nrows) ;

        //std::size_t num_chunks = (m_nrows+m_chunk_size-1)/m_chunk_size ;
        #pragma omp parallel
        {
          #pragma omp single
          {
            for(std::size_t chunk = 0; chunk < m_nrows; chunk += m_chunk_size)
            {
              #pragma omp task
              {

                std::size_t end = std::min(chunk + m_chunk_size, m_nrows) ;

                for (std::size_t irow = chunk; irow < end; ++irow)
                {
                  double const* matrix_ptr = &m_values[irow * m_nrows];
                  double value = 0.0;

                  for (std::size_t jcol = 0; jcol < m_nrows; ++jcol)
                  {
                    value += matrix_ptr[jcol] * x[jcol];
                  }

                  y[irow] = value;
                }
              }
            }
          }
          #pragma omp taskwait
        }
      }

      void omptilemult(VectorType const& x, VectorType& y) const
      {
        assert(x.size()>=m_nrows) ;
        assert(y.size()>=m_nrows) ;

        std::size_t tile_size = m_chunk_size;  // Size of each tile (rows and columns)
        std::size_t num_tiles_row = (m_nrows + tile_size - 1) / tile_size;
        std::size_t num_tiles_col = (m_nrows + tile_size - 1) / tile_size;

        #pragma omp parallel
        {
          #pragma omp single
          {
            for (std::size_t row_tile = 0; row_tile < num_tiles_row; ++row_tile) {
                for (std::size_t col_tile = 0; col_tile < num_tiles_col; ++col_tile) {
                    #pragma omp task
                    {
                      std::size_t row_start = row_tile * tile_size;
                      std::size_t row_end = std::min(row_start + tile_size, m_nrows);

                      std::size_t col_start = col_tile * tile_size;
                      std::size_t col_end = std::min(col_start + tile_size, m_nrows);

                      for (std::size_t irow = row_start; irow < row_end; ++irow) {
                        double const* matrix_ptr = &m_values[irow * m_nrows];
                        double value = 0.0;

                        for (std::size_t jcol = col_start; jcol < col_end; ++jcol) {
                          value += matrix_ptr[jcol] * x[jcol];
                        }

                        #pragma omp atomic
                        y[irow] += value;
                      }
                    }
                }
            }
          }
        }
      }

      void tbbmult(VectorType const& x, VectorType& y) const
      {
          assert(x.size() >= m_nrows);
          assert(y.size() >= m_nrows);

          // Parallelize over the rows of the matrix
          tbb::parallel_for(std::size_t(0), m_nrows, [&](std::size_t irow) {
              double value = 0.0;
              double const* matrix_ptr = &m_values[irow * m_nrows];  // Pointer to the start of row irow

              // Perform the matrix-vector multiplication for the current row
              for (std::size_t jcol = 0; jcol < m_nrows; ++jcol) {
                  value += matrix_ptr[jcol] * x[jcol];  // Multiply and accumulate
              }

              y[irow] = value;  // Store the result in the output vector
          });
      }

      void tbbrangemult(VectorType const& x, VectorType& y) const {
        assert(x.size() >= m_nrows);
        assert(y.size() >= m_nrows);

        tbb::parallel_for(tbb::blocked_range<std::size_t>(0, m_nrows),
        [&](const tbb::blocked_range<std::size_t>& r) {
          for (std::size_t irow = r.begin(); irow < r.end(); ++irow) {
            double value = 0.0;
            double const* matrix_ptr = &m_values[irow * m_nrows];

            for (std::size_t jcol = 0; jcol < m_nrows; ++jcol) {
                value += matrix_ptr[jcol] * x[jcol];
            }

            y[irow] = value;
          }
        });  
      }

      void tbbrange2dmult(VectorType const& x, VectorType& y) const
      {
        assert(x.size() >= m_nrows);
        assert(y.size() >= m_nrows);

        std::size_t block_size = 64;  // Example block size, tune based on your matrix size
        tbb::parallel_for(tbb::blocked_range2d<std::size_t>(0, m_nrows, 0, m_nrows, block_size, block_size),
        [&](const tbb::blocked_range2d<std::size_t>& r) {
          for (std::size_t irow = r.rows().begin(); irow < r.rows().end(); ++irow) {
            double value = 0.0;
            double const* matrix_ptr = &m_values[irow * m_nrows];

            for (std::size_t jcol = r.cols().begin(); jcol < r.cols().end(); ++jcol) {
              value += matrix_ptr[jcol] * x[jcol];
            }

            y[irow] = value;
          }
        });
      }

    private:
      // number of lines
      std::size_t         m_nrows = 0;
      std::size_t         m_ncols  = 0 ;

      // matrix values
      std::vector<double> m_values ;

      int m_chunk_size = 1 ;

  };

} /* namespace PPTP */

#endif /* SRC_MATRIXVECTOR_DENSEMATRIX_H_ */
