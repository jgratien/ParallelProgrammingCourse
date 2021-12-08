/*
 * CSRMatrix.h
 *
 *  Created on: Sep 20, 2018
 *      Author: gratienj
 */

#ifndef SRC_MATRIXVECTOR_CSRMATRIX_H_
#define SRC_MATRIXVECTOR_CSRMATRIX_H_

template <typename T>
std::string tostring(std::vector<T> &x, int delTrailingZeros);

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

    CSRMatrix(std::vector<int> rowoff, std::vector<int> colidx, std::vector<double> values) {
      m_kcol = rowoff;
      m_cols = colidx ;
      m_values = values;
      m_nrows = rowoff.size() - 1;
      m_nnz   = values.size() ;
    }

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

    int* m_kcol_ptr() {
      return m_kcol.data();
    }

    int* m_cols_ptr() {
      return m_cols.data();
    }

    double* m_values_ptr() {
      return m_values.data();
    }

    std::vector<int> & get_Rowoff() {
      return m_kcol;
    }

    void keepFirstNRows(int n) {
      m_kcol.resize(n + 1);
      m_nrows = n;
      m_cols.resize(m_kcol[n] - 1);
      m_values.resize(m_kcol[n] - 1);
      m_nnz = m_kcol[n] - 1;
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
      for( auto const& row : rows) {
        nnz += row.size() ;
      }
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

    std::string string_m_values() const {
      std::string s;
      s += "y(";
      for (unsigned int i=0; i < m_values.size(); ++i) {
        s += std::to_string(m_values[i]);
        s.erase(s.find_last_not_of('0') + 1, std::string::npos);

        if (i < m_values.size() - 1) {s += "|";}
      }
      s += ")";
      return s;
    }

    std::string string_m_kcol() const {
      std::string s;
      s += "y(";
      for (unsigned int i=0; i < m_kcol.size(); ++i) {
        s += std::to_string(m_kcol[i]);

        if (i < m_kcol.size() - 1) {s += "|";}
      }
      s += ")";
      return s;
    }

    std::string string_m_cols() const {
      std::string s;
      s += "y(";
      for (unsigned int i=0; i < m_cols.size(); ++i) {
        s += std::to_string(m_cols[i]);

        if (i < m_cols.size() - 1) {s += "|";}
      }
      s += ")";
      return s;
    }

    void mult(VectorType const& x, VectorType& y) const
    {
      /*if (node != 0) {
        std::cout << "-------------- Entering mult  (node " << node << ") " << std::flush;
        std::string s2("Working with y of size " + std::to_string(y.size()) + "\n");
        std::string s3("And with m_rowoff of size " + std::to_string(m_kcol.size()) + " : " + string_m_kcol()+ "\n");
        std::string s4("And with m_cols of size " + std::to_string(m_cols.size()) + " : " + string_m_cols()+ "\n");
        std::string s5("And with m_values of size " + std::to_string(m_values.size()) + " : " + string_m_values());
        std::cout << s2 << s3 << s4 << s5 << std::endl;
      }*/
      assert(x.size()>=m_nrows) ;
      assert(y.size()>=m_nrows) ;
      for(std::size_t irow =0; irow<m_nrows;++irow)
      {
        //std::cout << "irow = " << irow << std::endl;
        double value = 0 ;
        //std::cout << "y = " ;
        for( int k = m_kcol[irow]; k < m_kcol[irow+1];++k)
        {
          //std::cout << m_values[k] << "*" << x[m_cols[k]] << "+" ;
          value += m_values[k]*x[m_cols[k]] ;
        }
        //std::cout << " = " << value << std::endl;
        y[irow] = value ;
      }
      //std::cout << " exiting mult -------------- " << std::endl;
    }


    void ompmult(VectorType const& x, VectorType& y) const
    {
      assert(x.size()>=m_nrows) ;
      assert(y.size()>=m_nrows) ;
      {
         // todo OPENMP
      }
    }
    /*
    int* cut(int start, int end) {
      int val = m_kcol[end];
      int
      int i = 0;
      while m_kcol[i]
    }*/

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
