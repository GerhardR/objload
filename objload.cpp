#include "objload.h"

int main(int argc, char ** argv){
    if(argc > 1){
        Model m = loadModelFromFile(argv[1]);
#if 1
        std::cout << m << std::endl;
#else
        std::cout << m.vertex.size()/3 << " vertices, " << m.texCoord.size()/2 << 
                     " texcoords, " << m.normal.size()/3 << " normals, " <<
                     m.face.size()/3 << " faces" << std::endl;
#endif
    }

    return 0;
}
