#include <bits/stdc++.h>
#include "tinyxml2.h"
#include "tinyxml2.cpp"
#include <vector>
#include <string>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

using namespace std;
using namespace tinyxml2;

int width=800,height=800;
float posX=0, posY=0, posZ=0;
float lookX=0, lookY=0, lookZ=0;
float upX=0, upY=0, upZ=0;

float fov=60, near=1, far=1000;


struct Group {
    float translateX = 0, translateY = 0, translateZ = 0;
    float rotateX = 0, rotateY = 0, rotateZ = 0, angle = 0;
    float scaleX = 1, scaleY = 1, scaleZ = 1;
    std::vector<std::string> models;
    std::vector<Group> subgroups;
};



float alfa = 0.0f, beta_ = 0.0f, radius = 5.0f;


void spherical2Cartesian() {
    radius = sqrt((posX * posX) + (posY * posY) + (posZ * posZ));
	posX = radius * cos(beta_) * sin(alfa);
	posY = radius * sin(beta_);
	posZ = radius * cos(beta_) * cos(alfa);
}


void changeSize(int w, int h)
{
	if (h == 0)
		h = 1;
	float ratio = w * 1.0f / h;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, w, h);
	gluPerspective(fov, ratio, near, far);
	glMatrixMode(GL_MODELVIEW);
}


std::vector<std::string> toDrawModels;
std::vector<std::tuple<float, float, float>> translations;
std::vector<std::tuple<float, float, float, float>> rotations;
std::vector<std::tuple<float, float, float>> scales;


std::vector<Group> groups; // Armazena os grupos principais


void parseXML(void) {
    const char* path = "../../xml//test_2_3.xml";

    XMLDocument xmldoc;
    xmldoc.LoadFile(path);
    XMLNode* pworld = xmldoc.FirstChild();

    if(pworld) {


        XMLElement* windowdata = pworld->FirstChildElement("window");
        if (windowdata) {
            windowdata->QueryIntAttribute("width", &width);
            windowdata->QueryIntAttribute("height", &height);
        }


    
        XMLElement* cameradata = pworld->FirstChildElement("camera");
        if(cameradata) {


            
            XMLElement* position = cameradata->FirstChildElement("position");
            if (position) {
                position->QueryFloatAttribute("x", &posX);
                position->QueryFloatAttribute("y", &posY);
                position->QueryFloatAttribute("z", &posZ);

				radius = sqrt((posX * posX) + (posY * posY) + (posZ * posZ));
				beta_ = asin(posY / radius);
				alfa = asin(posX / (radius * cos(beta_)));
            }




            XMLElement* plookAt = cameradata->FirstChildElement("lookAt");
            if (plookAt) {
                plookAt->QueryFloatAttribute("x", &lookX);
                plookAt->QueryFloatAttribute("y", &lookY);
                plookAt->QueryFloatAttribute("z", &lookZ);
            }



            XMLElement* pup = cameradata->FirstChildElement("up");
            if (pup) {
                pup->QueryFloatAttribute("x", &upX);
                pup->QueryFloatAttribute("y", &upY);
                pup->QueryFloatAttribute("z", &upZ);
            }




            XMLElement* projection = cameradata->FirstChildElement("projection");
            if (projection) {
                projection->QueryFloatAttribute("fov", &fov);
                projection->QueryFloatAttribute("near", &near);
                projection->QueryFloatAttribute("far", &far);
            }
        }


        // Processa grupos de forma recursiva
        std::vector<XMLElement*> stack;
        XMLElement* pgroup = pworld->FirstChildElement("group");

        if (pgroup) stack.push_back(pgroup);

        while (!stack.empty()) {
            XMLElement* currentGroup = stack.back();
            stack.pop_back();

            // Transformações
            float translateX = 0, translateY = 0, translateZ = 0;
            float rotateX = 0, rotateY = 0, rotateZ = 0, angle = 0;
            float scaleX = 1, scaleY = 1, scaleZ = 1;

            XMLElement* ptransform = currentGroup->FirstChildElement("transform");
            if (ptransform) {
                XMLElement* ptranslate = ptransform->FirstChildElement("translate");
                if (ptranslate) {
                    ptranslate->QueryFloatAttribute("x", &translateX);
                    ptranslate->QueryFloatAttribute("y", &translateY);
                    ptranslate->QueryFloatAttribute("z", &translateZ);
                }

                XMLElement* protate = ptransform->FirstChildElement("rotate");
                if (protate) {
                    protate->QueryFloatAttribute("axisX", &rotateX);
                    protate->QueryFloatAttribute("axisY", &rotateY);
                    protate->QueryFloatAttribute("axisZ", &rotateZ);
                    protate->QueryFloatAttribute("angle", &angle);
                }

                XMLElement* pscale = ptransform->FirstChildElement("scale");
                if (pscale) {
                    pscale->QueryFloatAttribute("x", &scaleX);
                    pscale->QueryFloatAttribute("y", &scaleY);
                    pscale->QueryFloatAttribute("z", &scaleZ);
                }
            }

            // Armazena transformações
            translations.push_back({translateX, translateY, translateZ});
            rotations.push_back({angle, rotateX, rotateY, rotateZ});
            scales.push_back({scaleX, scaleY, scaleZ});

            // Modelos
            XMLElement* pmodels = currentGroup->FirstChildElement("models");
            if (pmodels) {
                XMLElement* pmodel = pmodels->FirstChildElement("model");
                while (pmodel) {
                    const char* model = pmodel->Attribute("file");
                    if (model) {
                        toDrawModels.push_back(model);
                    }
                    pmodel = pmodel->NextSiblingElement("model");
                }
            }

            // Subgrupos
            XMLElement* subgroup = currentGroup->FirstChildElement("group");
            while (subgroup) {
                stack.push_back(subgroup);
                subgroup = subgroup->NextSiblingElement("group");
            }
        }
    }
}




void drawModel(void)
{
    for (size_t modelIndex = 0; modelIndex < toDrawModels.size(); modelIndex++)
    {
        string t = toDrawModels[modelIndex];  // Pegamos o nome do modelo
        string path_model = "../../3d/" + t;
        ifstream file(path_model);
        float x, y, z;

        if (!file.is_open())
        {
            cerr << "ERROR loading " << t << " model" << endl;
        }
        else
        {
            // Salva o estado atual da matriz de transformações
            glPushMatrix();

            // Aplicar Translação
            if (modelIndex < translations.size()) {
                glTranslatef(
                    get<0>(translations[modelIndex]),
                    get<1>(translations[modelIndex]),
                    get<2>(translations[modelIndex])
                );
            }

            // Aplicar Rotação
            if (modelIndex < rotations.size()) {
                glRotatef(
                    get<0>(rotations[modelIndex]), // ângulo
                    get<1>(rotations[modelIndex]), // eixo X
                    get<2>(rotations[modelIndex]), // eixo Y
                    get<3>(rotations[modelIndex])  // eixo Z
                );
            }

            // Aplicar Escala
            if (modelIndex < scales.size()) {
                glScalef(
                    get<0>(scales[modelIndex]),
                    get<1>(scales[modelIndex]),
                    get<2>(scales[modelIndex])
                );
            }

            // Desenhar o modelo
            glBegin(GL_TRIANGLES);
            glColor3f(1.0f, 1.0f, 1.0f);
            string line;
            while (getline(file, line))
            {
                istringstream linestream(line);
                if (linestream >> x >> y >> z)
                {
                    glVertex3f(x, y, z);
                }
            }
            glEnd();

            // Restaura a matriz de transformações
            glPopMatrix();

            file.close();
        }
    }
}


void renderScene() {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(posX,posY,posZ,
              lookX,lookY,lookZ,
              upX,upY,upZ);
    glPolygonMode(GL_FRONT, GL_LINE);
    spherical2Cartesian();
    // axis
    glBegin(GL_LINES);

    
    glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(-100.0f, 0.0f, 0.0f);
	glVertex3f(100.0f, 0.0f, 0.0f);

	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, -100.0f, 0.0f);
	glVertex3f(0.0f, 100.0f, 0.0f);

	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, -100.0f);
	glVertex3f(0.0f, 0.0f, 100.0f);

	glEnd();
    
    drawModel();

    glutSwapBuffers();
}

void processKeys(unsigned char c, int xx, int yy) {
    switch (c) {
        case '+':
        	posX *= 0.8;
			posY *= 0.8;
			posZ *= 0.8;
            spherical2Cartesian();
            glutPostRedisplay();
        break;
        case '-':
            posX /= 0.8;
			posY /= 0.8;
			posZ /= 0.8;
            spherical2Cartesian();
            glutPostRedisplay();
        break;
        default:
        break;
    }    
}


void processSpecialKeys(int key, int xx, int yy) {
	switch (key) {

	case GLUT_KEY_RIGHT:
		alfa -= 0.1; break;

	case GLUT_KEY_LEFT:
		alfa += 0.1; break;

	case GLUT_KEY_UP:
		beta_ += 0.1f;
		if (beta_ > 1.5f)
			beta_ = 1.5f;
		break;

	case GLUT_KEY_DOWN:
		beta_ -= 0.1f;
		if (beta_ < -1.5f)
			beta_ = -1.5f;
		break;

	case GLUT_KEY_PAGE_DOWN: radius -= 0.1f;
		if (radius < 0.1f)
			radius = 0.1f;
		break;

	case GLUT_KEY_PAGE_UP: 
        radius += 0.1f; 
        break;
	}
	spherical2Cartesian();
    glutPostRedisplay();
}


int main(int argc, char** argv)
{
	parseXML();
    
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(width, height);
	glutCreateWindow("CG_G11");
	
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);

    glutKeyboardFunc(processKeys);
	glutSpecialFunc(processSpecialKeys);
    
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	
	glutMainLoop();
	
	return 1;
}


