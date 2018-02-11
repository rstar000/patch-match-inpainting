#include "ndarray.h"
#include <iostream>
template<typename T>
ndarray<T>::ndarray(std::vector<int> array_shape)
{
    Resize(array_shape);
}

template<typename T>
ndarray<T>::ndarray()
{
    num_elem = 0;
}

template<typename T>
std::vector<int> ndarray<T>::Shape() const
{
    return shape;
}

template<typename T>
int ndarray<T>::Dimentions() const
{
    return num_dims;
}

template<typename T>
T &ndarray<T>::operator()(const std::initializer_list<int> &il)
{
    int index = 0;
    int i = 0;
    for (auto x : il) {
        index += x * offsets[i];
        i++;
    }
    return values[index];
}

template<typename T>
T &ndarray<T>::operator()(int dim1, ...)
{
    int index = dim1 * offsets[0];
    va_list vl;
    va_start(vl,dim1);
    for(int i = 1; i < num_dims;i++)
    {
        int val=va_arg(vl,int);
        index += val * offsets[i];
    }
    va_end(vl);
    return values[index];
}

template<typename T>
void ndarray<T>::Resize(std::vector<int> array_shape)
{
    shape = array_shape;
    int dim = shape.size();
    num_elem = shape[0];
    num_dims = dim;
    for(int i = 1; i < dim; i++) {
        num_elem *= shape[i];
    }
    values.resize(num_elem);
    offsets.resize(dim);
    for(int i = 0; i < dim; i++) {
        int m = 1;
        for(int j = i+1; j < dim; j++) {
            m *= shape[j];
        }
        offsets[i] = m;
    }
}

template<typename T>
std::tuple<int, int, int> ndarray<T>::shapeTuple()
{
    return std::make_tuple(shape[0], shape[1], shape[2]);
}

template class ndarray<float>;
template class ndarray<int>;
