//
// Created by cmo on 2023/10/31.
//

#ifndef TRIDENTDP_GROUPROOTTRANS_HH
#define TRIDENTDP_GROUPROOTTRANS_HH
#include <iostream>
#include <TFile.h>
#include <TTree.h>

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
    TTree* t_waveform;
    TTree* t_hits;

    // data branches
    int run_id{0};
    uint64_t ch_id{0};
    float t_start{0};
    uint32_t n_samp{0};
    std::vector<uint16_t> adc_val{};

};


#endif //TRIDENTDP_GROUPROOTTRANS_HH
