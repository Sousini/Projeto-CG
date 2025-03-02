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

//Variaveis da Projeção
float fov=60, near=1, far=1000;
string toDrawModels="";

float alfa = 0.0f, beta_ = 0.0f, radius = 5.0f;


void spherical2Cartesian() {
    radius = sqrt((posX * posX) + (posY * posY) + (posZ * posZ));
	posX = radius * cos(beta_) * sin(alfa);
	posY = radius * sin(beta_);
	posZ = radius * cos(beta_) * cos(alfa);
}


void changeSize(int w, int h)
{
	// Prevent a divide by zero, when window is too short
	// (you can�t make a window with zero width).
	if (h == 0)
		h = 1;
	// compute window's aspect ratio
	float ratio = w * 1.0f / h;
	// Set the projection matrix as current
	glMatrixMode(GL_PROJECTION);
	// Load the identity matrix
	glLoadIdentity();
	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);
	// Set the perspective
	gluPerspective(fov, ratio, near, far);
	// return to the model view matrix mode
	glMatrixMode(GL_MODELVIEW);
}

void parseXML(void) {
    const char* path = "../tests/test_files_phase_1/test_1_5.xml";

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

        XMLElement* pgroup = pworld->FirstChildElement("group");
        if(pgroup) {

            XMLElement* pmodels = pgroup->FirstChildElement("models");
            if(pmodels) {
                XMLElement* pmodel = pmodels->FirstChildElement("model");
                while(pmodel) {
                    toDrawModels += pmodel->Attribute("file");             
                    toDrawModels += ";";
                    pmodel = pmodel->NextSiblingElement("model");
                }
            }
        }
    
    }

}

void drawModel(void)
{
	std::string model_name="";
    string t;
	int i = 0;
    stringstream ss(toDrawModels);
	//Para cada modelo, desenhar o que está no path + model
	while (getline(ss,t,';'))
	{
		model_name = t;
		std::string line;
		std::string path_model = "../3d/" + model_name;
		ifstream file(path_model);
		float x, y, z;

		if (!file.is_open())
		{
            cout << "ERROR loading " << t << " model";
		}else{
            glBegin(GL_TRIANGLES);
            glColor3f(1.0f, 1.0f, 1.0f);
			while (getline(file, line))
			{
                istringstream linestream(line);
                if (linestream >> x >> y >> z)
                {
                    glVertex3f(x, y, z);
                }
			}
			glEnd();
			
        }
        file.close();
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

	case GLUT_KEY_PAGE_UP: radius += 0.1f; break;
	}
	spherical2Cartesian();
    glutPostRedisplay();
}


int main(int argc, char** argv)
{
	//load xml
	parseXML();
    
	// Glut init
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(width, height);
	glutCreateWindow("CG_TP16_phase1");
	
	// Callbacks
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);
    glutKeyboardFunc(processKeys);
	glutSpecialFunc(processSpecialKeys);
    

	// Settings OpenGL
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	
	// Run
	glutMainLoop();
	
	return 1;
}


