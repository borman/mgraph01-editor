#ifndef CONVOLUTION_H
#define CONVOLUTION_H

#include <QImage>

template<class V>
class Matrix
{
  public:
    Matrix(int size)
      : m_size(size), m_d(new V[size*size])
    {
      memset(m_d, 0, size*size*sizeof(V));
    }
    ~Matrix()
    {
      delete[] m_d;
    }

    int size() const { return m_size; }

    V at(int x, int y) const { return m_d[m_size*y + x]; }
    void set(int x, int y, V v) { m_d[m_size*y + x] = v; }

  private: 
    int m_size;
    V *m_d;
};

Matrix<double> unsharp(int size, double sigma, double alpha);
Matrix<double> gaussian(int size, double sigma);
void convolve(QImage &img, const Matrix<double> &m);

void median(QImage &img, int size);

#endif // CONVOLUTION_H
