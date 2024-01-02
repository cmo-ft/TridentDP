//
// Created by cmo on 2023/11/20.
//

#ifndef TRIDENTDP_EVENT_HH
#define TRIDENTDP_EVENT_HH

#include <vector>
#include <queue>
#include "data_structure/trident_ds.hh"
#include "constant.hh"

// One event is defined by a time window with length 20us.

struct SingleHit{
    float start_t;
    float width_t;
    float peak_height;
    float np;
    uint64_t ch_id;
};

struct SingleWaveform{
    float start_t;
    float baseline_value;
    // std::vector<float> adc_voltage;
    float adc_voltage[num_samples_per_batch];
    uint64_t ch_id;
};

struct SingleTrigger{
    int num_hits_required = 4;
    float time_window = 20;
    float r_min = 1;
    float r_max = 300;
    bool DOM_level = false;
};


class Event {
public:
    Event(float start_t);
    int GetTriggerLevel() {return trigger_level;}
    float GetStartTime() {return start_t;}
    const std::vector<SingleHit>& GetHits() {return hits;}
    const std::vector<SingleWaveform>& GetWaveforms() {return waveforms;}
    // std::vector<const RawSegment *> GetRawSegments() {return segments;}
    // void AddSegment(const RawSegment *seg){ segments.push_back(seg); }

    /*!
     * Reconstruct hit based on a RawSegment (waveform).
     * @param seg: RawSegment to be reconstructed.
     * @return: Number of reconstructed hits.
     */
    float AddSegment(const RawSegment *seg);


    /*!
     * A template for local coincidence trigger.
     * @param p_hits_begin: a queue of hits to be checked.
     * @param num_hits_required: Coincidence level should be larger than this to pass the trigger.
     * @param time_window: time window for this trigger. It should be smaller than max_time_window. [ns]
     * @param r_min: Min radius to be considered.
     * @param r_max: Maximum radius to be considered.
     * @param dom_level_trig: trigger on dom level instead of hit level.
     * @return: bool value which illustrates if the hits have passed the trigger.
     */
    bool LocalCoincidenceTrigger(SingleHit *p_hits_begin, ulong hits_len, SingleTrigger &trigger) const;

    void RegisterTrigger(int num_hits_required, float time_window, float r_min, float r_max,
                         bool dom_level_trig = false);

    /*!
     * Execute trigger flow. The later trigger should be restrict tighter than the earlier one.
     * @return: trigger_level.
     */
    int TriggerFlow();

private:
    std::vector<SingleTrigger> trigger_flow;
    int trigger_level{0};
    float start_t{0};
    std::vector<SingleHit> hits;
    std::vector<SingleWaveform> waveforms;
    // std::vector<const RawSegment*> segments;
};


#endif //TRIDENTDP_EVENT_HH
