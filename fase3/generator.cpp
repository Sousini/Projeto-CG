#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <vector>
#include <string>

using namespace std;

ofstream createFile(const string &filename) {
    ofstream file("3d/" + filename);

    if (!file.is_open()) {
        cerr << "Error opening file" << endl;
        exit(1);
    }

    return file;
}

void multVector(float m[4][4], float *v, float *res) {
    for (int j = 0; j < 4; ++j) {
        res[j] = 0.0f;
        for (int k = 0; k < 4; ++k) {
            res[j] += v[k] * m[j][k];
        }
    }
}

void multMatrix(float a[4][4], float b[4][4], float res[4][4]) {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            res[i][j] = 0.0f;
            for (int k = 0; k < 4; ++k) {
                res[i][j] += a[i][k] * b[k][j];
            }
        }
    }
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
    float meio = lenght / 2;
    
    for (int i = 0; i < divisions; i++)
    {

        float pz = (size * i) - meio;
        float pz2 = (size * (i + 1)) - meio;
        for (int j = 0; j < divisions; j++)
        {
            float px = (size * j) - meio;
            float px2 = (size * (j + 1)) - meio;

            vertices << px << " " << -meio << " " << pz << endl;
            vertices << px2 << " " << -meio << " " << pz << endl;
            vertices << px << " " << -meio << " " << pz2 << endl;

            vertices << px2 << " " << -meio << " " << pz << endl;
            vertices << px2 << " " << -meio << " " << pz2 << endl;
            vertices << px << " " << -meio << " " << pz2 << endl;

            vertices << px << " " << lenght - meio << " " << pz << endl;
            vertices << px << " " << lenght - meio << " " << pz2 << endl;
            vertices << px2 << " " << lenght - meio << " " << pz << endl;

            vertices << px2 << " " << lenght - meio << " " << pz << endl;
            vertices << px << " " << lenght - meio << " " << pz2 << endl;
            vertices << px2 << " " << lenght - meio << " " << pz2 << endl;
        }
    }

    for (int i = 0; i < divisions; i++)
    {
        float pz = (size * i) - meio;
        float pz2 = (size * (i + 1)) - meio;
        for (int j = 0; j < divisions; j++)
        {
            float py = (size * j) - meio;
            float py2 = (size * (j + 1)) - meio;

            vertices << -meio << " " << py << " " << pz << endl;
            vertices << -meio << " " << py << " " << pz2 << endl;
            vertices << -meio << " " << py2 << " " << pz << endl;

            vertices << -meio << " " << py2 << " " << pz << endl;
            vertices << -meio << " " << py << " " << pz2 << endl;
            vertices << -meio << " " << py2 << " " << pz2 << endl;

            vertices << lenght - meio << " " << py << " " << pz << endl;
            vertices << lenght - meio << " " << py2 << " " << pz << endl;
            vertices << lenght - meio << " " << py << " " << pz2 << endl;

            vertices << lenght - meio << " " << py2 << " " << pz << endl;
            vertices << lenght - meio << " " << py2 << " " << pz2 << endl;
            vertices << lenght - meio << " " << py << " " << pz2 << endl;
        }
    }

    for (int i = 0; i < divisions; i++)
    {
        float px = (size * i) - meio;
        float px2 = (size * (i + 1)) - meio;
        for (int j = 0; j < divisions; j++)
        {
            float py = (size * j) - meio;
            float py2 = (size * (j + 1)) - meio;

            vertices << px << " " << py << " " << -meio << endl;
            vertices << px << " " << py2 << " " << -meio << endl;
            vertices << px2 << " " << py << " " << -meio << endl;

            vertices << px << " " << py2 << " " << -meio << endl;
            vertices << px2 << " " << py2 << " " << -meio << endl;
            vertices << px2 << " " << py << " " << -meio << endl;

            vertices << px << " " << py << " " << lenght - meio << endl;
            vertices << px2 << " " << py << " " << lenght - meio << endl;
            vertices << px << " " << py2 << " " << lenght - meio << endl;

            vertices << px << " " << py2 << " " << lenght - meio << endl;
            vertices << px2 << " " << py << " " << lenght - meio << endl;
            vertices << px2 << " " << py2 << " " << lenght - meio << endl;
        }
    }
    
    file << vertices.str();
    file.close();
}

void createSphere(float radius, int slices, int stacks,const string &filename) {
    ofstream file = createFile(filename);
    stringstream vertices;

    float px3, pz3, px4, pz4;


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
    float altura = (height / stacks);
    for (int i = 0; i < stacks; i++)
    {
        for (int j = 0; j < slices; j++)
        {
            float angAtual = slice * i;
            float angProx = angAtual + slice;

            float py = i * altura;
            float py2 = (i + 1) * altura;

            float px = radius * sin(angAtual) * (stacks - i) / stacks;
            float pz = radius * cos(angAtual) * (stacks - i) / stacks;
            float px2 = radius * sin(angProx) * (stacks - i) / stacks;
            float pz2 = radius * cos(angProx) * (stacks - i) / stacks;

            float x2 = radius * sin(angAtual) * (stacks - i - 1) / stacks;
            float z2 = radius * cos(angAtual) * (stacks - i - 1) / stacks;
            float x2Prox = radius * sin(angProx) * (stacks - i - 1) / stacks;
            float z2Prox = radius * cos(angProx) * (stacks - i - 1) / stacks;

            vertices << x2 << " " << py2 << " " << z2 << endl;
            vertices << px2 << " " << py << " " << pz2 << endl;
            vertices << x2Prox << " " << py2 << " " << z2Prox << endl;

            vertices << px << " " << py << " " << pz << endl;
            vertices << px2 << " " << py << " " << pz2 << endl;
            vertices << x2 << " " << py2 << " " << z2 << endl;
        }
    }

    for (int i = 0; i < slices; i++)
    {
        float angAtual = slice * i;
        float angProx = angAtual + slice;

        float px = radius * sin(angAtual);
        float pz = radius * cos(angAtual);
        float px2 = radius * sin(angProx);
        float pz2 = radius * cos(angProx);

        vertices << 0 << " " << 0 << " " << 0 << endl;
        vertices << px2 << " " << 0 << " " << pz2 << endl;
        vertices << px << " " << 0 << " " << pz << endl;
    }
    file << vertices.str();
    file.close();
}


void createTorus(float innerRadius, float outerRadius, int sides, int rings, const string &filename) {
    ofstream file = createFile(filename);
    stringstream vertices;

    float ring = (2 * M_PI) / rings;
    float side = (2 * M_PI) / sides;

    float radius_torus = (outerRadius - innerRadius) / 2;
    float center_torus = innerRadius + radius_torus;

    for (int i = 0; i < rings; i++)
    {
        for (int j = 0; j < sides; j++)
        {

            float y = radius_torus * sin(i * ring);
            float next_y = radius_torus * sin((i + 1) * ring);

            float x1 = (center_torus + radius_torus * cos(i * ring)) * cos(j * side);
            float z1 = (center_torus + radius_torus * cos(i * ring)) * sin(j * side);
            float next_x1 = (center_torus + radius_torus * cos(i * ring)) * cos((j + 1) * side);
            float next_z1 = (center_torus + radius_torus * cos(i * ring)) * sin((j + 1) * side);

            float x2 = (center_torus + radius_torus * cos((i + 1) * ring)) * cos(j * side);
            float z2 = (center_torus + radius_torus * cos((i + 1) * ring)) * sin(j * side);
            float next_x2 = (center_torus + radius_torus * cos((i + 1) * ring)) * cos((j + 1) * side);
            float next_z2 = (center_torus + radius_torus * cos((i + 1) * ring)) * sin((j + 1) * side);

            // primeiro triângulo
            vertices << x1 << ' ' << y << ' ' << z1 << '\n';
            vertices << x2 << ' ' << next_y << ' ' << z2 << '\n';
            vertices << next_x1 << ' ' << y << ' ' << next_z1 << '\n';
            // segundo triângulo
            vertices << x2 << ' ' << next_y << ' ' << z2 << '\n';
            vertices << next_x2 << ' ' << next_y << ' ' << next_z2 << '\n';
            vertices << next_x1 << ' ' << y << ' ' << next_z1 << '\n';

            
        }
    }

    file << vertices.str();
    file.close();
}

float bezier(float u, float v, float bezier_matrix[4][4], float control_points_matrix[4][4])
{
    float temp_vec[4];
    float temp_matrix[4][4];
    float final_matrix[4][4];
    float u_vec[4];
    float v_vec[4];

    u_vec[0] = u * u * u; u_vec[1] = u * u; u_vec[2] = u; u_vec[3] = 1;
    v_vec[0] = v * v * v; v_vec[1] = v * v; v_vec[2] = v; v_vec[3] = 1;


    multMatrix(bezier_matrix, control_points_matrix , temp_matrix);
    multMatrix(temp_matrix, bezier_matrix, final_matrix);
    multVector(final_matrix, v_vec, temp_vec);

    float result = 0.0f;
    for(int i=0; i<4; ++i) {
        result += u_vec[i] * temp_vec[i];
    }
    return result;
}


void createBezier(const std::string &patchfile, int tesselation, const std::string &filename)
{
    ifstream patch_stream(patchfile);
    ofstream file = createFile(filename);

    stringstream vertices;
    string line;

    int num_patches;
    getline(patch_stream, line);
    num_patches = stoi(line);


    vector<vector<int>> patch_indices(num_patches, vector<int>(16));
    for (int i = 0; i < num_patches; ++i) {
        getline(patch_stream, line);
        stringstream ss(line);
        string segment;
        for (int j = 0; j < 16; ++j) {
            getline(ss, segment, ',');
            size_t first = segment.find_first_not_of(' ');
            size_t last = segment.find_last_not_of(' ');
            patch_indices[i][j] = stoi(segment.substr(first, (last - first + 1)));
        }
    }

    int num_control_points;
    getline(patch_stream, line);
    num_control_points = stoi(line);

    vector<vector<float>> control_points(num_control_points, vector<float>(3));
    for (int i = 0; i < num_control_points; ++i) {
        getline(patch_stream, line);
        stringstream ss(line);
        string segment;
        for (int j = 0; j < 3; ++j) {
            getline(ss, segment, ',');
            size_t first = segment.find_first_not_of(' ');
            size_t last = segment.find_last_not_of(' ');
            control_points[i][j] = stof(segment.substr(first, (last - first + 1)));
        }
    }
    patch_stream.close();

    float bezier_matrix[4][4] = {
        {-1.0f,  3.0f, -3.0f, 1.0f},
        { 3.0f, -6.0f,  3.0f, 0.0f},
        {-3.0f,  3.0f,  0.0f, 0.0f},
        { 1.0f,  0.0f,  0.0f, 0.0f}
    };


    float step = 1.0f / tesselation;

    for (int p = 0; p < num_patches; ++p) {

        float matrix_x[4][4], matrix_y[4][4], matrix_z[4][4];

        for (int i = 0; i < 4; ++i) { 
            for (int j = 0; j < 4; ++j) {
                int cp_flat_index = i * 4 + j; 
                int control_point_index = patch_indices[p][cp_flat_index]; 

                matrix_x[j][i] = control_points[control_point_index][0];
                matrix_y[j][i] = control_points[control_point_index][1];
                matrix_z[j][i] = control_points[control_point_index][2];
            }
        }

        for (int i = 0; i < tesselation; ++i) { 
            for (int j = 0; j < tesselation; ++j) {

                float u = i * step;
                float v = j * step;
                float u_next = (i + 1) * step;
                float v_next = (j + 1) * step;

                if (i == tesselation - 1) u_next = 1.0f;
                if (j == tesselation - 1) v_next = 1.0f;

                float p00x = bezier(u, v, bezier_matrix, matrix_x);
                float p00y = bezier(u, v, bezier_matrix, matrix_y);
                float p00z = bezier(u, v, bezier_matrix, matrix_z);

                float p10x = bezier(u_next, v, bezier_matrix, matrix_x); 
                float p10y = bezier(u_next, v, bezier_matrix, matrix_y);
                float p10z = bezier(u_next, v, bezier_matrix, matrix_z);

                float p01x = bezier(u, v_next, bezier_matrix, matrix_x); 
                float p01y = bezier(u, v_next, bezier_matrix, matrix_y);
                float p01z = bezier(u, v_next, bezier_matrix, matrix_z);

                float p11x = bezier(u_next, v_next, bezier_matrix, matrix_x);
                float p11y = bezier(u_next, v_next, bezier_matrix, matrix_y);
                float p11z = bezier(u_next, v_next, bezier_matrix, matrix_z);

                vertices << p00x << " " << p00y << " " << p00z << endl;
                vertices << p10x << " " << p10y << " " << p10z << endl;
                vertices << p01x << " " << p01y << " " << p01z << endl;

                vertices << p10x << " " << p10y << " " << p10z << endl;
                vertices << p11x << " " << p11y << " " << p11z << endl;
                vertices << p01x << " " << p01y << " " << p01z << endl;
            }
        }
    }

    file << vertices.str();
    file.close();

}

int main(int argc, char* argv[]) {
    if (argc < 5) {
        cerr << "Usage:\n";
        cerr << "  " << argv[0] << " plane <length> <divisions> <filename>\n";
        cerr << "  " << argv[0] << " sphere <radius> <slices> <stacks> <filename>\n";
        cerr << "  " << argv[0] << " box <length> <divisions> <filename>\n";
        cerr << "  " << argv[0] << " cone <radius> <height> <slices> <stacks> <filename>\n";
        cerr << "  " << argv[0] << " torus <innerRadius> <outerRadius> <slices> <loops> <filename>\n";
        cerr << "  " << argv[0] << " patch <patch_file> <tessellation> <filename>\n";
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

    } else if (shape == "torus") {
        if (argc != 7) {
            cerr << "Usage: " << argv[0] << " torus <innerRadius> <outerRadius> <slices> <loops> <filename>\n";
            return 1;
        }

        float innerRadius = stof(argv[2]);
        float outerRadius = stof(argv[3]);
        int slices = stoi(argv[4]);
        int loops = stoi(argv[5]);
        string filename = argv[6];

        createTorus(innerRadius, outerRadius, slices, loops, filename);

        cout << "Torus file '" << filename << "' generated successfully!\n";

    }else if (shape == "patch") {
        if (argc != 5) {
            cerr << "Usage: " << argv[0] << " patch <patch_file> <tessellation> <filename>\n";
            return 1;
        }
        string patch_filepath = argv[2];
        int tessellation = stoi(argv[3]);
        string output_filename = argv[4];
        createBezier(patch_filepath, tessellation, output_filename);
        cout << "Bezier patch file '3d/" << output_filename << "' generated successfully!\n";
    } else {
        cerr << "Error: Unsupported shape '" << shape << "'\n";
        return 1;
    }

    return 0;
}