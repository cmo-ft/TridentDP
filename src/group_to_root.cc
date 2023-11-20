#include <iostream>
#include <bs3/pbs.hh>

#include "transformer/GroupRootTrans.hh"


using std::cerr;

using std::endl;
using std::string;
using std::vector;
using namespace std;

int main(int argc, char *argv[]) {
    std::string ifname, ofname{"signal.root"};
    for (int option; (option = getopt(argc, argv, "i:o:")) > 0;) {
        switch (option) {
            case 'i':
                ifname = optarg;
                break;
            case 'o':
                ofname = optarg;
                break;
            default:
                break;
        }
    }

    if (ifname.empty()) {
        cerr << "Usage:" << argv[0]
             << " -i input_name -o output_name"
             << endl;
        return 1;
    }

    namespace pu = pbsu;
    namespace pf = pbsf;

    auto data_file = open_sequential_input_file(ifname, TridentRealm ());
    auto group_data = data_file.read_one_type<GroupData>();

GroupRootTrans gr_trans{ofname};

    for (const auto &data : group_data) {
        gr_trans.ReadData(data);
    }
    gr_trans.Write();

}
