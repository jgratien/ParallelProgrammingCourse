/*
 * DenseMatrix.h
 *
 *  Created on: Sep 24, 2018
 *      Author: gratienj
 */

#ifndef SRC_MATRIXVECTOR_DENSEMATRIX_H_
#define SRC_MATRIXVECTOR_DENSEMATRIX_H_

#include "tbb/tbb.h"
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
           // OpenMP
	   assert(x.size()>=m_nrows) ;
           assert(y.size()>=m_nrows) ;
           
	   
	   
           #pragma omp parallel for
	   for(std::size_t irow =0; irow<m_nrows;++irow)
           {
	       double value = 0 ;
	       double const* matr = m_values.data()+irow*m_nrows;
               for( std::size_t k = 0; k < m_nrows;++k)
               {
                   value += matr[k]*x[k] ;
               }
               y[irow] = value ;
           }
	   
      }

      void omptaskmult(VectorType const& x, VectorType& y) const
      {
        assert(x.size()>=m_nrows) ;
        assert(y.size()>=m_nrows) ;

        std::size_t nb_task = (m_nrows+m_chunk_size-1)/m_chunk_size ;
        #pragma omp parallel shared(x,y)
        {

          #pragma omp single
          for(std::size_t i=0;i<nb_task;++i)
          {
            #pragma omp task
            {
              std::size_t debut = i*m_chunk_size ;
              std::size_t fin = std::min(debut+m_chunk_size,m_nrows) ;
              for(std::size_t irow =debut; irow<fin;++irow)
              {
                double const* ptr_matrix = &m_values[irow*m_nrows] ;
                double value = 0 ;
                for(std::size_t jcol =0; jcol<m_nrows;++jcol)
                {
                  value += ptr_matrix[jcol]*x[jcol] ;
                }
                y[irow] = value ;
                ptr_matrix += m_nrows ;
              }
            }
          }
        }
      }

      void omptilemult(VectorType const& x, VectorType& y) const
      {
    	assert(x.size() >= m_nrows);
    	assert(y.size() >= m_nrows);

    	std::size_t nb_task = (m_nrows + m_chunk_size - 1) / m_chunk_size;

    	#pragma omp parallel shared(x, y)
    	{
           #pragma omp single
           {
               for (std::size_t i = 0; i < nb_task; ++i) {
                   for (std::size_t j = 0; j < nb_task; ++j) {
                    #pragma omp task
                    {
                        std::size_t i_begin = i * m_chunk_size;
                        std::size_t i_end = std::min(i_begin + m_chunk_size, m_nrows);
                        std::size_t j_begin = j * m_chunk_size;
                        std::size_t j_end = std::min(j_begin + m_chunk_size, m_ncols);

                        for (std::size_t irow = i_begin; irow < i_end; ++irow) {
                            double const* matrix_ptr = &m_values[irow * m_ncols];
                            double value = 0;
                            for (std::size_t jcol = j_begin; jcol < j_end; ++jcol) {
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
        assert(x.size()>=m_nrows) ;
        assert(y.size()>=m_nrows) ;

        {
            
        }
      }


      void tbbrangemult(VectorType const& x, VectorType& y) const
      {
        assert(x.size()>=m_nrows) ;
        assert(y.size()>=m_nrows) ;
	double const* matr = m_values.data();
        {
		tbb::parallel_for(std::size_t(0),m_nrows, [&](std::size_t irow){
			    double value = 0;
			    for ( std::size_t k = 0; k < m_nrows ; ++k ){
			    	value += matr[irow*m_nrows + k]*x[k];
			    }
			    y[irow] = value;
			    } );
        }
      }

    void tbbrange2dmult(VectorType const& x, VectorType& y) const
    {
       assert(x.size() >= m_nrows);
       assert(y.size() >= m_nrows);

       tbb::spin_mutex mutex;

       tbb::parallel_for(
           tbb::blocked_range2d<std::size_t>(0, m_nrows, m_chunk_size, 0, m_nrows, m_chunk_size),
            [&](const tbb::blocked_range2d<std::size_t>& range)
           {
               for (std::size_t i = range.rows().begin(); i < range.rows().end(); ++i)
               {
                   const double* row_start = &m_values[i * m_nrows];
                   double row_sum = 0.0;

                   for (std::size_t j = range.cols().begin(); j < range.cols().end(); ++j)
                   {
                       row_sum += row_start[j] * x[j];
                   }

                   {
                       tbb::spin_mutex::scoped_lock lock(mutex);
                       y[i] += row_sum;
                   }
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
