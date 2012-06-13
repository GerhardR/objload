#ifndef OBJLOAD_H_
#define OBJLOAD_H_

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <set>

struct Model {
    std::vector<float> vertex; // 3 * N entries
    std::vector<float> texCoord; // 2 * N entries
    std::vector<float> normal; // 3 * N entries
    
    std::vector<unsigned short> face; // assume triangels and uniform indexing
};

struct ObjModel {
    struct FaceVertex {
        FaceVertex() : v(-1), t(-1), n(-1) {}
        int v, t, n;
        
        bool operator<( const FaceVertex & other ) const {
            return (v < other.v) || (v == other.v && t < other.t ) || (v == other.v && t == other.t && n < other.n);
        }
    };

    std::vector<float> vertex; // 3 * N entries
    std::vector<float> texCoord; // 2 * N entries
    std::vector<float> normal; // 3 * N entries

    std::vector<FaceVertex> face;
    std::vector<unsigned> face_start;
};

inline ObjModel parseObjModel( std::istream & in);
inline void tesselateObjModel( ObjModel & obj);
inline ObjModel tesselateObjModel( const ObjModel & obj );
inline Model convertToModel( const ObjModel & obj );

inline Model loadModel( std::istream & in );
inline Model loadModelFromString( const std::string & in );
inline Model loadModelFromFile( const std::string & in );

inline std::ostream & operator<<( std::ostream & out, const Model & m );
inline std::ostream & operator<<( std::ostream & out, const ObjModel::FaceVertex & f);

template <typename T>
inline std::istream & operator>>(std::istream & in, std::vector<T> & vec ){
    T temp;
    in >> temp;
    if(in) 
        vec.push_back(temp);
    return in;
}

inline std::istream & operator>>( std::istream & in, ObjModel::FaceVertex & f){
    int val;
    if(in >> f.v){
        if(in.good()){
            if(in.peek() == '/'){
                in.get();
                in >> f.t;
                in.clear();
                if(in.peek() == '/'){
                    in.get();
                    in >> f.n;
                    in.clear();
                }
            }
            in.clear();
        }
        --f.v;
        --f.t;
        --f.n;
    }
    // std::cout << f << std::endl;
    return in;
}

ObjModel parseObjModel( std::istream & in ){
    char line[1024];
    std::string op;
    std::istringstream line_in;

    ObjModel data;

    while(in.good()){
        in.getline(line, 1023);
        line_in.clear();
        line_in.str(line);

        line_in >> op;
        if(op == "v")
            line_in >> data.vertex >> data.vertex >> data.vertex;
        else if(op == "vt")
            line_in >> data.texCoord >> data.texCoord >> data.texCoord;
        else if(op == "vn")
            line_in >> data.normal >> data.normal >> data.normal;
        else if(op == "f") {
            data.face_start.push_back(data.face.size());
            while(line_in >> data.face) ;
        }
    }
    data.face_start.push_back(data.face.size());
    return data;
}

void tesselateObjModel( ObjModel & obj){
    std::vector<ObjModel::FaceVertex> output;
    std::vector<unsigned> output_start;
    output.reserve(obj.face.size());
    output_start.reserve(obj.face_start.size());
    
    for(std::vector<unsigned>::const_iterator s = obj.face_start.begin(); s != obj.face_start.end() - 1; ++s){
        const unsigned size = *(s+1) - *s;
        if(size > 3){
            const ObjModel::FaceVertex & start_vertex = obj.face[*s];
            for( int i = 1; i < size-1; ++i){
                output_start.push_back(output.size());
                output.push_back(start_vertex);
                output.push_back(obj.face[*s+i]);
                output.push_back(obj.face[*s+i+1]);
            }
        } else {
            output_start.push_back(output.size());
            output.insert(output.end(), obj.face.begin() + *s, obj.face.begin() + *(s+1));
        }
    }
    output_start.push_back(output.size());
    obj.face.swap(output);
    obj.face_start.swap(output_start);
}

Model convertToModel( const ObjModel & obj ) {
    // insert all into a set to make unique
    std::set<ObjModel::FaceVertex> unique(obj.face.begin(), obj.face.end());

    // build a new model with repeated vertices/texcoords/normals to have single indexing
    Model model;
    for(std::set<ObjModel::FaceVertex>::const_iterator f = unique.begin(); f != unique.end(); ++f){
        // cout << (*f) << endl;
        model.vertex.insert(model.vertex.end(), obj.vertex.begin() + 3*f->v, obj.vertex.begin() + 3*f->v + 3);
        if(!obj.texCoord.empty()){
            const int index = (f->t > -1) ? f->t : f->v;
            model.texCoord.insert(model.texCoord.end(), obj.texCoord.begin() + 2*index, obj.texCoord.begin() + 2*index + 2);
        }
        if(!obj.normal.empty()){
            const int index = (f->n > -1) ? f->n : f->v;
            model.normal.insert(model.normal.end(), obj.normal.begin() + 3*index, obj.normal.begin() + 3*index + 3);
        }
    }
    // look up unique index and transform face descriptions
    for(std::vector<ObjModel::FaceVertex>::const_iterator f = obj.face.begin(); f != obj.face.end(); ++f){
        const int index = std::distance(unique.begin(), unique.find(*f));
        model.face.push_back(index);
    }
    return model;
}

ObjModel tesselateObjModel( const ObjModel & obj ){
    ObjModel result = obj;
    tesselateObjModel(result);
    return result;
}

Model loadModel( std::istream & in ){
    ObjModel model = parseObjModel(in);
    tesselateObjModel(model);
    return convertToModel(model);
}

Model loadModelFromString( const std::string & str ){
    std::istringstream in(str);
    return loadModel(in);
}

Model loadModelFromFile( const std::string & str) {
    std::ifstream in(str.c_str());
    return loadModel(in);
}

inline std::ostream & operator<<( std::ostream & out, const ObjModel::FaceVertex & f){
    out << f.v << "\t" << f.t << "\t" << f.n;
    return out;
}

inline std::ostream & operator<<( std::ostream & out, const Model & m ){
    if(!m.vertex.empty()){
        out << "vertex\n";
        for(int i = 0; i < m.vertex.size(); ++i){
            out << m.vertex[i] << "\t";
            if((i % 3) == 2)
                out << std::endl;
        }
    }
    if(!m.texCoord.empty()){
        out << "texCoord\n";
        for(int i = 0; i < m.texCoord.size(); ++i){
            out << m.texCoord[i] << "\t";
            if((i % 2) == 1)
                out << std::endl;
        }
    }
    if(!m.normal.empty()){
        out << "normal\n";
        for(int i = 0; i < m.normal.size(); ++i){
            out << m.normal[i] << "\t";
            if((i % 3) == 2)
                out << std::endl;
        }
    }
    if(!m.face.empty()){
        out << "face\n";
        for(int i = 0; i < m.face.size(); ++i){
            out << m.face[i] << "\t";
            if((i % 3) == 2)
                out << std::endl;
        }
    }
    return out;
}

#endif // OBJLOAD_H_
