#include "Vector.h"
#include <cmath>


template<int n>
Vector<n>::Vector(){
    arr = new float[n];
    for(int i = 0; i < n; i++){
        arr[i]=0.0;
    }  
}
template<int n>
Vector<n>::Vector(std::initializer_list<float> initList){
    arr = new float[n];

    int initSize = initList.size();
    if(initSize >= n){
        int scored = 0;
        for(float val : initList){
            if(scored >= n){
                break;
            }
            arr[scored++] = val;
        }
    }else{
        int scored = 0;
        // get all of initList
        for(float val : initList){
            arr[scored++] = val;
        }
        while(scored < n){
            arr[scored++] = 0.0;
        }
    }
}
template<int n>
Vector<n>::Vector(float* copyArr){
    arr = copyArr;
}

template<int n>
Vector<n>::Vector(const Vector<n>& v){
    arr = new float[n];
    for(int i = 0; i < n; i++){
        arr[i] = v[i];
    }
}

template<int n>
Vector<n>::Vector(const Matrix<n,1 >& matrixObj){
    // create a vector based on Nx1 matrix
    arr = new float[n];
    for(int i = 0; i < n; i++){
        arr[i] = matrixObj[i][0];
    }
}
template<int n>
Vector<n>::operator Matrix<n,1>() const{
    Matrix<n,1> result;
    for(int i = 0; i < n; i++){
        result[i][0] = arr[i];
    }
    return result;
}


template<int n>
Vector<n>& Vector<n>::operator=(const Vector<n>& vObj){
    
    // delete mem first
    delete[] arr;

    int size = vObj.getN();
    arr = new float[size];
    for(int i = 0; i < size; i++){
        arr[i] = vObj[i];
    }

    return *this;
}

template<int n>
Vector<n>::~Vector(){
    // if issues. comment out
    delete[] arr;
}

template<int n>
int Vector<n>::getN() const{
    return n;
}


template<int n>
float Vector<n>::magnitude() const{
    float size = 0;

    for(int i = 0; i < n; i++){
        size += (arr[i]) * (arr[i]);
    }

    return std::sqrt(size);
}

template<int n>
Vector<n> Vector<n>::operator+(const Vector<n> vObj) const{
    Vector<n> newVectorObject(*this);
    for(int i = 0; i < n; i++){
        newVectorObject[i] = arr[i] + vObj.arr[i];
    }

    return newVectorObject;
}

template<int n>
Vector<n> Vector<n>::operator-(const Vector<n> vObj) const{
    Vector<n> newVectorObject(*this);
    for(int i = 0; i < n; i++){
        newVectorObject[i] = arr[i] - vObj.arr[i];
    }

    return newVectorObject;
}


// scaling
template<int n>
Vector<n> Vector<n>::operator*(const float fScalar) const{
    Vector<n> result;
    
    for(int i = 0; i < n; i++){
        (result[i]) = ((arr[i])) * fScalar;
    }
    return result;
}

// dot product
template<int n>
float Vector<n>::operator*(const Vector<n> vObj) const{
    float result = 0.0f;
    for(int i = 0; i < n; i++){
        result += ((arr[i])) * (vObj[i]);
    }
    return result;
}

// finally, the cross product
template<>
Vector<3> Vector<3>::crossProduct(const Vector<3> w) const {
    Vector<3> result;
    result[0] = arr[1] * w[2] - arr[2] * w[1];
    result[1] = arr[2] * w[0] - arr[0] * w[2];
    result[2] = arr[0] * w[1] - arr[1] * w[0];
    return result;
}

template<int n>
Vector<n> Vector<n>::unitVector() const{
    float magnitude = this->magnitude();
    Vector<n> result;
    if(magnitude == 0){
        throw "Invalid unit vector";

    }else{
        for(int i = 0; i < n; i++){
            (result[i]) = ((arr[i])) / magnitude;
        }
    }
    return result;
}
