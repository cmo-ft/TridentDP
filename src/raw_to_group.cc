#include <unistd.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "transformer/RawGroupTrans.hh"

template <typename T> void read_value(const char *s, T &v) {
    std::istringstream iss(s);
    iss >> v;
}

int main(int argc, char *argv[]) {
    namespace pu = pbsu;
    namespace rtt = raw_group_trans;

    using std::cout;

    using pbss::parse;

    std::string output;
    uint32_t run_no = 0;
    uint32_t group_no = 0;
    uint32_t gap = 5000;

    for (int opt; (opt = getopt(argc, argv, "o:r:g:a")) > 0;) {
        switch (opt) {
            case 'o':
                output = optarg;
                break;
            case 'r':
                read_value(optarg, run_no);
                break;
            case 'g':
                read_value(optarg, group_no);
                break;
            case 'a':
                read_value(optarg, gap);
                break;
        }
    }

    if (output.empty()) {
        cout
            << "Usage: " << argv[0]
            << " -o output [-r run_no] [-g group_no] [-a gap] inputs ...\n";
        return 1;
    }


    auto outfile = open_sequential_output_file(output, TridentRealm());
    auto outiter = outfile.write_iterator();

    for (std::string filename : pu::make_range(argv + optind, argv + argc)) {
        std::ifstream file(filename);
        if (file.good())
            copy(rtt::read_file(file, run_no, group_no, gap),
                 outiter);
        else
            return 1;
    }

    return 0;
}
