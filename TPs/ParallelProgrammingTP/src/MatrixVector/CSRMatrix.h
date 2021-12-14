#ifndef SRC_MATRIXVECTOR_CSRMATRIX_H_
#define SRC_MATRIXVECTOR_CSRMATRIX_H_
namespace PPTP
{
class CSRMatrix
{
  public:
    typedef std::vector<double>         VectorType ;
    typedef std::tuple<int,int,double>  MatrixEntryType ;
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
  public:
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
    int * cols()  {
        return m_cols.data();
    }
    int * kcol() {
        return m_kcol.data();
    }
    double * values()  {
        return m_values.data();
    }
    // void init(std::size_t nrows, size_t nnz){
    //   m_nrows = nrows;
    //   m_nnz = nnz;
    //   m_kcol.resize(nrows + 1);
    //   m_cols.resize(nnz);
    //   m_values.resize(nnz);
    // }
    void init(std::size_t localSize, size_t nnz){
      m_nrows = localSize - 1;
      m_nnz = nnz;
      m_kcol.resize(localSize);
      m_cols.resize(nnz);
      m_values.resize(nnz);
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
    // CSRMatrix generateSimpleCSR4x4(){
    //     /* We generate the matrix :
    //     [0, 0, 1, 0]
    //     [2, 0, 0, 3]
    //     [0, 0, 4, 0]
    //     [5, 6, 0, 0]
    //     */
    //     std::vector<int>    kcol = {0,1,3,4,6};
    //     std::vector<int>    cols = {2,0,3,2,0,1};
    //     std::vector<double> values = {1,2,3,4,5,6};
    //     return CSRMatrix(kcol, cols, values);
    // }
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
      }
};
} /* namespace PPTP */
#endif /* SRC_MATRIXVECTOR_CSRMATRIX_H_ */
