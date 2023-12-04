//
// Created by cmo on 2023/11/20.
//

#include <cmath>
#include "constant.hh"
#include "data_structure/Event.hh"

using std::vector;
using std::queue;

Event::Event(float start_t): start_t(start_t) {
    RegisterTrigger(4, 20, 1, 300, false);
}


float Event::AddSegment(const RawSegment *seg) {
    float total_npe = 0.;
    auto& adc_val = seg->adcValue;
    float seg_start_t = (float) seg->startTime * time_per_sample;

    // TODO: Find the right way to calculate baseline.
    ushort baseline_value = 8620;
    // TODO: Decide the suitable threshold & single_hit_integral
    float threshold = 1.5, single_hit_integral = 17.;

    // vector<float> adc_voltage(adc_val.size());
    // for (ulong i_val=0; i_val < adc_val.size(); i_val++){
    //     adc_voltage[i_val] = (float) (baseline_value - adc_val[i_val] / (pow(2, 14) - 1) * 2.16 * 1000);
    // }
    waveforms.push_back({seg_start_t, {}, seg->channelNumber});
    auto adc_voltage = waveforms[waveforms.size()-1].adc_voltage;
    for (int i_val=0; i_val < num_samples_per_batch; i_val++){
        adc_voltage[i_val] =  (float) ((float) (baseline_value - adc_val[i_val]) / (pow(2, 14) - 1) * 2.16 * 1000);
    }

    // Both a flag shows if there are hits in current i_val and a value records the hit-start time.
    int hit_start_idx = -1;
    // Sum of adc_voltage for a triggered bump.
    float integral = 0, peak_height = 0;
    // for(ulong i_val=0; i_val < adc_voltage.size() - 3; i_val++){
    for (int i_val=0; i_val < num_samples_per_batch - 3; i_val++){
        if (hit_start_idx != -1){
            integral += adc_voltage[i_val];
            peak_height = peak_height>adc_voltage[i_val]? peak_height : adc_voltage[i_val];
            if(adc_voltage[i_val] < threshold && adc_voltage[i_val + 1] < threshold && adc_voltage[i_val + 2] < threshold){
            // If there are three consecutive values below the threshold: triggering over

                // int hits_in_previous_bump = (int) std::round(integral/single_hit_integral);
                float npe = integral / single_hit_integral;

                total_npe += npe;
                // for(int i=0; i<hits_in_previous_bump; i++){
                //     hits.push_back({seg_start_t + (float )hit_start_idx*time_per_sample, seg->channelNumber});
                // }
                hits.push_back({seg_start_t+(float)hit_start_idx*time_per_sample,
                                (float)(i_val-hit_start_idx)*time_per_sample, peak_height, npe,
                                seg->channelNumber});
                hit_start_idx = -1;
            }
        } else if(adc_voltage[i_val] > threshold && adc_voltage[i_val + 1] > threshold && adc_voltage[i_val + 2] > threshold){
        // If there are three consecutive values over the threshold: triggering start
            hit_start_idx = (int) i_val;
            peak_height = adc_voltage[i_val];
        }
    }
    return total_npe;
}


void Event::RegisterTrigger(int num_hits_required, float time_window, float r_min, float r_max, bool dom_level_trig) {
    trigger_flow.push_back({num_hits_required, time_window, r_min, r_max, dom_level_trig});
}


bool Event::LocalCoincidenceTrigger(SingleHit *p_hits_begin, ulong hits_len, SingleTrigger &trigger) const{
    // if (time_window > baseline_time_window){
    //     std::cerr << "Time window of local coincidence trigger (" << time_window
    //               << "ns) should be smaller than the max_time_window (" << baseline_time_window << "ns)." << std::endl;
    //     throw std::exception();
    // }
    // q_hits.pop();
    return true;
}


int Event::TriggerFlow() {
    float current_tw = trigger_flow[0].time_window;
    ulong hits_begin=0, hits_len=0;
    while (hits_begin+hits_len<hits.size()) {
        // Collect all hits inside the current time window
        if (hits_len == 0 || hits[hits_begin + hits_len].start_t < hits[hits_begin].start_t + current_tw) {
            hits_len++;
        } else {

            if(LocalCoincidenceTrigger(&(hits[hits_begin]), hits_len, trigger_flow[trigger_level])){
                trigger_level++;

                if(trigger_level == (int) trigger_flow.size()){
                    break;
                }
            } else {
                hits_begin += hits_len + 1;
                hits_len = 0;
            }
        }
    }
    return trigger_level;
    // const int num_triggers = 2;
    // float current_tw=baseline_time_window, next_tw=0;
    // float current_r_min=baseline_r_min, current_r_max=baseline_r_max;
    // int num_hits_required=baseline_nhits;

    // bool flag_pass_highest_trigger = false;
    // ulong hits_begin=0, hits_len=0;
    // while (hits_begin+hits_len<hits.size()){
    //     // Collect all hits inside the current time window
    //     if (hits_len==0 || hits[hits_begin+hits_len].hit_t< hits[hits_begin].hit_t + current_tw){
    //         hits_len++;
    //     } else {
    //         switch (trigger_level) {
    //             case 0:
    //                 next_tw = 10;
    //                 break;
    //             case 1:
    //                 current_r_min = 1, current_r_max = 70;
    //                 current_tw = 10, next_tw = 0;
    //                 break;
    //             default:
    //                 flag_pass_highest_trigger = true;
    //         }
    //         if(LocalCoincidenceTrigger(&(hits[hits_begin]), hits_len, SingleTrigger())){
    //             trigger_level++;
    //             current_tw = next_tw;
    //                if(trigger_level == trigger_flow.size()){
    //                    break;
    //                }
    //         } else {
    //             hits_begin += hits_len + 1;
    //             hits_len = 0;
    //         }
    //     }
    // }
    // // initialize the coincident hit queue
    // queue<SingleHit*> q_coin_hits;
    // q_coin_hits.push(&hits[0]);
    //
    // bool flag_pass_highest_trigger = false;
    // for (auto & q_end_hit : hits){
    //     if (q_coin_hits.empty() || q_end_hit.hit_t < q_coin_hits.front()->hit_t + max_time_window){
    //         q_coin_hits.push(&q_end_hit);
    //     } else{
    //         // Execute coincidence trigger flow here.
    //         switch (trigger_level) {
    //             case 0:
    //                 LocalCoincidenceTrigger(q_coin_hits, 2, 0,
    //                                         0, 0, false);
    //                 trigger_level++;
    //                 break;
    //             default:
    //                 flag_pass_highest_trigger = true;
    //         }
    //         if (flag_pass_highest_trigger) break;
    //     }
    // }
    // return trigger_level;
}


