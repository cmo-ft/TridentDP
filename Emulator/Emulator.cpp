#include <iostream>
#include <TTree.h>
#include <TFile.h>
#include <unordered_map>

#include "WaveformSimulator.hh"
#include <iostream>
#include <fstream>

using std::vector;
using std::ofstream;
using std::ios;
using std::unordered_map;

int main(int argc, char** argv){
    TFile* infile;
    // FILE* outfile;
    std::string waveform_path;
    if(argc != 4){
        std::cerr << "Wrong input arguments! " <<
                  "Please input: Emulator [data_path] [out_path] [waveform_data_path]" << std::endl;
        return -1;
    }

    infile =  new TFile(argv[1]);
    // outfile = fopen(argv[2], "w");
    ofstream outfile(argv[2], ios::out | ios::binary);
    waveform_path = argv[3];

    const double time_per_bin = 2;
    const double time_window = 5000; // ns
    const unsigned int protocol = 0xb2;
    const double mV2ADC = (pow(2,14)-1)*1000/2.16;

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


        size_t evt_size = 0;
        unordered_map<int, vector<double>> channel_id_voltage;
        for(size_t ihit=0; ihit<pmt_hit_t->size(); ihit++){
            // int dom_id = (*pmt_dom_id)[ihit];
            // channel_id = dom_id * 100 + pmt_id
            int channel_id = (*pmt_dom_id)[ihit]*100 + (*pmt_pmt_id)[ihit];
            float t = (*pmt_hit_t)[ihit];
            if(channel_id_voltage.find(channel_id) == channel_id_voltage.end()){
                channel_id_voltage[channel_id] = WaveformSimulator::GenerateBaseline((int) (time_window / time_per_bin) );
                // each dom contains channel_id_voltage[dom_id].size() + 1 data. +1 since one data is for DomId
                evt_size += channel_id_voltage[channel_id].size() + 1;
            }
            wf_simulator.AddSinglePhoton(channel_id_voltage[channel_id], t, time_per_bin);
        }

        // Check threshold for each waveform
        for(auto &iter: channel_id_voltage) {
            vector<double> &v_voltage = iter.second;
            WaveformSimulator::CheckThreshold(v_voltage);
        }

        // write event to file
        // format: protocol, channel_id, event_id (time stamp), v0, v1, ..., v2499
        unsigned int header{0};
        for(auto &iter: channel_id_voltage){
            // outfile << protocol << " "; //<< evt_size << " " << channel_id_voltage.size() << " ";
            // outfile.write((char*)&protocol, 1);
            // Size for current block:  size(adv_value)  4(header)  4 (ch_id)  8(t_start)  8(t_busy)
            auto block_size = iter.second.size() * 2 + 4 + 4 + 8 + 8;
            // std::cout<<iter.second.size()<<std::endl;
            // std::cout<<bloc_size<<std::endl;
            // outfile.write((char*)&bloc_size, 3);

            // modify header into the format that is readable by raw_reader
            // block_size = 302 * 2 + 24;
            header = (protocol<<24);
            header = (header & 0xFF000000) | block_size;
            outfile.write((char*)&header, 4);

            vector<double>& v_voltage = iter.second;
            // Write channel_id and start time
            // board id
            outfile.write((char*)&(iter.first), 4);
            unsigned long t_begin = ientry + 10;
            outfile.write((char*)&t_begin, 8);

            unsigned long tbusy{0};
            outfile.write((char*)&tbusy, 8);

            // Write Voltage
            for(auto &vol: v_voltage){
                // outfile << vol << " ";
                unsigned short adc_val = (short) (vol * mV2ADC);
                // unsigned short adc_val = 8555;

                outfile.write((char*)&adc_val, 2);
            }
            v_voltage.clear();
        }
    }
    outfile.close();

    return 0;
}