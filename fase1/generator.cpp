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

void createBox(float lenght, int divisions, const std::string &filename) {
    std::ofstream file = createFile(filename);
    std::stringstream vertices;

    float divisions_lenght = lenght / divisions;
    float trans = lenght / 2;
    // Base cima e de baixo
    for (int i = 0; i < divisions; i++)
    {
        // para deslocarmos o plano para a origem
        float z = (divisions_lenght * i) - trans;
        float next_z = (divisions_lenght * (i + 1)) - trans;
        for (int j = 0; j < divisions; j++)
        {
            float x = (divisions_lenght * j) - trans;
            float next_x = (divisions_lenght * (j + 1)) - trans;

            // Parte de cima
            // Triangulo do lado esquerdo
            vertices << x << " " << -trans << " " << z << std::endl;
            vertices << next_x << " " << -trans << " " << z << std::endl;
            vertices << x << " " << -trans << " " << next_z << std::endl;

            // Triangulo do lado esquerdo
            vertices << next_x << " " << -trans << " " << z << std::endl;
            vertices << next_x << " " << -trans << " " << next_z << std::endl;
            vertices << x << " " << -trans << " " << next_z << std::endl;

            // Parte de baixo
            // Triangulo do lado esquerdo
            vertices << x << " " << lenght - trans << " " << z << std::endl;
            vertices << x << " " << lenght - trans << " " << next_z << std::endl;
            vertices << next_x << " " << lenght - trans << " " << z << std::endl;

            // Triangulo do lado esquerdo
            vertices << next_x << " " << lenght - trans << " " << z << std::endl;
            vertices << x << " " << lenght - trans << " " << next_z << std::endl;
            vertices << next_x << " " << lenght - trans << " " << next_z << std::endl;
        }
    }

    // Lateral 1 e 3
    for (int i = 0; i < divisions; i++)
    {
        // para deslocarmos o plano para a origem
        float z = (divisions_lenght * i) - trans;
        float next_z = (divisions_lenght * (i + 1)) - trans;
        for (int j = 0; j < divisions; j++)
        {
            float y = (divisions_lenght * j) - trans;
            float next_y = (divisions_lenght * (j + 1)) - trans;

            // Parte de cima
            // Triangulo do lado esquerdo
            vertices << -trans << " " << y << " " << z << std::endl;
            vertices << -trans << " " << y << " " << next_z << std::endl;
            vertices << -trans << " " << next_y << " " << z << std::endl;

            // Triangulo do lado esquerdo
            vertices << -trans << " " << next_y << " " << z << std::endl;
            vertices << -trans << " " << y << " " << next_z << std::endl;
            vertices << -trans << " " << next_y << " " << next_z << std::endl;

            // Parte de Baixo
            // Triangulo do lado esquerdo
            vertices << lenght - trans << " " << y << " " << z << std::endl;
            vertices << lenght - trans << " " << next_y << " " << z << std::endl;
            vertices << lenght - trans << " " << y << " " << next_z << std::endl;

            // Triangulo do lado esquerdo
            vertices << lenght - trans << " " << next_y << " " << z << std::endl;
            vertices << lenght - trans << " " << next_y << " " << next_z << std::endl;
            vertices << lenght - trans << " " << y << " " << next_z << std::endl;
        }
    }

    // Lateral 2 e 4
    for (int i = 0; i < divisions; i++)
    {
        // para deslocarmos o plano para a origem
        float x = (divisions_lenght * i) - trans;
        float next_x = (divisions_lenght * (i + 1)) - trans;
        for (int j = 0; j < divisions; j++)
        {
            float y = (divisions_lenght * j) - trans;
            float next_y = (divisions_lenght * (j + 1)) - trans;

            // Parte de cima
            // Triangulo do lado esquerdo
            vertices << x << " " << y << " " << -trans << std::endl;
            vertices << x << " " << next_y << " " << -trans << std::endl;
            vertices << next_x << " " << y << " " << -trans << std::endl;

            // Triangulo do lado esquerdo
            vertices << x << " " << next_y << " " << -trans << std::endl;
            vertices << next_x << " " << next_y << " " << -trans << std::endl;
            vertices << next_x << " " << y << " " << -trans << std::endl;

            // Parte de Baixo
            // Triangulo do lado esquerdo
            vertices << x << " " << y << " " << lenght - trans << std::endl;
            vertices << next_x << " " << y << " " << lenght - trans << std::endl;
            vertices << x << " " << next_y << " " << lenght - trans << std::endl;

            // Triangulo do lado esquerdo
            vertices << x << " " << next_y << " " << lenght - trans << std::endl;
            vertices << next_x << " " << y << " " << lenght - trans << std::endl;
            vertices << next_x << " " << next_y << " " << lenght - trans << std::endl;
        }
    }
    
    file << vertices.str();
    file.close();
}

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

void createCone(float radius, float height, int slices, int stacks,const std::string &filename)
{
    std::ofstream file = createFile(filename);
    std::stringstream vertices;

    float slice = (2 * M_PI) / slices;

    // Lateral
    float height_stack = height / stacks;
    for (int i = 0; i < stacks; i++)
    {
        for (int j = 0; j < slices; j++)
        {
            float ang1 = slice * j;
            float ang2 = ang1 + slice;

            // Alturas das stacks
            float y = i * height_stack;
            float next_y = (i + 1) * height_stack;

            // Pontos do cilindro
            float x1 = radius * sin(ang1) * (stacks - i) / stacks;
            float z1 = radius * cos(ang1) * (stacks - i) / stacks;
            float next_x1 = radius * sin(ang2) * (stacks - i) / stacks;
            float next_z1 = radius * cos(ang2) * (stacks - i) / stacks;

            // Pontos do cilindro
            float x2 = radius * sin(ang1) * (stacks - i - 1) / stacks;
            float z2 = radius * cos(ang1) * (stacks - i - 1) / stacks;
            float next_x2 = radius * sin(ang2) * (stacks - i - 1) / stacks;
            float next_z2 = radius * cos(ang2) * (stacks - i - 1) / stacks;

            // Triangulos da esquerda
            vertices << x2 << " " << next_y << " " << z2 << std::endl;
            vertices << next_x1 << " " << y << " " << next_z1 << std::endl;
            vertices << next_x2 << " " << next_y << " " << next_z2 << std::endl;

            // Triangulos da direita
            vertices << x1 << " " << y << " " << z1 << std::endl;
            vertices << next_x1 << " " << y << " " << next_z1 << std::endl;
            vertices << x2 << " " << next_y << " " << z2 << std::endl;
        }
    }

    // Base
    for (int i = 0; i < slices; i++)
    {
        // Radianos da base do cilindro
        float ang1 = slice * i;
        float ang2 = ang1 + slice;

        // Pontos do cilindro
        float x = radius * sin(ang1);
        float z = radius * cos(ang1);
        float next_x = radius * sin(ang2);
        float next_z = radius * cos(ang2);

        // Cada triangulo
        vertices << 0 << " " << 0 << " " << 0 << std::endl;
        vertices << next_x << " " << 0 << " " << next_z << std::endl;
        vertices << x << " " << 0 << " " << z << std::endl;
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
        std::cerr << "  " << argv[0] << " box <length> <divisions> <filename>\n";
        std::cerr << "  " << argv[0] << " cone <radius> <height> <slices> <stacks> <filename>\n";
        // todo cilindro
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

    } else if (shape == "box") {
        if (argc != 5) {
            std::cerr << "Usage: " << argv[0] << " plane <length> <divisions> <filename>\n";
            return 1;
        }

        float length = std::stof(argv[2]);
        int divisions = std::stoi(argv[3]);
        std::string filename = argv[4];
        createBox(length, divisions, filename);
        std::cout << "Box file '" << filename << "' generated successfully!\n";
    } else if (shape == "cone") {
        if (argc != 7) {
            std::cerr << "Usage: " << argv[0] << " cone <radius> <height> <slices> <stacks> <filename>\n";
            return 1;
        }

        float radius = std::stof(argv[2]);
        float height = std::stof(argv[3]);
        int slices = std::stoi(argv[4]);
        int stacks = std::stoi(argv[5]);
        std::string filename = argv[6];

        createCone(radius, height, slices, stacks, filename);
        std::cout << "Cone file '" << filename << "' generated successfully!\n";

    }
     else {
        std::cerr << "Error: Unsupported shape '" << shape << "'\n";
        return 1;
    }

    return 0;
}