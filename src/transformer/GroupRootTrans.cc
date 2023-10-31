//
// Created by cmo on 2023/10/31.
//

#include "transformer/GroupRootTrans.hh"


GroupRootTrans::GroupRootTrans(std::string  ofname)
    : _ofname{std::move(ofname)} {
    out_file = new TFile(_ofname.c_str(), "RECREATE");
    t_waveform = new TTree("Waveform", "Waveform");

    t_waveform->Branch("RunId", &run_id, "RunId/I");
    t_waveform->Branch("ChId", &ch_id, "ChId/L");
    t_waveform->Branch("t0", &t_start, "t0/F");
    t_waveform->Branch("n_samp", &n_samp, "n_samp/I");
    t_waveform->Branch("adc_val", &adc_val);
}

void GroupRootTrans::ReadData(const GroupData &data) {
    run_id = (int) data.runNumber;
    // skip empty events
    if (data.segments.empty())
        return;
    ClearData();

    for(auto &seg: data.segments){
        ch_id = seg.channelNumber;
        t_start = 2e-9f * (float) seg.startTime;
        n_samp = seg.sampleSize;
        adc_val = seg.adcValue;
        t_waveform->Fill();
    }
}

void GroupRootTrans::Write() {
    if(out_file != nullptr) {
        out_file->Write();
    }
}

GroupRootTrans::~GroupRootTrans() {
    delete t_waveform;
    delete out_file;
}

void GroupRootTrans::ClearData() {
    run_id = 0;
    ch_id = 0;
    t_start = 0;
    n_samp = 0;
    adc_val.clear();
}
