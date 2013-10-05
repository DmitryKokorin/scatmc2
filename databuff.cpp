#include "databuff.h"


DataBuff::DataBuff(const int phiSize, const int thetaSize,
                   const OrdersContainer &orders)
  : totalContribution(phiSize, thetaSize)
  , orderContributions()
{
    OrdersContainer::const_iterator i = orders.begin();
    for (; i != orders.end(); ++i) {

        orderContributions.insert(std::pair<int, IntensityContribution>(*i,
                             IntensityContribution(phiSize, thetaSize)));
    }
}

void DataBuff::clear()
{
    totalContribution.clear();

    Contributions::iterator i = orderContributions.begin();

    for (; i != orderContributions.end(); ++i)
        i->second.clear();
}

DataBuff& DataBuff::operator+=(const DataBuff &other)
{
    totalContribution += other.totalContribution;

    Contributions::iterator i = orderContributions.begin();
    Contributions::const_iterator j = other.orderContributions.begin();

    for (; i != orderContributions.end(); ++i, ++j)
        i->second += j->second;

    return *this;
}

void DataBuff::append(const IntensityContribution &ic, const int order)
{
    totalContribution += ic;

    Contributions::iterator i = orderContributions.find(order);
    if (i != orderContributions.end())
        i->second += ic;
}

std::ostream &operator<< (std::ostream &stream, const DataBuff &/*buff*/)
{
    return stream;
}

