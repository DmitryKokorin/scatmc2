#pragma once

#include <ostream>
#include <map>
#include <vector>

#include "intensity_contribution.h"



class DataBuff
{
public:

    typedef std::vector<int>                     OrdersContainer;
    typedef std::map<int, IntensityContribution> Contributions;

    DataBuff(const int phiSize, const int thetaSize,
            const OrdersContainer &orders = OrdersContainer());

    void clear();

    DataBuff& operator+=(const DataBuff &other);
    void append(const IntensityContribution &ic, const int order);

    friend std::ostream &operator<< (std::ostream &stream,
                                     const DataBuff &buff);
    
private:

    IntensityContribution totalContribution;
    Contributions         orderContributions;
};
