//
// Created by cmo on 2023/11/27.
//

#ifndef TRIDENTDP_CONSTANT_HH
#define TRIDENTDP_CONSTANT_HH

const float sampling_frequency = 5e8;
const float time_per_sample = 1e9 / 5e8; //unit: ns
const int num_samples_per_batch = 500;
const float time_window_per_batch = num_samples_per_batch * time_per_sample;

#endif //TRIDENTDP_CONSTANT_HH
