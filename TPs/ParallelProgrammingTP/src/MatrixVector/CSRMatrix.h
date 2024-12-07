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

struct CSRData {
    std::vector<int> kcol;  // Pointer to the range of m_kcol
    std::vector<int> cols;   // Pointer to the range of m_cols
    std::vector<double> values; // Pointer to the range of m_values
    std::size_t nrows;
    std::size_t nnz;
};

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

    CSRData data() {
      return CSRData{m_kcol, m_cols, m_values, m_nrows, m_nnz};
    }
    

    const int* kcol() const {
      return m_kcol.data() ;
    }

    const int* cols() const{
      return m_cols.data() ;
    }

    const double* values() const{
      return m_values.data() ;
    }


    // Method to extract a submatrix
    CSRMatrix extractSubmatrix(std::size_t start, std::size_t end) const {
      // Check bounds
      if (start >= end || end > m_nrows) {
        throw std::out_of_range("Invalid range for submatrix extraction");
      }

      // Create new CSRMatrix for the result
      CSRMatrix submatrix;
      submatrix.m_nrows = end - start; // Number of rows in the submatrix
      // Adjust kcols for the submatrix
      submatrix.m_kcol.resize(submatrix.m_nrows + 1);
      std::size_t nnz_start = m_kcol[start]; // First non-zero index in the range
      for (std::size_t i = start; i < end; ++i) {
        submatrix.m_kcol[i - start] = m_kcol[i] - nnz_start;
      }
      submatrix.m_kcol[submatrix.m_nrows] = m_kcol[end] - nnz_start; // Final row pointer
  
      // Extract relevant cols and values
      std::size_t nnz_end = m_kcol[end]; // Last non-zero index in the range
      submatrix.m_cols.assign(m_cols.begin() + nnz_start, m_cols.begin() + nnz_end);
      submatrix.m_values.assign(m_values.begin() + nnz_start, m_values.begin() + nnz_end);
      submatrix.m_nnz = nnz_end - nnz_start;

      return submatrix;
   }

    void copyCSRMatrixFromData(const std::vector<double>& data) {      
      // Update matrix metadata
      m_nrows = static_cast<int>(data[0]);
      m_nnz = static_cast<int>(data[1]);

      // Resize vectors in the CSRMatrix
      m_kcol.resize(m_nrows + 1);
      m_cols.resize(m_nnz);
      m_values.resize(m_nnz);

      // Copy data from the received struct into the vectors
      std::copy(data.begin() + 2, data.begin() + 2 + (m_nrows + 1), m_kcol.begin());
      std::copy(data.begin() + 2 + (m_nrows + 1), data.begin() + 2 + (m_nrows + 1) + m_nnz, m_cols.begin());
      std::copy(data.begin() + 2 + (m_nrows + 1) + m_nnz, data.end(), m_values.begin());

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
