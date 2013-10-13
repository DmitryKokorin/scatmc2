#include "scatmc.h"

#include "mathcompat.h"

#include <sstream>
#include <fstream>
#include <iostream>
#include <omp.h>


#include "common.h"

#include "channel.h"
#include "partition.h"
#include "photon.h"
#include "indicatrix.h"
#include "freepath.h"
#include "optics.h"
#include "distance.h"




int main(int argc, char ** argv)
{
    ScatMCApp app;

    int res = 0;

    if (app.getOpts(argc, argv)) {

        res = app.run();
    }
    else {

        app.printHelp();
        res = -1;
    }

    return res;
}


/////////////////////////////////////////////


ScatMCApp::ScatMCApp()
  : options_()
  , m_eLength()
  , m_oLength()
  , m_eChannelProb()
  , m_eEscFunction()
  , m_oEscFunction()
  , m_eNorm()
  , m_oNorm()
  , m_photonCnt(0)
  , m_saveRate(0)
  , m_buffers()
{
}

bool ScatMCApp::getOpts(int argc, char ** argv)
{
    options_.execName = argv[0];

    for (int i = 1; i < argc; ++i) {

        std::string arg = argv[i];

        if (arg == "--workdir") {

            if (++i == argc)
                return false;

            options_.workDir = argv[i];

            if (!options_.workDir.empty())
                options_.workDir += '/';
        }
        else if (arg == "--seed") {

            if (++i == argc)
                return false;

            std::stringstream stream(argv[i]);
            stream >> options_.seed;
        }
        else if (arg == "--photons") {

            if (++i == argc)
                return false;

            std::stringstream stream(argv[i]);
            stream >> options_.maxPhotons;
        }
        else if (arg == "--phisize") {

            if (++i == argc)
                return false;

            std::stringstream stream(argv[i]);
            stream >> options_.phiSize;
        }
        else if (arg == "--thetasize") {

            if (++i == argc)
                return false;

            std::stringstream stream(argv[i]);
            stream >> options_.thetaSize;
        }
        else if (arg == "--maxtheta") {

            if (++i == argc)
                return false;

            std::stringstream stream(argv[i]);
            stream >> options_.maxTheta;
        }
        else if (arg == "--escphisize") {

            if (++i == argc)
                return false;

            std::stringstream stream(argv[i]);
            stream >> options_.escPhiSize;
        }
        else if (arg == "--escthetasize") {

            if (++i == argc)
                return false;

            std::stringstream stream(argv[i]);
            stream >> options_.escThetaSize;
        }
        else if (arg == "--esczsize") {

            if (++i == argc)
                return false;

            std::stringstream stream(argv[i]);
            stream >> options_.escZSize;
        }
        else if (arg == "--escmaxz") {

            if (++i == argc)
                return false;

            std::stringstream stream(argv[i]);
            stream >> options_.escMaxZ;
        }
        else if (arg == "--scatterings") {

            if (++i == argc)
                return false;

            std::stringstream stream(argv[i]);
            stream >> options_.maxScatterings;
        }
        else if (arg == "--loadoeprofile") {

            if (++i == argc)
                return false;

            options_.oeProfileOptions = Load;
            options_.oeProfileName    = argv[i];
        }
        else if (arg == "--loadeoprofile") {

            if (++i == argc)
                return false;

            options_.eoProfileOptions = Load;
            options_.eoProfileName    = argv[i];
        }
        else if (arg == "--loadeeprofile") {

            if (++i == argc)
                return false;

            options_.eeProfileOptions = Load;
            options_.eeProfileName    = argv[i];
        }
        else if (arg == "--loadofreepath") {

            if (++i == argc)
                return false;

            options_.oFreePathOptions = Load;
            options_.oFreePathName    = argv[i];
        }
        else if (arg == "--loadefreepath") {

            if (++i == argc)
                return false;

            options_.eFreePathOptions = Load;
            options_.eFreePathName    = argv[i];
        }
        else if (arg == "--loadeeprobability") {

            if (++i == argc)
                return false;

            options_.eeProbabilityOptions = Load;
            options_.eeProbabilityName    = argv[i];
        }
        else if (arg == "--loadeescfunction") {

            if (++i == argc)
                return false;

            options_.eEscFunctionOptions = Load;
            options_.eEscFunctionName    = argv[i];
        }
        else if (arg == "--loadoescfunction") {

            if (++i == argc)
                return false;

            options_.oEscFunctionOptions = Load;
            options_.oEscFunctionName    = argv[i];
        }
        else if (arg == "--loadonorm") {

            if (++i == argc)
                return false;

            options_.oNormOptions = Load;
            options_.oNormName    = argv[i];
        }
        else if (arg == "--loadenorm") {

            if (++i == argc)
                return false;

            options_.eNormOptions = Load;
            options_.eNormName    = argv[i];
        }
        else if (arg == "--saveoeprofile") {

            if (++i == argc)
                return false;

            options_.oeProfileOptions = Save;
            options_.oeProfileName    = argv[i];
        }
        else if (arg == "--saveeoprofile") {

            if (++i == argc)
                return false;

            options_.eoProfileOptions = Save;
            options_.eoProfileName    = argv[i];
        }
        else if (arg == "--saveeeprofile") {

            if (++i == argc)
                return false;

            options_.eeProfileOptions = Save;
            options_.eeProfileName    = argv[i];
        }
        else if (arg == "--saveofreepath") {

            if (++i == argc)
                return false;

            options_.oFreePathOptions = Save;
            options_.oFreePathName    = argv[i];
        }
        else if (arg == "--saveefreepath") {

            if (++i == argc)
                return false;

            options_.eFreePathOptions = Save;
            options_.eFreePathName    = argv[i];
        }
        else if (arg == "--saveeeprobability") {

            if (++i == argc)
                return false;

            options_.eeProbabilityOptions = Save;
            options_.eeProbabilityName    = argv[i];
        }
        else if (arg == "--saveeescfunction") {

            if (++i == argc)
                return false;

            options_.eEscFunctionOptions = Save;
            options_.eEscFunctionName    = argv[i];
        }
        else if (arg == "--saveoescfunction") {

            if (++i == argc)
                return false;

            options_.oEscFunctionOptions = Save;
            options_.oEscFunctionName    = argv[i];
        }
        else if (arg == "--saveonorm") {

            if (++i == argc)
                return false;

            options_.oNormOptions = Save;
            options_.oNormName    = argv[i];
        }
        else if (arg == "--saveenorm") {

            if (++i == argc)
                return false;

            options_.eNormOptions = Save;
            options_.eNormName    = argv[i];
        }

        else
            return false;
    }

    return true;
}


int ScatMCApp::run()
{
    using namespace std;

    cout.precision(17);
    cout << scientific;

    cerr.precision(17);
    cerr << scientific;

    cerr << "# seed = "      << options_.seed       << endl;
    cerr << "# photons = "   << options_.maxPhotons << endl;
    cerr << "# H = "         << Optics::H           << endl;
    cerr << "# lambda = "    << Optics::lambda      << endl;
    cerr << "# thetaSize = " << options_.thetaSize  << endl;
    cerr << "# phiSize = "   << options_.phiSize    << endl;
    cerr << "# maxTheta = "  << options_.maxTheta   << endl;



    //TODO: get orders from parameters
    //allocate arrays for individual scattering orders data
    int orders[] = {0, 1, 2, 3, 4, 5, 10, 50, 100, 300, 500, 1000, \
                    3000, 5000, 10000, 30000, 50000, 100000};

    int ordersLength = sizeof(orders)/sizeof(orders[0]);

    for (int i = 0; i < ordersLength; ++i) {

        m_buffers.push_back(Buffer(orders[i]));
    }


#if !defined TEST
    //free path
    if (!prepareFreePath<Optics::OBeam>(m_oLength, "o",
                options_.oFreePathOptions, options_.oFreePathName)) {

        return -1;
    }
#endif

    if (!prepareFreePath<Optics::EBeam>(m_eLength, "e",
                options_.eFreePathOptions, options_.eFreePathName)) {

        return -1;
    }

    //e channel probability
    if (!prepareEChannelProb(m_eChannelProb))
        return -1;

Partition pOE, pEO, pEE;

#if !defined TEST
    if (!preparePartition<IndicatrixOE>(pOE, "o-e",
                options_.oeProfileOptions, options_.oeProfileName)) {

        return -1;
    }

    if (!preparePartition<IndicatrixEO>(pEO, "e-o",
                options_.eoProfileOptions, options_.eoProfileName)) {

        return -1;
    }
#endif

    if (!preparePartition<IndicatrixEE>(pEE, "e-e",
                options_.eeProfileOptions, options_.eeProfileName)) {

        return -1;
    }

 
#if !defined TEST
    //free path
    if (!prepareEscFunction<Optics::OBeam>(m_oEscFunction, "o",
                options_.oEscFunctionOptions, options_.oEscFunctionName)) {

        return -1;
    }
#endif

    if (!prepareEscFunction<Optics::EBeam>(m_eEscFunction, "e",
                options_.eEscFunctionOptions, options_.eEscFunctionName)) {

        return -1;
    }

#if !defined TEST
    //free path
    if (!prepareNorm<Optics::OBeam>(m_oNorm, "o",
                options_.oNormOptions, options_.oNormName)) {

        return -1;
    }
#endif

    if (!prepareNorm<Optics::EBeam>(m_eNorm, "e",
                options_.eNormOptions, options_.eNormName)) {

        return -1;
    }


    cerr << "scattering..." << endl;
    Photon::init(&m_oLength, &m_eLength, &pOE, &pEO, &pEE, &m_eChannelProb);

    const int64_t flushRate = 1000;
    m_saveRate  = omp_get_max_threads()*flushRate;

    const Float t = 0.5*M_PI; //angle with director
    const Vector3 initVector = Vector3(cos(t), 0, sin(t)).normalize();
    cerr << "initial angle: " << t << endl;


    Data::setResolution(options_.phiSize, options_.thetaSize, options_.maxTheta);


    //main loop

    #pragma omp parallel
    {
        RngEngine rng_engine;

        rng_engine.seed(options_.seed + kSeedIncrement*omp_get_thread_num());

        BuffersList buffers = m_buffers;

        int64_t scatteredCount = 0;

        #pragma omp for schedule (dynamic)
        for (int64_t i = 0; i < options_.maxPhotons; ++i) {

            Photon ph(rng_engine, initVector, Optics::ECHANNEL);

            while (   (ph.scatterings < options_.maxScatterings)
                   && (ph.weight > options_.minWeight)         ) {

                ph.move();

                if (Optics::OCHANNEL == ph.channel)
                    processScattering<Optics::OBeam>(ph, buffers);
                else
                    processScattering<Optics::EBeam>(ph, buffers);

                ph.scatter();

            }

            if (++scatteredCount == flushRate)
                flushBuffers(scatteredCount, buffers);
        }

        flushBuffers(scatteredCount, buffers);
    }


    output();
    return 0;
}

template <typename T>
void ScatMCApp::processScattering(const Photon &ph,
                                  BuffersList &buffers)
{
    Buffer buff(ph.scatterings);
    //calculate 2d-array of contributions
    
    Indicatrix<T, Optics::OBeam> indO(ph.s_i, Optics::director);
    Indicatrix<T, Optics::EBeam> indE(ph.s_i, Optics::director);

    Float otheta = symmetrizeTheta(Angle(ph.s_i, Optics::director).theta);
    Float norm = Optics::OCHANNEL == ph.channel ? m_oNorm(otheta)
                                                : m_eNorm(otheta);

    const Float thetaStep = Data::thetaMax / Data::thetaSize;
    const Float phiStep   = 2*M_PI / Data::phiSize;

    for (int i = 0; i < Data::thetaSize; ++i)
        for (int j = 0; j < Data::phiSize; ++j) {

            Float theta_s = i*thetaStep;
            Float phi_s   = j*phiStep;

            Float sintheta_s = sin(theta_s);
            Float costheta_s = cos(theta_s);

            Vector3 s_s = Vector3(sintheta_s*cos(phi_s),
                                  sintheta_s*sin(phi_s),
                                  -costheta_s);

            Angle a_s = Angle(s_s, Optics::director);

            Float dist   = std::abs(ph.pos.z() / s_s.z());
            Float x      = ph.pos.x() + dist*s_s.x();
            Float y      = ph.pos.y() + dist*s_s.y();

            Float symmetrizedTheta = symmetrizeTheta(a_s.theta);

            Float oLengthFactor = exp(-dist/m_oLength(symmetrizedTheta));
            Float eLengthFactor = exp(-dist/m_eLength(symmetrizedTheta));

            Vector3 R  = Vector3(x, y, 0);
            Vector3 qe = Optics::EBeam::k(s_s, a_s)*Optics::k0;
            Vector3 qo = Optics::OBeam::k(s_s, a_s)*Optics::k0;

            Float oProbFactor = indO(s_s)/norm;
            Float eProbFactor = indE(s_s)/norm;

            Float oLadderRes = ph.weight*(oLengthFactor*oProbFactor);
            Float oCyclicRes = oLadderRes*cos(qo*R);
            Float eLadderRes = ph.weight*(eLengthFactor*eProbFactor);
            Float eCyclicRes = eLadderRes*cos(qe*R);

            buff.oLadder.data[j][i] += oLadderRes;
            buff.oCyclic.data[j][i] += oCyclicRes;
            buff.eLadder.data[j][i] += eLadderRes;
            buff.oCyclic.data[j][i] += eCyclicRes;

        } 

    //append to buff

    BuffersList::iterator i = buffers.begin();
    for (; i != buffers.end(); ++i) {

        if (   i->order == buff.order
            || i->order == 0 ) {

            *i += buff;
        }
    }
}

void ScatMCApp::output()
{
    BuffersList::const_iterator i = m_buffers.begin();
    for (; i != m_buffers.end(); ++i) {

        outputBuffer(*i, options_.workDir);
    }
}

void ScatMCApp::printHelp()
{
    using namespace std;

    cerr 
    << "Usage: " << options_.execName << "[options]" << endl

    << "Available options:" << endl
    << "--workdir [path]\t\t\toutput path" << endl
    << "--seed [seed]\t\t\tseed for random numbers generator" << endl
    << "--photons [photons]\t\t\tnumber of photons to scatter" << endl
    << "--scatterings [scatterings]\t\t\tmax scatterings for each photon" << endl
    << "--maxtime [maxtime]\t\t\tmax scattering time for each photon" << endl
    << "--points [points]\t\t\tnumber of sampling points" << endl

    << "--loadoeprofile [filename]\t\tload o-e profile from file"  << endl
    << "--loadeoprofile [filename]\t\tload e-o profile from file"  << endl
    << "--loadeeprofile [filename]\t\tload e-e profile from file"  << endl
    << "--loadoefreepath [filename]\t\tload o-beam free path from file" << endl
    << "--loadeofreepath [filename]\t\tload e-beam free path from file" << endl
    << "--loadeeprobability [filename]\t\tload e-e probability from file" << endl

    << "--saveoeprofile [filename]\t\tsave o-e profile to file"    << endl
    << "--saveeoprofile [filename]\t\tsave e-o profile to file"    << endl
    << "--saveeeprofile [filename]\t\tsave e-e profile to file" << endl
    << "--saveoefreepath [filename]\t\tsave o-beam free path to file"  << endl
    << "--saveeofreepath [filename]\t\tsave e-beam free path to file"  << endl
    << "--saveeeprobability [filename]\t\tsave e-e probability to file"  << endl;
}

bool ScatMCApp::prepareEChannelProb(LinearInterpolation& l)
{
    using namespace std;

    if (options_.eeProbabilityOptions == Load) {

        cerr << "loading e-e probability file..." << endl;

        if (!l.load(options_.eeProbabilityName)) {

            cerr << "can't load e-e probability data" << endl;
            return false;
        }
    }

    if (   options_.eeProbabilityOptions == Create
        || options_.eeProbabilityOptions == Save) {

        cerr << "calculating e-e probability data..." << endl;
        createEChannelProb<Optics::EBeam>(l);
    }

    if (options_.eeProbabilityOptions == Save) {

        cerr << "saving e-e probability data to file..." << endl;

        if (!l.save(options_.eeProbabilityName)) {

            cerr << "can't save e-e probability data" << endl;
            return false;
        }
    }

    cerr << "done" << endl;

    return true;
}

void ScatMCApp::flushBuffers(int64_t &scatteredCount, BuffersList &buffers)
{
    #pragma omp critical
    {
        m_photonCnt += scatteredCount;

        BuffersList::iterator i = m_buffers.begin();
        BuffersList::iterator j = buffers.begin();

        for (; i != m_buffers.end(); ++i, ++j) {

            *i += *j;
            j->clear();
        }

        std::cerr << "Photons: " << m_photonCnt << std::endl;

        if (0 == m_photonCnt % m_saveRate)
            output();
    }

    scatteredCount = 0.;
}
