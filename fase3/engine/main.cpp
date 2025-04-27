#include <bits/stdc++.h>
#include "tinyxml2.h"
#include "tinyxml2.cpp"
#include <vector>
#include <string>
#include <cmath>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

#include <math.h>

using namespace std;
using namespace tinyxml2;

const char *config_path = "../tests/test_files_phase_3/test_3_1.xml";

int width = 800, height = 800;
float posX = 0, posY = 0, posZ = 0;
float lookX = 0, lookY = 0, lookZ = 0;
float upX = 0, upY = 1, upZ = 0;
float fov = 60, near_ = 1, far_ = 1000;

int startX, startY, tracking = 0;
float alfa = 0.0f, beta_ = 0.0f, radius = 5.0f;
int camera_mode = 0;

float time_elapsed = 0;
int timebase = 0;

struct Point
{
    float x = 0, y = 0, z = 0;
};

enum TransformType
{
    TRANSLATE,
    ROTATE,
    SCALE
};

struct Transform
{
    TransformType type;
    float x = 0, y = 0, z = 0;
    float angle = 0;
    float time = 0;
    bool align = false;
    vector<Point> points;
};

struct Model
{
    string filename;
    GLuint vboId = 0;
    int vertexCount = 0;
    vector<Point> vertices;
};

struct Group
{
    vector<Transform> transforms;
    vector<Model> models;
    vector<Group> childGroups;

    struct Color
    {
        float r = 1, g = 1, b = 1;
    } color;
};

Group rootGroup;

void normalize(Point &v)
{
    float l = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    if (l != 0.0f)
    {
        v.x /= l;
        v.y /= l;
        v.z /= l;
    }
}

Point cross(Point a, Point b)
{
    Point res;
    res.x = a.y * b.z - a.z * b.y;
    res.y = a.z * b.x - a.x * b.z;
    res.z = a.x * b.y - a.y * b.x;
    return res;
}

void buildRotMatrix(Point x, Point y, Point z, float *m)
{

    m[0] = x.x;
    m[4] = y.x;
    m[8] = z.x;
    m[12] = 0;
    m[1] = x.y;
    m[5] = y.y;
    m[9] = z.y;
    m[13] = 0;
    m[2] = x.z;
    m[6] = y.z;
    m[10] = z.z;
    m[14] = 0;
    m[3] = 0;
    m[7] = 0;
    m[11] = 0;
    m[15] = 1;
}

void getCatmullRomPoint(float t, Point p0, Point p1, Point p2, Point p3, Point &pos, Point &deriv)
{

    float m[4][4] = {{-0.5f, 1.5f, -1.5f, 0.5f},
                     {1.0f, -2.5f, 2.0f, -0.5f},
                     {-0.5f, 0.0f, 0.5f, 0.0f},
                     {0.0f, 1.0f, 0.0f, 0.0f}};

    pos.x = 0.0f;
    pos.y = 0.0f;
    pos.z = 0.0f;
    deriv.x = 0.0f;
    deriv.y = 0.0f;
    deriv.z = 0.0f;

    float T[4] = {t * t * t, t * t, t, 1.0f};
    float T_deriv[4] = {3 * t * t, 2 * t, 1.0f, 0.0f};

    Point P[4] = {p0, p1, p2, p3};

    for (int i = 0; i < 4; ++i)
    {
        float a_pos = 0.0f;
        float a_deriv = 0.0f;
        for (int j = 0; j < 4; ++j)
        {
            a_pos += T[j] * m[j][i];
            a_deriv += T_deriv[j] * m[j][i];
        }
        pos.x += a_pos * P[i].x;
        pos.y += a_pos * P[i].y;
        pos.z += a_pos * P[i].z;

        deriv.x += a_deriv * P[i].x;
        deriv.y += a_deriv * P[i].y;
        deriv.z += a_deriv * P[i].z;
    }
}

void getGlobalCatmullRomPoint(float gt, const vector<Point> &controlPoints, Point &pos, Point &deriv)
{
    int POINT_COUNT = controlPoints.size();
    if (POINT_COUNT < 4)
        return;

    float t = gt * POINT_COUNT;
    int index = floor(t);
    t = t - index;

    int indices[4];
    indices[0] = (index + POINT_COUNT - 1) % POINT_COUNT;
    indices[1] = (index) % POINT_COUNT;
    indices[2] = (index + 1) % POINT_COUNT;
    indices[3] = (index + 2) % POINT_COUNT;

    getCatmullRomPoint(t, controlPoints[indices[0]], controlPoints[indices[1]],
                       controlPoints[indices[2]], controlPoints[indices[3]], pos, deriv);
}

void loadModelVBO(Model &model)
{
    string path_model = "../3d/" + model.filename;
    ifstream file(path_model);
    if (!file.is_open())
    {
        cerr << "ERROR loading " << model.filename << " model for VBO" << endl;
        return;
    }

    vector<float> vertexData;
    float x, y, z;
    string line;
    while (getline(file, line))
    {
        istringstream linestream(line);
        if (linestream >> x >> y >> z)
        {
            vertexData.push_back(x);
            vertexData.push_back(y);
            vertexData.push_back(z);
        }
    }
    file.close();

    model.vertexCount = vertexData.size() / 3;
    glGenBuffers(1, &model.vboId);
    glBindBuffer(GL_ARRAY_BUFFER, model.vboId);
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void cameraXML(XMLElement *groupElement, Group &group)
{
    if (!groupElement)
        return;
    XMLElement *transformElement = groupElement->FirstChildElement("transform");
    if (transformElement)
    {
        for (XMLElement *trans = transformElement->FirstChildElement(); trans != nullptr; trans = trans->NextSiblingElement())
        {
            const char *name = trans->Value();
            Transform t;
            if (strcmp(name, "translate") == 0)
            {
                t.type = TRANSLATE;
                trans->QueryFloatAttribute("x", &t.x);
                trans->QueryFloatAttribute("y", &t.y);
                trans->QueryFloatAttribute("z", &t.z);
                trans->QueryFloatAttribute("time", &t.time);
                trans->QueryBoolAttribute("align", &t.align);

                XMLElement *pointElement = trans->FirstChildElement("point");
                if (pointElement)
                {
                    t.x = t.y = t.z = 0;
                    while (pointElement)
                    {
                        Point p;
                        pointElement->QueryFloatAttribute("x", &p.x);
                        pointElement->QueryFloatAttribute("y", &p.y);
                        pointElement->QueryFloatAttribute("z", &p.z);
                        t.points.push_back(p);
                        pointElement = pointElement->NextSiblingElement("point");
                    }
                }
            }
            else if (strcmp(name, "rotate") == 0)
            {
                t.type = ROTATE;
                trans->QueryFloatAttribute("angle", &t.angle);
                trans->QueryFloatAttribute("time", &t.time);
                trans->QueryFloatAttribute("x", &t.x);
                trans->QueryFloatAttribute("y", &t.y);
                trans->QueryFloatAttribute("z", &t.z);
            }
            else if (strcmp(name, "scale") == 0)
            {
                t.type = SCALE;
                t.x = t.y = t.z = 1.0f;
                trans->QueryFloatAttribute("x", &t.x);
                trans->QueryFloatAttribute("y", &t.y);
                trans->QueryFloatAttribute("z", &t.z);
            }
            group.transforms.push_back(t);
        }
    }

    XMLElement *modelsElement = groupElement->FirstChildElement("models");
    if (modelsElement)
    {
        XMLElement *modelElement = modelsElement->FirstChildElement("model");
        while (modelElement)
        {
            const char *filename = modelElement->Attribute("file");
            if (filename)
            {
                Model m;
                m.filename = filename;

                loadModelVBO(m);
                group.models.push_back(m);
            }
            modelElement = modelElement->NextSiblingElement("model");
        }
    }

    XMLElement *colorsElement = groupElement->FirstChildElement("colors");
    if (colorsElement)
    {
        XMLElement *colorElement = colorsElement->FirstChildElement("color");
        if (colorElement)
        {
            colorElement->QueryFloatAttribute("r", &group.color.r);
            colorElement->QueryFloatAttribute("g", &group.color.g);
            colorElement->QueryFloatAttribute("b", &group.color.b);

            if (group.color.r > 1.0f || group.color.g > 1.0f || group.color.b > 1.0f)
            {
                group.color.r /= 255.0f;
                group.color.g /= 255.0f;
                group.color.b /= 255.0f;
            }
        }
    }

    XMLElement *childGroupElement = groupElement->FirstChildElement("group");
    while (childGroupElement)
    {
        Group childGroup;
        cameraXML(childGroupElement, childGroup);
        group.childGroups.push_back(childGroup);
        childGroupElement = childGroupElement->NextSiblingElement("group");
    }
}

void loadScene(const char *filename)
{
    XMLDocument xmldoc;
    xmldoc.LoadFile(filename);
    XMLNode *pworld = xmldoc.FirstChildElement("world");
    XMLElement *windowdata = pworld->FirstChildElement("window");
    windowdata->QueryIntAttribute("width", &width);
    windowdata->QueryIntAttribute("height", &height);

    XMLElement *cameradata = pworld->FirstChildElement("camera");
    if (cameradata)
    {
        XMLElement *position = cameradata->FirstChildElement("position");
        if (position)
        {
            position->QueryFloatAttribute("x", &posX);
            position->QueryFloatAttribute("y", &posY);
            position->QueryFloatAttribute("z", &posZ);
        }
        XMLElement *plookAt = cameradata->FirstChildElement("lookAt");
        if (plookAt)
        {
            plookAt->QueryFloatAttribute("x", &lookX);
            plookAt->QueryFloatAttribute("y", &lookY);
            plookAt->QueryFloatAttribute("z", &lookZ);
        }
        XMLElement *pup = cameradata->FirstChildElement("up");
        if (pup)
        {
            pup->QueryFloatAttribute("x", &upX);
            pup->QueryFloatAttribute("y", &upY);
            pup->QueryFloatAttribute("z", &upZ);
        }
        else
        {
            upX = 0;
            upY = 1;
            upZ = 0;
        }
        XMLElement *projection = cameradata->FirstChildElement("projection");
        if (projection)
        {
            projection->QueryFloatAttribute("fov", &fov);
            projection->QueryFloatAttribute("near", &near_);
            projection->QueryFloatAttribute("far", &far_);
        }
    }

    radius = sqrt(posX * posX + posY * posY + posZ * posZ);
    if (radius > 0)
    {
        beta_ = asin(posY / radius);

        float cosBeta = cos(beta_);
        if (abs(cosBeta) > 1e-6)
        {
            float sinAlphaArg = posX / (radius * cosBeta);

            sinAlphaArg = std::max(-1.0f, std::min(1.0f, sinAlphaArg));
            alfa = asin(sinAlphaArg);

            if (posZ < 0)
            {
                alfa = M_PI - alfa;
            }
            else if (posX < 0 && posZ > 0)
            {
            }
        }
        else
        {
            alfa = 0;
        }
    }
    else
    {
        beta_ = 0;
        alfa = 0;
        radius = 10;

        posX = radius * cos(beta_) * sin(alfa);
        posY = radius * sin(beta_);
        posZ = radius * cos(beta_) * cos(alfa);
    }

    XMLElement *rootGroupElement = pworld->FirstChildElement("group");
    if (rootGroupElement)
    {
        cameraXML(rootGroupElement, rootGroup);
    }
}

void drawModel(const Group &group)
{
    glPushMatrix();
    for (const auto &transform : group.transforms)
    {
        if (transform.type == TRANSLATE)
        {
            if (transform.time > 0 && transform.points.size() >= 4)
            {
                glPushAttrib(GL_CURRENT_BIT);
                glBegin(GL_LINE_STRIP);
                for (int i = 0; i <= 100; ++i)
                {
                    Point curve_pos, curve_deriv;
                    float curve_gt = static_cast<float>(i) / 100;
                    getGlobalCatmullRomPoint(curve_gt, transform.points, curve_pos, curve_deriv);
                    glVertex3f(curve_pos.x, curve_pos.y, curve_pos.z);
                }

                glEnd();
                glPopAttrib();
                Point pos, deriv;
                float gt = fmod(time_elapsed / transform.time, 1.0f);
                getGlobalCatmullRomPoint(gt, transform.points, pos, deriv);
                glTranslatef(pos.x, pos.y, pos.z);

                if (transform.align)
                {
                    Point x_axis, y_axis, z_axis;
                    z_axis = deriv;
                    normalize(z_axis);
                    Point upVec = {0.0f, 1.0f, 0.0f};
                    x_axis = cross(upVec, z_axis);
                    normalize(x_axis);
                    y_axis = cross(z_axis, x_axis);
                    float rotMatrix[16];
                    buildRotMatrix(x_axis, y_axis, z_axis, rotMatrix);
                    glMultMatrixf(rotMatrix);
                }
            }
            else
            {
                glTranslatef(transform.x, transform.y, transform.z);
            }
        }
        else if (transform.type == ROTATE)
        {
            if (transform.time > 0)
            {
                float current_angle = fmod(time_elapsed * (360.0f / transform.time), 360.0f);
                glRotatef(current_angle, transform.x, transform.y, transform.z);
            }
            else
            {
                glRotatef(transform.angle, transform.x, transform.y, transform.z);
            }
        }
        else if (transform.type == SCALE)
        {
            glScalef(transform.x, transform.y, transform.z);
        }
    }
    glColor3f(group.color.r, group.color.g, group.color.b);
    for (const auto &model : group.models)
    {
        if (model.vboId != 0 && model.vertexCount > 0)
        {
            glBindBuffer(GL_ARRAY_BUFFER, model.vboId);
            glVertexPointer(3, GL_FLOAT, 0, 0);
            glDrawArrays(GL_TRIANGLES, 0, model.vertexCount);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
    }
    for (const auto &childGroup : group.childGroups)
    {
        drawModel(childGroup);
    }
    glPopMatrix();
}

void spherical2Cartesian()
{
    if (camera_mode == 0)
    {
        posX = radius * cos(beta_) * sin(alfa);
        posY = radius * sin(beta_);
        posZ = radius * cos(beta_) * cos(alfa);
    }
    else
    {
        float lx = cos(beta_) * sin(alfa);
        float ly = sin(beta_);
        float lz = cos(beta_) * cos(alfa);
        lookX = posX + lx;
        lookY = posY + ly;
        lookZ = posZ + lz;
    }
}

void changeSize(int w, int h)
{
    width = w;
    height = h;
    if (h == 0)
        h = 1;
    float ratio = w * 1.0f / h;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, w, h);
    gluPerspective(fov, ratio, near_, far_);
    glMatrixMode(GL_MODELVIEW);
}

void renderScene()
{

    int time_now = glutGet(GLUT_ELAPSED_TIME);
    float delta_time_sec = (time_now - timebase) / 1000.0f;
    timebase = time_now;
    time_elapsed += delta_time_sec;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    gluLookAt(posX, posY, posZ,
              lookX, lookY, lookZ,
              upX, upY, upZ);

    // with colors use this one :)
    // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    // axis
    //
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

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    drawModel(rootGroup);

    glutSwapBuffers();
}

void processKeys(unsigned char c, int xx, int yy)
{
    float moveSpeed = 0.5f * radius * 0.1;

    switch (c)
    {
    case 'r':
        loadScene(config_path);
        break;
    case 'm':
        camera_mode = 1 - camera_mode;
        if (camera_mode == 0)
        {

            float dx = posX - lookX;
            float dy = posY - lookY;
            float dz = posZ - lookZ;
            radius = sqrt(dx * dx + dy * dy + dz * dz);
            if (radius > 1e-5)
            {
                beta_ = asin(dy / radius);

                float cosBeta = cos(beta_);
                if (abs(cosBeta) > 1e-5)
                {
                    float sinAlphaArg = dx / (radius * cosBeta);
                    sinAlphaArg = std::max(-1.0f, std::min(1.0f, sinAlphaArg));
                    alfa = asin(sinAlphaArg);

                    if (dz < 0)
                    {
                        alfa = M_PI - alfa;
                    }
                }
                else
                {
                    alfa = (dx > 0) ? M_PI / 2.0 : -M_PI / 2.0;
                }
            }
            else
            {
                radius = 5.0;
                alfa = 0;
                beta_ = 0;
            }

            spherical2Cartesian();
        }
        else
        {
            float dx = lookX - posX;
            float dy = lookY - posY;
            float dz = lookZ - posZ;
            float len = sqrt(dx * dx + dy * dy + dz * dz);
            if (len > 1e-5)
            {
                dx /= len;
                dy /= len;
                dz /= len;
                beta_ = asin(dy);

                float cosBeta = cos(beta_);
                if (abs(cosBeta) > 1e-5)
                {
                    float sinAlphaArg = dx / cosBeta;
                    sinAlphaArg = std::max(-1.0f, std::min(1.0f, sinAlphaArg));
                    alfa = asin(sinAlphaArg);

                    if (dz < 0)
                    {
                        alfa = M_PI - alfa;
                    }
                }
                else
                {
                    alfa = (dx > 0) ? M_PI / 2.0 : -M_PI / 2.0;
                }
            }
            else
            {
                alfa = 0;
                beta_ = 0;
            }
        }
        break;

    case 'w':
        if (camera_mode == 1)
        {
            float dx = lookX - posX;
            float dy = lookY - posY;
            float dz = lookZ - posZ;

            float len = sqrt(dx * dx + dy * dy + dz * dz);
            if (len > 0)
            {
                dx /= len;
                dy /= len;
                dz /= len;
            }
            posX += dx * moveSpeed;
            posY += dy * moveSpeed;
            posZ += dz * moveSpeed;

            lookX += dx * moveSpeed;
            lookY += dy * moveSpeed;
            lookZ += dz * moveSpeed;
        }
        break;
    case 's':
        if (camera_mode == 1)
        {
            float dx = lookX - posX;
            float dy = lookY - posY;
            float dz = lookZ - posZ;
            float len = sqrt(dx * dx + dy * dy + dz * dz);
            if (len > 0)
            {
                dx /= len;
                dy /= len;
                dz /= len;
            }
            posX -= dx * moveSpeed;
            posY -= dy * moveSpeed;
            posZ -= dz * moveSpeed;
            lookX -= dx * moveSpeed;
            lookY -= dy * moveSpeed;
            lookZ -= dz * moveSpeed;
        }
        break;
    case 'd':
        if (camera_mode == 1)
        {
            float dx = lookX - posX;
            float dy = lookY - posY;
            float dz = lookZ - posZ;

            float rx = upY * dz - upZ * dy;
            float ry = upZ * dx - upX * dz;
            float rz = upX * dy - upY * dx;

            float len = sqrt(rx * rx + ry * ry + rz * rz);
            if (len > 0)
            {
                rx /= len;
                ry /= len;
                rz /= len;
            }

            posX -= rx * moveSpeed;
            posY -= ry * moveSpeed;
            posZ -= rz * moveSpeed;
            lookX -= rx * moveSpeed;
            lookY -= ry * moveSpeed;
            lookZ -= rz * moveSpeed;
        }
        break;
    case 'a':
        if (camera_mode == 1)
        {
            float dx = lookX - posX;
            float dy = lookY - posY;
            float dz = lookZ - posZ;
            float rx = upY * dz - upZ * dy;
            float ry = upZ * dx - upX * dz;
            float rz = upX * dy - upY * dx;
            float len = sqrt(rx * rx + ry * ry + rz * rz);
            if (len > 0)
            {
                rx /= len;
                ry /= len;
                rz /= len;
            }

            posX += rx * moveSpeed;
            posY += ry * moveSpeed;
            posZ += rz * moveSpeed;
            lookX += rx * moveSpeed;
            lookY += ry * moveSpeed;
            lookZ += rz * moveSpeed;
        }
        break;
    case '+':
        if (camera_mode == 0)
        {
            radius *= 0.9f;
            if (radius < 0.1f)
                radius = 0.1f;
            spherical2Cartesian();
        }
        break;
    case '-':
        if (camera_mode == 0)
        {
            radius *= 1.1f;
            spherical2Cartesian();
        }
        break;
    }
}

void processSpecialKeys(int key, int xx, int yy)
{
    float angleStep = 0.05f;
    float zoomStep = 0.2f;

    if (camera_mode == 0)
    {
        switch (key)
        {
        case GLUT_KEY_RIGHT:
            alfa -= angleStep;
            break;
        case GLUT_KEY_LEFT:
            alfa += angleStep;
            break;
        case GLUT_KEY_UP:
            beta_ += angleStep;
            if (beta_ > M_PI_2 - 0.01f)
                beta_ = M_PI_2 - 0.01f;
            break;
        case GLUT_KEY_DOWN:
            beta_ -= angleStep;

            if (beta_ < -M_PI_2 + 0.01f)
                beta_ = -M_PI_2 + 0.01f;
            break;
        case GLUT_KEY_PAGE_DOWN:
            radius += zoomStep;
            break;
        case GLUT_KEY_PAGE_UP:
            radius -= zoomStep;
            if (radius < 0.1f)
                radius = 0.1f;
            break;
        }
        spherical2Cartesian();
    }
}

void processMouseButtons(int button, int state, int xx, int yy)
{
    if (state == GLUT_DOWN)
    {
        startX = xx;
        startY = yy;
        if (button == GLUT_LEFT_BUTTON)
            tracking = 1;
        else if (button == GLUT_RIGHT_BUTTON)
            tracking = 2;
        else
            tracking = 0;
    }
    else if (state == GLUT_UP)
    {
        tracking = 0;
    }
}

void processMouseMotion(int xx, int yy)
{
    int deltaX = xx - startX;
    int deltaY = yy - startY;

    if (tracking == 1)
    {
        alfa -= deltaX * 0.005f;
        beta_ += deltaY * 0.005f;

        if (beta_ > M_PI_2 - 0.01f)
            beta_ = M_PI_2 - 0.01f;
        else if (beta_ < -M_PI_2 + 0.01f)
            beta_ = -M_PI_2 + 0.01f;

        spherical2Cartesian();
    }
    else if (tracking == 2)
    {
        if (camera_mode == 0)
        {
            radius -= deltaY * 0.1f;
            if (radius < 0.1f)
                radius = 0.1f;
            spherical2Cartesian();
        }
    }

    startX = xx;
    startY = yy;
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(width, height);
    glutCreateWindow("CG G11");
    glewInit();

    loadScene(config_path);

    glutReshapeWindow(width, height);

    glutDisplayFunc(renderScene);
    glutReshapeFunc(changeSize);
    glutIdleFunc(glutPostRedisplay);

    glutKeyboardFunc(processKeys);
    glutSpecialFunc(processSpecialKeys);
    glutMouseFunc(processMouseButtons);
    glutMotionFunc(processMouseMotion);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glEnableClientState(GL_VERTEX_ARRAY);

    timebase = glutGet(GLUT_ELAPSED_TIME);
    glutMainLoop();

    return 0;
}