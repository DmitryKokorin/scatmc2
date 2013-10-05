#include "intensity_contribution.h"


#include "common.h"
#include "string.h"


IntensityContribution::IntensityContribution(const int phiSize,
                                             const int thetaSize)
  : phiSize_(phiSize)
  , thetaSize_(thetaSize)
  , data_(0)
{
    data_ = allocate2dArray<Float>(phiSize_, thetaSize_);
    clear();
}

IntensityContribution::IntensityContribution(const IntensityContribution &other)
  : phiSize_(other.phiSize_)
  , thetaSize_(other.thetaSize_)
  , data_(0)
{
    data_ = allocate2dArray<Float>(phiSize_, thetaSize_);
    clear();

    *this += other;
}

IntensityContribution &
IntensityContribution::operator=(const IntensityContribution &other)
{
    free2dArray(data_);
    phiSize_ = other.phiSize_;
    thetaSize_ = other.thetaSize_;
    data_ = allocate2dArray<Float>(phiSize_, thetaSize_);
    clear();

    *this += other;

    return *this;
}


IntensityContribution::~IntensityContribution()
{
    free2dArray(data_);
}

void IntensityContribution::clear()
{
    memset(*data_, 0, phiSize_*thetaSize_*sizeof(Float));
}

IntensityContribution& IntensityContribution::
operator+=(const IntensityContribution& rhv)
{
    for (int i = 0; i < phiSize_*thetaSize_; ++i)
        *data_[i] += *rhv.data_[i];

    return *this;
}


std::ostream &operator<<(std::ostream &stream,
                                const IntensityContribution  &ic)
{
    for (int i = 0; i < ic.thetaSize_; ++i) {

        for (int j = 0; j < ic.phiSize_; ++j)
            stream << ic.data_[j][i] << '\t';

        stream << '\n';
    }

    return stream;
}
