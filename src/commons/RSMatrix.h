#ifndef RSMATRIX_H
#define RSMATRIX_H

#include <QObject>
#include <QList>

class RSMatrix  : public QObject
{
    Q_OBJECT

public:
    RSMatrix();
    RSMatrix(int rows, int cols);
    RSMatrix(int rows, int cols, QList<double> data);
    ~RSMatrix();

private:
    void allocate(int rows, int cols);
    double factor(int i, int j) const;
    double determinant(int size) const;
    void getStdList(QList<double> dataL, double *dataS) const;

public:
    static RSMatrix* scalar(RSMatrix* mA, double s);
    static double scalar(QList<double> rowA, QList<double> colB);
    static RSMatrix* mul(RSMatrix* mA, RSMatrix* mB);
    static RSMatrix* vandermonde(QList<double> x, int degree);
    static RSMatrix* equationSolver(RSMatrix* mA, RSMatrix* mB);
    static int factorial(int n);
    static int permutation(int n);
    static int combination(int n, int k);

    static QList<double> interpolationSVD(QList<double> x, QList<double> y, int degree);

public:
    double* getData() const;
    double getData(int i, int j) const;
    void setData(int row, int col, double data);
    void setData(int rows, int cols, double *data);
    int getRows() const;
    int getCols() const;
    void setData(QList<double> data);
    RSMatrix* row(int i) const;
    QList<double> rows(int i) const;
    RSMatrix* col(int j) const;
    QList<double> cols(int j) const;
    double determinant() const;
    RSMatrix* minor(int i, int j) const;
    double cofactor(int i, int j) const;
    RSMatrix* comatrix() const;
    RSMatrix* adjugate() const;
    RSMatrix* transpose() const;
    RSMatrix* inverse() const;
    RSMatrix* pseudoInverse();
    QList<double> toList() const;

private:
    double *m_mat;
    int m_rows;
    int m_cols;
};

#endif // RSMATRIX_H
