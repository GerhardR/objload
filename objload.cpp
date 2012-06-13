#include "objload.h"
#include <sstream>
#include <fstream>
#include <set>

using namespace std;

template <typename T>
inline istream & operator>>(istream & in, vector<T> & vec ){
    T temp;
    in >> temp;
    if(in)
        vec.push_back(temp);
    return in;
}

struct Store {
    struct FaceVertex {
        FaceVertex() : v(-1), t(-1), n(-1) {}
        int v, t, n;
        
        bool operator<( const FaceVertex & other ) const {
            return (v < other.v) || (v == other.v && t < other.t ) || (v == other.v && t == other.t && n < other.n);
        }
    };

    typedef vector<FaceVertex> Face;
    typedef set<FaceVertex> UniqueVertices;

    Model data;
    vector<Face> faces;
    
    void parse( const std::string & line ){
        string op;

        istringstream in(line);
        in >> op;

        if(op == "#")
            return;
        else if(op == "v")
            in >> data.vertex >> data.vertex >> data.vertex;
        else if(op == "vt")
            in >> data.texCoord >> data.texCoord >> data.texCoord;
        else if(op == "vn")
            in >> data.normal >> data.normal >> data.normal;
        else if(op == "f") {
            Face newFace;
            while(in >> newFace) ;
            if(newFace.size() == 4) {
                // cout << "4 face will split\n";
                newFace.insert(newFace.begin()+3, newFace[0]);
                newFace.insert(newFace.begin()+4, newFace[2]);
            } else if(newFace.size() > 4) {
                cout << "Ups long face, cut back to 3\n";
                newFace.resize(3);
            }
            faces.push_back(newFace);
        }
    }

    Model buildModel() const;
};

ostream & operator<<( ostream & out, const Store::FaceVertex & f){
    out << f.v << "\t" << f.t << "\t" << f.n;
    return out;
}

istream & operator>>( istream & in, Store::FaceVertex & f){
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
//    cout << f << endl;
    return in;
}

Model Store::buildModel() const {
    UniqueVertices unique;
    for(vector<Face>::const_iterator f = faces.begin(); f != faces.end(); ++f){
        for(Face::const_iterator ff = f->begin(); ff != f->end(); ++ff)
            unique.insert(*ff);
    }
    Model model;
    for(UniqueVertices::const_iterator f = unique.begin(); f != unique.end(); ++f){
        // cout << (*f) << endl;
        model.vertex.insert(model.vertex.end(), data.vertex.begin() + 3*f->v, data.vertex.begin() + 3*f->v + 3);
        if(!data.texCoord.empty()){
            const int index = (f->t > -1) ? f->t : f->v;
            model.texCoord.insert(model.texCoord.end(), data.texCoord.begin() + 2*index, data.texCoord.begin() + 2*index + 2);
        }
        if(!data.normal.empty()){
            const int index = (f->n > -1) ? f->n : f->v;
            model.normal.insert(model.normal.end(), data.normal.begin() + 3*index, data.normal.begin() + 3*index + 3);
        }
    }
    for(vector<Face>::const_iterator f = faces.begin(); f != faces.end(); ++f){
        for(Face::const_iterator ff = f->begin(); ff != f->end(); ++ff){
            const int index = distance(unique.begin(), unique.find(*ff));
            model.face.push_back(index);
        }
    }
    return model;
}

Model loadModel( std::istream & in ){
    char line[1024];
    Store store;
    
    while(in.good()){
        in.getline(line, 1023);
        store.parse(line);
    }
    return store.buildModel();
}

Model loadModelFromString( const std::string & str ){
    istringstream in(str);
    return loadModel(in);
}

Model loadModelFromFile( const std::string & str) {
    ifstream in(str.c_str());
    return loadModel(in);
}

std::ostream & operator<<( std::ostream & out, const Model & m ){
    if(!m.vertex.empty()){
        out << "vertex\n";
        for(int i = 0; i < m.vertex.size(); ++i){
            cout << m.vertex[i] << "\t";
            if((i % 3) == 2)
                cout << endl;
        }
    }
    if(!m.texCoord.empty()){
        out << "texCoord\n";
        for(int i = 0; i < m.texCoord.size(); ++i){
            cout << m.texCoord[i] << "\t";
            if((i % 2) == 1)
                cout << endl;
        }
    }
    if(!m.normal.empty()){
        out << "normal\n";
        for(int i = 0; i < m.normal.size(); ++i){
            cout << m.normal[i] << "\t";
            if((i % 3) == 2)
                cout << endl;
        }
    }
    if(!m.face.empty()){
        out << "face\n";
        for(int i = 0; i < m.face.size(); ++i){
            cout << m.face[i] << "\t";
            if((i % 3) == 2)
                cout << endl;
        }
    }
    return out;
}

#if 1
int main(int argc, char ** argv){
    if(argc > 1){
        Model m = loadModelFromFile(argv[1]);
        cout << m << endl;
    }

    return 0;
}
#endif