#include "Matrix.h"

template<int n, int m>
Matrix<n,m>::Matrix(){
    arr = new float*[n];
    for(int i = 0; i < n; i++){
        arr[i] = new float[m];
        for(int k = 0; k < m; k++){
            arr[i][k] = 0;
        }
    }
}

template<int n, int m>
Matrix<n,m>::Matrix(float** other){
    arr = other;
}

template<int n, int m>
Matrix<n,m>::Matrix(const Matrix<n,m>& matrixObj){
    // deep copy
    arr = new float*[n];

    for(int row = 0; row < n; row++){

        arr[row] = new float[m];

        for(int col = 0; col < m; col++){
            arr[row][col] = matrixObj.arr[row][col];
        }
    }
}

template<int n, int m>
Matrix<n,m>::~Matrix(){
    for(int i = 0; i < n; i++){
        delete[] arr[i];
    }
    delete[] arr;
}

template<int n, int m>
int Matrix<n,m>::getN() const{
    return n;
}

template<int n, int m>
int Matrix<n,m>::getM() const{
    return m;
}

template<int n, int m>
Matrix<n,m>& Matrix<n,m>::operator=(const Matrix<n,m>& matrixObj){
    if(arr != nullptr){
        for(int i = 0; i < n; i++){
            delete[] arr[i];
        }
        delete[] arr;
    }

    arr = new float*[n];
    for(int r = 0; r < n; r++){
        arr[r] = new float[m];
        for(int c = 0; c < m; c++){
            arr[r][c] = matrixObj[r][c];
        }
    }
    return *this;
}

template<int n, int m>
template<int a>
Matrix<n,a> Matrix<n,m>::operator*(const Matrix<m,a> B) const {
    Matrix<n,a> result;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < a; j++) {
            float sum = 0.0f;
            for (int k = 0; k < m; k++) {
                sum += arr[i][k] * B[k][j];
            }
            result[i][j] = sum;
        }
    }
    return result;
}


template<int n, int m>
Matrix<n,m> Matrix<n,m>::operator*(const float scalar) const{
    Matrix<n,m> result(*this);

    for(int i = 0; i < n; i++){
        for(int j = 0; j < m; j++){
            (result[i][j]) *= scalar;
        }
    }
    return result;
}

template<int n, int m>
Matrix<n,m> Matrix<n,m>::operator+(const Matrix<n,m> matrixObj) const{
    Matrix<n,m> result;

    for(int r = 0; r < n; r++){
        for(int c = 0; c < m; c++){
            result[r][c] = arr[r][c] + matrixObj[r][c];
        }
    }
    return result;
}

template<int n, int m>
Matrix<m,n> Matrix<n,m>::operator~() const{
    Matrix<m,n> result;

    for(int row = 0; row < n; row++){
        for(int col = 0; col < m; col++){
            (result[col][row]) = (arr[row][col]);
        }
    }
    return result;
}


template<int n, int m>
float Matrix<n,m>::determinant() const{
    if(n != m){
        throw "Matrix is not a square";
    }
    float determinant = 0.f;
    for(int i = 0; i < n; i++){
        Matrix<n-1, n-1> minor;
        for(int r = 1; r < n; r++){
            int idx = 0;
            for(int c = 0; c < n; c++){
                if(c == i) continue;

                minor[r-1][idx] = arr[r][c];
                idx++;
            }
        }
        float cofactor = ((i % 2 == 0)? 1.0f: -1.0f) * (arr[0][i]) * minor.determinant();
        determinant += cofactor;
    }
    return determinant;
}

template <>
float Matrix<1,1>::determinant() const{
    return arr[0][0];
}


template <>
float Matrix<2,2>::determinant() const{
    return arr[0][0]*arr[1][1] - arr[0][1]*arr[1][0];
}