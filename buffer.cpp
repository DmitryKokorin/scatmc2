#include "buffer.h"

#include <cstring>
#include <cmath>
#include <fstream>
#include <sstream>



int   Data::phiSize    = 1;
int   Data::thetaSize  = 1;
Float Data::thetaMax   = 0.;
Float Data::phiStep    = 2*M_PI / Data::phiSize;
Float Data::thetaStep  = Data::thetaMax / Data::thetaSize;


void Data::setResolution(const int _phiSize,
                         const int _thetaSize,
                         const Float _thetaMax)
{
    phiSize   = _phiSize;
    thetaSize = _thetaSize;
    thetaMax  = _thetaMax;

    phiStep   = 2*M_PI / Data::phiSize;
    thetaStep = thetaMax / thetaSize;
}

Data::Data()
  : data(0)
{
    data = allocate2dArray<Float>(phiSize, thetaSize);
    clear();
}

Data::Data(const Data& other)
  : data(0)
{
    data = allocate2dArray<Float>(phiSize, thetaSize);
    *this = other;
}

Data::~Data()
{
    free2dArray(data);
}

Data& Data::operator=(const Data &_other)
{
    memcpy(*data, *_other.data, sizeof(Float)*phiSize*thetaSize);

    return *this;
}

Data& Data::operator+=(const Data &_other)
{
    for (int i = 0; i < phiSize*thetaSize; ++i)
        (*data)[i] += (*_other.data)[i];

    return *this;
}

void Data::clear()
{
    memset(*data, 0, sizeof(Float)*phiSize*thetaSize);
}


std::ostream &operator<<(std::ostream &stream, const Data &data)
{
    const Float thetaStep = Data::thetaMax / Data::thetaSize;
    const Float phiStep   = 2*M_PI / Data::phiSize;

    stream.setf(std::ios::scientific);
    stream.precision(17);

    for (int i = 0; i < Data::thetaSize; ++i) {

        Float theta = i*thetaStep;

        for (int j = 0; j < Data::phiSize; ++j) {

            Float phi   = j*phiStep;
            stream << theta << '\t' << phi << '\t' << data.data[j][i] << '\n';
        }
    }

    return stream;
}


///////////////////////////////////////////////
// Buffer


Buffer::Buffer(const int _order)
  : oLadder()
  , oCyclic()
  , eLadder()
  , eCyclic()
  , order(_order)
  
{
}

Buffer::Buffer(const Buffer &other)
  : oLadder(other.oLadder)
  , oCyclic(other.oCyclic)
  , eLadder(other.eLadder)
  , eCyclic(other.eCyclic)
  , order(other.order)
{
}

Buffer &Buffer::operator=(const Buffer &other)
{
    oLadder = other.oLadder;
    oCyclic = other.oCyclic;
    eLadder = other.eLadder;
    eCyclic = other.eCyclic;

    return *this;
}

Buffer &Buffer::operator+=(const Buffer &other)
{
    oLadder += other.oLadder;
    oCyclic += other.oCyclic;
    eLadder += other.eLadder;
    eCyclic += other.eCyclic;

    return *this;
}

void Buffer::clear()
{
    oLadder.clear();
    oCyclic.clear();
    eLadder.clear();
    eCyclic.clear();
}


void outputBuffer(const Buffer &buff, const std::string &dir)
{
    std::stringstream ss;
    std::fstream      file;
    
    ss << dir << "/o_ladder_" << buff.order << ".txt";
    file.open(ss.str().c_str(), std::ios::out);
    file << buff.oLadder;
    file.close();
    ss.str("");

    ss << dir << "/o_cyclic_" << buff.order << ".txt";
    file.open(ss.str().c_str(), std::ios::out);
    file << buff.oCyclic;
    file.close();
    ss.str("");

    ss << dir << "/e_ladder_" << buff.order << ".txt";
    file.open(ss.str().c_str(), std::ios::out);
    file << buff.eLadder;
    file.close();
    ss.str("");

    ss << dir << "/e_cyclic_" << buff.order << ".txt";
    file.open(ss.str().c_str(), std::ios::out);
    file << buff.eCyclic;
    file.close();
}
