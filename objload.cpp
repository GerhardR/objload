#include "objload.h"

int main(int argc, char ** argv){
    if(argc > 1){
        Model m = loadModelFromFile(argv[1]);
        std::cout << m << std::endl;
    }

    return 0;
}
