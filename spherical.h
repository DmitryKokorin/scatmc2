#pragma once

#include "integrate.h"

namespace spherical {

template <class T>
class PhiFunctor
{
public:

    PhiFunctor(T &func_,
               const Float sint_s_, const Float cost_s_) :
        func(func_),
        sint_s(sint_s_),
        cost_s(cost_s_)
    {}

    inline Float operator()(const Float phi)
    {
        Vector3 s_s = Vector3(sint_s*cos(phi), sint_s*sin(phi), cost_s);

        return  sint_s * func(s_s);
    }

protected:

    T &func;

    Float sint_s;
    Float cost_s;
};


template <class T>
class ThetaFunctor
{
public:

    ThetaFunctor(T &func_) :
        func(func_)
    {}

    inline Float operator()(const Float theta)
    {
        Float cost_s = cos(theta);
        Float sint_s = sin(theta);

        PhiFunctor<T> functor(func, sint_s, cost_s);
        return integrate::adaptive(functor, 0., 2.*M_PI);
    }

protected:

    T &func;
};

template <class T>
Float integral(T &func_,
               const Float tolerance_ = 1.0e-10,
               const Float minTheta_ = 0.,
               const Float maxTheta_ = M_PI)
{
    ThetaFunctor<T> thetaFunctor(func_);
    Float res = integrate::adaptive(thetaFunctor, minTheta_, maxTheta_, tolerance_);
    return res;
}

}
