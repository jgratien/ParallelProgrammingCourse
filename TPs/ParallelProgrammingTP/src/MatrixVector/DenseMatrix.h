






#ifndef SRC_MATRIXVECTOR_DENSEMATRIX_H_
#define SRC_MATRIXVECTOR_DENSEMATRIX_H_

#include "tbb/tbb.h"
#include <math.h>

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
		m_ncols = nrows ;
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
          m_values[irow*m_ncols+jcol] = val ;
        }
      }
	  
      double& operator()(std::size_t i,std::size_t j)
      {
        assert(m_nrows>0) ;
        assert(i<m_nrows) ;
        assert(j<m_nrows) ;
        return m_values[i*m_ncols+j] ;
      }
      double operator()(std::size_t i,std::size_t j) const
      {
        assert(m_nrows>0) ;
        assert(i<m_nrows) ;
        assert(j<m_nrows) ;
        return m_values[i*m_ncols+j] ;
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
          for(std::size_t jcol =0; jcol<m_ncols;++jcol)
          {
            value += matrix_ptr[jcol]*x[jcol] ;
          }
          y[irow] = value ;
          matrix_ptr += m_ncols ;
        }
      }
	  
      void ompmult(VectorType const& x, VectorType& y, int nb_threads) const
      {
		if(nb_threads>0)
			omp_set_num_threads(nb_threads) ;
		
        assert(x.size()>=m_nrows) ;
        assert(y.size()>=m_nrows) ;
        {
           #pragma omp parallel shared (x,y)
		   {
			 #pragma omp for
             for(std::size_t irow =0; irow<m_nrows;++irow)
             {
               double value = 0 ;
               for(std::size_t jcol =0; jcol<m_ncols;++jcol)
               {
                 value += m_values[irow*m_ncols+jcol]*x[jcol] ;
               }
               y[irow] = value ;
             }
           }
        }
      }
	  
      void omptaskmult(VectorType const& x, VectorType& y, int const& nb_threads) const
      {
		if(nb_threads>0)
			omp_set_num_threads(nb_threads) ;
		
        assert(x.size()>=m_nrows) ;
        assert(y.size()>=m_nrows) ;
        std::size_t nb_task = (m_nrows+m_chunk_size-1)/m_chunk_size ;
		size_t local_nrows = m_nrows/nb_task ;
        {
			#pragma omp parallel
			{
			  int id = omp_get_thread_num();
			  #pragma omp single
			  {
				std::cout<<"Thread ( th_id="<<id<<", nb_th="<<nb_threads<<") create "<<nb_task<<" tasks"<<std::endl ;
				for(int itask=0;itask<nb_task;++itask)
				{
				  #pragma omp task
				  {
					std::size_t end = 0 ;
					if (itask==nb_task-1)
						end = m_nrows ;
					else
						end = (itask+1)*local_nrows ;
					
					for(std::size_t irow = itask*local_nrows; irow<end; ++irow)
					{
						double value = 0 ;
						for(std::size_t jcol =0; jcol<m_ncols;++jcol)
						{
							value += m_values[irow*m_ncols+jcol]*x[jcol] ;
						}
						y[irow] = value ;
					}
				  }
				}
			  }
			  #pragma omp taskwait
			}
        }
      }
	  
	  
      void omptilemult(VectorType const& x, VectorType& y, int const& nb_threads) const
      {
		if(nb_threads>0)
			omp_set_num_threads(nb_threads) ;
		 
        assert(x.size()>=m_nrows) ;
        assert(y.size()>=m_nrows) ;
        std::size_t nb_task = (m_nrows+m_chunk_size-1)/m_chunk_size ;
		size_t local_nrows = m_nrows/sqrt(nb_task) ;
		
        {
			#pragma omp parallel
			{
			  int id = omp_get_thread_num();
			  #pragma omp single
			  {
				std::cout<<"Thread ( th_id="<<id<<", nb_th="<<nb_threads<<") create "<<nb_task<<" tasks"<<std::endl ;
				for(int itask_row=0;itask_row<sqrt(nb_task);++itask_row)
				{
					for(int itask_col=0;itask_col<sqrt(nb_task);++itask_col)
					{
					  #pragma omp task
					  {
						std::size_t end_row = 0 ;
						if (itask_row==sqrt(nb_task)-1)
							end_row = m_nrows ;
						else
							end_row = (itask_row+1)*local_nrows ;
						
						std::size_t end_col = 0 ;
						if (itask_col==sqrt(nb_task)-1)
							end_col = m_nrows ;
						else
							end_col = (itask_col+1)*local_nrows ;
						
						for(std::size_t irow = itask_row*local_nrows; irow<end_row; ++irow)
						{
							double value = 0 ;
							for(std::size_t jcol =itask_col*local_nrows; jcol<end_col;++jcol)
							{
								value += m_values[irow*m_ncols+jcol]*x[jcol] ;
							}
							#pragma omp atomic
							y[irow] += value ;
						}
					  }
					}
				}
			  }
			  #pragma omp taskwait
			}
        }
      }
	  
	  
      void tbbmult(VectorType const& x, VectorType& y, int nb_threads) const
      {
        assert(x.size()>=m_nrows) ;
        assert(y.size()>=m_nrows) ;
		
		size_t local_nrows = m_nrows/nb_threads ;
        {
            tbb::parallel_for( int(0), nb_threads,
			[&]( int i )
				{
					for(std::size_t irow = i*local_nrows; irow<(i+1)*local_nrows;++irow)
					{
						double value = 0 ;
						for(std::size_t jcol =0; jcol<m_ncols;++jcol)
						{
							value += m_values[irow*m_ncols+jcol]*x[jcol] ;
						}
						y[irow] = value ;
					}
				}
			);
        }
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
    private:
      // number of lines
      std::size_t         m_nrows = 0 ;
      std::size_t         m_ncols  = 0 ;
      // matrix values
      std::vector<double> m_values ;
      int m_chunk_size = 1 ;
  };
} /* namespace PPTP */
#endif /* SRC_MATRIXVECTOR_DENSEMATRIX_H_ */