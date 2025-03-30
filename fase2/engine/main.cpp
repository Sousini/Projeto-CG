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

struct Group {
    float translateX = 0, translateY = 0, translateZ = 0;
    float rotateX = 0, rotateY = 0, rotateZ = 0, angle = 0;
    float scaleX = 1, scaleY = 1, scaleZ = 1;
    float r = 0, g = 0, b = 0;
    std::vector<std::string> models;
    std::vector<Group> subgroups;
};

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
std::vector<std::tuple<float, float, float>> translations;
std::vector<std::tuple<float, float, float, float>> rotations;
std::vector<std::tuple<float, float, float>> scales;
std::vector<std::tuple<float, float, float>> colors;


std::vector<Group> groups; // Armazena os grupos principais
const char* path = "../../xml/solar_system.xml";

void parseXML(void) {
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
/*
//cores
float r = 0, g = 0, b = 0;

XMLElement* pcolors = currentGroup->FirstChildElement("colors");
if (pcolors) {
    XMLElement* pcolor = pcolors->FirstChildElement("color");
    while (pcolor) {
        pcolor->QueryFloatAttribute("r", &r);
        pcolor->QueryFloatAttribute("g", &g);
        pcolor->QueryFloatAttribute("b", &b);
    }
}
colors.push_back({r, g, b});
*/


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

void drawModel() {

    // Recursive function to process groups
    // Takes the TinyXML element for the current group
    std::function<void(XMLElement*)> processGroup =
        [&](XMLElement* groupElement) { // Pass the XML element
        if (!groupElement) return;

        glPushMatrix(); // Save the current transformation state

        // Process transformations IN ORDER
        XMLElement* transformElement = groupElement->FirstChildElement("transform");
        if (transformElement) {
            // Iterate through transform elements sequentially
            for (XMLElement* trans = transformElement->FirstChildElement(); trans != nullptr; trans = trans->NextSiblingElement()) {
                const char* name = trans->Value(); // Get tag name (translate, rotate, scale)

                if (strcmp(name, "translate") == 0) {
                    float x = 0, y = 0, z = 0;
                    trans->QueryFloatAttribute("x", &x);
                    trans->QueryFloatAttribute("y", &y);
                    trans->QueryFloatAttribute("z", &z);
                    glTranslatef(x, y, z); // Apply translation
                } else if (strcmp(name, "rotate") == 0) {
                    float angle = 0, x = 0, y = 0, z = 0;
                    trans->QueryFloatAttribute("angle", &angle);
                    // Make sure attribute names match XML (x, y, z for axis)
                    trans->QueryFloatAttribute("x", &x);
                    trans->QueryFloatAttribute("y", &y);
                    trans->QueryFloatAttribute("z", &z);
                    glRotatef(angle, x, y, z); // Apply rotation
                } else if (strcmp(name, "scale") == 0) {
                    float x = 1, y = 1, z = 1; // Default scale is 1
                    trans->QueryFloatAttribute("x", &x);
                    trans->QueryFloatAttribute("y", &y);
                    trans->QueryFloatAttribute("z", &z);
                    glScalef(x, y, z); // Apply scaling
                }
                // Add other transform types if needed
            }
        }

        

        // Draw models in this group
        XMLElement* models = groupElement->FirstChildElement("models");
        if (models) {
            XMLElement* model = models->FirstChildElement("model");
            while (model) {
                const char* filename = model->Attribute("file");
                if (filename) {
                    string path_model = "../../3d/" + string(filename); // Adjust path as needed
                    ifstream file(path_model);

                    if (!file.is_open()) {
                        cerr << "ERROR loading " << filename << " model" << endl;
                        model = model->NextSiblingElement("model");
                        continue;
                    }

                    // Set color before drawing primitives
                    glColor3f(1.0f, 1.0f, 1.0f); // White lines/triangles

                    // Consider using Vertex Buffer Objects (VBOs) for performance
                    // For now, immediate mode:
                    
                    glBegin(GL_TRIANGLES);
                    float x, y, z;
                    string line;
                    while (getline(file, line)) {
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
                            glColor3f(r, g, b);
                            glVertex3f(x, y, z);
                        } else {
                             // Handle potential errors in file format
                             cerr << "Warning: Could not parse line in " << filename << ": " << line << endl;
                        }
                    }
                    glEnd();
                    
                    file.close();
                }
                model = model->NextSiblingElement("model");
            }
        }

        // Process subgroups recursively
        XMLElement* subgroup = groupElement->FirstChildElement("group");
        while (subgroup) {
            // No need for extra push/pop here, the recursive call handles it
            processGroup(subgroup); // Recurse into the subgroup
            subgroup = subgroup->NextSiblingElement("group");
        }

        glPopMatrix(); // Restore the transformation state from before this group
    };

    // --- Load XML and Start Processing ---
    // You should only load/parse the XML once, ideally not in the draw loop
    // For this example structure, we load it here to start the recursion
    // Or whichever file you want to render
    XMLDocument xmldoc;
    if (xmldoc.LoadFile(path) != XML_SUCCESS) {
         cerr << "Error loading XML file: " << path << endl;
         // Handle error appropriately, maybe draw nothing or show an error message
         return;
    }

    XMLElement* world = xmldoc.FirstChildElement("world");
    if (!world) {
        cerr << "Error: <world> tag not found in XML." << endl;
        return;
    }

    // Find the first top-level group and start processing
    XMLElement* rootGroupElement = world->FirstChildElement("group");
    if (rootGroupElement) {
        processGroup(rootGroupElement);
    } else {
        cerr << "Warning: No top-level <group> found in XML." << endl;
    }
    // --- End XML Processing ---
}

// }


void renderScene() {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(posX,posY,posZ,
              lookX,lookY,lookZ,
              upX,upY,upZ);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    spherical2Cartesian();
    // axis
    /*
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
    */
    
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
            parseXML();
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

    float camera_rotationX = 4.5 * 3.14 / width;
	float camera_rotationY = 3 * 3.14 / height;
	if (camera_mode)
	{
		camera_rotationX = 10 * 3.14 / width;
		camera_rotationY = 10 * 3.14 / height;
	}

	if (camera_mode)
	{
		if (xx > startX) {
			alfa -= camera_rotationX;
			startX = xx;
		}
		if (xx < startX) {
			alfa += camera_rotationX;
			startX = xx;
		}

		if (yy > startY && beta_ >= -(3.14 / 2)) {
			beta_ -= camera_rotationY;
			startY = yy;
		}
	    if (yy < startY && beta_ <= (3.14 / 2)) {
			beta_ += camera_rotationY;
			startY = yy;
		}
		startY = yy;
		startX = xx;
		glutPostRedisplay();
        spherical2Cartesian();
	}
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
    glutMouseFunc(processMouseButtons);
	glutMotionFunc(processMouseMotion);
    
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	
	glutMainLoop();
	
	return 1;
}


