//
// Created by cmo on 2023/10/31.
//

#ifndef TRIDENTDP_GROUPROOTTRANS_HH
#define TRIDENTDP_GROUPROOTTRANS_HH
#include <iostream>
#include <TFile.h>
#include <TTree.h>

#include "constant.hh"
#include "data_structure/Event.hh"
#include "data_structure/trident_ds.hh"

class GroupRootTrans {
public:
    explicit GroupRootTrans(std::string ofname);
    ~GroupRootTrans();

    void ReadData(const GroupData &data);
    void Write();
    void ClearData();

private:
    std::string _ofname;

    // file and tree for output
    TFile* out_file;
    int run_id{0};

    // waveform tree
    TTree* t_waveforms;
    float start_t{0};
    uint64_t ch_id{0};
    int num_samples = num_samples_per_batch;
    float adc_voltage[num_samples_per_batch];

    // hits tree
    TTree* t_hits;
    float h_start_t;
    float width_t;
    float peak_height;
    float np;
    uint64_t h_ch_id;
    // // data branches
    // uint32_t n_samp{0};
    // std::vector<uint16_t> adc_val{};

    std::vector<Event> events{};

};


#endif //TRIDENTDP_GROUPROOTTRANS_HH
