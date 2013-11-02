#pragma once

#include <string>
#include <cstdio>
#include "mathcompat.h"

#include "optics.h"
#include "indicatrix.h"
#include "freepath.h"
#include "spherical.h"


class EscFunction
{
public:

    EscFunction();
    ~EscFunction();

    template <class T>
    bool create(const LinearInterpolation& FreePathO,
                const LinearInterpolation& FreePAthE,
                const ULong thetaSize,
                const ULong phiSize,
                const ULong zSize,
                const Float maxZ);

    Float operator()(const Float theta, const Float phi, const Float z) const;

    bool load(const std::string& name);
    bool save(const std::string& name);


private:

    void recalcSteps();

    Float m_maxZ;

    ULong m_thetaSize;
    ULong m_phiSize;
    ULong m_zSize;

    Float m_thetaStep;
    Float m_phiStep;
    Float m_zStep;

    Float ***m_array;

    EscFunction(const EscFunction&);
    EscFunction& operator=(const EscFunction&);
};

namespace escfunction {

template <class T1, class T2>
class Functor
{
public:

    Functor(T1& indO_, T2& indE_, const Float z_,
            const LinearInterpolation& FreePathO_,
            const LinearInterpolation& FreePathE_)
        : indO(indO_)
        , indE(indE_)
        , z(z_)
        , FreePathO(FreePathO_)
        , FreePathE(FreePathE_)
    {}

    inline Float operator()(const Vector3 &_s)
    {
        if (std::abs(_s.z()) < kMachineEpsilon)
            return 0.;

        Float dist = z / _s.z();
        Float theta = symmetrizeTheta(Angle(_s, Optics::director).theta);

        return   indO(_s)*exp(-dist/FreePathO(theta)) +
               + indE(_s)*exp(-dist/FreePathE(theta));
    }

protected:

    T1& indO;
    T2& indE;

    Float z;

    const LinearInterpolation& FreePathO;
    const LinearInterpolation& FreePathE;
};

template <class T1, class T2>
class FunctorNorm
{
public:

    FunctorNorm(T1& indO_, T2& indE_) :
        indO(indO_),
        indE(indE_)
    {}

    inline Float operator()(const Vector3 &_s)
    {
        return indO(_s) + indE(_s);
    }

protected:

    T1& indO;
    T2& indE;
};




} // namespace 



template <class T>
bool EscFunction::create(   const LinearInterpolation& oFreePath,
                            const LinearInterpolation& eFreePath,
                            const ULong thetaSize,
                            const ULong phiSize,
                            const ULong zSize,
                            const Float maxZ)
{

    typedef Indicatrix<T, Optics::OBeam> IndicatrixO;
    typedef Indicatrix<T, Optics::EBeam> IndicatrixE;


    m_array = allocate3dArray<Float>(zSize, phiSize, thetaSize);

    fprintf(stderr, "calculating escape function\n");

    m_maxZ      = maxZ;
    m_thetaSize = thetaSize;
    m_phiSize   = phiSize;
    m_zSize     = zSize;

    recalcSteps();

    #pragma omp parallel for
    for (int i = 0; i < (int) thetaSize; ++i) {

        Float t_i = i*m_thetaStep;
        Float cost_i = cos(t_i);
        Float sint_i = sin(t_i);

        for (ULong j = 0; j < phiSize; ++j) {

            Float p_i = j*m_phiStep;

            Vector3 s_i = Vector3(sint_i*cos(p_i), sint_i*sin(p_i), cost_i);
            IndicatrixO indO(s_i, Optics::director);
            IndicatrixE indE(s_i, Optics::director);


            for (ULong k = 0; k < zSize; ++k) {

                Float z = k*m_zStep;

                using namespace escfunction;
                using namespace spherical;

                Functor<IndicatrixO, IndicatrixE>     functor(indO, indE, z,
                                                        oFreePath, eFreePath);

                FunctorNorm<IndicatrixO, IndicatrixE> functorNorm(indO, indE);

                Float norm = integral(functorNorm, 1e-5);
                Float res = k > 0 ? integral(functor, 1e-5, 0.5*M_PI, M_PI)
                                  : 0.5*norm;

                m_array[k][j][i] = res / norm;

                fprintf(stderr, "k=%lu\n", k);
            }

            fprintf(stderr, "%lu\t%lu\n", (ULong)i, j);
        }
    }

    return true;
}
