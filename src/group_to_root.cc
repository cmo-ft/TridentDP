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

    GroupRootTrans* gr_trans = nullptr;

    int num_group_per_rootfile = 10;
    int i_data = 0;
    for (const auto &data : group_data) {
        if (gr_trans==nullptr){
            gr_trans = new GroupRootTrans(ofname + to_string(i_data / num_group_per_rootfile) + ".root");
        }

        std::cout << "Reading data " << i_data << "..." << std::endl << std::flush;
        gr_trans->ReadData(data);

        if ((i_data+1) % num_group_per_rootfile == 0){
            gr_trans->Write();
            delete gr_trans;
            gr_trans = nullptr;
        }
        i_data += 1;
    }

    if (gr_trans!=nullptr){
        gr_trans->Write();
        delete gr_trans;
    }

}
