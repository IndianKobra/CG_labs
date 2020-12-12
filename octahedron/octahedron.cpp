#include <GL/glut.h>
#include <iostream>
#include <sstream>
#include "SOIL.h"

#define WIN_HEIGHT 750
#define WIN_WIDTH 750

GLuint textures[8];

GLfloat sun_rotation = 1;
GLfloat dx_sun_rotation = 1;
GLfloat light_pos[] = {0, 0, 1, 1};

GLfloat opened = 0; // раздвигание граней куба
int texture_mode = 2;

GLfloat ox_rotation = 0;
GLfloat dx_rotation = 0.5;
GLfloat oy_rotation = 0;
GLfloat dy_rotation = 0;

void change_size(GLsizei, GLsizei);

void timer(int);

void draw_colored_oct();


void Draw() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


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
    // textures initialization


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

    std::cout << "w - turn up\n"
                 "s - turn down\n"
                 "a - turn left\n"
                 "d - turn right\n"
                 "q - stop rotation\n"
                 "o - open octahedron";


    // control panel
    glutKeyboardFunc(glutNormalKeys);
    glutDisplayFunc(Draw);
    glutReshapeFunc(change_size);
    glutTimerFunc(5, timer, 0);



    int width[8], height[8];
    glGenTextures(8, textures);
    unsigned char *images[8];
    std::stringstream filename;
    for(int i = 0; i < 8; i++)
    {
        filename << "../textures/txt" << i << ".bmp";
        std::cout << filename.str();
        images[i] = SOIL_load_image(filename.str().c_str(), &width[i], &height[i], 0,SOIL_LOAD_RGB);

        glBindTexture(GL_TEXTURE_2D, textures[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width[i], height[i], 0, GL_RGB, GL_UNSIGNED_BYTE, images[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        filename.str("");
    }
    for(int i = 0 ; i < 8; i++)
    {
        SOIL_free_image_data(images[i]);
    }

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

            0.0f - opened, 0.0f - opened, -2.0f - opened, // нижняя левая грань
            0.0f - opened, -2.0f - opened, 0.0f - opened,
            -2.0f - opened, 0.0f - opened, 0.0f - opened,
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


    GLfloat texCoords[] = { 0.0, 0.0,  // Нижний левый угол
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

    if (texture_mode == 0) {
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
    }

    if (texture_mode == 2) {
        // все грани имеют разные текстуры
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textures[0]);
        glDrawElements(GL_TRIANGLES,3,GL_UNSIGNED_BYTE,lower_back_face);
        glBindTexture(GL_TEXTURE_2D, textures[1]);
        glDrawElements(GL_TRIANGLES,3,GL_UNSIGNED_BYTE,bottom_left_side);
        glBindTexture(GL_TEXTURE_2D, textures[2]);
        glDrawElements(GL_TRIANGLES,3,GL_UNSIGNED_BYTE,bottom_right_side);
        glBindTexture(GL_TEXTURE_2D, textures[3]);
        glDrawElements(GL_TRIANGLES,3,GL_UNSIGNED_BYTE,lower_front);
        glBindTexture(GL_TEXTURE_2D, textures[4]);
        glDrawElements(GL_TRIANGLES,3,GL_UNSIGNED_BYTE,back_face);
        glBindTexture(GL_TEXTURE_2D, textures[5]);
        glDrawElements(GL_TRIANGLES,3,GL_UNSIGNED_BYTE,front_face);
        glBindTexture(GL_TEXTURE_2D, textures[6]);
        glDrawElements(GL_TRIANGLES,3,GL_UNSIGNED_BYTE,right_side);
        glBindTexture(GL_TEXTURE_2D, textures[7]);
        glDrawElements(GL_TRIANGLES,3,GL_UNSIGNED_BYTE,left_side);
        glDisable(GL_TEXTURE_2D);
    }

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
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