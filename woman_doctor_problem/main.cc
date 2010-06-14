#include <stdint.h>
#include <cstdio>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cassert>
#include <stdexcept>

using namespace std;

template <class T, unsigned int N>
class Vector {
public:
    Vector(const T init) {
        for (unsigned int i = 0; i < N; ++i)
            elements[i] = init;
    }

    T& operator[](unsigned index) {
        if (index >= N)
            throw out_of_range(__func__);

        return elements[index];
    }

    Vector<T, N>& operator=(const Vector<T, N>& rhs) {
        for (unsigned int i = 0; i < N; ++i)
            elements[i] = rhs.elements[i];

        return *this;
    }

    T sum() const {
        T s = 0;

        for (unsigned int i = 0; i < N; ++i)
            s += elements[i];

        return s;
    }

    static T sum(Vector<T, N> vector) {
        return vector.sum();
    }

    T min() const {
        T e = elements[0];

        for (unsigned int i = 0; i < N; ++i)
            e = std::min(e, elements[i]);

        return e;
    }

    static T min(Vector<T, N> vector) {
        return vector.min();
    }

    Vector<T, N>& operator+=(const Vector<T, N>& rhs) {
        T e = this->min();

        for (unsigned int i = 0; i < N; ++i)
            elements[i] = std::max(elements[i], e + rhs.elements[i]);

        return *this;
    }

    Vector<T, N> operator+(const Vector<T, N>& rhs) {
        Vector<T, N> result = *this;
        return (result += rhs);
    }

    Vector<T, N>& operator-=(const Vector<T, N>& rhs) {
        for (unsigned int i = 0; i < N; ++i)
            elements[i] = std::max(std::min(elements[i], elements[i] - rhs.elements[i]), 0);

        return *this;
    }

    Vector<T, N> operator-(const Vector<T, N>& rhs) {
        Vector<T, N> result = *this;
        return (result -= rhs);
    }

    void print() const {
        cout << "<";

        for (unsigned int i = 0; i < N; ++i)
            cout << setw(7) << elements[i];

        cout << ">" << endl;
    }

private:
    T elements[N];
};


int main() {
    int count = 0;

    Vector<int, 12> v[12] = 0;
    Vector<int, 12> x = 0;

    for (int i = 0; i < 12; ++i)
        for (int j = 0; j < 12; ++j)
            v[i][j] = ((j + i + 1) > 12) ? (j + i + 1) - 12 : (j + i + 1);

    cout <<"-------------------------------------------------------------------------" << endl;

    for (int i = 0; i < 12; ++i) {
        cout << "v" << setw(2) << setfill('0') << i << setfill(' ') << " = ";
        v[i].print();
    }

    while (x.min() < 60 * 60) {
        cout << ++count << "-------------------------------------------------------------------------" << endl;

        int bestIndex = 0;
        int bestMin = 0;

        for (int i = 0; i < 12; ++i) {
            int tmpMin = Vector<int, 12>::min(x + v[i]);

            if (bestMin < tmpMin) {
                bestMin = tmpMin;
                bestIndex = i;
            }
        }

        cout << "+v" << setw(2) << setfill('0') << bestIndex << setfill(' ') << " = ";
        v[bestIndex].print();

        x += v[bestIndex];

        cout << " x   = ";
        x.print();
    }

    cout <<"-------------------------------------------------------------------------" << endl;

    return 0;
}

