// ------------------------------------------------------------------------------------------------------------------------------------------
// REGLAS DEL JUEGO: 
// ------------------------------------------------------------------------------------------------------------------------------------------
//
// 1 - El "mundo" está representado por una grid en 2D 
// 2 - Cada cuadrado del grid representa una criatura, o "celula". Cada celula puede estar viva (blanco) o muerta (negro)
// 3 - En cada momento del tiempo (paso de la simulacion) cada celula revisa el estado de sus primeros vecinos. El estado 
// de la celula cambia segun el numero de vecinos vivos:
//    a - Cada celula viva con menos de dos vecinos vivos muere ("underpopulation")
//    b - Cada celula viva con dos o tres vecinos sobrevive a la proxima generación
//    c - Cada celula viva con mas de tres vecinos vivos muere ("overpopulation")
//    d - Cada celula muerta con exactamente tres vecinos se vuelve una celula viva ("reproduction")
//
// ------------------------------------------------------------------------------------------------------------------------------------------

#include <GL/glut.h>
#include <iostream>
#include <cmath>
#include <stdio.h>
#include <fstream>
#include <new>
#include <stdlib.h>
#include <time.h>

using namespace std;

#define NX 542
#define NY 812
#define PIXEL 1
#define PROB 1      // Probabilidad de celula viva, ayuda a controlar la densidad de poblacion
#define XSIZE NX * PIXEL
#define YSIZE NY * PIXEL

#define GLIDERGUNX 36
#define GLIDERGUNY 9

#define EATER 4

#define GATE 270

#define FRAMERATE 1

int **mat1, **mat2;

void setup(){
    srand(time(NULL));      // seed del RNG en funcion de la hora de sistema
    mat1 = new int*[NX];    // Paso actual
    mat2 = new int*[NX];    // Siguiente paso

    for (int i = 0; i < NX; i++) {
        mat1[i] = new int[NY];
        mat2[i] = new int[NY];
    }

    for (int i = 0; i < NX; i++) {
        for (int j = 0; j < NY; j++) {
            mat1[i][j] = 0;
            mat2[i][j] = 0;
        }
    }

    glMatrixMode(GL_PROJECTION);
    glOrtho(0, XSIZE, 0, YSIZE, -1, 1);     // Establece sistema de coordenadas
    glClearColor(0.0, 0.0, 0.0, 0.0);       // Establece el color RGB-alpha del fondo
}

int survives(int x, int y){
    int aux = 0;
    if (!x || !y || (x == NX - 1) || (y == NY - 1)) {
        // Si la celula se encuentra en el borde derecho, superior, izquierdo, e inferior, está muerta
        return 0;
    } else {
        for (int i = x - 1; i <= x + 1; i++) {
            for (int j = y - 1; j <= y + 1; j++) {
                aux += mat1[i][j];
            }
        }
        aux -= mat1[x][y];      // Resta 1 si la celula está viva, 0 si no.
        aux = (mat1[x][y]) ? ((aux == 2) || (aux == 3)) : (aux == 3);
        // Condicional resumido. Si mat1[x][y] = 1, entonces chequea aux y devuelve un 1 si es igual a 2 o 3.
        // Si mat1[x][y] = 0, entonces chequea aux y devuelve un 1 solo si es igual a 3.
        return aux;
    }
}

// Función para crear una configuración inicial aleatoria
void randomConfig(){
    srand(time(NULL));

    for (int i = 0; i < NX; i++) {
        for (int j = 0; j < NY; j++) {
            mat1[i][j] = PROB > (rand()%10);
        }
    }    
}

// Funciones para flipear una matriz cuadrada
void flipVertical(int startX, int startY, int sizeX, int sizeY){
    for (int i = 0; i < sizeX; i++) {
        for (int j = 0; j < sizeY / 2; j++) {
            int temp = mat1[startX + i][startY + j];
            mat1[startX + i][startY + j] = mat1[startX + i][startY + sizeY - j - 1];
            mat1[startX + i][startY + sizeY - j - 1] = temp;
        }
    }    
}
void flipHorizontal(int startX, int startY, int sizeX, int sizeY){
    for (int j = 0; j < sizeY; j++) {
        for (int i = 0; i < sizeX / 2; i++) {
            int temp = mat1[startX + i][startY + j];
            mat1[startX + i][startY + j] = mat1[startX + sizeX - i - 1][startY + j];
            mat1[startX + sizeX - i - 1][startY + j] = temp;
        }
    }
}

// Función para crearme Glider Guns con las diferentes orientaciones posibles (u = up, d = down, r = right, l = left), donde me permite ingresar el paso en el que se encuentra con step
void gliderGunH(int startX, int startY, string orientation, int step){
    for (int i = 0; i < GLIDERGUNX; i++) {
        for (int j = 0; j < GLIDERGUNY; j++) {
            mat1[startX + i][startY + j] = 0;
        }
    }
    // Cuadrado izquierdo
    mat1[startX][startY + 4] = 1;
    mat1[startX][startY + 5] = 1;
    mat1[startX + 1][startY + 4] = 1;
    mat1[startX + 1][startY + 5] = 1;

    // Cuadrado derecho
    mat1[startX + GLIDERGUNX - 1][startY + 2] = 1;
    mat1[startX + GLIDERGUNX - 1][startY + 3]  = 1;
    mat1[startX + GLIDERGUNX - 2][startY + 2] = 1;
    mat1[startX + GLIDERGUNX - 2][startY + 3]  = 1;

    // Forma izquierda inicial
    mat1[startX + 12][startY + 2] = 1;
    mat1[startX + 13][startY + 2] = 1;
    mat1[startX + 11][startY + 3] = 1;
    mat1[startX + 15][startY + 3] = 1;
    mat1[startX + 10][startY + 4] = 1;
    mat1[startX + 16][startY + 4] = 1;   
    mat1[startX + 10][startY + 5] = 1;
    mat1[startX + 14][startY + 5] = 1;
    mat1[startX + 16][startY + 5] = 1;
    mat1[startX + 17][startY + 5] = 1;
    mat1[startX + 10][startY + 6] = 1;
    mat1[startX + 16][startY + 6] = 1; 
    mat1[startX + 11][startY + 7] = 1;
    mat1[startX + 15][startY + 7] = 1; 
    mat1[startX + 12][startY + 8] = 1;
    mat1[startX + 13][startY + 8] = 1;   

    // Forma derecha inicial
    mat1[startX + 24][startY] = 1;
    mat1[startX + 22][startY + 1] = 1;
    mat1[startX + 24][startY + 1] = 1;
    mat1[startX + 20][startY + 2] = 1;
    mat1[startX + 21][startY + 2] = 1;
    mat1[startX + 20][startY + 3] = 1;
    mat1[startX + 21][startY + 3] = 1;
    mat1[startX + 20][startY + 4] = 1;
    mat1[startX + 21][startY + 4] = 1;
    mat1[startX + 22][startY + 5] = 1;
    mat1[startX + 24][startY + 5] = 1;
    mat1[startX + 24][startY + 6] = 1;

    int tempArray[GLIDERGUNX][GLIDERGUNY];
    for (int steps = 0; steps < step; steps++) {
        for (int i = 0; i < GLIDERGUNX; i++) {
            for (int j = 0; j < GLIDERGUNY; j++) {
                mat2[startX + i][startY + j] = survives(startX + i, startY + j);
            }
        }
        for (int i = 0; i < GLIDERGUNX; i++) {
            for (int j = 0; j < GLIDERGUNY; j++) {
                tempArray[i][j] = mat1[startX + i][startY + j];
            }
        }
        for (int i = 0; i < GLIDERGUNX; i++) {
            for (int j = 0; j < GLIDERGUNY; j++) {
                mat1[startX + i][startY + j] = mat2[startX + i][startY + j];
                mat2[startX + i][startY + j] = tempArray[i][j];
            }
        }    
        // Estos dos pasos me aseguran que no se me rompa todo al poner pasos mayores a 15
        mat1[startX + 22][startY + GLIDERGUNY - 1] = 0;
        mat1[startX + 21][startY + GLIDERGUNY - 1] = 0;
    }

    if (orientation == "dr") {
        flipVertical(startX, startY, GLIDERGUNX, GLIDERGUNY);
    } else if (orientation == "ul") {
        flipHorizontal(startX, startY, GLIDERGUNX, GLIDERGUNY);
    } else if (orientation == "dl") {
        flipVertical(startX, startY, GLIDERGUNX, GLIDERGUNY);
        flipHorizontal(startX, startY, GLIDERGUNX, GLIDERGUNY);
    }

}
void gliderGunV(int startX, int startY, string orientation, int step){
    for (int i = 0; i < GLIDERGUNY; i++) {
        for (int j = 0; j < GLIDERGUNX; j++) {
            mat1[startX + i][startY + j] = 0;
        }
    }
    // Cuadrado izquierdo
    mat1[startX + 4][startY] = 1;
    mat1[startX + 5][startY] = 1;
    mat1[startX + 4][startY + 1] = 1;
    mat1[startX + 5][startY + 1] = 1;

    // Cuadrado derecho
    mat1[startX + 2][startY + GLIDERGUNX - 1] = 1;
    mat1[startX + 3][startY + GLIDERGUNX - 1]  = 1;
    mat1[startX + 2][startY + GLIDERGUNX - 2] = 1;
    mat1[startX + 3][startY + GLIDERGUNX - 2]  = 1;

    // Forma izquierda inicial
    mat1[startX + 2][startY + 12] = 1;
    mat1[startX + 2][startY + 13] = 1;
    mat1[startX + 3][startY + 11] = 1;
    mat1[startX + 3][startY + 15] = 1;
    mat1[startX + 4][startY + 10] = 1;
    mat1[startX + 4][startY + 16] = 1;   
    mat1[startX + 5][startY + 10] = 1;
    mat1[startX + 5][startY + 14] = 1;
    mat1[startX + 5][startY + 16] = 1;
    mat1[startX + 5][startY + 17] = 1;
    mat1[startX + 6][startY + 10] = 1;
    mat1[startX + 6][startY + 16] = 1; 
    mat1[startX + 7][startY + 11] = 1;
    mat1[startX + 7][startY + 15] = 1; 
    mat1[startX + 8][startY + 12] = 1;
    mat1[startX + 8][startY + 13] = 1;   

    // Forma derecha inicial
    mat1[startX][startY + 24] = 1;
    mat1[startX + 1][startY + 22] = 1;
    mat1[startX + 1][startY + 24] = 1;
    mat1[startX + 2][startY + 20] = 1;
    mat1[startX + 2][startY + 21] = 1;
    mat1[startX + 3][startY + 20] = 1;
    mat1[startX + 3][startY + 21] = 1;
    mat1[startX + 4][startY + 20] = 1;
    mat1[startX + 4][startY + 21] = 1;
    mat1[startX + 5][startY + 22] = 1;
    mat1[startX + 5][startY + 24] = 1;
    mat1[startX + 6][startY + 24] = 1;

    int tempArray[GLIDERGUNX][GLIDERGUNY];
    for (int steps = 0; steps < step; steps++) {
        for (int i = 0; i < GLIDERGUNY; i++) {
            for (int j = 0; j < GLIDERGUNX; j++) {
                mat2[startX + i][startY + j] = survives(startX + i, startY + j);
            }
        }
        for (int i = 0; i < GLIDERGUNY; i++) {
            for (int j = 0; j < GLIDERGUNX; j++) {
                tempArray[i][j] = mat1[startX + i][startY + j];
            }
        }
        for (int i = 0; i < GLIDERGUNY; i++) {
            for (int j = 0; j < GLIDERGUNX; j++) {
                mat1[startX + i][startY + j] = mat2[startX + i][startY + j];
                mat2[startX + i][startY + j] = tempArray[i][j];
            }
        }    
        // Estos dos pasos me aseguran que no se me rompa todo al poner pasos mayores a 15
        mat1[startX + GLIDERGUNY - 1][startY + 22] = 0;
        mat1[startX + GLIDERGUNY - 1][startY + 22] = 0;
    }

    if (orientation == "dr") {
        flipVertical(startX, startY, GLIDERGUNY, GLIDERGUNX);
    } else if (orientation == "ul") {
        flipHorizontal(startX, startY, GLIDERGUNY, GLIDERGUNX);
    } else if (orientation == "dl") {
        flipVertical(startX, startY, GLIDERGUNY, GLIDERGUNX);
        flipHorizontal(startX, startY, GLIDERGUNY, GLIDERGUNX);
    }
}

// Función para crearme gliders individuales
void glider(int startX, int startY, int step, string orientation){
    for (int i = startX; i < startX + 4; i++) {
        for (int j = startY; j < startY + 4; j++) {
            mat1[i][j] = 0;
        }
    }
    mat1[startX][startY + 1] = 1;
    mat1[startX + 1][startY + 1] = 1;
    mat1[startX + 2][startY + 1] = 1;
    mat1[startX + 2][startY + 2] = 1;
    mat1[startX + 1][startY + 3] = 1;

    int tempArray[4][4];
    for (int steps = 0; steps < step; steps++) {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                mat2[startX + i][startY + j] = survives(startX + i, startY + j);
            }
        }
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                tempArray[i][j] = mat1[startX + i][startY + j];
            }
        }
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                mat1[startX + i][startY + j] = mat2[startX + i][startY + j];
                mat2[startX + i][startY + j] = tempArray[i][j];
            }
        }    
    }
    if (orientation == "ur") {
        flipVertical(startX, startY, 4, 4);
    } else if (orientation == "dl") {
        flipHorizontal(startX, startY, 4, 4);
    } else if (orientation == "ul") {
        flipVertical(startX, startY, 4, 4);
        flipHorizontal(startX, startY, 4, 4);
    }
}

// Función para crearme un Eater con las diferentes orientaciones posibles
void eaterV(int startX, int startY, string orientation){
    for (int i = 0; i < EATER; i++) {
        for (int j = 0; j < EATER; j++) {
            mat1[startX + i][startY + j] = 0;
        }
    }
    mat1[startX][startY] = 1;
    mat1[startX + 1][startY] = 1;
    mat1[startX][startY + 1] = 1;
    mat1[startX + 2][startY + 1] = 1;
    mat1[startX + 2][startY + 2] = 1;
    mat1[startX + 2][startY + 3] = 1;
    mat1[startX + 3][startY + 3] = 1;
    if (orientation == "dr") {
        flipVertical(startX, startY, EATER, EATER);
    } else if (orientation == "ul") {
        flipHorizontal(startX, startY, EATER, EATER);
    } else if (orientation == "dl") {
        flipVertical(startX, startY, EATER, EATER);
        flipHorizontal(startX, startY, EATER, EATER);
    }
}
void eaterH(int startX, int startY, string orientation){
    for (int i = 0; i < EATER; i++) {
        for (int j = 0; j < EATER; j++) {
            mat1[startX + i][startY + j] = 0;
        }
    }
    mat1[startX][startY] = 1;
    mat1[startX][startY + 1] = 1;
    mat1[startX + 1][startY] = 1;
    mat1[startX + 1][startY + 2] = 1;
    mat1[startX + 2][startY + 2] = 1;
    mat1[startX + 3][startY + 2] = 1;
    mat1[startX + 3][startY + 3] = 1;
    if (orientation == "dr") {
        flipVertical(startX, startY, EATER, EATER);
    } else if (orientation == "ul") {
        flipHorizontal(startX, startY, EATER, EATER);
    } else if (orientation == "dl") {
        flipVertical(startX, startY, EATER, EATER);
        flipHorizontal(startX, startY, EATER, EATER);
    }
}

void screen(){
    glClear(GL_COLOR_BUFFER_BIT);

    for (int i = 0; i < NX; i++) {
        for (int j = 0; j < NY; j++) {
            glPointSize(PIXEL);     // Define el tamaño del pixel a dibujar
            glColor3f(mat1[i][j], mat1[i][j], mat1[i][j]);      // Pinta el pixel de negro si está muerta, y de blanco si está viva
            glBegin(GL_POINTS);     // Empieza a dibujar puntos
                glVertex2f((i + 0.5) * PIXEL, j * PIXEL);       // Multiplico por PIXEL para pasarlo de numero de celda a coordenada
            glEnd();
        }
    }

    for (int i = 0; i < NX; i++) {
        for (int j = 0; j < NY; j++) {
            mat2[i][j] = survives(i, j);
        }
    }
    glutSwapBuffers();
}

// Compuerta NOT. FUNCIONANDO
void notGate(int startX, int startY, string orientation){
    gliderGunH(startX + 90, startY + 1, "ur", 21);
    eaterH(startX + 167, startY + 25, "dr");
    
    gliderGunV(startX + 65, startY + 109, "dr", 22);
    gliderGunV(startX + 47, startY + 200, "dr", 17);
    gliderGunH(startX + 30, startY + 187, "ur", 28);
    for (int i = 0; i < 3; i++){
        glider(startX + 79 + 15 * i, startY + 114 - 15 * i, 3, "dr");
        glider(startX + 60 + 15 * i, startY + 206 - 15 * i, 2, "dr");
    }
    for (int i = 0; i < 4; i++){
        glider(startX + 72 + 15 * i, startY + 121 - 15 * i, 1, "dr");
    }
    glider(startX + 110, startY + 7, 2, "ur");
    glider(startX + 52,startY + 195, 1, "ur");
    if (orientation == "dr") {
        flipVertical(startX, startY, GATE, GATE);
    } else if (orientation == "ul") {
        flipHorizontal(startX, startY, GATE, GATE);
    } else if (orientation == "dl") {
        flipVertical(startX, startY, GATE, GATE);
        flipHorizontal(startX, startY, GATE, GATE);
    }
}

// Compuerta AND. FUNCIONANDO
void andGate(int startX, int startY, string orientation){
    gliderGunV(startX + 65, startY + 6, "ur", 1);
    gliderGunH(startX + 1, startY + 113, "dr", 9);
    eaterH(startX + 81, startY + 82, "ur");
    eaterH(startX + 42, startY + 122, "dl");
    gliderGunH(startX + 6, startY + 166, "dr", 28);
    gliderGunV(startX + 46, startY + 156, "dl", 9);
    gliderGunV(startX + 149, startY + 216, "dl", 2);
    gliderGunV(startX + 154, startY + 51, "ur", 1);
    gliderGunH(startX + 167, startY + 68, "ul", 20);
    gliderGunV(startX + 138, startY + 125, "ur", 15);
    gliderGunH(startX + 121, startY + 165, "dr", 26);
    eaterV(startX + 102, startY + 151, "ul");
    eaterV(startX + 168, startY + 201, "ul");
    gliderGunH(startX + 230, startY + 121, "ul", 12);
    eaterV(startX + 168, startY + 23, "dr");
    if (orientation == "dr") {
        flipVertical(startX, startY, GATE, GATE);
    } else if (orientation == "ul") {
        flipHorizontal(startX, startY, GATE, GATE);
    } else if (orientation == "dl") {
        flipVertical(startX, startY, GATE, GATE);
        flipHorizontal(startX, startY, GATE, GATE);
    }
}

// Compuerta OR. FUNCIONANDO
void orGate(int startX, int startY, string orientation){
    gliderGunH(startX + 94, startY + 20, "ur", 2);
    gliderGunV(startX + 134, startY + 3, "ul", 13);
    eaterH(startX + 163, startY + 32, "dr");
    gliderGunV(startX + 253, startY + 31, "ul", 20);
    gliderGunH(startX + 1, startY + 121, "ur", 9);
    gliderGunV(startX + 66, startY + 111, "dr", 22);
    gliderGunH(startX + 105, startY + 119, "ur", 3);
    gliderGunH(startX + 61, startY + 220, "ur", 15);
    gliderGunV(startX + 78, startY + 233, "dr", 4);
    eaterV(startX + 146, startY + 91, "dl");
    gliderGunV(startX + 200, startY + 161, "dr", 5);
    gliderGunH(startX + 213, startY + 171, "dl", 24);
    eaterH(startX + 171, startY + 158, "dr");
    eaterH(startX + 251, startY + 178, "dr");
    eaterH(startX + 215,startY + 215, "ur");
    eaterV(startX + 91, startY + 177, "dr");
    eaterV(startX + 140, startY + 246, "ur");
    gliderGunH(startX + 161, startY + 220, "ur", 25);
    gliderGunV(startX + 178, startY + 233, "dr", 14);

    glider(startX + 205, startY + 225, 3, "dr");
    glider(startX + 190, startY + 240, 3, "dr");
    glider(startX + 182, startY + 227, 2, "ur");
    glider(startX + 111, startY + 220, 3, "dr");
    glider(startX + 96, startY + 235, 3, "dr");
    glider(startX + 88, startY + 242, 1, "dr");
    glider(startX + 87, startY + 232, 2, "ur");
    glider(startX + 80, startY + 225, 0, "ur");

    // Todos estos gliders son para determinar la configuración inicial, hace que no se rompa todo.
    for (int i = 0; i < 5; i++){
        glider(startX + 73 + 15 * i, startY + 123 - 15 * i, 1, "dr");
        glider(startX + 80 + 15 * i, startY + 116 - 15 * i, 3, "dr");
    } 
    for (int i = 0; i < 6; i++){
        glider(startX + 26 + 15 * i, startY + 132 + 15 * i, 0, "ur");
        glider(startX + 33 + 15 * i, startY + 139 + 15 * i, 2, "ur");
        glider(startX + 211 - 15 * i, startY + 211 - 15 * i, 0, "ur");
        glider(startX + 203 - 15 * i, startY + 203 - 15 * i, 2, "ur");
    }
    for (int i = 0; i < 3; i++){
        glider(startX + 187 + 15 * i, startY + 132 + 15 * i, 3, "dl");
    }    
    glider(startX + 224, startY + 169, 1, "dl");
    glider(startX + 210, startY + 170, 2, "dr");

    if (orientation == "dr") {
        flipVertical(startX, startY, GATE, GATE);
    } else if (orientation == "ul") {
        flipHorizontal(startX, startY, GATE, GATE);
    } else if (orientation == "dl") {
        flipVertical(startX, startY, GATE, GATE);
        flipHorizontal(startX, startY, GATE, GATE);
    }
}

// Compuerta no lógica, rota un glider stream viajando UP-RIGHT, a uno viajando DOWN-RIGHT. FUNCIONANDO
void rotator(int startX, int startY, string orientation){

    gliderGunV(startX + 32, startY + 217, "dr", 9); //9
    gliderGunH(startX + 4, startY + 99, "ur", 54);

    gliderGunV(startX + 44, startY + 82, "ul", 35);
    eaterV(startX + 101, startY + 182, "ur");
    eaterH(startX + 16, startY + 172, "ul");
    eaterV(startX + 109, startY + 112, "ur");
    eaterV(startX + 130, startY + 89, "dl");
    gliderGunV(startX + 147, startY + 22, "ul", 28);

    gliderGunV(startX + 172, startY + 147, "dr", 37);
    gliderGunH(startX + 185, startY + 157, "dl", 26);

    for (int i = 0; i < 6; i++){
        glider(startX + 144 - 15 * i, startY + 43 + 15 * i, 3, "ul");
        glider(startX + 136 - 15 * i, startY + 51 + 15 * i, 1, "ul");
    } 

    for (int i = 0; i < 7; i++){
        glider(startX + 51 + 15 * i, startY + 217 - 15 * i, 0, "dr");
    }
    for (int i = 0; i < 8; i++){
        glider(startX + 43 + 15 * i, startY + 225 - 15 * i, 2, "dr");
    }
    for (int i = 0; i < 3; i++){
        mat1[startX + 156][startY + 110 + i] = 1;
        mat1[startX + 156][startY + 116 + i] = 1;
        mat1[startX + 152 + i][startY + 114] = 1;
        mat1[startX + 158 + i][startY + 114] = 1;
        glider(startX + 166 + 15 * i, startY + 125 + 15 * i, 3, "dl");
        mat1[startX + 189 + i][startY + 153] = 1;
    }
    glider(startX + 181, startY + 156, 0, "dr");
    glider(startX + 25, startY + 106, 1, "ur");
    glider(startX + 32, startY + 113, 3, "ur");
    glider(startX + 47, startY + 128, 3, "ur");
    glider(startX + 39, startY + 105, 2, "ul");

    mat1[startX + 189][startY + 149] = 1;
    mat1[startX + 190][startY + 149] = 1;
    mat1[startX + 191][startY + 149] = 1;
    mat1[startX + 193][startY + 149] = 1;
    mat1[startX + 190][startY + 150] = 1;
    mat1[startX + 194][startY + 150] = 1;
    mat1[startX + 190][startY + 151] = 1;
    mat1[startX + 192][startY + 151] = 1;
    mat1[startX + 193][startY + 151] = 1;
    mat1[startX + 194][startY + 151] = 1;
    mat1[startX + 192][startY + 152] = 1;
    mat1[startX + 193][startY + 152] = 1;

    if (orientation == "dr"){
        flipVertical(startX, startY, GATE, GATE);
    } else if (orientation == "ul"){
        flipHorizontal(startX, startY, GATE, GATE);
    } else if (orientation == "dl"){
        flipVertical(startX, startY, GATE, GATE);
        flipHorizontal(startX, startY, GATE, GATE);
    }

}

// Compuerta no lógica que permite que dos streams se crucen sin romperse, acepta "l" o "r" como orientación. FUNCIONANDO
void crossover(int startX, int startY, string orientation){
    gliderGunH(startX + 5, startY + 110, "ur", 0);
    gliderGunH(startX + 55, startY + 219, "ur", 11);
    gliderGunV(startX + 72, startY + 232, "dr", 0);
    eaterH(startX + 101, startY + 166, "dr");
    eaterV(startX + 141, startY + 232, "ur");
    gliderGunH(startX + 131, startY + 88, "ur", 18);
    gliderGunV(startX + 148, startY + 101, "dr", 7);
    gliderGunV(startX + 258, startY + 135, "dl", 11);
    if (orientation == "r"){
        flipHorizontal(startX, startY, GATE, GATE);
    } 
}

// Compuerta no lógica que duplica la entrada. FUNCIONANDO
void duplicator(int startX, int startY, string orientation){
    gliderGunH(startX + 5, startY + 98, "ur", 28); 
    gliderGunV(startX + 45, startY + 81, "ul", 9); 
    gliderGunV(startX + 148, startY + 21, "ul", 2); 
    gliderGunV(startX + 153, startY + 66, "ul", 0); 
    gliderGunH(startX + 113, startY + 83, "ur", 19); 
    gliderGunV(startX + 33, startY + 216, "dr", 13); 
    gliderGunH(startX + 128, startY + 173, "dr", 24); 
    gliderGunV(startX + 145, startY + 133, "ur", 13); 
    gliderGunH(startX + 215, startY + 141, "ul", 11); 
    eaterH(startX + 17, startY + 171, "ul");
    eaterV(startX + 110, startY + 112, "ur");
    eaterV(startX + 102, startY + 181, "ur");
    eaterH(startX + 124, startY + 251, "ul");

    for (int i = 0; i < 6; i++){
        glider(startX + 214 - 15 * i, startY + 160 + 15 * i, 0, "ul");
        glider(startX + 221 - 15 * i, startY + 153 + 15 * i, 0, "ul");
        glider(startX + 144 - 15 * i, startY + 43 + 15 * i, 3, "ul");
        glider(startX + 136 - 15 * i, startY + 51 + 15 * i, 3, "ul");
        glider(startX + 46 + 15 * i, startY + 222 - 15 * i, 2, "dr");
    }
    for (int i = 0; i < 5; i++){
        glider(startX + 53 + 15 * i, startY + 215 - 15 * i, 0, "dr");
    }    
}

// Compuerta que crea stream de gliders para crear un bit 1. FUNCIONANDO
void gliderGate(int startX, int startY, string orientation){
    gliderGunV(startX + GATE - 39, startY + GATE - 51, "ur", 29);
    gliderGunH(startX + GATE - 56, startY + GATE - 11, "dr", 10);

    if (orientation == "dr") {
        flipVertical(startX, startY, GATE, GATE);
    } else if (orientation == "ul") {
        flipHorizontal(startX, startY, GATE, GATE);
    } else if (orientation == "dl") {
        flipVertical(startX, startY, GATE, GATE);
        flipHorizontal(startX, startY, GATE, GATE);
    }
}

void swap() {
    int **tmp;
    tmp = mat1;
    mat1 = mat2;
    mat2 = tmp;
}

void timer(int) {
    screen();
    swap();
    glutTimerFunc(FRAMERATE, timer, 0);
}

int main (int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitWindowSize(XSIZE, YSIZE);
    glutInitWindowPosition(10,10);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutCreateWindow("Game of Life");
    setup();

    gliderGate(1, 1, "ur");
    notGate(GATE + 1, GATE + 1, "ur");

    glutDisplayFunc(screen);
    timer(0);
    glutMainLoop();

    return EXIT_SUCCESS;
}