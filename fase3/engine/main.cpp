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


int startX, startY, tracking = 0;
float fov=60, near=1, far=1000;
int camera_mode = 0;


float alfa = 0.0f, beta_ = 0.0f, radius = 5.0f;

void spherical2Cartesian() {
    radius = sqrt((posX * posX) + (posY * posY) + (posZ * posZ));
    if(camera_mode){
        lookX = radius * cos(beta_) * sin(alfa);
        lookZ = radius * cos(beta_) * cos(alfa);
        lookY = radius * sin(beta_);
    }else{
        posX = radius * cos(beta_) * sin(alfa);
        posY = radius * sin(beta_);
        posZ = radius * cos(beta_) * cos(alfa);
    }

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
const char* path = "../tests/test_files_phase_3/test_3_2.xml";

void cameraXML(void) {
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
    }else {
        cerr << "ERROR loading XML file: " << path << endl;
    }
}

void drawModel() {
    // Processa os grupos de forma recursiva
    std::function<void(XMLElement*)> processGroup = [&](XMLElement* groupElement) {
        if (!groupElement) return;

        glPushMatrix();
        XMLElement* transformElement = groupElement->FirstChildElement("transform");
        if (transformElement) {
            for (XMLElement* trans = transformElement->FirstChildElement(); trans != nullptr; trans = trans->NextSiblingElement()) {
                const char* name = trans->Value(); //(translate, rotate, scale)

                if (strcmp(name, "translate") == 0) {
                    float x = 0, y = 0, z = 0;
                    trans->QueryFloatAttribute("x", &x);
                    trans->QueryFloatAttribute("y", &y);
                    trans->QueryFloatAttribute("z", &z);
                    glTranslatef(x, y, z); // Aplica translate
                } else if (strcmp(name, "rotate") == 0) {
                    float angle = 0, x = 0, y = 0, z = 0;
                    trans->QueryFloatAttribute("angle", &angle);
                    trans->QueryFloatAttribute("x", &x);
                    trans->QueryFloatAttribute("y", &y);
                    trans->QueryFloatAttribute("z", &z);
                    glRotatef(angle, x, y, z); // Aplica rotate
                } else if (strcmp(name, "scale") == 0) {
                    float x = 1, y = 1, z = 1; 
                    trans->QueryFloatAttribute("x", &x);
                    trans->QueryFloatAttribute("y", &y);
                    trans->QueryFloatAttribute("z", &z);
                    glScalef(x, y, z); // Aplica scale
                }
            }
        }
        // Desenho dos modelos
        XMLElement* models = groupElement->FirstChildElement("models");
        if (models) {
            XMLElement* model = models->FirstChildElement("model");
            while (model) {
                const char* filename = model->Attribute("file");
                if (filename) {
                    string path_model = "../3d/" + string(filename);
                    ifstream file(path_model);

                    if (!file.is_open()) {
                        cerr << "ERROR loading " << filename << " model" << endl;
                        model = model->NextSiblingElement("model");
                        continue;
                    }

                    glBegin(GL_TRIANGLES);
                    float x, y, z;
                    string line;
                    while (getline(file, line)) {
                        glColor3f(1.0f, 1.0f, 1.0f);
                        XMLElement* colorsElement = groupElement->FirstChildElement("colors");
                        float r = 0, g = 0, b = 0;
                        if(colorsElement) {
                            for(XMLElement* col = colorsElement->FirstChildElement("color"); col != nullptr; col = col->NextSiblingElement()) {
                                if(strcmp(col->Value(), "color") == 0) {
                                    col->QueryFloatAttribute("r", &r);
                                    col->QueryFloatAttribute("g", &g);
                                    col->QueryFloatAttribute("b", &b);
                                }
                            }
                        }
                        istringstream linestream(line);
                        if (linestream >> x >> y >> z) {
                            if(colorsElement) glColor3f(r, g, b);
                            glVertex3f(x, y, z);
                        }
                    }
                    glEnd();
                    
                    file.close();
                }
                model = model->NextSiblingElement("model");
            }
        }
        // Processa os subgrupos de forma recursiva
        XMLElement* subgroup = groupElement->FirstChildElement("group");
        while (subgroup) {
            processGroup(subgroup); // Recursiva
            subgroup = subgroup->NextSiblingElement("group");
        }
        glPopMatrix();
    };

    XMLDocument xmldoc;
    xmldoc.LoadFile(path);
    XMLElement* world = xmldoc.FirstChildElement("world");
    XMLElement* rootGroupElement = world->FirstChildElement("group");
    if (rootGroupElement) {
        processGroup(rootGroupElement);
    }
}

void renderScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(posX,posY,posZ,
              lookX,lookY,lookZ,
              upX,upY,upZ);
    // with colors use this one :)
    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    // -------------------------------------

    // with lines plus axis
    glPolygonMode(GL_FRONT, GL_LINE);
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
    // -------------------------------------
    spherical2Cartesian();  
    drawModel();
    glutSwapBuffers();
}

void processKeys(unsigned char c, int xx, int yy) {
    switch (c) {
        case '+':
            if(!camera_mode){
                posX *= 0.8;
                posY *= 0.8;
                posZ *= 0.8;
                spherical2Cartesian();
                glutPostRedisplay();
            }
        break;
        case 'r':
            cameraXML();
            camera_mode = 0;
            spherical2Cartesian();
            glutPostRedisplay();
            break;
        case '-':
            if(!camera_mode){
                posX /= 0.8;
                posY /= 0.8;
                posZ /= 0.8;
                spherical2Cartesian();
                glutPostRedisplay();
            }
        break;
        case 'm':
            camera_mode = (camera_mode + 1) % 2;
            break;
        case 'w':
            if (camera_mode)
            {
                posX += (lookX * 0.05);
                posY += (lookY * 0.05);
                posZ += (lookZ * 0.05);
                spherical2Cartesian();
                glutPostRedisplay();
            }
		break;
	    case 's':
            if (camera_mode)
            {
                posX -= (lookX * 0.05);
                posY -= (lookY * 0.05);
                posZ -= (lookZ * 0.05);
                spherical2Cartesian();
                glutPostRedisplay();
            }
		break;
        default:
        break;
    }    
}

void processSpecialKeys(int key, int xx, int yy) {
    if(!camera_mode){
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
}

void processMouseButtons(int button, int state, int xx, int yy) {
	if (state == GLUT_DOWN && camera_mode)  {
		startX = xx;
		startY = yy;
		if (button == GLUT_LEFT_BUTTON)
			tracking = 1;
		else if (button == GLUT_RIGHT_BUTTON)
			tracking = 2;
		else
			tracking = 0;
	}
	else if (state == GLUT_UP && camera_mode) {
		if (tracking == 1) {
			alfa += (xx - startX);
			beta_ += (yy - startY);
		}
		else if (tracking == 2) {
			
			radius -= yy - startY;
			if (radius < 3)
				radius = 3.0;
		}
		tracking = 0;
	}
}

void processMouseMotion(int xx, int yy) {
	if (camera_mode)
	{
		float X = 10 * 3.14 / width;
		float Y = 10 * 3.14 / height;

		if (xx > startX) {
			alfa -= X;
			startX = xx;
		}
		if (xx < startX) {
			alfa += X;
			startX = xx;
		}

		if (yy > startY && beta_ >= -(3.14 / 2)) {
			beta_ -= Y;
			startY = yy;
		}
	    if (yy < startY && beta_ <= (3.14 / 2)) {
			beta_ += Y;
			startY = yy;
		}
		startX = xx;
		startY = yy;
		glutPostRedisplay();
        spherical2Cartesian();
	}
}


int main(int argc, char** argv)
{
	cameraXML();
    
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(width, height);
	glutCreateWindow("CG_G11");
	
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);

    glutKeyboardFunc(processKeys);
	glutSpecialFunc(processSpecialKeys);
    glutMouseFunc(processMouseButtons);
	glutMotionFunc(processMouseMotion);
    
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	
	glutMainLoop();
	
	return 1;
}

