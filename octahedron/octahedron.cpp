#include <GL/glut.h>
#include <iostream>
#include "SOIL.h"

#define WIN_HEIGHT 750
#define WIN_WIDTH 750

//GLfloat ox_rotation = 0;

GLuint cube; // cube display list

GLfloat sun_rotation = 1;
GLfloat dx_sun_rotation = 1;
GLfloat light_pos[] = {0, 0, 1, 1};
GLfloat opened = 0; // раздвигание граней куба

GLfloat ox_rotation = 0;
GLfloat dx_rotation = 0.5;
GLfloat oy_rotation = 0;
GLfloat dy_rotation = 0;

void draw_cube();

void change_size(GLsizei, GLsizei);

void timer(int);

void draw_colored_oct();


void Draw() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // visibility
//    glEnable(GL_BLEND);
//    glDepthMask(GL_FALSE);
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // grid
//    glCallList(cube);

    //  Octahedron
    draw_colored_oct();

    // light and sphere
    glLoadIdentity();
    glTranslatef(0, 0, -12);  // сторона камеры
    glRotatef((sun_rotation), 0, 1, 0);
    glTranslatef(0, 0, -10);  // сторона октаэдра

    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);

    GLUquadricObj *quadObj;
    quadObj = gluNewQuadric();
    glColor3d(1, 1, 0);
    gluQuadricDrawStyle(quadObj, GLU_LINE);
    gluSphere(quadObj, 0.5, 10, 10);

    glutSwapBuffers();
}

void glutNormalKeys(unsigned char key, int x, int y) {
    switch (key) {
        case 'w':
            dy_rotation = -0.5;
            break;
        case 'a':
            dx_rotation = -0.5;
            break;
        case 's':
            dy_rotation = 0.5;
            break;
        case 'd':
            dx_rotation = 0.5;
            break;
        case 'q':
            dy_rotation = 0;
            dx_rotation = 0;
            break;
        case 'o':
            if (!opened)
                opened = 0.2;
            else
                opened = 0;
            break;
        default:
            break;
    }
}

void init() {
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);

    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    glutInitWindowSize(WIN_WIDTH, WIN_HEIGHT);
    glutCreateWindow("Octahedron");

//    cube = glGenLists(1);
//    draw_cube();
    std::cout << "w - turn up\n"
                 "s - turn down\n"
                 "a - turn left\n"
                 "d - turn right\n"
                 "q - stop rotation\n"
                 "o - open octahedron";

    glutDisplayFunc(Draw);
    glutReshapeFunc(change_size);
    glutTimerFunc(5, timer, 0);

    // control panel
    glutKeyboardFunc(glutNormalKeys);

    init();
    glutMainLoop();

    return 0;
}

void draw_colored_oct() {
    glLoadIdentity();

    glTranslatef(0, 0, -5); // перемещаем объект по z для "вытягивания" октаэдра
    glRotatef((ox_rotation), 0, 1, 0);
    glRotatef((oy_rotation), 1, 0, 0);

    GLfloat oct_vertices[] = {
            -2.0f - opened, 0.0f + opened, 0.0f + opened, //0 передняя грань
            0.0f - opened, 2.0f + opened, 0.0f + opened,  //1
            0.0f - opened, 0.0f + opened, 2.0f + opened,  //2

            0.0f + opened, 0.0f + opened, 2.0f + opened,  //2 правая боковая
            0.0f + opened, 2.0f + opened, 0.0f + opened,  //1
            2.0f + opened, 0.0f + opened, 0.0f + opened,  //3

            2.0f + opened, 0.0f + opened, 0.0f - opened,  //3 задняя грань
            0.0f + opened, 2.0f + opened, 0.0f - opened,  //1
            0.0f + opened, 0.0f + opened, -2.0f - opened, //4

            0.0f - opened, 0.0f + opened, -2.0f - opened, //4 левая боковая
            0.0f - opened, 2.0f + opened, 0.0f - opened,  //1
            -2.0f - opened, 0.0f + opened, 0.0f - opened, //0

            -2.0f - opened, 0.0f - opened, 0.0f + opened, //0 нижняя передняя грань
            0.0f - opened, -2.0f - opened, 0.0f + opened, //5
            0.0f - opened, 0.0f - opened, 2.0f + opened,  //2

            0.0f + opened, 0.0f - opened, 2.0f + opened,  //2 нижняя правая грань
            0.0f + opened, -2.0f - opened, 0.0f + opened, //5
            2.0f + opened, 0.0f - opened, 0.0f + opened,  //3

            2.0f + opened, 0.0f - opened, 0.0f - opened,  //3 нижняя задняя грань
            0.0f + opened, -2.0f - opened, 0.0f - opened, //5
            0.0f + opened, 0.0f - opened, -2.0f - opened, //4
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

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    glVertexPointer(3, GL_FLOAT, 0, oct_vertices);
    glNormalPointer(GL_FLOAT, 0, oct_normals);

    GLubyte front_face[] = {0, 1, 2};             // передняя грань
    GLubyte right_side[] = {3, 4, 5};             // правая грань
    GLubyte back_face[] = {6, 7, 8};              // задняя грань
    GLubyte left_side[] = {9, 10, 11};            // левая грань
    GLubyte lower_front[] = {12, 13, 14};         // нижняя передняя грань
    GLubyte bottom_right_side[] = {15, 16, 17};   // нижняя правая грань
    GLubyte lower_back_face[] = {18, 19, 20};     // нижняя задняя грань

    // отрисовка цвета граней
    glEnable(GL_COLOR_MATERIAL);
    glColor4f(0.00, 0.32, 0.48, 0.5);
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_BYTE, lower_back_face);

    // нижняя левая  грань
    glShadeModel(GL_SMOOTH);
    glBegin(GL_TRIANGLES);

    glColor4f(1.0, 0.0, 0.0, 0.5);
    glVertex3d(0.0f - opened, 0.0f - opened, -2.0f - opened);

    glColor4f(0.0, 1.0, 0.0, 0.5);
    glVertex3d(0.0f - opened, -2.0f - opened, 0.0f - opened);

    glColor4f(0.0, 0.0, 1.0, 0.5);
    glVertex3d(-2.0f - opened, 0.0f - opened, 0.0f - opened);

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
}

/**
 * Rendering grid
 * */
void draw_cube() {
    glLoadIdentity();

    glNewList(cube, GL_COMPILE);
    glTranslatef(0, 0, 10);
    glColor4f(1.0, 0.0, 0.0, 1.0);

    for (int i = 0; i < 31; i++) {
        glBegin(GL_LINE_LOOP);
        glVertex3f(-30, 30 - i * 2, 30);
        glVertex3f(30, 30 - i * 2, 30);
        glVertex3f(30, 30 - i * 2, -30);
        glVertex3f(-30, 30 - i * 2, -30);
        glEnd();

        glBegin(GL_LINE_LOOP);
        glVertex3f(30 - i * 2, 30, 30);
        glVertex3f(30 - i * 2, -30, 30);
        glVertex3f(30 - i * 2, -30, -30);
        glVertex3f(30 - i * 2, 30, -30);
        glEnd();

        glBegin(GL_LINE_LOOP);
        glVertex3f(-30, 30, 30 - i * 2);
        glVertex3f(30, 30, 30 - i * 2);
        glVertex3f(30, -30, 30 - i * 2);
        glVertex3f(-30, -30, 30 - i * 2);
        glEnd();
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