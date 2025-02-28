#include "tinyxml2.h"
#include <vector>
#include <string>



#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif


void changeSize(int w, int h) {
    

    if(h == 0)
        h = 1;

    
    float ratio = w * 1.0 / h;


    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glViewport(0, 0, w, h);

    gluPerspective(45.0f ,ratio, 1.0f ,1000.0f);

    glMatrixMode(GL_MODELVIEW);
}




struct WindowData {
    int width;
    int height;
};

struct Position {
    float x;
    float y;
    float z;
};

struct LookAt {
    float x;
    float y;
    float z;
};


struct Up {
    float x;
    float y;
    float z;
};


struct Projection {
    float fov;
    float near;
    float far;
};



struct CameraData {
    Position position;
    LookAt lookAt;
    Up up;
    Projection projection;
};

struct WorldData {
    WindowData window;
    CameraData camera;
    
};





void parseXML(const std::string &teste_xml) {
    std::string path = "..\\..\\testes";
    path += teste_xml;
    path += ".xml";

    XMLDocument xmldoc;
    XMLError eResult = xmldoc.LoadFile(path.c_str());
    XML CheckResult(eResult);



    XMLElement* pworld = doc.FirstChildElement("world");

    if(pworld) {
        
        XMLElement* pwindow = world->FirstChildElement("window");
        if (pwindow) {
            
            window->QueryIntAttribute("width", &windowdata.width);
            window->QueryIntAttribute("height", &windowdata.height);
        }
    
        
        XMLElement* pcamera = world->FirstChildElement("camera");
        if(pcamera) {
            
            XMLElement* position = camera->FirstChildElement("position");
            if (position) {
                position->QueryFloatAttribute("x", &cameraData.position.x);
                position->QueryFloatAttribute("y", &cameraData.position.y);
                position->QueryFloatAttribute("z", &cameraData.position.z);
            }

            XMLElement* plookAt = camera->FirstChildElement("lookAt");
            if (plookAt) {
                lookAt->QueryFloatAttribute("x", &cameraData.lookAt.x);
                lookAt->QueryFloatAttribute("y", &cameraData.lookAt.y);
                lookAt->QueryFloatAttribute("z", &cameraData.lookAt.z);
            }

            XMLElement* pup = camera->FirstChildElement("up");
            if (pup) {
                up->QueryFloatAttribute("x", &cameraData.up.x);
                up->QueryFloatAttribute("y", &cameraData.up.y);
                up->QueryFloatAttribute("z", &cameraData.up.z);
            }

            XMLElement* projection = camera->FirstChildElement("projection");
            if (projection) {
                projection->QueryFloatAttribute("fov", &cameraData.projection.fov);
                projection->QueryFloatAttribute("near", &cameraData.projection.near);
                projection->QueryFloatAttribute("far", &cameraData.projection.far);
            }
        }

        XMLElement* pgroup = world->FirstChildElement("group");
        if(pgroup) {

            XMLElement* pmodels = group->FirstChildElement("models");
            std::vector<std::string> files;


            if(pmodels) {

                XMLElement* pmodel = models->FirstChildElement("model");

                
                while(pmodel) {
                    const char* fileAttr = pmodel->Attribute("file");
                    if(fileAttr){
                        files.push_back(fileAttr);
                    }
                    
                    pmodel = pmodel->NextSiblingElement("model");
                }
            }
        }
    
    }

}

void renderScene() {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();

    gluLookAt(cameraData.position.x, cameraData.position.y, cameraData.position.z,
              cameraData.lookAt.x, cameraData.lookAt.y, cameraData.lookAt.z,
              cameraData.up.x, cameraData.up.y, cameraData.up.z);

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

    glSwapBuffers();
}


