#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>

using namespace std;

ofstream createFile(const string &filename) {
    ofstream file("3d/" + filename);

    if (!file.is_open()) {
        cerr << "Error opening file" << endl;
        exit(1);
    }

    return file;
}

void createPlane(int length, int divisions, const string &filename) {
    ofstream file = createFile(filename);
    stringstream vertices;

    float px = -length / 2.0f;
    float py = 0.0f;
    float pz = -length / 2.0f;
    float step = static_cast<float>(length) / divisions;

    // Criar os pontos da grelha
    int points = (divisions + 1) * (divisions + 1);
    float array[points][3];

    int index = 0;
    for (int j = 0; j <= divisions; j++) {
        for (int i = 0; i <= divisions; i++) {
            array[index][0] = px + i * step;
            array[index][1] = py;
            array[index][2] = pz + j * step;
            index++;
        }
    }
    // Criar os triângulos
    
    for(int j = 0; j < divisions; j++) {
        for(int i = 0; i < divisions;i++) {
            int k = i + j*(divisions+1);
        
            //Triangulo 1
            vertices << array[k][0] << " " << array[k][1] << " " << array[k][2] << endl;
            vertices << array[k + (divisions+1)][0] << " " << array[k + (divisions+1)][1] << " " << array[k + (divisions+1)][2] << endl;
            vertices << array[k + (divisions+2)][0] << " " << array[k + (divisions+2)][1] << " " << array[k + (divisions+2)][2] << endl;

            //Triangulo 2
            vertices << array[k][0] << " " << array[k][1] << " " << array[k][2] << endl;
            vertices << array[k + (divisions+2)][0] << " " << array[k + (divisions+2)][1] << " " << array[k + (divisions+2)][2] << endl;
            vertices << array[k+1][0] << " " << array[k+1][1] << " " << array[k+1][2] << endl;
        }
    }

    file << vertices.str();
    file.close();
}


void createBox(float lenght, int divisions, const string &filename) {
    ofstream file = createFile(filename);
    stringstream vertices;

    float size = lenght / divisions;
    float trans = lenght / 2;
    
    for (int i = 0; i < divisions; i++)
    {

        float pz = (size * i) - trans;
        float pz2 = (size * (i + 1)) - trans;
        for (int j = 0; j < divisions; j++)
        {
            float px = (size * j) - trans;
            float px2 = (size * (j + 1)) - trans;

            vertices << px << " " << -trans << " " << pz << endl;
            vertices << px2 << " " << -trans << " " << pz << endl;
            vertices << px << " " << -trans << " " << pz2 << endl;

            vertices << px2 << " " << -trans << " " << pz << endl;
            vertices << px2 << " " << -trans << " " << pz2 << endl;
            vertices << px << " " << -trans << " " << pz2 << endl;

            vertices << px << " " << lenght - trans << " " << pz << endl;
            vertices << px << " " << lenght - trans << " " << pz2 << endl;
            vertices << px2 << " " << lenght - trans << " " << pz << endl;

            vertices << px2 << " " << lenght - trans << " " << pz << endl;
            vertices << px << " " << lenght - trans << " " << pz2 << endl;
            vertices << px2 << " " << lenght - trans << " " << pz2 << endl;
        }
    }

    for (int i = 0; i < divisions; i++)
    {
        float pz = (size * i) - trans;
        float pz2 = (size * (i + 1)) - trans;
        for (int j = 0; j < divisions; j++)
        {
            float y = (size * j) - trans;
            float next_y = (size * (j + 1)) - trans;

            vertices << -trans << " " << y << " " << pz << endl;
            vertices << -trans << " " << y << " " << pz2 << endl;
            vertices << -trans << " " << next_y << " " << pz << endl;

            vertices << -trans << " " << next_y << " " << pz << endl;
            vertices << -trans << " " << y << " " << pz2 << endl;
            vertices << -trans << " " << next_y << " " << pz2 << endl;

            vertices << lenght - trans << " " << y << " " << pz << endl;
            vertices << lenght - trans << " " << next_y << " " << pz << endl;
            vertices << lenght - trans << " " << y << " " << pz2 << endl;

            vertices << lenght - trans << " " << next_y << " " << pz << endl;
            vertices << lenght - trans << " " << next_y << " " << pz2 << endl;
            vertices << lenght - trans << " " << y << " " << pz2 << endl;
        }
    }

    for (int i = 0; i < divisions; i++)
    {
        float px = (size * i) - trans;
        float px2 = (size * (i + 1)) - trans;
        for (int j = 0; j < divisions; j++)
        {
            float y = (size * j) - trans;
            float next_y = (size * (j + 1)) - trans;

            vertices << px << " " << y << " " << -trans << endl;
            vertices << px << " " << next_y << " " << -trans << endl;
            vertices << px2 << " " << y << " " << -trans << endl;

            vertices << px << " " << next_y << " " << -trans << endl;
            vertices << px2 << " " << next_y << " " << -trans << endl;
            vertices << px2 << " " << y << " " << -trans << endl;

            vertices << px << " " << y << " " << lenght - trans << endl;
            vertices << px2 << " " << y << " " << lenght - trans << endl;
            vertices << px << " " << next_y << " " << lenght - trans << endl;

            vertices << px << " " << next_y << " " << lenght - trans << endl;
            vertices << px2 << " " << y << " " << lenght - trans << endl;
            vertices << px2 << " " << next_y << " " << lenght - trans << endl;
        }
    }
    
    file << vertices.str();
    file.close();
}

void createSphere(int radius, int slices, int stacks,const string &filename) {
    ofstream file = createFile(filename);
    stringstream vertices;

    float px1, py1, pz1, px2, py2, pz2, px3, pz3, px4, pz4;


    //angulos
    float alpha = 2*M_PI/slices;
    float beta = M_PI/stacks;
        
    for(int i = 0; i < slices; i++) {
        float px1 = radius * sin(i * alpha) * sin(beta);
        float py1 = radius * cos(beta);
        float pz1 = radius * cos(i * alpha) * sin(beta);

        float px2 = radius * sin((i + 1) * alpha) * sin(beta);
        float py2 = py1;
        float pz2 = radius * cos((i + 1) * alpha) * sin(beta);

        for(int j = 0; j < (stacks);j++) {

            px1 = radius * cos(M_PI_2-j*beta) * sin(i*alpha);
            py1 = radius * sin (M_PI_2-j*beta);
            pz1 = radius * cos(M_PI_2-j*beta) * cos(i*alpha);

            px2 = radius * cos(M_PI_2-(j+1)*beta) * sin(i*alpha);
            pz2 = radius * cos(M_PI_2-(j+1)*beta) * cos(i*alpha);
            
            px3 = radius * cos(M_PI_2-(j+1)*beta) * sin((i+1)*alpha);
            py2 = radius * sin (M_PI_2-(j+1)*beta);
            pz3 = radius * cos(M_PI_2-(j+1)*beta) * cos((i+1)*alpha);

            px4 = radius * cos(M_PI_2-j*beta) * sin((i+1)*alpha);
            pz4 = radius * cos(M_PI_2-j*beta) * cos((i+1)*alpha);


            //Triangulo 1
            vertices << px1 << " " << py1 << " " << pz1 << endl;
            vertices << px2 << " " << py2 << " " << pz2 << endl;
            vertices << px3 << " " << py2 << " " << pz3 << endl;

            //Triangulo 2
            vertices << px1 << " " << py1 << " " << pz1 << endl;
            vertices << px3 << " " << py2 << " " << pz3 << endl;
            vertices << px4 << " " << py1 << " " << pz4 << endl;
        }
    }

    file << vertices.str();
    file.close();
}

void createCone(float radius, float height, int slices, int stacks,const string &filename)
{
    ofstream file = createFile(filename);
    stringstream vertices;

    float slice = (2 * M_PI) / slices;

    float height_stack = height / stacks;
    for (int i = 0; i < stacks; i++)
    {
        for (int j = 0; j < slices; j++)
        {
            float ang1 = slice * j;
            float ang2 = ang1 + slice;

            float py = i * height_stack;
            float py2 = (i + 1) * height_stack;

            float px = radius * sin(ang1) * (stacks - i) / stacks;
            float pz = radius * cos(ang1) * (stacks - i) / stacks;
            float px2 = radius * sin(ang2) * (stacks - i) / stacks;
            float pz2 = radius * cos(ang2) * (stacks - i) / stacks;

            float x2 = radius * sin(ang1) * (stacks - i - 1) / stacks;
            float z2 = radius * cos(ang1) * (stacks - i - 1) / stacks;
            float next_x2 = radius * sin(ang2) * (stacks - i - 1) / stacks;
            float next_z2 = radius * cos(ang2) * (stacks - i - 1) / stacks;

            vertices << x2 << " " << py2 << " " << z2 << endl;
            vertices << px2 << " " << py << " " << pz2 << endl;
            vertices << next_x2 << " " << py2 << " " << next_z2 << endl;

            vertices << px << " " << py << " " << pz << endl;
            vertices << px2 << " " << py << " " << pz2 << endl;
            vertices << x2 << " " << py2 << " " << z2 << endl;
        }
    }

    for (int i = 0; i < slices; i++)
    {
        float ang1 = slice * i;
        float ang2 = ang1 + slice;

        float px = radius * sin(ang1);
        float pz = radius * cos(ang1);
        float px2 = radius * sin(ang2);
        float pz2 = radius * cos(ang2);

        vertices << 0 << " " << 0 << " " << 0 << endl;
        vertices << px2 << " " << 0 << " " << pz2 << endl;
        vertices << px << " " << 0 << " " << pz << endl;
    }
    file << vertices.str();
    file.close();
}

// main para testar o plane

int main(int argc, char* argv[]) {
    if (argc < 5) {
        cerr << "Usage:\n";
        cerr << "  " << argv[0] << " plane <length> <divisions> <filename>\n";
        cerr << "  " << argv[0] << " sphere <radius> <slices> <stacks> <filename>\n";
        cerr << "  " << argv[0] << " box <length> <divisions> <filename>\n";
        cerr << "  " << argv[0] << " cone <radius> <height> <slices> <stacks> <filename>\n";
        // todo cilindro
        return 1;
    }

    string shape = argv[1];

    if (shape == "plane") {
        if (argc != 5) {
            cerr << "Usage: " << argv[0] << " plane <length> <divisions> <filename>\n";
            return 1;
        }

        float length = stof(argv[2]);
        int divisions = stoi(argv[3]);
        string filename = argv[4];

        createPlane(length, divisions, filename);
        cout << "Plane file '" << filename << "' generated successfully!\n";

    } else if (shape == "sphere") {
        if (argc != 6) {
            cerr << "Usage: " << argv[0] << " sphere <radius> <slices> <stacks> <filename>\n";
            return 1;
        }

        float radius = stof(argv[2]);
        int slices = stoi(argv[3]);
        int stacks = stoi(argv[4]);
        string filename = argv[5];

        createSphere(radius, slices, stacks, filename);
        cout << "Sphere file '" << filename << "' generated successfully!\n";

    } else if (shape == "box") {
        if (argc != 5) {
            cerr << "Usage: " << argv[0] << " plane <length> <divisions> <filename>\n";
            return 1;
        }

        float length = stof(argv[2]);
        int divisions = stoi(argv[3]);
        string filename = argv[4];
        createBox(length, divisions, filename);
        cout << "Box file '" << filename << "' generated successfully!\n";
    } else if (shape == "cone") {
        if (argc != 7) {
            cerr << "Usage: " << argv[0] << " cone <radius> <height> <slices> <stacks> <filename>\n";
            return 1;
        }

        float radius = stof(argv[2]);
        float height = stof(argv[3]);
        int slices = stoi(argv[4]);
        int stacks = stoi(argv[5]);
        string filename = argv[6];

        createCone(radius, height, slices, stacks, filename);
        cout << "Cone file '" << filename << "' generated successfully!\n";

    }
     else {
        cerr << "Error: Unsupported shape '" << shape << "'\n";
        return 1;
    }

    return 0;
}