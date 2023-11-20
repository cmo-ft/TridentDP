//
// Created by cmo on 2023/11/20.
//

#include <iostream>
#include "data_structure/Event.hh"

using std::vector;
using std::queue;


int Event::ReconHits(RawSegment* seg) {
    int num_hits = 0;
    auto& adc_val = seg->adcValue;
    for(ulong ival=0; ival<adc_val.size()-2; ival++){
        auto dy = adc_val[ival+1] - adc_val[ival];
        auto ddy = adc_val[ival] + adc_val[ival+2] - 2*adc_val[ival+1];
        if(dy>=0 && ddy>0){
            num_hits++;
            hits.push_back({static_cast<float>(ival), seg->channelNumber});
        }
    }
    return num_hits;
}


bool Event::LocalCoincidenceTrigger(SingleHit *p_hits_begin, ulong hits_len, int num_hits_required,
                                    float time_window, float r_min, float r_max, bool dom_level_trig) const{
    // if (time_window > baseline_time_window){
    //     std::cerr << "Time window of local coincidence trigger (" << time_window
    //               << "ns) should be smaller than the max_time_window (" << baseline_time_window << "ns)." << std::endl;
    //     throw std::exception();
    // }
    // q_hits.pop();
}


int Event::TriggerFlow() {
    float time_window=baseline_time_window, next_time_window=0;
    float r_min=baseline_r_min, r_max=baseline_r_max;
    int num_hits_required=baseline_nhits;

    bool flag_pass_highest_trigger = false;
    ulong hits_begin=0, hits_len=0;
    while (hits_begin+hits_len<hits.size()){
        if (hits_len==0 || hits[hits_begin+hits_len].hit_t< hits[hits_begin].hit_t + time_window){
            hits_len++;
        } else {
            switch (trigger_level) {
                case 0:
                    next_time_window = 10;
                    break;
                case 1:
                    r_min = 1, r_max = 70;
                    time_window = 10, next_time_window = 0;
                    break;
                default:
                    flag_pass_highest_trigger = true;
            }
            if( LocalCoincidenceTrigger(&(hits[hits_begin]), hits_len,
                                        num_hits_required, time_window, r_min, r_max) ){
                trigger_level++;
                time_window = next_time_window;
            } else if(flag_pass_highest_trigger){
                break;
            } else {
                hits_begin += hits_len + 1;
                hits_len = 0;
            }
        }
    }

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

