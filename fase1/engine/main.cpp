#include "tinyxml2.h"



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


void parseXML(const std::string &teste_xml) {
    std::string path = "..\\..\\testes";
    path += teste_xml;
    path += ".xml";

    XMLDocument xmldoc;
    XMLError eResult = xmldoc.LoadFile(path.c_str());
    XML CheckResult(eResult);



    XMLElement* world = doc.FirstChildElement("world");

    if(!world) {
        std::cerr << "No world element found" << std::endl;
        return -1;
    }


    //falta criar as structs para preencher com os atributos
    XMLElement* window = world->FirstChildElement("window");
    if (window == nullptr) return XML_ERROR_PARSING_ELEMENT;


    XMLElement* camera = world->FirstChildElement("camera");
    if (camera == nullptr) return XML_ERROR_PARSING_ELEMENT;

    XMLElement* position = camera->FirstChildElement("position");
    if (position == nullptr) return XML_ERROR_PARSING_ELEMENT;

    XMLElement* lookAt = camera->FirstChildElement("lookAt");
    if (lookAt == nullptr) return XML_ERROR_PARSING_ELEMENT;

    XMLElement* up = camera->FirstChildElement("up");
    if (up == nullptr) return XML_ERROR_PARSING_ELEMENT;

    XMLElement* projection = camera->FirstChildElement("projection");
    if (projection == nullptr) return XML_ERROR_PARSING_ELEMENT;

    XMLElement* group  = world->FirstChildElement("group");
    if (group == nullptr) return XML_ERROR_PARSING_ELEMENT;

    XMLElement* transform = group->FirstChildElement("transform");
    if (transform == nullptr) return XML_ERROR_PARSING_ELEMENT;

    XMLElement* translate = transform->FirstChildElement("translate");
    if (translate == nullptr) return XML_ERROR_PARSING_ELEMENT;

    XMLElement* rotate = transform->FirstChildElement("rotate");
    if (rotate == nullptr) return XML_ERROR_PARSING_ELEMENT;

    XMLElement* scale = transform->FirstChildElement("scale");
    if (scale == nullptr) return XML_ERROR_PARSING_ELEMENT;

    XMLElement* models = group->FirstChildElement("models");
    if (models == nullptr) return XML_ERROR_PARSING_ELEMENT;

    XMLElement* modelList = models -> FirstChildElement("model");
    std::vector<Model> modelVector;
    while(modelList != nullptr) {
        
    }

}

void renderScene() {

}


