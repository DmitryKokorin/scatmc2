#pragma once

#include "spherical.h"
#include "indicatrix.h"


namespace idxnorm {

template <typename T>
class Functor
{
public:

    typedef Indicatrix<T, Optics::OBeam> IndicatrixO;
    typedef Indicatrix<T, Optics::EBeam> IndicatrixE;

    Functor(IndicatrixO &_indO, IndicatrixE &_indE)
      : indO_(_indO)
      , indE_(_indE)
    {}

    inline Float operator()(const Vector3 &s) const
    {
        return indO_(s) + indE_(s);
    }

private:

    IndicatrixO &indO_;
    IndicatrixE &indE_;
};

} //namespace



template <class T>
void createNorm(LinearInterpolation& li, const int kPoints = 1000)
{

    li.resize(kPoints);
    li.setRange(0., 0.5*M_PI);

    const Float kResolution = li.resolution();

    #pragma omp parallel for
    for (int i = 1; i < kPoints; ++i) {

        Float theta_i     = i*kResolution;
        Angle   a_i = Angle(theta_i);
        Vector3 s_i = createSomeDeviantVector(Optics::director, a_i).normalize();

        //create coordinate system

        Vector3 v2 = crossProduct(s_i, Optics::director).normalize();
        Vector3 v3 = crossProduct(v2, s_i);

        Matrix3 mtx = invert(createTransformMatrix(s_i, v2, v3));
        Vector3 nn = mtx*Optics::director;

        Indicatrix<T, Optics::OBeam> indO = Indicatrix<T, Optics::OBeam>(Vector3(1., 0., 0.), nn);
        Indicatrix<T, Optics::EBeam> indE = Indicatrix<T, Optics::EBeam>(Vector3(1., 0., 0.), nn);

        idxnorm::Functor<T> functor(indO, indE);
        li[i] = spherical::integral(functor);
    }

    li[0] = li[1];
}
