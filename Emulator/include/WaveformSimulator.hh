//
// Created by cmo on 2023/10/13.
//

#ifndef HDOM_WAVEFORMSIMULATOR_HH
#define HDOM_WAVEFORMSIMULATOR_HH

#include "Math/Interpolator.h"
#include "TRandom3.h"
#include "yaml-cpp/yaml.h"

class WaveformSimulator {
public:
    explicit WaveformSimulator(std::string wf_file);
    static std::vector<double> GenerateBaseline(int num_bins, double std=0.31);
    void AddSinglePhoton(std::vector<double>& v_voltage, double photon_hit_time, double time_per_bin=2) const;
    void AddVectorPhotons(std::vector<double>& v_voltage,  std::vector<float> *photon_hit_time, double time_per_bin=2,
                          bool threshold=true) const;
    static void CheckThreshold(std::vector<double>& v_voltage);

    YAML::Node root_node;
    ROOT::Math::Interpolator* pInterpWf = nullptr;
    double wf_time_length{};

private:
    void LoadWaveformTemplate();

    std::string waveform_template_file;
    std::vector<double> wf_time;
    std::vector<double> wf_voltage;

};


#endif //HDOM_WAVEFORMSIMULATOR_HH
