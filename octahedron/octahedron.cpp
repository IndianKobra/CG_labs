#include <GL/glut.h>
#include <iostream>
#include <sstream>
#include "SOIL.h"

#define WIN_HEIGHT 750
#define WIN_WIDTH 750
#define textures_count 8

GLuint textures[textures_count];
GLuint cutOctahedron;

// sphere rotation params
GLfloat sun_rotation = 1;
GLfloat dx_sun_rotation = 1;
GLfloat light_pos[] = {0, 0, 1, 1};

GLfloat opened = 0; // раздвигание граней куба
const GLfloat oct_side_len = 2.0f;
const int partitions_number = 4;

GLfloat ox_rotation = 0.0f;
GLfloat dx_rotation = 0.5f;
GLfloat oy_rotation = 0.0f;
GLfloat dy_rotation = 0.0f;

bool visibility = true;
bool light = true;
bool cut_oct = false;

enum class COLOR_MODE : int {
    COLOR = 0,
    DIFFERENT_TEXTURES = 1,
    ONE_TEXTURE = 2
};

namespace KEY {
    const char TEXTURE = 't';
    const char VISIBLE = 'x';
    const char OPEN_OCT = 'o';
    const char LIGHT_MODE = 'l';
    const char CUT_OCT = 'c';
    const char STOP = 'q';
    const char TURN_UP = 'w';
    const char TURN_DOWN = 's';
    const char TURN_LEFT = 'a';
    const char TURN_RIGHT = 'd';
}

COLOR_MODE texture_mode = COLOR_MODE::COLOR;

void draw_colored_oct();

void draw_cut_triangles();

void Draw();

void create_cut_triangles_list(float);

void change_size(GLsizei, GLsizei);

void timer(int);

void glutNormalKeys(unsigned char, int, int);


void textures_init() {
    int width[textures_count],
            height[textures_count];

    glGenTextures(textures_count, textures);
    unsigned char *images[textures_count];

    std::stringstream filename;
    for (int i = 0; i < 8; i++) {
        filename << "../textures/txt" << i << ".bmp";
        images[i] = SOIL_load_image(filename.str().c_str(), &width[i], &height[i],
                                    nullptr, SOIL_LOAD_RGB);

        glBindTexture(GL_TEXTURE_2D, textures[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width[i], height[i], 0,
                     GL_RGB, GL_UNSIGNED_BYTE, images[i]);

        // texture rendering
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        filename.str("");
    }

    for (auto &image : images) {
        SOIL_free_image_data(image);
        glBindTexture(GL_TEXTURE_2D, 0); // отвязка объекта
    }
}

void light_init() {
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);

    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
}

int main(int argc, char **argv) {
    using namespace KEY;
    std::cout << TURN_UP << " - turn up\n"
              << TURN_DOWN << " - turn down\n"
              << TURN_LEFT << " - turn left\n"
              << TURN_RIGHT << " - turn right\n"
              << STOP << " - stop rotation\n"
              << OPEN_OCT << " - open octahedron\n"
              << TEXTURE << " - enumerate texture mods\n"
              << VISIBLE << " - transparency control\n"
              << LIGHT_MODE << " - light control\n"
              << CUT_OCT << " - cut octahedron faces\n";

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    glutInitWindowSize(WIN_WIDTH, WIN_HEIGHT);
    glutCreateWindow("Octahedron");

    cutOctahedron = glGenLists(1);
    create_cut_triangles_list(oct_side_len / partitions_number);

    // Callback functions init
    glutDisplayFunc(Draw);
    glutReshapeFunc(change_size);
    glutTimerFunc(5, timer, 0);
    glutKeyboardFunc(glutNormalKeys);

    light_init();
    textures_init();

    glutMainLoop();

    return 0;
}

void draw_colored_oct() {
    glLoadIdentity();

    glTranslatef(0, 0, -5); // перемещаем объект по z для "вытягивания" октаэдра
    glRotatef((ox_rotation), 0, 1, 0);
    glRotatef((oy_rotation), 1, 0, 0);

    GLfloat oct_vertices[] = {
            -oct_side_len - opened, 0.0f + opened, 0.0f + opened, //0 передняя грань
            0.0f - opened, oct_side_len + opened, 0.0f + opened,  //1
            0.0f - opened, 0.0f + opened, oct_side_len + opened,  //2

            0.0f + opened, 0.0f + opened, oct_side_len + opened,  //2 правая боковая
            0.0f + opened, oct_side_len + opened, 0.0f + opened,  //1
            oct_side_len + opened, 0.0f + opened, 0.0f + opened,  //3

            oct_side_len + opened, 0.0f + opened, 0.0f - opened,  //3 задняя грань
            0.0f + opened, oct_side_len + opened, 0.0f - opened,  //1
            0.0f + opened, 0.0f + opened, -oct_side_len - opened, //4

            0.0f - opened, 0.0f + opened, -oct_side_len - opened, //4 левая боковая
            0.0f - opened, oct_side_len + opened, 0.0f - opened,  //1
            -oct_side_len - opened, 0.0f + opened, 0.0f - opened, //0

            -oct_side_len - opened, 0.0f - opened, 0.0f + opened, //0 нижняя передняя грань
            0.0f - opened, -oct_side_len - opened, 0.0f + opened, //5
            0.0f - opened, 0.0f - opened, oct_side_len + opened,  //2

            0.0f + opened, 0.0f - opened, oct_side_len + opened,  //2 нижняя правая грань
            0.0f + opened, -oct_side_len - opened, 0.0f + opened, //5
            oct_side_len + opened, 0.0f - opened, 0.0f + opened,  //3

            oct_side_len + opened, 0.0f - opened, 0.0f - opened,  //3 нижняя задняя грань
            0.0f + opened, -oct_side_len - opened, 0.0f - opened, //5
            0.0f + opened, 0.0f - opened, -oct_side_len - opened, //4

            0.0f - opened, 0.0f - opened, -oct_side_len - opened, // нижняя левая грань
            0.0f - opened, -oct_side_len - opened, 0.0f - opened,
            -oct_side_len - opened, 0.0f - opened, 0.0f - opened,
    };

    GLfloat oct_normals[] = {
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f, 1.0f,
            -1.0f, -1.0f, 1.0f,
            -1.0f, -1.0f, 1.0f,

            1.0f, -1.0f, 1.0f,
            1.0f, -1.0f, 1.0f,
            1.0f, -1.0f, 1.0f,

            -1.0f, -1.0f, 1.0f,
            -1.0f, -1.0f, 1.0f,
            -1.0f, -1.0f, 1.0f,

            1.0f, -1.0f, 1.0f,
            1.0f, -1.0f, 1.0f,
            1.0f, -1.0f, 1.0f,

            1.0f, 1.0f, -1.0f,
            1.0f, 1.0f, -1.0f,
            1.0f, 1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
    };


    GLfloat texCoords[] = {0.0, 0.0,  // Нижний левый угол
                           1.0, 0.0,  // Нижний правый угол
                           0.5, 1.0,  // верхняя центральная сторона
                           0.0, 0.0,
                           1.0, 0.0,
                           0.5, 1.0,
                           0.0, 0.0,
                           1.0, 0.0,
                           0.5, 1.0,
                           0.0, 0.0,
                           1.0, 0.0,
                           0.5, 1.0,
                           0.0, 0.0,
                           1.0, 0.0,
                           0.5, 1.0,
                           0.0, 0.0,
                           1.0, 0.0,
                           0.5, 1.0,
                           0.0, 0.0,
                           1.0, 0.0,
                           0.5, 1.0,
                           0.0, 0.0,
                           1.0, 0.0,
                           0.5, 1.0,};

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glTexCoordPointer(2, GL_FLOAT, 0, texCoords);

    glVertexPointer(3, GL_FLOAT, 0, oct_vertices);
    glNormalPointer(GL_FLOAT, 0, oct_normals);

    GLubyte front_face[] = {0, 1, 2};             // передняя грань
    GLubyte right_side[] = {3, 4, 5};             // правая грань
    GLubyte back_face[] = {6, 7, 8};              // задняя грань
    GLubyte left_side[] = {9, 10, 11};            // левая грань
    GLubyte lower_front[] = {12, 13, 14};         // нижняя передняя грань
    GLubyte bottom_right_side[] = {15, 16, 17};   // нижняя правая грань
    GLubyte lower_back_face[] = {18, 19, 20};     // нижняя задняя грань
    GLubyte bottom_left_side[] = {21, 22, 23};    // нижняя левая грань

    switch (texture_mode) {
        case COLOR_MODE::COLOR:
            // отрисовка цвета граней
            glEnable(GL_COLOR_MATERIAL);
            glColor4f(0.00, 0.32, 0.48, 0.5);
            glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_BYTE, lower_back_face);

            // нижняя левая  грань цвета ЛГБТ(запрещено в РФ)
            // грань описана последней, индексы 63+
            glShadeModel(GL_SMOOTH);
            glBegin(GL_TRIANGLES);

            glColor4f(1.0, 0.0, 0.0, 0.5);
            glVertex3d(oct_vertices[63], oct_vertices[64], oct_vertices[65]);

            glColor4f(0.0, 1.0, 0.0, 0.5);
            glVertex3d(oct_vertices[66], oct_vertices[67], oct_vertices[68]);

            glColor4f(0.0, 0.0, 1.0, 0.5);
            glVertex3d(oct_vertices[69], oct_vertices[70], oct_vertices[71]);

            glEnd();

            glColor4f(0.32, 0.16, 0.36, 0.5);
            glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_BYTE, bottom_right_side);
            glColor4f(0.82, 0.21, 0.0, 0.5);
            glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_BYTE, lower_front);
            glColor4f(1.0, 1.0, 1.0, 0.5);
            glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_BYTE, back_face);
            glColor4f(0.0, 1.0, 0.0, 0.5);
            glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_BYTE, front_face);
            glColor4f(1.0, 0.0, 1.0, 0.5);
            glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_BYTE, right_side);
            glColor4f(1.0, 0.0, 0.0, 0.5);
            glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_BYTE, left_side);

            break;
        case COLOR_MODE::DIFFERENT_TEXTURES:
            // все грани имеют разные текстуры
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, textures[0]);
            glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_BYTE, lower_back_face);
            glBindTexture(GL_TEXTURE_2D, textures[1]);
            glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_BYTE, bottom_left_side);
            glBindTexture(GL_TEXTURE_2D, textures[2]);
            glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_BYTE, bottom_right_side);
            glBindTexture(GL_TEXTURE_2D, textures[3]);
            glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_BYTE, lower_front);
            glBindTexture(GL_TEXTURE_2D, textures[4]);
            glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_BYTE, back_face);
            glBindTexture(GL_TEXTURE_2D, textures[5]);
            glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_BYTE, front_face);
            glBindTexture(GL_TEXTURE_2D, textures[6]);
            glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_BYTE, right_side);
            glBindTexture(GL_TEXTURE_2D, textures[7]);
            glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_BYTE, left_side);
            glDisable(GL_TEXTURE_2D);
            break;
        case COLOR_MODE::ONE_TEXTURE:
            // перебор текстур
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, textures[rand() % 7]);
            glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_BYTE, bottom_left_side);
            glDisable(GL_TEXTURE_2D);

            glEnable(GL_COLOR_MATERIAL);
            glColor4f(0.00, 0.32, 0.48, 0.5);
            glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_BYTE, lower_back_face);
            glColor4f(0.32, 0.16, 0.36, 0.5);
            glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_BYTE, bottom_right_side);
            glColor4f(0.82, 0.21, 0.0, 0.5);
            glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_BYTE, lower_front);
            glColor4f(1.0, 1.0, 1.0, 0.5);
            glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_BYTE, back_face);
            glColor4f(0.0, 1.0, 0.0, 0.5);
            glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_BYTE, front_face);
            glColor4f(1.0, 0.0, 1.0, 0.5);
            glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_BYTE, right_side);
            glColor4f(1.0, 0.0, 0.0, 0.5);
            glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_BYTE, left_side);
            break;
    }

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void draw_cut_triangles(){
    glLoadIdentity();

    glTranslatef(0, 0, -5); // перемещаем объект по z для "вытягивания" октаэдра
    glRotatef((ox_rotation), 0, 1, 0);
    glRotatef((oy_rotation), 1, 0, 0);

    glColor4f(0.0, 1.0, 0.0, 0.5);
    glCallList(cutOctahedron);
}

void Draw() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!visibility) {
        glEnable(GL_BLEND);
        glDepthMask(GL_FALSE);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    } else {
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
    }

    //  Octahedron
    !cut_oct ? draw_colored_oct() : draw_cut_triangles();

    // light and sphere
    glLoadIdentity();
    glTranslatef(0, 0, -12);  // сторона камеры
    glRotatef((sun_rotation), 0, 1, 0);
    glTranslatef(0, 0, -10);  // сторона октаэдра

    glLightfv(GL_LIGHT0, GL_POSITION, light_pos); // полное освещение

    GLUquadricObj *quadObj;
    quadObj = gluNewQuadric();
    glColor3d(1, 1, 0);
    gluQuadricDrawStyle(quadObj, GLU_LINE);
    gluSphere(quadObj, 0.5, 10, 10);

    glutSwapBuffers();
}

/**
 * @param p - length of the split part
 * */
void create_cut_triangles_list(float p) {
    glLoadIdentity();
    glNewList(cutOctahedron, GL_COMPILE);

    for (GLfloat l = -(oct_side_len + p), i = -p; i >= -(oct_side_len + 4 * 0.1); l += p, i -= p) {
        i -= 0.1;

        glBegin(GL_POLYGON);
        glVertex3d(0, i, l + 2 * p);
        glVertex3d(0, i + p, l + p);
        glVertex3d(l + p, i + p, 0);
        glVertex3d(l + 2 * p, i, 0);
        glEnd();

        glBegin(GL_POLYGON);
        glVertex3d(0, i, l + 2 * p);
        glVertex3d(0, i + p, l + p);
        glVertex3d(-(l + p), i + p, 0);
        glVertex3d(-(l + 2 * p), i, 0);
        glEnd();

        glBegin(GL_POLYGON);
        glVertex3d(0, i, -(l + 2 * p));
        glVertex3d(0, i + p, -(l + p));
        glVertex3d(l + p, i + p, 0);
        glVertex3d(l + 2 * p, i, 0);
        glEnd();

        glBegin(GL_POLYGON);
        glVertex3d(0, i, -(l + 2 * p));
        glVertex3d(0, i + p, -(l + p));
        glVertex3d(-(l + p), i + p, 0);
        glVertex3d(-(l + 2 * p), i, 0);
        glEnd();
    }

    for (GLfloat l = -(oct_side_len + p), i = p; i <= oct_side_len + 4 * 0.1; l += p, i += p) {
        glBegin(GL_POLYGON);
        glVertex3d(0, i, l + 2 * p);
        glVertex3d(0, i - p, l + p);
        glVertex3d(l + p, i - p, 0);
        glVertex3d(l + 2 * p, i, 0);
        glEnd();

        glBegin(GL_POLYGON);
        glVertex3d(0, i, l + 2 * p);
        glVertex3d(0, i - p, l + p);
        glVertex3d(-(l + p), i - p, 0);
        glVertex3d(-(l + 2 * p), i, 0);
        glEnd();

        glBegin(GL_POLYGON);
        glVertex3d(0, i, -(l + 2 * p));
        glVertex3d(0, i - p, -(l + p));
        glVertex3d(l + p, i - p, 0);
        glVertex3d(l + 2 * p, i, 0);
        glEnd();

        glBegin(GL_POLYGON);
        glVertex3d(0, i, -(l + 2 * p));
        glVertex3d(0, i - p, -(l + p));
        glVertex3d(-(l + p), i - p, 0);
        glVertex3d(-(l + 2 * p), i, 0);
        glEnd();

        i += 0.1;
    }
    glEndList();
}

void change_size(GLsizei w, GLsizei h) {
    if (h == 0)
        h = 1;

    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(120, (GLdouble) w / h, 1, 100);
    glMatrixMode(GL_MODELVIEW);
    glViewport(0, 0, (GLdouble) w, (GLdouble) h);

    Draw();
}

void timer(int = 0) {
    glutPostRedisplay();
    glutTimerFunc(13, timer, 1);

    ox_rotation += dx_rotation;
    oy_rotation += dy_rotation;

    sun_rotation += dx_sun_rotation;
}

void glutNormalKeys(unsigned char key, int x, int y) {
    switch (key) {
        case KEY::TURN_UP:
            dy_rotation = -0.5;
            break;
        case KEY::TURN_LEFT:
            dx_rotation = -0.5;
            break;
        case KEY::TURN_DOWN:
            dy_rotation = 0.5;
            break;
        case KEY::TURN_RIGHT:
            dx_rotation = 0.5;
            break;
        case KEY::STOP:
            dx_rotation = 0;
            dy_rotation = 0;
            break;
        case KEY::OPEN_OCT:
            if (!opened)
                opened = 0.2;
            else
                opened = 0;

            break;
        case KEY::TEXTURE:
            if (texture_mode == COLOR_MODE::ONE_TEXTURE) {
                //glDisable(GL_TEXTURE_2D);
                texture_mode = COLOR_MODE::COLOR;
            } else if (texture_mode == COLOR_MODE::COLOR) {
                //glEnable(GL_TEXTURE_2D);
                texture_mode = COLOR_MODE::DIFFERENT_TEXTURES;
            } else {
                texture_mode = COLOR_MODE::ONE_TEXTURE;
            }

            break;
        case KEY::VISIBLE:
            visibility = !visibility;
            break;
        case KEY::LIGHT_MODE:
            if (light)
                glDisable(GL_LIGHT0);
            else
                glEnable(GL_LIGHT0);

            light = !light;
            break;
        case KEY::CUT_OCT:
            cut_oct = !cut_oct;
            break;
        default:
            break;
    }
}
