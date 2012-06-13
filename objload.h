#ifndef OBJLOAD_H_
#define OBJLOAD_H_

#include <iostream>
#include <vector>
#include <string>

struct Model {
    std::vector<float> vertex; // 3 * N entries
    std::vector<float> texCoord; // 2 * N entries
    std::vector<float> normal; // 3 * N entries
    
    std::vector<int> face; // assume triangels and uniform indexing
};

Model loadModel( const std::string & str);
Model loadModelFromString( std::istream & in );
Model loadModelFromFile( std::istream & in );

std::ostream & operator<<( std::ostream & out, const Model & m );

#endif // OBJLOAD_H_
