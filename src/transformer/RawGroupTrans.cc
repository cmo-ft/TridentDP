//
// Created by cmo on 2023/10/31.
//

#include "transformer/RawGroupTrans.hh"

#include <x86intrin.h>

#include <iostream>

namespace pu = pbsu;

namespace raw_group_trans {
    using pbss::parse;

    // very frequently used, give it a short name
    // parse 4 bytes from s
    #define rw(s) parse<uint32_t>(s)

    bool check_magic(uint8_t magic, uint32_t word) {
        if ((word >> 24) == magic)
            return true;
        return false;
    }

    // extract size preceded by a 4-bit magic
    // if magic mismatch, return 0; this is because all sizes include this word
    // itself, so must be at least 1.  0 can also be used in bool check.
    uint32_t checked_extract_size(uint8_t magic, uint32_t word) {
        if ((word >> 24) == magic)
            return word & 0x00ffffff;
        return 0;
    }

    std::pair<uint32_t, uint32_t> block_size(uint32_t word) {
        // all we have now is type2
        auto size = 0u;
        size = checked_extract_size(0xb1, word);
        if (size != 0)
            return {size, 1};
        size = checked_extract_size(0xb2, word);
        if (size != 0)
            return {size, 2};
        return {0, 0};
    }

    uint64_t read_segtime(std::istream &s) {
        // Read 8 bytes and get time = time_high (4bytes) + time_low (4bytes)
        uint32_t word;
        word = rw(s);
        uint64_t low = word;
        word = rw(s);
        uint64_t high = word;
        return (high << 32) + low;
    }

    RawSegment read_segment(std::istream &&s, uint32_t size, uint32_t type) {
        /*
         * Get RawSegment from the data block.
         * Params:
         *      s: stream after the header
         *      size: size of whole data block
         *      type: 1 or 2
         * Return: RawSegment
         */
        // First 4 bytes: channel id
        uint32_t word;
        word = rw(s);
        auto channelNumber = word;

        // Following 8 bytes: start time in unit [bin]
        // Actual time (in ns) = startTime * 2e-9
        auto startTime = read_segtime(s);

        // Size for adc values: size - 4(header) - 4 (ch_id) - 8(t_start) - 8(t_busy)
        // 2 byte per count. So need to divide by 2.
        auto segment_size = (size - 4 - 4 - 8 - 8)/2;

        // what is busy and sample_size?
        uint64_t busy = 0;
        // uint32_t sample_size = 4;
        uint32_t sample_size = segment_size;

        if (type == 2) {
            busy = read_segtime(s);
            // sample_size = 2;
        }
        std::vector<uint16_t> segment(segment_size, 0);
        s.read(reinterpret_cast<char *>(&*segment.begin()), segment_size * 2);
        return {startTime, channelNumber, segment, busy, sample_size, 0.};
    }

    RawSegment read_block(uint32_t size, std::istream &s, uint32_t type) {
        /*
         * Read the whole data block after header.
         * Params:
         *      size: size of whole data block
         *      s: stream after the header
         *      type: 1 or 2
         * Return: RawSegment
         */
        // first 4 bytes are header
        size_t body_size = size - 4;
        std::string block_body(body_size, 0);
        s.read(&*block_body.begin(), body_size);
        return read_segment(std::istringstream(block_body), size, type);
    }


    read_iterator::read_iterator(std::istream &s, uint32_t run_no, uint32_t group_no, uint32_t gap)
        : run_number(run_no), group_number(group_no), group_gap{gap}{
        stream_ptr = &s;
        ++*this;
    }


    constexpr auto min_gap = 500u;

    read_iterator &read_iterator::operator++() {
        while (true) {
            try {
                // parse the first 4 bytes
                // word>>24 (first 1 byte) represents protocol:
                auto word = rw(*stream_ptr);
                auto size_type = block_size(word);
                //  word & 0x00ffffff (later 3 bytes) represents size of the whole data (including header)
                auto size = size_type.first;
                // depending on protocol: 0xb1 means type 1 and 0xb2 means type 2
                // For now we ignore type 2
                auto type = size_type.second;
                if (size == 0) {
                    if (check_magic(0xa1, word))
                        continue;
                } else {
                    // Get RawSegment (t_start, ch_id, adv_val, tbusy...)
                    auto segment = read_block(size, *stream_ptr, type);

                    // Get the end time in unit [bin]
                    auto seg_end = segment.startTime + segment.adcValue.size();
                    /*
                     * The segments is the actual data for this group; current_end and group_start labels the last and the first time of current group.
                     * if segments is empty  OR  group is short enough  OR  gap between group and segment is short enough
                     * AND if current_end==0  OR  gap between group and start time is short enough  OR  (group not empty  AND  group is short enough
                     * THEN add current segment into group
                     */
                    if (!(!segments.empty() && current_end - group_start > 1000000 &&
                          current_end + min_gap < segment.startTime)) {
                        if (current_end == 0 || current_end + group_gap > segment.startTime ||
                            (!segments.empty() && current_end - group_start < 1000000)) {
                            segments.push_back(segment);
                            current_end = current_end > seg_end ? current_end : seg_end;
                            group_start = segment.startTime < group_start
                                          ? segment.startTime
                                          : group_start;
                            // continue to read rest blocks
                            continue;
                        }
                    }
                    group = {run_number, group_number++, group_start, current_end,
                             std::move(segments)};
                    segments = {segment};
                    group_start = segment.startTime;
                    current_end = seg_end;
                    break;
                }
            } catch (const pbss::early_eof_error &) {
                if (!segments.empty()) {
                    group = {run_number, group_number++, group_start, current_end,
                             std::move(segments)};
                    segments = {};
                } else {
                    stream_ptr = nullptr;
                }
            }
            return *this;
        }
    }
} // namespace raw_tel_trans
