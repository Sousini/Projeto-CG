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




// main para testar o plane

int main(int argc, char* argv[]) {
    if (argc != 5) {
        std::cerr << "Usage: " << argv[0] << " plane <length> <divisions> <filename>\n";
        return 1;
    }

    std::string shape = argv[1];
    if (shape != "plane") {
        std::cerr << "Error: Unsupported shape '" << shape << "'\n";
        return 1;
    }

    int length = std::stoi(argv[2]);
    int divisions = std::stoi(argv[3]);
    std::string filename = argv[4];

    createPlane(length, divisions, filename);
    std::cout << "Plane file '" << filename << "' generated successfully!\n";

    return 0;
}