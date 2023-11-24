//
// Created by cmo on 2023/11/20.
//

#include "data_structure/Event.hh"

using std::vector;
using std::queue;

Event::Event() {
    RegisterTrigger(4, 20, 1, 300, false);
}


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
}


int Event::TriggerFlow() {
    float current_tw = trigger_flow[0].time_window;
    ulong hits_begin=0, hits_len=0;
    while (hits_begin+hits_len<hits.size()) {
        // Collect all hits inside the current time window
        if (hits_len == 0 || hits[hits_begin + hits_len].hit_t < hits[hits_begin].hit_t + current_tw) {
            hits_len++;
        } else {

            if(LocalCoincidenceTrigger(&(hits[hits_begin]), hits_len, trigger_flow[trigger_level])){
                trigger_level++;

                if(trigger_level == trigger_flow.size()){
                    break;
                }
            } else {
                hits_begin += hits_len + 1;
                hits_len = 0;
            }
        }
    }
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


