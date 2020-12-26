#include <GL/glut.h>
#include <iostream>
#include <sstream>
#include "SOIL.h"

#define WIN_HEIGHT 750
#define WIN_WIDTH 750
#define TEXTURES_COUNT 9
#define PARTITIONS_NUMBER 4

GLuint textures[TEXTURES_COUNT];
GLuint cutOctahedron;
GLuint cutOctahedronOpened;

// Sphere rotation params
GLfloat sun_rotation = 1;
GLfloat dx_sun_rotation = 0.9;

GLfloat light0_pos[] = {0, 0, 1, 1};
GLfloat light0_diffuse[] = {1.0, 1.0, 1.0};

GLfloat opened = 0; // splitting octahedron faces
const GLfloat oct_side_len = 2.0f;

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

const GLfloat oct_normals[] = {
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f, // Front face
        1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f, // Right side face
        -1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f, // Back face
        -1.0f, -1.0f, 1.0f,

        1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, 1.0f, // Left side face
        1.0f, -1.0f, 1.0f,

        -1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f, // Lower front face
        -1.0f, -1.0f, 1.0f,

        1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, 1.0f, // Lower right side face
        1.0f, -1.0f, 1.0f,

        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f, // Lower back face
        1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f, // Lower left side face
        -1.0f, -1.0f, -1.0f,
};

void draw_colored_oct();

void draw_cut_triangles(GLuint);

void Draw();

void create_cut_triangles_list(GLuint, GLfloat, GLfloat= 0);

void change_size(GLsizei, GLsizei);

void timer(int);

void glutNormalKeys(unsigned char, int, int);


void textures_init() {
    int width[TEXTURES_COUNT],
            height[TEXTURES_COUNT];

    glGenTextures(TEXTURES_COUNT, textures);
    unsigned char *images[TEXTURES_COUNT];

    std::stringstream filename;
    for (int i = 0; i < TEXTURES_COUNT; i++) {
        filename << "../textures/txt" << i << ".bmp";
        images[i] = SOIL_load_image(filename.str().c_str(), &width[i], &height[i],
                                    nullptr, SOIL_LOAD_RGB);

        glBindTexture(GL_TEXTURE_2D, textures[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width[i], height[i], 0,
                     GL_RGB, GL_UNSIGNED_BYTE, images[i]);

        // Texture rendering
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        filename.str("");
    }

    for (auto &image : images) {
        SOIL_free_image_data(image);
        glBindTexture(GL_TEXTURE_2D, 0); // Object unlinking
    }
}

void light_init() {
    glClearColor(0, 0, 0, 0);

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, light0_pos);
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

    // Cut octahedron preparation
    GLfloat opened_part_length = oct_side_len / PARTITIONS_NUMBER;
    cutOctahedron = glGenLists(2);
    create_cut_triangles_list(cutOctahedron, opened_part_length);
    glEndList();

    // Opened version of the cut octahedron
    cutOctahedronOpened = cutOctahedron + 1;
    create_cut_triangles_list(cutOctahedronOpened, opened_part_length, 0.1f);
    glEndList();

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

    glTranslatef(0, 0, -5); // Moving the object along z to "stretch" the octahedron
    glRotatef((ox_rotation), 0, 1, 0);
    glRotatef((oy_rotation), 1, 0, 0);

    // This one maybe calculated automatic but I don't want to do this
    GLfloat oct_vertices[] = {
            -oct_side_len - opened, 0.0f + opened, 0.0f + opened, //0 front face
            0.0f - opened, oct_side_len + opened, 0.0f + opened,  //1
            0.0f - opened, 0.0f + opened, oct_side_len + opened,  //2

            0.0f + opened, 0.0f + opened, oct_side_len + opened,  //2 right side face
            0.0f + opened, oct_side_len + opened, 0.0f + opened,  //1
            oct_side_len + opened, 0.0f + opened, 0.0f + opened,  //3

            oct_side_len + opened, 0.0f + opened, 0.0f - opened,  //3 back face
            0.0f + opened, oct_side_len + opened, 0.0f - opened,  //1
            0.0f + opened, 0.0f + opened, -oct_side_len - opened, //4

            0.0f - opened, 0.0f + opened, -oct_side_len - opened, //4 left side face
            0.0f - opened, oct_side_len + opened, 0.0f - opened,  //1
            -oct_side_len - opened, 0.0f + opened, 0.0f - opened, //0

            -oct_side_len - opened, 0.0f - opened, 0.0f + opened, //0 lower front face
            0.0f - opened, -oct_side_len - opened, 0.0f + opened, //5
            0.0f - opened, 0.0f - opened, oct_side_len + opened,  //2

            0.0f + opened, 0.0f - opened, oct_side_len + opened,  //2 lower right side face
            0.0f + opened, -oct_side_len - opened, 0.0f + opened, //5
            oct_side_len + opened, 0.0f - opened, 0.0f + opened,  //3

            oct_side_len + opened, 0.0f - opened, 0.0f - opened,  //3 lower back face
            0.0f + opened, -oct_side_len - opened, 0.0f - opened, //5
            0.0f + opened, 0.0f - opened, -oct_side_len - opened, //4

            0.0f - opened, 0.0f - opened, -oct_side_len - opened, // lower left side face
            0.0f - opened, -oct_side_len - opened, 0.0f - opened,
            -oct_side_len - opened, 0.0f - opened, 0.0f - opened,
    };


    GLfloat texCoords[] = {0.0, 0.0,  // Lower left corner
                           1.0, 0.0,  // Lower right corner
                           0.5, 1.0,  // Upper center side
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

    const GLubyte front_face[] = {0, 1, 2};
    const GLubyte right_side[] = {3, 4, 5};
    const GLubyte back_face[] = {6, 7, 8};
    const GLubyte left_side[] = {9, 10, 11};
    const GLubyte lower_front[] = {12, 13, 14};
    const GLubyte lower_right_side[] = {15, 16, 17};
    const GLubyte lower_back_face[] = {18, 19, 20};
    const GLubyte lower_left_side[] = {21, 22, 23};

    switch (texture_mode) {
        case COLOR_MODE::COLOR:
            // Each face has it's own color
            glEnable(GL_COLOR_MATERIAL);


            glColor4f(0.00, 0.32, 0.48, 0.5);
            glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_BYTE, lower_back_face);


            /* Lower left face has lgbt color.
             * The face is described last, 63+ indexes.
             * GL_NORMAL_ARRAY doesn't work with self-written vertexes*/
            glShadeModel(GL_SMOOTH);
            glBegin(GL_TRIANGLES);

                glNormal3f(-1.0f, -1.0f, -1.0f);
                glColor4f(1.0, 0.0, 0.0, 0.5);
                glVertex3d(oct_vertices[63], oct_vertices[64], oct_vertices[65]);

                glNormal3f(-1.0f, -1.0f, -1.0f);
                glColor4f(0.0, 1.0, 0.0, 0.5);
                glVertex3d(oct_vertices[66], oct_vertices[67], oct_vertices[68]);

                glNormal3f(-1.0f, -1.0f, -1.0f);
                glColor4f(0.0, 0.0, 1.0, 0.5);
                glVertex3d(oct_vertices[69], oct_vertices[70], oct_vertices[71]);

            glEnd();


            glColor4f(0.32, 0.16, 0.36, 0.5);
            glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_BYTE, lower_right_side);

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
            // Each octahedron face hase it's own texture
            glEnable(GL_TEXTURE_2D);

                glBindTexture(GL_TEXTURE_2D, textures[0]);
                glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_BYTE, lower_back_face);

                glBindTexture(GL_TEXTURE_2D, textures[1]);
                glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_BYTE, lower_left_side);

                glBindTexture(GL_TEXTURE_2D, textures[2]);
                glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_BYTE, lower_right_side);

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
            // One texture for all octahedron faces
            glEnable(GL_TEXTURE_2D);

                glBindTexture(GL_TEXTURE_2D, textures[8]);

                glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_BYTE, lower_left_side);
                glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_BYTE, lower_back_face);
                glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_BYTE, lower_right_side);
                glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_BYTE, lower_front);
                glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_BYTE, back_face);
                glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_BYTE, front_face);
                glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_BYTE, right_side);
                glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_BYTE, left_side);

            glDisable(GL_TEXTURE_2D);

            break;
    }

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

/**
 * @param display_list - chosen display list
 * */
void draw_cut_triangles(GLuint display_list) {
    glLoadIdentity();

    glTranslatef(0, 0, -5);
    glRotatef((ox_rotation), 0, 1, 0);
    glRotatef((oy_rotation), 1, 0, 0);

    glCallList(display_list);
}

void Draw() {
    glDepthMask(GL_TRUE); // Opengl 4.6 reshape bug fix
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!visibility) {
        glEnable(GL_BLEND);
        glDepthMask(GL_FALSE);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    } else {
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
    }


    //  Octahedron
    if (!cut_oct)
        draw_colored_oct();
    else
        opened == 0 ? draw_cut_triangles(cutOctahedron) : draw_cut_triangles(cutOctahedronOpened);

    // Light and sphere
    glLoadIdentity();
    glTranslatef(0, 0, -12);  // Camera side
    glRotatef((sun_rotation), 0, 1, 0);
    glTranslatef(0, 0, -10);  // Octahedron side

    glLightfv(GL_LIGHT0, GL_POSITION, light0_pos);

    GLUquadricObj *quadObj;
    quadObj = gluNewQuadric();
    glColor3d(1, 1, 0);
    gluQuadricDrawStyle(quadObj, GLU_LINE);
    gluSphere(quadObj, 0.5, 10, 10);

    gluDeleteQuadric(quadObj);

    glutSwapBuffers();
}

/**
 * @param display_list - variable that will store display list
 * @param part_length - length of the split part
 * @param detach_length - detach length from the center of the octahedron
 * */
void create_cut_triangles_list(GLuint display_list, GLfloat part_length, GLfloat detach_length) {
    glLoadIdentity();
    glNewList(display_list, GL_COMPILE);

    /*
     * In the next two cycles l and i are just temporary params for vertex's calculating
     * */

    // The bottom part of the cut octahedron
    for (GLfloat l = -(oct_side_len + part_length),
                 i = -part_length; i >= -(oct_side_len + 4 * 0.1); l += part_length, i -= part_length) {

        i -= 0.1;

        // Lower left face - lgbt color
        glBegin(GL_POLYGON);
            glNormal3f(1.0f, -1.0f, 1.0f);

            glColor4f(1.0, 0.0, 0.0, 0.5);
            glVertex3d(-detach_length, i - detach_length, l + 2 * part_length - detach_length);

            glColor4f(0.0, 1.0, 0.0, 0.5);
            glVertex3d(-detach_length, i + part_length - detach_length, l + part_length - detach_length);

            glColor4f(0.0, 0.0, 1.0, 0.5);
            glVertex3d(l + part_length - detach_length, i + part_length - detach_length, -detach_length);

            glColor4f(1.0, 1.0, 0.0, 0.5);
            glVertex3d(l + 2 * part_length - detach_length, i - detach_length, -detach_length);

        glEnd();


        // Lower back face
        glColor4f(0.00, 0.32, 0.48, 0.5);
        glBegin(GL_POLYGON);
            glNormal3f(1.0f, 1.0f, -1.0f);

            glVertex3d(detach_length, i - detach_length, l + 2 * part_length - detach_length);

            glVertex3d(detach_length, i + part_length - detach_length, l + part_length - detach_length);

            glVertex3d(-(l + part_length) + detach_length, i + part_length - detach_length, -detach_length);

            glVertex3d(-(l + 2 * part_length) + detach_length, i - detach_length, -detach_length);

        glEnd();


        // Lower front face
        glColor4f(0.82, 0.21, 0.0, 0.5);
        glBegin(GL_POLYGON);
            glNormal3f(-1.0f, -1.0f, 1.0f);

            glVertex3d(-detach_length, i - detach_length, -(l + 2 * part_length) + detach_length);

            glVertex3d(-detach_length, i + part_length - detach_length, -(l + part_length) + detach_length);

            glVertex3d(l + part_length - detach_length, i + part_length - detach_length, detach_length);

            glVertex3d(l + 2 * part_length - detach_length, i - detach_length, detach_length);

        glEnd();


        // Lower right side
        glColor4f(0.32, 0.16, 0.36, 0.5);
        glBegin(GL_POLYGON);
            glNormal3f(-1.0f, 1.0f, -1.0f);

            glVertex3d(detach_length, i - detach_length, -(l + 2 * part_length) + detach_length);

            glVertex3d(detach_length, i + part_length - detach_length, -(l + part_length) + detach_length);

            glVertex3d(-(l + part_length) + detach_length, i + part_length - detach_length, detach_length);

            glVertex3d(-(l + 2 * part_length) + detach_length, i - detach_length, detach_length);

        glEnd();

    }

    // The upper part of the cut octahedron
    for (GLfloat l = -(oct_side_len + part_length), i = part_length;
         i <= oct_side_len + 4 * 0.1; l += part_length, i += part_length) {

        // Left side
        glColor4f(1.0, 0.0, 0.0, 0.5);
        glBegin(GL_POLYGON);
            glNormal3f(-1.0f, -1.0f, -1.0f);

            glVertex3d(-detach_length, i + detach_length, l + 2 * part_length - detach_length);

            glVertex3d(-detach_length, i - part_length + detach_length, l + part_length - detach_length);

            glVertex3d(l + part_length - detach_length, i - part_length + detach_length, -detach_length);

            glVertex3d(l + 2 * part_length - detach_length, i + detach_length, -detach_length);

        glEnd();


        // Back face
        glColor4f(1.0, 1.0, 1.0, 0.5);
        glBegin(GL_POLYGON);
            glNormal3f(-1.0f, 1.0f, 1.0f);

            glVertex3d(detach_length, i + detach_length, l + 2 * part_length - detach_length);

            glVertex3d(detach_length, i - part_length + detach_length, l + part_length - detach_length);

            glVertex3d(-(l + part_length) + detach_length, i - part_length + detach_length, -detach_length);

            glVertex3d(-(l + 2 * part_length) + detach_length, i + detach_length, -detach_length);

        glEnd();


        // Front face
        glColor4f(0.0, 1.0, 0.0, 0.5);
        glBegin(GL_POLYGON);
            glNormal3f(1.0f, -1.0f, -1.0f);

            glVertex3d(-detach_length, i + detach_length, -(l + 2 * part_length) + detach_length);

            glVertex3d(-detach_length, i - part_length + detach_length, -(l + part_length) + detach_length);

            glVertex3d(l + part_length - detach_length, i - part_length + detach_length, detach_length);

            glVertex3d(l + 2 * part_length - detach_length, i + detach_length, detach_length);

        glEnd();


        // Right side
        glColor4f(1.0, 0.0, 1.0, 0.5);
        glBegin(GL_POLYGON);
            glNormal3f(1.0f, 1.0f, 1.0f);

            glVertex3d(detach_length, i + detach_length, -(l + 2 * part_length) + detach_length);

            glVertex3d(detach_length, i - part_length + detach_length, -(l + part_length) + detach_length);

            glVertex3d(-(l + part_length) + detach_length, i - part_length + detach_length, detach_length);

            glVertex3d(-(l + 2 * part_length) + detach_length, i + detach_length, detach_length);

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
                texture_mode = COLOR_MODE::COLOR;
            } else if (texture_mode == COLOR_MODE::COLOR) {
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