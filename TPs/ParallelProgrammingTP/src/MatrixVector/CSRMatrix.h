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

    CSRMatrix(std::size_t nrows, std::size_t nnz, std::vector<int> kcol, std::vector<int> cols, std::vector<double> values)
        : m_nrows(nrows), m_nnz(nnz), m_kcol(kcol), m_cols(cols), m_values(values)
    {}

    virtual ~CSRMatrix(){}

    void setChunkSize(int chunk_size)
    {
      m_chunk_size = chunk_size ;
    }

    std::size_t nrows() const {
      return m_nrows ;
    }

    std::vector<int> kcol() const {
        return m_kcol;
    }

    std::vector<int> cols() const {
        return m_cols;
    }

    std::size_t nnz() const {
      return m_nnz ;
    }

    std::vector<double> values() {
        return m_values;
    }

    double* data() {
        return m_values.data();
    }

    int* data_cols() {
        return m_cols.data();
    }

    int* data_kcol() {
        return m_kcol.data();
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

	
	void print()
	{
		printf("\nMatrix kcol\n");
		for (std::size_t i = 0; i<m_nrows+1; i++) {
			std::cout << m_kcol[i] << " ; ";
		}
		printf("\nMatrix cols\n");
		for (std::size_t i = 0; i<m_kcol[m_nrows]-m_kcol[0]; i++) {
			std::cout << m_cols[i] << " ; ";
		}
		printf("\nMatrix values\n");
		for (std::size_t i = 0; i<m_kcol[m_nrows]-m_kcol[0]; i++) {
			std::cout << m_values[i] << " ; ";
		}
		
	}
	
	void setSparse(std::vector<int> const kcol, std::vector<int> const cols, std::vector<double> const values)
	{
		m_kcol.resize(kcol.size()) ;
		m_cols.resize(cols.size()) ;
		m_values.resize(values.size()) ;
		for(int k=0; k<kcol.size();++k)
			m_kcol[k]=kcol[k];
		for(int c=0; c<cols.size();++c)
			m_cols[c]=cols[c];
		for(int v=0; v<values.size();++v)
			m_values[v]=values[v];
		m_nrows=kcol.size()-1;
	}
	
	void mult(VectorType const& x, VectorType& y) const
    {
      assert(x.size()>=m_nrows) ;
      assert(y.size()>=m_nrows) ;
      for(std::size_t irow =0; irow<m_nrows;++irow)
      {
        double value = 0 ;
        for( int k = (m_kcol[irow] - m_kcol[0]); k <( m_kcol[irow+1] - m_kcol[0]);++k)
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
