//
// Created by cmo on 2023/11/20.
//

#ifndef TRIDENTDP_EVENT_HH
#define TRIDENTDP_EVENT_HH

#include <vector>
#include <queue>
#include "data_structure/trident_ds.hh"

// One event is defined by a time window with length 20us.

struct SingleHit{
    float hit_t;
    uint64_t ch_id;
};



class Event {
public:
    int GetTriggerLevel() {return trigger_level;}
    float GetStartTime() {return start_t;}
    const std::vector<SingleHit>& GetHits() {return hits;}
    const std::vector<RawSegment*>& GetRawSegments() {return segments;}
    void AddSegment(RawSegment* seg){ segments.push_back(seg); }

    /*!
     * Reconstruct hit based on a RawSegment (waveform).
     * @param seg: RawSegment to be reconstructed.
     * @return: Number of reconstructed hits.
     */
    int ReconHits(RawSegment* seg);


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
    bool LocalCoincidenceTrigger(SingleHit *p_hits_begin, ulong hits_len, int num_hits_required,
                                 float time_window, float r_min, float r_max, bool dom_level_trig = false) const;

    /*!
     * Execute trigger flow. The later trigger should be restrict tighter than the earlier one.
     * @return: trigger_level.
     */
    int TriggerFlow();

private:
    float baseline_time_window = 20;
    int baseline_nhits = 2;
    float baseline_r_min = 1, baseline_r_max = 100;
    int trigger_level{0};
    float start_t{0};
    std::vector<SingleHit> hits;
    std::vector<RawSegment*> segments;
};


#endif //TRIDENTDP_EVENT_HH
