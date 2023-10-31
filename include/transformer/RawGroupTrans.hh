//
// Created by cmo on 2023/10/31.
//

#ifndef TRIDENTDP_RAWGROUPTRANS_HH
#define TRIDENTDP_RAWGROUPTRANS_HH


#include <bs3/pbs.hh>
#include <cstdint>
#include <istream>
#include <stdexcept>
#include "trident_ds.hh"

namespace raw_group_trans {

// thrown on format error in input file
    class format_error : public std::domain_error {
    public:
        explicit format_error(const char *msg = "Malformed data file")
            : std::domain_error(msg) {}
    };

    struct read_iterator {
        typedef std::input_iterator_tag iterator_category;
        typedef GroupData value_type;
        typedef std::ptrdiff_t difference_type;
        typedef const value_type *pointer;
        typedef const value_type &reference;

        // default constructor makes end iterator
        read_iterator() : stream_ptr(nullptr) {}

        // construction from stream
        read_iterator(std::istream &s, uint32_t run_no, uint32_t group_no, uint32_t gap = 2500);

    private:
        std::istream *stream_ptr;
        GroupData group;
        std::vector<RawSegment> segments; // temporary segments
        uint64_t current_end = 0;
        uint32_t run_number{};
        uint32_t group_number{};
        uint32_t group_gap = 2500; // clocks, 1 clock = 4 ns, gap of 10 us
        uint64_t group_start = 0xfffffffffffffffflu;

    public:
        bool operator==(const read_iterator &other) const {
            return stream_ptr == other.stream_ptr;
        }

        bool operator!=(const read_iterator &other) const {
            return !((*this) == other);
        }

        /*
         * CORE of the read_iterator
         */
        read_iterator &operator++();

        reference operator*() const { return group; }

        pointer operator->() const { return &group; }
    };

    // helper returning range
    pbsu::range<read_iterator> inline read_file(std::istream &s, uint32_t run_no,
                                                uint32_t group_no,
                                                uint32_t gap = 2500) {
        return {{s, run_no, group_no, gap}, {}};
    }

} // namespace raw_tel_trans


#endif //TRIDENTDP_RAWGROUPTRANS_HH
