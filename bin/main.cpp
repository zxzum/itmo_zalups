#include <iostream>
#include "lib.hpp"

int main(int argc, char**argv){
    if(argc<3){
        std::cout<<"usage: itmoloops score.txt out.wav\n";
        return 1;
    }
    return run_itmoloops(argv[1], argv[2]);
}