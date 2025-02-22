#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>

std::ofstream createFile(const std::string &filename) {
    std::ofstream file("3d/" + filename);

    if (!file.is_open()) {
        std::cerr << "Error opening file" << std::endl;
        exit(1);
    }

    return file;
}

void createPlane(int length, int divisions, const std::string &filename) {
    std::ofstream file = createFile(filename);
    std::stringstream vertices;

    float px = -length / 2.0f;
    float py = length / 2.0f;
    float pz = 0.0f;
    float step = static_cast<float>(length) / divisions;

    // Criar os pontos da grelha
    int points = (divisions + 1) * (divisions + 1);
    float array[points][3];

    int index = 0;
    for (int j = 0; j <= divisions; j++) {
        for (int i = 0; i <= divisions; i++) {
            array[index][0] = px + i * step;
            array[index][1] = py - j * step;
            array[index][2] = pz;
            index++;
        }
    }


    // Criar os triângulos
    
    for(int j = 0; j < divisions; j++) {
        for(int i = 0; i < divisions;i++) {
            int k = i + j*(divisions+1);
        
            //Triangulo 1
            vertices << array[k][0] << " " << array[k][1] << " " << array[k][2] << std::endl;
            vertices << array[k + (divisions+1)][0] << " " << array[k + (divisions+1)][1] << " " << array[k + (divisions+1)][2] << std::endl;
            vertices << array[k + (divisions+2)][0] << " " << array[k + (divisions+2)][1] << " " << array[k + (divisions+2)][2] << std::endl;

            //Triangulo 2
            vertices << array[k][0] << " " << array[k][1] << " " << array[k][2] << std::endl;
            vertices << array[k + (divisions+2)][0] << " " << array[k + (divisions+2)][1] << " " << array[k + (divisions+2)][2] << std::endl;
            vertices << array[k+1][0] << " " << array[k+1][1] << " " << array[k+1][2] << std::endl;
        }
    }

    file << vertices.str();
    file.close();
}



/*void createBox(int length, int divisions, const std::string &filename) {
    std::ofstream file = createFile(filename);
    


    
    
}*/


void createSphere(int radius, int slices, int stacks,const std::string &filename) {
    std::ofstream file = createFile(filename);
    std::stringstream vertices;

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

        vertices << "0 " << radius << " 0" << std::endl;  // Ponto do topo
        vertices << px1 << " " << py1 << " " << pz1 << std::endl;
        vertices << px2 << " " << py2 << " " << pz2 << std::endl;

        for(int j = 0; j < (stacks-2);j++) {

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
            vertices << px1 << " " << py1 << " " << pz1 << std::endl;
            vertices << px2 << " " << py2 << " " << pz2 << std::endl;
            vertices << px3 << " " << py2 << " " << pz3 << std::endl;

            //Triangulo 2
            vertices << px1 << " " << py1 << " " << pz1 << std::endl;
            vertices << px3 << " " << py2 << " " << pz3 << std::endl;
            vertices << px4 << " " << py1 << " " << pz4 << std::endl;
        }

        float pxb1 = radius * sin(i * alpha) * sin((stacks-1)* beta);
        float pyb1 = radius * cos((stacks-1)* beta);
        float pzb1 = radius * cos(i * alpha) * sin((stacks-1)* beta);
        
        float pxb2 = radius * sin((i + 1) * alpha) * sin((stacks-1)* beta);
        float pzb2 = radius * cos((i + 1) * alpha) * sin((stacks-1)* beta);

        vertices << "0 " << -radius << " 0" << std::endl; 
        vertices << pxb1 << " " << -pyb1 << " " << pzb1 << std::endl;
        vertices << pxb2 << " " << -pyb1 << " " << pzb2 << std::endl;
    }

    file << vertices.str();
    file.close();
}


// main para testar o plane

int main(int argc, char* argv[]) {
    if (argc < 5) {
        std::cerr << "Usage:\n";
        std::cerr << "  " << argv[0] << " plane <length> <divisions> <filename>\n";
        std::cerr << "  " << argv[0] << " sphere <radius> <slices> <stacks> <filename>\n";
        return 1;
    }

    std::string shape = argv[1];

    if (shape == "plane") {
        if (argc != 5) {
            std::cerr << "Usage: " << argv[0] << " plane <length> <divisions> <filename>\n";
            return 1;
        }

        float length = std::stof(argv[2]);
        int divisions = std::stoi(argv[3]);
        std::string filename = argv[4];

        createPlane(length, divisions, filename);
        std::cout << "Plane file '" << filename << "' generated successfully!\n";

    } else if (shape == "sphere") {
        if (argc != 6) {
            std::cerr << "Usage: " << argv[0] << " sphere <radius> <slices> <stacks> <filename>\n";
            return 1;
        }

        float radius = std::stof(argv[2]);
        int slices = std::stoi(argv[3]);
        int stacks = std::stoi(argv[4]);
        std::string filename = argv[5];

        createSphere(radius, slices, stacks, filename);
        std::cout << "Sphere file '" << filename << "' generated successfully!\n";

    } else {
        std::cerr << "Error: Unsupported shape '" << shape << "'\n";
        return 1;
    }

    return 0;
}