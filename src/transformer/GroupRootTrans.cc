//
// Created by cmo on 2023/10/31.
//

#include "transformer/GroupRootTrans.hh"


GroupRootTrans::GroupRootTrans(std::string  ofname)
    : _ofname{std::move(ofname)} {
    out_file = new TFile(_ofname.c_str(), "RECREATE");

    // waveform tree
    t_waveforms = new TTree("Waveforms", "Waveforms");
    t_waveforms->Branch("RunId", &run_id, "RunId/I");
    t_waveforms->Branch("ChId", &ch_id, "ChId/L");
    t_waveforms->Branch("t0", &start_t, "t0/F");
    t_waveforms->Branch("NumSamples", &num_samples, "NumSamples/I");
    t_waveforms->Branch("vol", adc_voltage, "vol[NumSamples]/F");


    // hits tree
    t_hits = new TTree("Hits", "Hits");
    t_hits->Branch("RunId", &run_id, "RunId/I");
    t_hits->Branch("t0", &h_start_t, "t0/F");
    t_hits->Branch("tWidth", &width_t, "tWidth/F");
    t_hits->Branch("peak", &peak_height, "peak/F");
    t_hits->Branch("np", &np, "np/F");
    t_hits->Branch("ChId", &h_ch_id, "ChId/l");
}

void GroupRootTrans::ReadData(const GroupData &data) {
    // skip empty events
    if (data.segments.empty())
        return;
    ClearData();
    run_id = (int) data.runNumber;

    for (auto& seg: data.segments){
        auto cur_time = (float) seg.startTime;
        if (events.empty() || cur_time >= events.back().GetStartTime() + time_window_per_event){
            events.emplace_back(cur_time);
        }
        events.back().AddSegment(&seg);

        // if (events.empty()){
        //     events.emplace_back(cur_time);
        //     events.back().AddSegment(&seg);
        // } else if(cur_time < events[events.size()-1].GetStartTime() + time_window_per_event){
        //     std::cout<<cur_time<<std::endl;
        //     events.back().AddSegment(&seg);
        // } else{
        //     // Fill the tree and start a new event
        // }
    }

    for(auto& event: events){
        // retrieve waveform
        for(auto& wf: event.GetWaveforms()){
            ch_id = wf.ch_id;
            start_t = wf.start_t;
            std::copy(std::begin(wf.adc_voltage), std::end(wf.adc_voltage),
                      std::begin(adc_voltage));
            t_waveforms->Fill();
        }

        // retrieve hits
        for(auto& hit: event.GetHits()){
            h_start_t = hit.start_t;
            width_t = hit.width_t;
            peak_height = hit.peak_height;
            np = hit.np;
            h_ch_id = hit.ch_id;
            t_hits->Fill();
        }
    }
}

void GroupRootTrans::Write() {
    if(out_file != nullptr) {
        out_file->Write();
    }
}

GroupRootTrans::~GroupRootTrans() {
    delete t_waveforms;
    delete out_file;
}

void GroupRootTrans::ClearData() {
    run_id = 0;

    start_t = 0;
    ch_id = 0;
    num_samples = num_samples_per_batch;
    // for(float & i : adc_voltage)
    //     i = 0;
    memset(adc_voltage, 0, sizeof(adc_voltage));

    h_start_t = 0;
    width_t = 0;
    peak_height = 0;
    np = 0;
    h_ch_id = 0;

}
