#pragma once

#include <list>
#include <string>

#include "common.h"

class Data
{
public:

    static void setResolution(const int _phiSize,
                              const int _thetaSize,
                              const Float _thetaMax);

    Data();
    Data(const Data &other);
    Data &operator=(const Data &other);

    ~Data();

    Data& operator+=(const Data &_other);

    void clear();

    friend std::ostream &operator<<(std::ostream &, const Data &);

    Float **data;

    static int   phiSize;
    static int   thetaSize;
    static Float thetaMax;
    static Float phiStep;
    static Float thetaStep;
};

class Buffer
{
public:

    Buffer(const int order = 0);
    Buffer(const Buffer &other);
    Buffer &operator=(const Buffer &other);
    Buffer &operator+=(const Buffer &other);

    void clear();

    Data oLadder;
    Data oCyclic;
    Data eLadder;
    Data eCyclic;

    int order;
};

void outputBuffer(const Buffer &buff, const std::string &dir = "");
