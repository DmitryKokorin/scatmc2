#pragma once

#include <stdint.h>
#include <iostream>

#include "common.h"


class IntensityContribution
{
public:

    IntensityContribution(const int phiSize, const int thetaSize);
    IntensityContribution(const IntensityContribution &);
    IntensityContribution &operator=(const IntensityContribution &);

    ~IntensityContribution();

    IntensityContribution& operator+=(const IntensityContribution& rhv);
    void clear();

    friend std::ostream &operator<<(std::ostream &stream,
                                    const IntensityContribution &ic);

    inline Float **data()        { return data_; }
    inline int phiSize()   const { return phiSize_; }
    inline int thetaSize() const { return thetaSize_; }

private:


    int phiSize_;
    int thetaSize_;

    Float **data_;
};

