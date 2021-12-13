/*
 * MatrixGenerator.h
 *
 *  Created on: Sep 20, 2018
 *      Author: gratienj
 */

#ifndef SRC_MATRIXVECTOR_MATRIXGENERATOR_H_
#define SRC_MATRIXVECTOR_MATRIXGENERATOR_H_

namespace PPTP
{

class MatrixGenerator
{
  public:
    MatrixGenerator(){}
    virtual ~MatrixGenerator(){}
    void genLaplacian(int nx,Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>& matrix)
    {
      double diag = 1. ;
      double offdiag = -0.1 ;
      int nrows = nx*nx ;
      matrix.resize(nrows,nrows) ;
      for(int i=0;i<nx;++i)
        for(int j=0;j<nx;++j)
        {
          int irow = j*nx+i ;
          matrix(irow,irow)=diag ;
          if(i>0)
            matrix(irow,irow-1)=offdiag ;
          if(i<nx-1)
            matrix(irow,irow+1)=offdiag ;
          if(j>0)
            matrix(irow,irow-nx)=offdiag ;
          if(j<nx-1)
            matrix(irow,irow+nx)=offdiag ;
        }
    }


    void genLaplacian(int nx,Eigen::SparseMatrix<double>& matrix)
    {
      typedef typename Eigen::Triplet<double> MatrixEntryType ;
      std::vector<MatrixEntryType> entries;
      double diag = 1. ;
      double offdiag = -0.1 ;
      int nrows = nx*nx ;
      matrix.resize(nrows,nrows) ;
      entries.reserve(nrows*5) ;
      for(int i=0;i<nx;++i)
        for(int j=0;j<nx;++j)
        {
          int irow = j*nx+i ;
          entries.push_back(MatrixEntryType(irow,irow,diag)) ;
          if(i>0)
            entries.push_back(MatrixEntryType(irow,irow-1,offdiag)) ;
          if(i<nx-1)
            entries.push_back(MatrixEntryType(irow,irow+1,offdiag)) ;
          if(j>0)
            entries.push_back(MatrixEntryType(irow,irow-nx,offdiag)) ;
          if(j<nx-1)
            entries.push_back(MatrixEntryType(irow,irow+nx,offdiag)) ;
        }
      matrix.setFromTriplets(entries.begin(),entries.end()) ;
    }

    template<typename MatrixT>
    void genLaplacian(int nx,MatrixT& matrix)
    {
      typedef typename MatrixT::MatrixEntryType MatrixEntryType ;
      std::vector<MatrixEntryType> entries;
      double diag = 1. ;
      double offdiag = -0.1 ;
      int nrows = nx*nx ;
      entries.reserve(nrows*5) ;
      for(int i=0;i<nx;++i)
        for(int j=0;j<nx;++j)
        {
          int irow = j*nx+i ;
          entries.push_back(MatrixEntryType(irow,irow,diag)) ;
          if(i>0)
            entries.push_back(MatrixEntryType(irow,irow-1,offdiag)) ;
          if(i<nx-1)
            entries.push_back(MatrixEntryType(irow,irow+1,offdiag)) ;
          if(j>0)
            entries.push_back(MatrixEntryType(irow,irow-nx,offdiag)) ;
          if(j<nx-1)
            entries.push_back(MatrixEntryType(irow,irow+nx,offdiag)) ;
        }
      matrix.setFromTriplets(nrows,entries) ;
    }

    void readFromFile(std::string const& file,Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>& matrix)
    {
      std::ifstream fin(file.c_str()) ;
      int nrows,nnz ;
      fin>>nrows>>nnz>>std::ws;

      matrix.resize(nrows,nrows) ;
      for(int k=0;k<nnz;++k)
      {
        int row,col ;
        double val ;
        fin>>row>>col>>val;
        matrix(row,col)=val ;
      }
    }

    void readFromFile(std::string const& file,Eigen::SparseMatrix<double>& matrix)
    {
      typedef typename Eigen::Triplet<double> MatrixEntryType ;
      std::vector<MatrixEntryType> entries;
      std::ifstream fin(file.c_str()) ;
      int nrows,nnz ;
      fin>>nrows>>nnz>>std::ws;

      matrix.resize(nrows,nrows) ;
      for(int k=0;k<nnz;++k)
      {
        int row,col ;
        double val ;
        fin>>row>>col>>val;
        entries.push_back(MatrixEntryType(row,col,val)) ;
      }
      matrix.setFromTriplets(entries.begin(),entries.end()) ;
    }

    template<typename MatrixT>
    void readFromFile(std::string const& file,MatrixT& matrix)
    {
      typedef typename MatrixT::MatrixEntryType MatrixEntryType ;
      std::vector<MatrixEntryType> entries;
      std::ifstream fin(file.c_str()) ;
      int nrows,nnz ;
      fin>>nrows>>nnz>>std::ws;
      for(int k=0;k<nnz;++k)
      {
        int row,col ;
        double val ;
        fin>>row>>col>>val;
        entries.push_back(MatrixEntryType(row,col,val)) ;
      }
      matrix.setFromTriplets(nrows,entries) ;
    }
};

} /* namespace PPTP */

#endif /* SRC_MATRIXVECTOR_MATRIXGENERATOR_H_ */
