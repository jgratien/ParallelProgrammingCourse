/*
 * CSRMatrix.h
 *
 *  Created on: Sep 20, 2018
 *      Author: gratienj
 */

#ifndef SRC_MATRIXVECTOR_CSRMATRIX_H_
#define SRC_MATRIXVECTOR_CSRMATRIX_H_

namespace PPTP
{

class CSRMatrix
{
  public:
    typedef std::vector<double>         VectorType ;
    typedef std::tuple<int,int,double>  MatrixEntryType ;

    CSRMatrix(std::size_t nrows=0)
    : m_nrows(nrows)
    {}
    virtual ~CSRMatrix(){}

    void setChunkSize(int chunk_size)
    {
      m_chunk_size = chunk_size ;
    }

    std::size_t nrows() const {
      return m_nrows ;
    }

    std::size_t nnz() const {
      return m_nnz ;
    }
	
	std::vector<int> kcol() const {
		return m_kcol;
	}

	std::vector<int> col() const {
		return m_cols;
	}
	
	std::vector<double> val() {
		return m_values;
	}

    void setFromTriplets(int nrows, std::vector<MatrixEntryType> const& entries)
    {
      std::vector< std::map<int,double> > rows(nrows) ;
      for( const auto& entry : entries )
      {
        int irow = std::get<0>(entry) ;
        int jcol = std::get<1>(entry) ;
        auto const& val = std::get<2>(entry) ;
        rows[irow].insert(std::make_pair(jcol,val)) ;
      }
      int nnz = 0 ;
      for( auto const& row : rows)
        nnz += row.size() ;
      m_nrows = nrows ;
      m_nnz   = nnz ;
      m_kcol.resize(m_nrows+1) ;
      m_cols.resize(m_nnz) ;
      m_values.resize(m_nnz) ;
      int k = 0 ;
      for(int irow = 0;irow<nrows;++irow)
      {
        m_kcol[irow] = k ;
        auto const& row_entries = rows[irow] ;
        for(auto const& entry : row_entries)
        {
          m_cols[k] = entry.first ;
          m_values[k] = entry.second ;
          ++k ;
        }
      }
      m_kcol[nrows] = k ;
    }

    void mult(VectorType const& x, VectorType& y) const
    {
      assert(x.size()>=m_nrows) ;
      assert(y.size()>=m_nrows) ;
      for(std::size_t irow =0; irow<m_nrows;++irow)
      {
        double value = 0 ;
        for( int k = m_kcol[irow]; k < m_kcol[irow+1];++k)
        {
          value += m_values[k]*x[m_cols[k]] ;
        }
        y[irow] = value ;
      }
    }
	
	void mult2(VectorType const& x, VectorType& y, std::size_t& nb_row) const
    {
      assert(x.size()>=nb_row) ;
      assert(y.size()>=nb_row) ;
      for(std::size_t irow =0; irow<nb_row;++irow)
      {
        double value = 0 ;
        for( int k = (m_kcol[irow] - m_kcol[0]); k < (m_kcol[irow+1] - m_kcol[0]);++k)
        {
          value += m_values[k]*x[m_cols[k]] ;
        }
        y[irow] = value ;
      }
    }


    void ompmult(VectorType const& x, VectorType& y) const
    {
      assert(x.size()>=m_nrows) ;
      assert(y.size()>=m_nrows) ;
      {
         // todo OPENMP
      }
    }
	
	void print_spmv(std::size_t number_row) {
		std::size_t indice = 0;
		for(std::size_t i =0; i<number_row; i++) {
			std::size_t number_in_line = m_kcol[i] - m_kcol[i+1];
			for(std::size_t j=0;j<m_nrows;j++){
				if(number_in_line == 0){
					std::cout<<"0 ";
				}
				if(number_in_line != 0 && m_cols[indice] == j) {
					std::cout<<m_values[indice]<< " ";
					indice = indice +1;
				}
				else{
					std::cout<<"0 ";
				}
			}
			std::cout<<"\n";
		}
	}
	
	void set_value(std::vector<int> const& row_list, std::vector<int> const& col_list, std::vector<double> const& val_list, std::size_t nb_col){
		m_kcol = row_list;
		m_cols = col_list;
		m_values = val_list;
		m_nrows = nb_col;
	}
	
  private:
    // number of lines
    std::size_t         m_nrows = 0;

    // number of non zero entries
    std::size_t         m_nnz   = 0 ;

    // rows offset
    std::vector<int>    m_kcol ;

    // columns indexes
    std::vector<int>    m_cols ;

    // matrix values
    std::vector<double> m_values ;

    int m_chunk_size = 1 ;
};

} /* namespace PPTP */

#endif /* SRC_MATRIXVECTOR_CSRMATRIX_H_ */
