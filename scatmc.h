#pragma once

#include <string>
#include <vector>

#include "databuff.h"
#include "interval.h"
#include "distance.h"
#include "partition.h"
#include "escfunction.h"
#include "idxnorm.h"


class Photon;


class ScatMCApp
{
public:

    ScatMCApp();

    int  run();
    bool getOpts(int argc, char ** argv);
    void printHelp();

    //random generator seeds in threads differ by this number
    static const int   kSeedIncrement = 1000; 


private:

    enum FileOptions { Create, Load, Save };

    struct Options
    {
        Options()
          : execName()
          , workDir()
          , eeProfileOptions(Create)
          , oeProfileOptions(Create)
          , eoProfileOptions(Create)
          , oFreePathOptions(Create)
          , eFreePathOptions(Create)
          , eeProbabilityOptions(Create)
          , eEscFunctionOptions(Create)
          , oEscFunctionOptions(Create)
          , oNormOptions(Create)
          , eNormOptions(Create)
          , eeProfileName()
          , oeProfileName()
          , eoProfileName()
          , oFreePathName()
          , eFreePathName()
          , eeProbabilityName()
          , eEscFunctionName()
          , oEscFunctionName()
          , oNormName()
          , eNormName()
          , seed(1000)
          , maxPhotons(1000)
          , maxScatterings(1000)
          , phiSize(100)
          , thetaSize(100)
          , escPhiSize(100)
          , escThetaSize(100)
          , escZSize(100)
          , escMaxZ(1e-4)
          , maxTheta(1e-4)
        {}

        std::string execName;
        std::string workDir;

        FileOptions eeProfileOptions;
        FileOptions oeProfileOptions;
        FileOptions eoProfileOptions;
        FileOptions oFreePathOptions;
        FileOptions eFreePathOptions;
        FileOptions eeProbabilityOptions;
        FileOptions eEscFunctionOptions;
        FileOptions oEscFunctionOptions;
        FileOptions oNormOptions;
        FileOptions eNormOptions;
        std::string eeProfileName;
        std::string oeProfileName;
        std::string eoProfileName;
        std::string oFreePathName;
        std::string eFreePathName;
        std::string eeProbabilityName;
        std::string eEscFunctionName;
        std::string oEscFunctionName;
        std::string oNormName;
        std::string eNormName;

        int     seed;
        int64_t maxPhotons;
        int64_t maxScatterings;
        int     phiSize;
        int     thetaSize;
        int     escPhiSize;
        int     escThetaSize;
        int     escZSize;
        Float   escMaxZ;
        Float   maxTheta;
    };

    Options options_;

    void flushBuffers(int64_t &scatteredCount, DataBuff &buff);
    void output();
    void processScattering(const Photon& ph, DataBuff& buff);


    template <typename T>
    bool  prepareFreePath(LinearInterpolation &l,
                         const char *name,
                         const int options,
                         const std::string &fileName);

    template <typename T>
    bool  prepareNorm(LinearInterpolation &l,
                      const char *name,
                      const int options,
                      const std::string &fileName);

    bool  prepareEChannelProb(LinearInterpolation& prob);

    template <typename T>
    bool  preparePartition(Partition &p,
                           const char *name,
                           const int options,
                           const std::string &fileName);

    template <typename T>
    bool  prepareEscFunction(EscFunction &e,
                         const char *name,
                         const int options,
                         const std::string &fileName);


    LinearInterpolation m_eLength;
    LinearInterpolation m_oLength;

    LinearInterpolation m_eChannelProb;

    EscFunction m_eEscFunction;
    EscFunction m_oEscFunction;

    LinearInterpolation m_eNorm;
    LinearInterpolation m_oNorm;

    int64_t m_photonCnt;
    int64_t m_saveRate;

    DataBuff *m_dataBuff;

private:

    ScatMCApp& operator=(const ScatMCApp& other);
    ScatMCApp(const ScatMCApp& other);
};

template <typename T>
bool ScatMCApp::prepareFreePath(LinearInterpolation &l,
                                const char *name,
                                const int options,
                                const std::string &fileName)
{
    using namespace std;

    if (options == Load) {

        cerr << "loading " << name << "-beam free path file..." << endl;

        if (!l.load(fileName)) {

            cerr << "can't load " << name << "-beam free path data" << endl;
            return false;
        }
    }

    if (options == Create || options == Save) {

        cerr << "calculating " << name << "-beam free path data..." << endl;
        createFreePath<T>(l);
    }

    if (options == Save) {

        cerr << "saving " << name << "-beam free path data to file..." << endl;

        if (!l.save(fileName)) {

            cerr << "can't save " << name << "-beam free path data" << endl;
            return false;
        }
    }

    cerr << "done" << endl;

    return true;
}

template <typename T>
bool ScatMCApp::prepareNorm(LinearInterpolation &l,
                            const char *name,
                            const int options,
                            const std::string &fileName)
{
    using namespace std;

    if (options == Load) {

        cerr << "loading " << name << "-beam indicatrix norm file..." << endl;

        if (!l.load(fileName)) {

            cerr << "can't load " << name << "-beam indicatrix data" << endl;
            return false;
        }
    }

    if (options == Create || options == Save) {

        cerr << "calculating " << name << "-beam free path data..." << endl;
        createNorm<T>(l);
    }

    if (options == Save) {

        cerr << "saving " << name << "-beam indicatrix norm data to file..." << endl;

        if (!l.save(fileName)) {

            cerr << "can't save " << name << "-beam indicatrix norm data" << endl;
            return false;
        }
    }

    cerr << "done" << endl;

    return true;
}



template <typename T>
bool ScatMCApp::prepareEscFunction(EscFunction &e,
                                   const char *name,
                                   const int options,
                                   const std::string &fileName)
{
    using namespace std;

    if (options == Load) {

        cerr << "loading " << name << "-beam escape function file..." << endl;

        if (!e.load(fileName)) {

            cerr << "can't load " << name << "-beam escape function" << endl;
            return false;
        }
    }

    if (options == Create || options == Save) {

        cerr << "calculating " << name << "-beam escape function data..." << endl;
        e.create<T>(m_oLength, m_eLength,
                options_.escPhiSize, options_.escThetaSize,
                options_.escZSize, options_.escMaxZ);
    }

    if (options == Save) {

        cerr << "saving " << name << "-beam escape function to file..." << endl;

        if (!e.save(fileName)) {

            cerr << "can't save " << name << "-beam escape function" << endl;
            return false;
        }
    }

    cerr << "done" << endl;

    return true;
}


template <typename T>
bool ScatMCApp::preparePartition(Partition &p,
                                 const char *name,
                                 const int options,
                                 const std::string &fileName)
{
    using namespace std;

    if (options == Load) {

        cerr << "loading " << name << " profile file..." << endl;

        if (!p.load(fileName)) {

            cerr << "can't load " << name << " profile data" << endl;
            return false;
        }
    }

    if (options == Create || options == Save) {

        typedef std::vector<Interval>  IntervalsContainer;
        IntervalsContainer intervals;


        cerr << "calculating profile intervals..." << endl;

        Float left = 0.01;
        Float right = 0.5* M_PI;

        //const Float tolerance = 0.1;
        const Float tolerance = 0.01;
        //const Float tolerance = 0.001;

        Float dist;
        int i = 0;

        intervals.push_back(Interval(0., left));

#if !defined TEST
        int num_threads = omp_get_max_threads();
#else
        int num_threads = 1;
#endif

        #pragma omp parallel for
        for (int j = 0; j < num_threads; ++j) {

            Float local_left = left + j/(Float)num_threads*(right-left);
            Float local_right = left + (j+1)/(Float)num_threads*(right-left);
            Float local_end = local_right;

            do {

                local_right = local_end;

                T ind_left = createIndicatrix<T>(local_left);

                while (1) {

                    T ind_right  = createIndicatrix<T>(local_right);

                    dist = ::distance(ind_left, ind_right, 0.01*tolerance);

                    if (dist > tolerance)
                        local_right = 0.5*(local_right + local_left);
                    else
                        break;

                                    }

                #pragma omp critical
                {
                    intervals.push_back(Interval(local_left, local_right));
                    std::cout << i << " "
                              << local_left << " "
                              << local_right << std::endl;

                    i++;
                }

                local_left = local_right;

            }
            while (local_right != local_end);
        }

        std::sort(intervals.begin(), intervals.end());

        cerr << "calculating " << name << " profile data..." << endl;

        const int chunksNum = intervals.size();
//#if !defined TEST
        #pragma omp parallel for schedule(dynamic)
//#endif
        for (int i = 0; i < chunksNum; ++i) {

              p.addChunk<T>(intervals[i].left, intervals[i].right);
        }
    }

    if (options == Save) {

        cerr << "saving " << name << " profile data to file..." << endl;

        if (!p.save(fileName)) {

            cerr << "can't save " << name << " profile data" << endl;
            return false;
        }
    }

    cerr << "done" << endl;

    return true;
}
