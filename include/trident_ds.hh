//
// Created by cmo on 2023/10/31.
//

#ifndef TRIDENTDP_TRIDENT_DS_HH
#define TRIDENTDP_TRIDENT_DS_HH

#include <bs3/pbs.hh>
#include <cstdint>
#include <map>
#include <string>
#include <vector>
#include "trident_ds-generated.hh"

PBSF_DECLARE_REALM(TridentRealm, 2023, PBSF_REGISTER_TYPE(1, RawData),
                   PBSF_REGISTER_TYPE(2, GroupData),
                   PBSF_REGISTER_TYPE(100, EventNumber),
                   PBSF_REGISTER_TYPE(200, uint32_t));

#endif //TRIDENTDP_TRIDENT_DS_HH
