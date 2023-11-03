//
// Created by cmo on 2023/10/13.
//

#include <utility>

#include "WaveformSimulator.hh"

WaveformSimulator::WaveformSimulator(std::string wf_file): waveform_template_file(std::move(wf_file)){
    LoadWaveformTemplate();
}

void WaveformSimulator::LoadWaveformTemplate() {
    root_node = YAML::LoadFile(waveform_template_file);
    auto template_size = root_node["time"].size();

    // 2ns per bin
    wf_time_length = 2 * template_size;

    for(size_t i=0; i<template_size; i++){
        wf_time.push_back(root_node["time"][i].as<double>());
        wf_voltage.push_back(root_node["voltage"][i].as<double>() * -1);
    }

    pInterpWf = new ROOT::Math::Interpolator(wf_time, wf_voltage, ROOT::Math::Interpolation::kLINEAR);

}

void WaveformSimulator::AddSinglePhoton(std::vector<double> &v_voltage, double photon_hit_time, double time_per_bin) const {
    int photon_hit_bin = int(photon_hit_time / time_per_bin);
    int wf_length = int(wf_time_length / time_per_bin);
    size_t max_size = std::min(v_voltage.size(), (size_t) (photon_hit_bin + wf_length));

    for(size_t ibin=photon_hit_bin; ibin<max_size; ibin++){
        double t = (ibin+1)*time_per_bin - photon_hit_time;
        t = t<2? 2: t;
        v_voltage[ibin] += pInterpWf->Eval(t );
    }
}

std::vector<double> WaveformSimulator::GenerateBaseline(int num_bins, double std) {
    auto rnd = new TRandom3(0);
    std::vector<double> baseline(num_bins);
    for(int i=0; i<num_bins; i++){
        baseline[i] = rnd->Gaus(0, std);
    }
    return baseline;
}

void WaveformSimulator::AddVectorPhotons(std::vector<double> &v_voltage, std::vector<float> *photon_hit_time, double time_per_bin,
                                         bool threshold) const {
    for(auto& t: *photon_hit_time){
        AddSinglePhoton(v_voltage, t, time_per_bin);
    }

    if(threshold){
        CheckThreshold(v_voltage);
    }

}

void WaveformSimulator::CheckThreshold(std::vector<double> &v_voltage) {
    for(double & i : v_voltage){
        i = (i<20) ? i : 9079 / (1 + exp(i / 2194 * (-1))) - 4540;
        i = std::min(i, 2160.);
        i = std::max(i, -2160.);
    }
}
