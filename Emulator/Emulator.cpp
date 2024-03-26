#include <iostream>
#include <TTree.h>
#include <TFile.h>
#include <unordered_map>

#include "constant.hh"
#include "WaveformSimulator.hh"
#include "data_structure/Event.hh"
#include <utility>
#include <fstream>

using std::vector;
using std::ofstream;
using std::ios;
using std::unordered_map;
using std::map;
using std::pair;
using std::string;

int main(int argc, char** argv){
    TFile* infile;
    std::string waveform_path;
    if(argc < 4){
        std::cerr << "Wrong input arguments! " <<
                  "Please input: Emulator [data_path] [out_path] [waveform_data_path] [1 if ROOT as output]" << std::endl;
        return -1;
    }

    infile =  new TFile(argv[1]);
    waveform_path = argv[3];

    ofstream outfile_bin(argv[2], ios::out | ios::binary);

    // If save ROOT file
    TFile* outfile_root = nullptr;
    TTree* t_hits = nullptr;
    float h_start_t, width_t, peak_height, np;
    uint64_t h_ch_id;
    bool save_root = false;
    if (argc == 5 && string(argv[4]) == "1"){
        std::cout << "Will create ROOT file: " << argv[2] << ".root" << std::endl;
        save_root = true;
        outfile_root = new TFile(TString(argv[2]) + ".root", "RECREATE");
        // hits tree
        t_hits = new TTree("Hits", "Hits");
        t_hits->Branch("t0", &h_start_t, "t0/F");
        t_hits->Branch("tWidth", &width_t, "tWidth/F");
        t_hits->Branch("peak", &peak_height, "peak/F");
        t_hits->Branch("np", &np, "np/F");
        t_hits->Branch("ChId", &h_ch_id, "ChId/l");
    }

    const unsigned int protocol = 0xb2;
    auto get_adc_from_mV = [](double mV)->short {
        return (short) (8620 - mV*(pow(2,14)-1) / 1000 / 2.16);
    };

    // waveform simulator
    WaveformSimulator wf_simulator(waveform_path);
    vector<float> *pmt_hit_t= nullptr;
    vector<int> *pmt_dom_id = nullptr, *pmt_pmt_id = nullptr;

    auto pmt_tree = (TTree*) infile->Get("PmtHit");
    pmt_tree->SetBranchAddress("t0", &pmt_hit_t);
    pmt_tree->SetBranchAddress("DomId", &pmt_dom_id);
    pmt_tree->SetBranchAddress("PmtId", &pmt_pmt_id);

    for(int ientry=0; ientry<pmt_tree->GetEntries(); ientry++){
        pmt_tree->GetEntry(ientry);

        // Get the start time of the event: min hit T
        float t_start = *std::min_element(pmt_hit_t->begin(), pmt_hit_t->end());

        // unordered_map<int, vector<double>> channel_id_voltage;
        map<pair<int, int>, vector<double>> channel_id_voltage;
        for(size_t ihit=0; ihit<pmt_hit_t->size(); ihit++){
            int channel_id = (*pmt_dom_id)[ihit]*100 + (*pmt_pmt_id)[ihit];
            // t = time relative to t_start
            float t = (*pmt_hit_t)[ihit] - t_start;
            // id of the time window for this hit
            int t_window_id = (int) (t / time_window_per_batch);
            auto patch_index_of_current_hit = std::make_pair(t_window_id, channel_id);
            // if(channel_id_voltage.find(channel_id) == channel_id_voltage.end()){
            if(channel_id_voltage.find(patch_index_of_current_hit) == channel_id_voltage.end()){
                channel_id_voltage[patch_index_of_current_hit] = WaveformSimulator::GenerateBaseline(num_samples_per_batch );
            }
            // Photon hit time is relative to the current time window start time.
            wf_simulator.AddSinglePhoton(channel_id_voltage[patch_index_of_current_hit],
                                         t - time_window_per_batch*t_window_id, time_per_sample);
        }

        // Check threshold for each waveform
        for(auto &iter: channel_id_voltage) {
            vector<double> &v_voltage = iter.second;
            WaveformSimulator::CheckThreshold(v_voltage);
        }

        // Create event
        Event cur_event(0);

        // write event to file
        // write to binary
        // format: protocol, channel_id, event_id (time stamp), v0, v1, ..., v999
        unsigned int header{0};
        for (auto &iter: channel_id_voltage) {
            RawSegment cur_seg;

            auto block_size = iter.second.size() * 2 + 4 + 4 + 8 + 8;

            header = (protocol << 24);
            header = (header & 0xFF000000) | block_size;
            outfile_bin.write((char *) &header, 4);

            vector<double> &v_voltage = iter.second;
            // Write channel_id and start time
            // board id
            cur_seg.channelNumber = iter.first.second;
            outfile_bin.write((char *) &(iter.first.second), 4);
            unsigned long t_begin = ientry * 100000 + (long) (iter.first.first * time_window_per_batch);
            cur_seg.startTime = t_begin;
            outfile_bin.write((char *) &t_begin, 8);

            unsigned long tbusy{0};
            outfile_bin.write((char *) &tbusy, 8);

            // Write Voltage
            for (auto &vol: v_voltage) {
                // outfile_bin << vol << " ";
                unsigned short adc_val = get_adc_from_mV(vol);
                cur_seg.adcValue.emplace_back(adc_val);
                // unsigned short adc_val = 8555;

                outfile_bin.write((char *) &adc_val, 2);
            }
            v_voltage.clear();

            // Add segment to Event
            cur_event.AddSegment(&cur_seg);
        }

        if (save_root) {
            // retrieve hits
            for (auto &hit: cur_event.GetHits()) {
                h_start_t = hit.start_t;
                width_t = hit.width_t;
                peak_height = hit.peak_height;
                np = hit.np;
                h_ch_id = hit.ch_id;
                t_hits->Fill();
            }
        }
    }
    outfile_bin.close();

    if (save_root){
        outfile_root->Write();
        outfile_root->Close();
    }
    return 0;
}