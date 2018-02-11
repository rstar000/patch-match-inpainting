#ifndef NDARRAY_H
#define NDARRAY_H

#include <vector>
#include <cstdarg>
#include <tuple>

template<typename T>
class ndarray
{
    std::vector<T> values;
    std::vector<int> shape;
    std::vector<int> offsets;
    int num_elem;
    int num_dims;
public:
    ndarray(std::vector<int> array_shape);

    ndarray();

    std::vector<int> Shape() const;

    int Dimentions() const;

    T& operator() (const std::initializer_list<int> &il);

    T& operator() (int dim1, ...);

    void Resize(std::vector<int> array_shape);

    std::tuple<int, int, int> shapeTuple();
};


#endif // NDARRAY_H
