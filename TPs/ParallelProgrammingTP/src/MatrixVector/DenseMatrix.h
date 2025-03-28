/**************************************************************
 * DenseMatrix.h
 *
 * DESCRIPTION:
 *   - A dense matrix class that supports various multiplication
 *     methods: serial, OpenMP (simple parallel for, tasks, tiles),
 *     and TBB range-based parallelism.
 *   - The matrix is stored internally as a 1D array of doubles.
 *   - For convenience, "m_nrows" == "m_ncols" unless explicitly
 *     initialized otherwise.
 *
 * NOTES:
 *   - `m_chunk_size` is used to define task or tile sizes for
 *     parallel algorithms.
 *   - Some methods (like TBB) rely on additional libraries (TBB).
 *
 * 
 *
 **************************************************************/

#ifndef SRC_MATRIXVECTOR_DENSEMATRIX_H_
#define SRC_MATRIXVECTOR_DENSEMATRIX_H_

#include "tbb/tbb.h"
#include <vector>
#include <tuple>
#include <cassert>
#include <algorithm>

namespace PPTP
{

/**
 * \class DenseMatrix
 * \brief A basic dense matrix storage class with multiple
 *        multiplication routines (serial, OpenMP, TBB).
 *
 * The matrix is square by default, but can be initialized to
 * rectangular dimensions with init(nrows, ncols).
 */
class DenseMatrix
{
public:
    /// A shorthand for a simple std::vector<double>.
    typedef std::vector<double> VectorType;
    /// A tuple representing an entry: (row, col, value).
    typedef std::tuple<int,int,double> MatrixEntryType;

    /**
     * \brief Constructor that can optionally set the number of rows
     *        (and by default, columns) for a square matrix.
     *
     * \param nrows  The initial matrix dimension (rows = cols).
     */
    DenseMatrix(std::size_t nrows=0)
    : m_nrows(nrows)
    , m_ncols(nrows)
    {
        init(nrows);
    }

    /// Virtual destructor for safe inheritance.
    virtual ~DenseMatrix() {}

    /// Returns the number of rows.
    std::size_t nrows() const {
        return m_nrows;
    }

    /// Returns the number of columns.
    std::size_t ncols() const {
        return m_ncols;
    }

    /**
     * \brief Specify a chunk size (block or tile size) for
     *        parallel tasks (OpenMP or TBB).
     *
     * \param chunk_size  The size of each partition for tasks/tiles.
     */
    void setChunkSize(int chunk_size)
    {
        m_chunk_size = chunk_size;
    }

    /**
     * \brief Resizes the matrix to be square with dimension nrows x nrows
     *        and fills it with zeroes.
     *
     * \param nrows  The new dimension (square).
     */
    void init(std::size_t nrows)
    {
        m_nrows = nrows;
        m_ncols = nrows;
        if (m_nrows > 0)
        {
            m_values.resize(m_nrows * m_ncols);
            m_values.assign(m_values.size(), 0.0);
        }
    }

    /**
     * \brief Resizes the matrix to nrows x ncols and fills with zeroes.
     *
     * \param nrows  The row count
     * \param ncols  The column count
     */
    void init(std::size_t nrows, std::size_t ncols)
    {
        m_nrows = nrows;
        m_ncols = ncols;
        if (m_nrows > 0)
        {
            m_values.resize(m_nrows * m_ncols);
            m_values.assign(m_values.size(), 0.0);
        }
    }

    /**
     * \brief Copy contents from another DenseMatrix of the same size.
     *
     * \param rhs  The source matrix to copy.
     */
    void copy(DenseMatrix const& rhs)
    {
        m_nrows = rhs.nrows();
        m_ncols = rhs.ncols();
        m_values.resize(m_nrows * m_ncols);
        for(std::size_t k = 0; k < m_values.size(); ++k)
        {
            m_values[k] = rhs.m_values[k];
        }
    }

    /**
     * \brief Build a square matrix from triplets specifying row, column, and value.
     *
     * \param nrows    The dimension (rows == cols).
     * \param entries  A list of (row, col, value) defining nonzero entries.
     */
    void setFromTriplets(int nrows, std::vector<MatrixEntryType> const& entries)
    {
        init(nrows); // Ensures square matrix
        for(const auto& entry : entries)
        {
            int irow = std::get<0>(entry);
            int jcol = std::get<1>(entry);
            double val = std::get<2>(entry);
            m_values[irow * m_nrows + jcol] = val;
        }
    }

    /// Overloaded operator() for read/write access to element (i, j).
    double& operator()(std::size_t i, std::size_t j)
    {
        assert(m_nrows > 0);
        assert(i < m_nrows);
        assert(j < m_ncols);
        return m_values[i * m_ncols + j];
    }

    /// Overloaded operator() for read-only access to element (i, j).
    double operator()(std::size_t i, std::size_t j) const
    {
        assert(m_nrows > 0);
        assert(i < m_nrows);
        assert(j < m_ncols);
        return m_values[i * m_ncols + j];
    }

    /**
     * \brief Returns a raw pointer to the underlying data array of
     *        matrix values (row-major).
     */
    double* data() {
        return m_values.data();
    }

    /**
     * \brief Serial matrix-vector multiplication: y = A*x
     *
     * \param x  The input vector (size >= m_ncols).
     * \param y  The output vector (size >= m_nrows).
     */
    void mult(VectorType const& x, VectorType& y) const
    {
        assert(x.size() >= m_ncols);
        assert(y.size() >= m_nrows);

        const double* matrix_ptr = m_values.data();
        for (std::size_t irow = 0; irow < m_nrows; ++irow)
        {
            double temp_val = 0.0;
            for (std::size_t jcol = 0; jcol < m_ncols; ++jcol)
            {
                temp_val += matrix_ptr[jcol] * x[jcol];
            }
            y[irow] = temp_val;
            matrix_ptr += m_ncols; // move to next row
        }
    }

    /**
     * \brief Simple OpenMP parallel-for matrix-vector multiply.
     *        Splits the loop over rows.
     */
    void ompmult(VectorType const& x, VectorType& y) const
    {
        assert(x.size() >= m_ncols);
        assert(y.size() >= m_nrows);

        #pragma omp parallel for
        for(std::size_t irow = 0; irow < m_nrows; ++irow)
        {
            const double* row_ptr = m_values.data() + irow * m_ncols;
            double accum = 0.0;
            for(std::size_t jcol = 0; jcol < m_ncols; ++jcol)
            {
                accum += row_ptr[jcol] * x[jcol];
            }
            y[irow] = accum;
        }
    }

    /**
     * \brief OpenMP task-based multiplication, dividing the row space
     *        into chunks of size `m_chunk_size`.
     */
    void omptaskmult(VectorType const& x, VectorType& y) const
    {
        assert(x.size() >= m_ncols);
        assert(y.size() >= m_nrows);

        std::size_t num_tasks = (m_nrows + m_chunk_size - 1) / m_chunk_size;

        #pragma omp parallel shared(x,y)
        {
            #pragma omp single
            {
                for(std::size_t i = 0; i < num_tasks; ++i)
                {
                    #pragma omp task
                    {
                        std::size_t row_start = i * m_chunk_size;
                        std::size_t row_end   = std::min(row_start + m_chunk_size, m_nrows);

                        for(std::size_t irow = row_start; irow < row_end; ++irow)
                        {
                            const double* row_ptr = &m_values[irow * m_ncols];
                            double sum_val = 0.0;
                            for(std::size_t jcol = 0; jcol < m_ncols; ++jcol)
                            {
                                sum_val += row_ptr[jcol] * x[jcol];
                            }
                            y[irow] = sum_val;
                        }
                    }
                }
            }
        }
    }

    /**
     * \brief OpenMP tiling approach for matrix-vector multiplication.
     *        Breaks both rows and columns into tiles (m_chunk_size).
     */
    void omptilemult(VectorType const& x, VectorType& y) const
    {
        assert(x.size() >= m_ncols);
        assert(y.size() >= m_nrows);

        std::size_t num_tiles = (m_nrows + m_chunk_size - 1) / m_chunk_size;

        #pragma omp parallel shared(x, y)
        {
            #pragma omp single
            {
                for (std::size_t tile_row = 0; tile_row < num_tiles; ++tile_row)
                {
                    for (std::size_t tile_col = 0; tile_col < num_tiles; ++tile_col)
                    {
                        #pragma omp task
                        {
                            std::size_t row_begin = tile_row * m_chunk_size;
                            std::size_t row_end   = std::min(row_begin + m_chunk_size, m_nrows);
                            std::size_t col_begin = tile_col * m_chunk_size;
                            std::size_t col_end   = std::min(col_begin + m_chunk_size, m_ncols);

                            for (std::size_t irow = row_begin; irow < row_end; ++irow)
                            {
                                const double* row_ptr = &m_values[irow * m_ncols];
                                double tile_val = 0.0;
                                for (std::size_t jcol = col_begin; jcol < col_end; ++jcol)
                                {
                                    tile_val += row_ptr[jcol] * x[jcol];
                                }

                                // Because we might add partial sums from multiple tiles,
                                // we must do an atomic update to avoid data races.
                                #pragma omp atomic
                                y[irow] += tile_val;
                            }
                        }
                    }
                }
            }
        }
    }

    /**
     * \brief TBB-based parallel_for approach for matrix-vector multiply.
     *        (Implementation is left as an exercise here; can be extended.)
     */
    void tbbmult(VectorType const& x, VectorType& y) const
    {
        assert(x.size() >= m_ncols);
        assert(y.size() >= m_nrows);
        // Implementation example can be added if needed
    }

    /**
     * \brief TBB parallel_for version that handles row ranges in parallel.
     */
    void tbbrangemult(VectorType const& x, VectorType& y) const
    {
        assert(x.size() >= m_ncols);
        assert(y.size() >= m_nrows);

        const double* matr_data = m_values.data();
        tbb::parallel_for(std::size_t(0), m_nrows, [&](std::size_t irow)
        {
            double val_accum = 0.0;
            for(std::size_t k = 0; k < m_ncols; ++k)
            {
                val_accum += matr_data[irow*m_ncols + k] * x[k];
            }
            y[irow] = val_accum;
        });
    }

    /**
     * \brief 2D blocked TBB range-based multiplication, dividing
     *        rows and columns into blocks of size m_chunk_size.
     */
    void tbbrange2dmult(VectorType const& x, VectorType& y) const
    {
        assert(x.size() >= m_nrows);
        assert(y.size() >= m_nrows);

        tbb::spin_mutex spin_mtx;  // For safely accumulating partial sums

        tbb::parallel_for(
            tbb::blocked_range2d<std::size_t>(0, m_nrows, m_chunk_size,
                                              0, m_nrows, m_chunk_size),
            [&](const tbb::blocked_range2d<std::size_t>& block2d)
            {
                for (std::size_t i = block2d.rows().begin(); i < block2d.rows().end(); ++i)
                {
                    const double* row_ptr = &m_values[i * m_nrows];
                    double row_sum = 0.0;

                    for (std::size_t j = block2d.cols().begin(); j < block2d.cols().end(); ++j)
                    {
                        row_sum += row_ptr[j] * x[j];
                    }

                    // Use a scoped lock for partial sum update to y[i]
                    {
                        tbb::spin_mutex::scoped_lock lock(spin_mtx);
                        y[i] += row_sum;
                    }
                }
            }
        );
    }

private:
    /// The total number of rows in the matrix.
    std::size_t m_nrows = 0;
    /// The total number of columns in the matrix.
    std::size_t m_ncols = 0;

    /// The actual storage array: row-major order, of size (m_nrows*m_ncols).
    std::vector<double> m_values;

    /// Chunk size used for tasks or tiling in parallel modes.
    int m_chunk_size = 1;
};

} /* namespace PPTP */

#endif /* SRC_MATRIXVECTOR_DENSEMATRIX_H_ */
