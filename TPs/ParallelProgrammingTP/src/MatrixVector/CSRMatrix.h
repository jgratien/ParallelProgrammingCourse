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

    CSRMatrix(std::vector<int> kcol, std::vector<int> cols, std::vector<double> values)
    {
        m_kcol = kcol;
        m_cols = cols;
        m_values = values;
        m_nnz   = m_values.size();
        m_nrows = (size_t)m_kcol.size() - 1;
    }

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

    double sparsity() const{
        // std::cout << "rows : " << m_nrows << std::endl;
        // std::cout << "nnz : " << m_nnz << std::endl;
        return m_nnz/(m_nrows*m_nrows);
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
        // std::cout << m_nrows << " = m_nrows \n";
        // std::cout << y.size() << " = y_size \n";
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
        #pragma omp parallel shared(x,y)
        {
          #pragma omp for schedule(dynamic,m_chunk_size) nowait  //static,dynamic,guided
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
    }
    void printVerySimple() const{
        std::cout << "Cols tab size : " << m_cols.size() << "\n";
        std::cout << "cols : [" << m_cols[0] << ", ..., " << m_cols[m_cols.size()-1] << "]\n";

        std::cout << "\nkcols size : " << m_kcol.size() << "\n";
        std::cout << "kcols : [" << m_kcol[0] << ", ..., " << m_kcol[m_kcol.size()-1] << "]\n";
    }

    void printSimple(){
        std::cout << "Values :\n";
        for(size_t i = 0; i < m_values.size(); ++i){
            std::cout << m_values[i] << ", ";
        }
        std::cout << std::endl;

        std::cout << "Cols :\n";
        for(size_t i = 0; i < m_cols.size(); ++i){
            std::cout << m_cols[i] << ", ";
        }
        std::cout << std::endl;

        std::cout << "Row ptr :\n";
        for(size_t i = 0; i < m_kcol.size(); ++i){
            std::cout << m_kcol[i] << ", ";
        }
        std::cout << std::endl;

    }
    void printCSR(){
        for(std::size_t irow=0; irow < m_nrows; ++irow){

        }
    }

    std::vector<double> values()const{
        return m_values;
    }
    std::vector<int> kcol()const{
        return m_kcol;
    }
    std::vector<int> cols()const{
        return m_cols;
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
