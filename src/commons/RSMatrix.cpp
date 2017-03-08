#include "RSMatrix.h"
#include <QtMath>
#include <gsl/gsl_linalg.h>

RSMatrix::RSMatrix()
{
    m_mat = 0;
    m_rows = 0;
    m_cols = 0;
}

RSMatrix::RSMatrix(int rows, int cols)
{
    allocate(rows, cols);
}

RSMatrix::RSMatrix(int rows, int cols, QList<double> data)
{
    allocate(rows, cols);
    setData(data);
}

RSMatrix::~RSMatrix()
{
    if(m_mat != 0)
    {
        delete[] m_mat;
        m_mat = 0;
    }
}

void RSMatrix::allocate(int rows, int cols)
{
    m_rows = rows;
    m_cols = cols;
    m_mat = new double[m_rows*m_cols];
}

double RSMatrix::factor(int i, int j) const
{
    double m_data = ((i + j) % 2 == 0) ? 1.0 : -1.0;
    return m_data;
}

double RSMatrix::determinant(int size) const
{
    Q_ASSERT(m_rows == m_cols);

    if(size == 1)
    {
        return getData(0, 0);
    }
    else if(size == 2)
    {
        return getData(0, 0)*getData(1, 1) - getData(1, 0)*getData(0, 1);
    }

    double m_data = 0;

    for(int j = 0; j < size; j++)
    {

        double m_f0j = factor(0, j);
        double m_c0j = getData(0, j);
        QScopedPointer<RSMatrix> m_m0j(minor(0, j));
        double m_d0j = m_m0j->determinant(size - 1);
        m_data += m_f0j*m_c0j*m_d0j;
    }

    return m_data;
}

void RSMatrix::getStdList(QList<double> dataL, double *dataS) const
{
    for(int i = 0; i < dataL.size(); i++)
    {
        dataS[i] = dataL.at(i);
    }
}

// dij = s*aij
RSMatrix* RSMatrix::scalar(RSMatrix* mA, double s)
{
    int m_rows = mA->getRows();
    int m_cols = mA->getCols();
    RSMatrix *m_data = new RSMatrix(m_rows, m_cols);

    for(int i = 0; i < m_rows; i++)
    {
        for(int j = 0; j < m_cols; j++)
        {
            double m_aij = mA->getData(i, j);
            double m_dij = s*m_aij;
            m_data->setData(i, j, m_dij);
        }
    }

    return m_data;
}

// di = ai*bi
double RSMatrix::scalar(QList<double> rowA, QList<double> colB)
{
    Q_ASSERT(rowA.size() == colB.size());

    int m_size = rowA.size();
    double m_data = 0;

    for(int i = 0; i < m_size; i++)
    {
        double m_ai = rowA.at(i);
        double m_bi = colB.at(i);
        double m_di = m_ai*m_bi;
        m_data += m_di;
    }

    return m_data;
}

// dij = s(ai,bj)
RSMatrix* RSMatrix::mul(RSMatrix* mA, RSMatrix* mB)
{
    Q_ASSERT(mA->getCols() == mB->getRows());

    int m_rows = mA->getRows();
    int m_cols = mB->getCols();
    RSMatrix *m_data = new RSMatrix(m_rows, m_cols);

    for(int i = 0; i < m_rows; i++)
    {
        QList<double> m_rowAi = mA->rows(i);

        for(int j = 0; j < m_cols; j++)
        {
            QList<double> m_colBj = mB->cols(j);
            double m_dij = scalar(m_rowAi, m_colBj);
            m_data->setData(i, j, m_dij);
        }
    }

    return m_data;
}

// dij = xi^j
RSMatrix* RSMatrix::vandermonde(QList<double> x, int degree)
{
    int m_rows = x.size();
    int m_cols = degree + 1;
    RSMatrix *m_data = new RSMatrix(m_rows, m_cols);

    for(int i = 0; i < m_rows; i++)
    {
        for(int j = 0; j < m_cols; j++)
        {
            double m_xi = x.at(i);
            double m_dij = qPow(m_xi, (double)j);
            m_data->setData(i, j, m_dij);
        }
    }

    return m_data;
}

// d = inv(a)*b
RSMatrix* RSMatrix::equationSolver(RSMatrix* mA, RSMatrix* mB)
{
    QScopedPointer<RSMatrix> m_invA;

    if(mA->getRows() == mA->getCols())
    {
        m_invA.reset(mA->inverse());
    }
    else
    {
        m_invA.reset(mA->pseudoInverse());
    }

    RSMatrix *m_data = mul(m_invA.data(), mB);
    return m_data;
}


// fd = fd*f(d-1)
int RSMatrix::factorial(int n)
{
    if(n == 0)
    {
        return 1;
    }

    int m_data = n*factorial(n - 1);
    return m_data;
}

int RSMatrix::permutation(int n)
{
    return factorial(n);
}

// cdk = fd/f(d - k)
int RSMatrix::combination(int n, int k)
{
    int m_fn = factorial(n);
    int m_fk = factorial(n - k);
    int m_data = m_fn/m_fk;
    return m_data;
}

// Ax = b // A(m x n)
QList<double> RSMatrix::interpolationSVD(QList<double> x, QList<double> y, int degree)
{
    QList<double> m_data;
    QScopedPointer<RSMatrix> m_dV(RSMatrix::vandermonde(x, degree));
    int m_M = m_dV.data()->getRows();
    int m_N = m_dV.data()->getCols();

    QScopedPointer<RSMatrix> m_dB(new RSMatrix(m_M, 1, y));

    gsl_matrix_view m_A = gsl_matrix_view_array (m_dV.data()->getData(), m_M, m_N);
    gsl_matrix *m_V = gsl_matrix_alloc(m_N, m_N);
    gsl_vector *m_S = gsl_vector_alloc(m_N);
    gsl_vector *m_Work = gsl_vector_alloc(m_N);

    gsl_linalg_SV_decomp(&m_A.matrix, m_V, m_S, m_Work);

    gsl_vector_view m_B = gsl_vector_view_array(m_dB.data()->getData(), m_M);
    gsl_vector *m_X = gsl_vector_alloc(m_N);

    gsl_linalg_SV_solve(&m_A.matrix, m_V, m_S, &m_B.vector, m_X);

    for(int i = 0; i < m_N; i++)
    {
        m_data.push_back(m_X->data[i]);
    }

    gsl_matrix_free(m_V);
    gsl_vector_free(m_S);
    gsl_vector_free(m_Work);
    gsl_vector_free(m_X);

    return m_data;
}

double* RSMatrix::getData() const
{
    return m_mat;
}

double RSMatrix::getData(int i, int j) const
{
    int k = i*m_cols + j;
    return m_mat[k];
}

void RSMatrix::setData(int i, int j, double data)
{
    int k = i*m_cols + j;
    m_mat[k] = data;
}

void RSMatrix::setData(int rows, int cols, double *data)
{
    m_rows = rows;
    m_cols = cols;
    m_mat = data;
}

int RSMatrix::getRows() const
{
    return m_rows;
}

int RSMatrix::getCols() const
{
    return m_cols;
}

void RSMatrix::setData(QList<double> data)
{
    int m_index = 0;

    for(int i = 0; i < m_rows; i++)
    {
        for(int j = 0; j < m_cols; j++)
        {
            int k = i*m_cols + j;
            double m_d = data.at(m_index++);
            m_mat[k] = m_d;
        }
    }
}

RSMatrix* RSMatrix::row(int i) const
{
    RSMatrix* m_rowI = new RSMatrix(1, m_cols);

    for(int j = 0; j < m_cols; j++)
    {
        double dj = getData(i, j);
        m_rowI->setData(0, j, dj);
    }

    return m_rowI;
}

QList<double> RSMatrix::rows(int i) const
{
    QList<double> m_data;

    for(int j = 0; j < m_cols; j++)
    {
        double dj = getData(i, j);
        m_data.push_back(dj);
    }

    return m_data;
}

RSMatrix* RSMatrix::col(int j) const
{
    RSMatrix* m_colJ = new RSMatrix(m_rows, 1);

    for(int i = 0; i < m_rows; i++)
    {
        double di = getData(i, j);
        m_colJ->setData(i, 0, di);
    }

    return m_colJ;
}

QList<double> RSMatrix::cols(int j) const
{
    QList<double> m_data;

    for(int i = 0; i < m_rows; i++)
    {
        double di = getData(i, j);
        m_data.push_back(di);
    }

    return m_data;
}

double RSMatrix::determinant() const
{
    Q_ASSERT(m_rows == m_cols);

    int m_size = m_rows;
    double m_data = determinant(m_size);
    return m_data;
}

RSMatrix* RSMatrix::minor(int i, int j) const
{
    Q_ASSERT(m_rows == m_cols);

    int m_size = m_rows - 1;
    RSMatrix* m_data = new RSMatrix(m_size, m_size);

    int iD = 0;

    for(int iC = 0; iC < m_rows; iC++)
    {
        if(iC == i)
        {
            continue;
        }


        int jD = 0;

        for(int jC = 0; jC < m_cols; jC++)
        {
            if(jC == j)
            {
                continue;
            }

            double m_dij = getData(iC, jC);
            m_data->setData(iD, jD, m_dij);

            jD++;
        }

        iD++;
    }

    return m_data;
}

// dij = fij*d(mij)
double RSMatrix::cofactor(int i, int j) const
{
    double m_fij = factor(i, j);
    QScopedPointer<RSMatrix> m_mij(minor(i, j));
    double m_dij = m_mij.data()->determinant();
    double m_data = m_fij*m_dij;
    return m_data;
}

// dij = cij
RSMatrix* RSMatrix::comatrix() const
{
    Q_ASSERT(m_rows == m_cols);

    RSMatrix *m_data = new RSMatrix(m_rows, m_cols);

    for(int i = 0; i < m_rows; i++)
    {
        for(int j = 0; j < m_cols; j++)
        {
            double m_cij = cofactor(i, j);
            m_data->setData(i, j, m_cij);
        }
    }

    return m_data;
}

// d = cT
RSMatrix* RSMatrix::adjugate() const
{
    Q_ASSERT(m_rows == m_cols);

    QScopedPointer<RSMatrix> m_c(comatrix());
    RSMatrix *m_data = m_c.data()->transpose();

    return m_data;
}

// dji = aij
RSMatrix* RSMatrix::transpose() const
{
    RSMatrix *m_data = new RSMatrix(m_cols, m_rows);

    for(int i = 0; i < m_rows; i++)
    {
        for(int j = 0; j < m_cols; j++)
        {
            double m_aij = getData(i, j);
            m_data->setData(j, i, m_aij);
        }
    }

    return m_data;
}

// d = (1/det)*adj
RSMatrix* RSMatrix::inverse() const
{
    Q_ASSERT(m_rows == m_cols);

    QScopedPointer<RSMatrix> m_adj(adjugate());
    double m_det = determinant();
    RSMatrix* m_data = scalar(m_adj.data(), 1/m_det);

    return m_data;
}

// d = inv(aT.a)*aT = aT*inv(a*aT)
RSMatrix* RSMatrix::pseudoInverse()
{
    QScopedPointer<RSMatrix> m_aT(transpose());
    QScopedPointer<RSMatrix> m_aL(mul(m_aT.data(), this));
    QScopedPointer<RSMatrix> m_aR(mul(this, m_aT.data()));
    double m_detL = m_aL.data()->determinant();
    double m_detR = m_aR.data()->determinant();
    QScopedPointer<RSMatrix> m_invA;
    RSMatrix *m_data = 0;

    if(m_detL != 0)
    {
        m_invA.reset(m_aL.data()->inverse());
        m_data = mul(m_invA.data(), m_aT.data());
    }
    else if(m_detR != 0)
    {
        m_invA.reset(m_aR.data()->inverse());
        m_data = mul(m_aT.data(), m_invA.data());
    }

    return m_data;
}

// di = aij
QList<double> RSMatrix::toList() const
{
    QList<double> m_data;

    if(m_cols == 1)
    {
        int j = 0;

        for(int i = 0; i < m_rows; i++)
        {
            double m_aij = getData(i, j);
            m_data.push_back(m_aij);
        }
    }
    else if(m_rows == 1)
    {
        int i = 0;

        for(int j = 0; j < m_cols; j++)
        {
            double m_aij = getData(i, j);
            m_data.push_back(m_aij);
        }
    }

    return m_data;
}
