//
// Created by cmo on 2023/11/3.
//
#include <iostream>
#include <fstream>

using namespace std;

int main(int argc, char** argv){
    // FILE* outfile;
    std::string waveform_path;
    if(argc != 2){
        std::cerr << "Wrong input arguments! " <<
                  "Please input: raw_reader [raw_data_path]" << std::endl;
        return -1;
    }

    uint32_t header;
    unsigned int block_size{0};
    unsigned int board_id{0};
    unsigned long t_begin{0};
    unsigned long tbusy{0};
    auto *buffer = new uint16_t[4000000];
    unsigned int sample_numer{0};


    ifstream infile(argv[1],ios::in|ios::binary);
    // infile.read((char *)&header, 1);
    // cout<<header<<endl<<flush;
    // infile.read((char *)&size, 3);
    // cout<<size<<endl<<flush;
    while(infile.read((char *)&header, 4)){
        // protocol & event size
        block_size = (header & 0xFFFFFF);
        cout << (header>>24) << " " << block_size << " ";

        infile.read((char *)&board_id, 4);
        infile.read((char *)&t_begin, 8);
        infile.read((char *)&tbusy, 8);

        sample_numer = (block_size - 24) / 2;
        infile.read((char *)buffer, sample_numer*2);

        cout << board_id << " " << t_begin << " " << tbusy << " ";
        cout<<flush;
        for(unsigned int isample=0; isample<sample_numer; isample++){
            cout << buffer[isample] << " ";
        }
        cout << endl;


    }

}