/*
 * nomprograma: Curva Relacional de Bezier | TAREA 03
 * descripcion: programa que grafica una curva
 *				usando el algoritmo Beizer Relacional
 *				ingresando puntos de control por el mouse
 *				tomando sus coordenadas. Se hace uso de Listas de Visualizacion
 *
 *				Se hace uso de un menu de opciones mostrando
 *				el modo en cual se esta usando, que originalmente
 *				es: |SIN ASIGNACION...|. El menu comprende:
 *					-Agregar Punto
 *					-Eliminar punto
 *					-Mover punto
 *					-Subir Peso			(aumentar el w[i])
 *					-Bajar Peso			(Disminuir el w[i])
 *					-Limpiar Ventana	(Esta opcion se puede realizar con la tecla "l")
 *					-Ocultar Curva
 *					-Cambiar Color Curva	(Pasa de Rojo -> Verde -> Azul siendo una lista circular)
 *					-Ocultar Puntos
 *					-Cambiar Color Puntos	(Pasa de Azul -> Rojo -> Verde siendo una lista circular)
 *					-Cerrar programa
 *				Siendo cada opcion autoexplicable.
 *
 * autores:
 *		-Oroche Quispe, Frank Anthony	(16200201)
 *		-Pacora Vega, Jonatan			(12200109)
 * fecha: 17/09/2020
 */
#include <GL/glut.h>
#include <math.h>

#define max_puntos 150 // numero maximo de puntos de control
#define miListaCurva 2
#define miListaPuntosControl 1

/* Estructura Punto */
struct Point {
	float x;
	float y;
};

/* Prototipos */
void Init(int , char *[]);
void reshape(int , int );
void keyboard(unsigned char, int, int);
void display(void);
void DibujarContenido(const double );
void click(int , int , int , int ); // Evento al realizar un click en la ventana
void menu_opciones(void); // Menu de opciones
void Modo(int ); // Modo que se esta usando
void Agregar_punto_control(int , int );
void Eliminar_punto_control(int , int );
void Subir_peso(int , int );
void Bajar_peso(int , int );
void dibujar_curva(double);
void dibujar_puntos_control(int );
void reiniciar(void); // Reinicia la cantidad de puntos de control a 0, limpiando la ventana
double factorial(int ); // Funcion factorial
double CoeficienteNewton(int , int ); // Funcion Coeficiente de Newton
double B(int , int , const double ); // B(t)
Point BezierRelacional(const double ); // funcion que calculo puntos de la curva relacional de Beizer

/* Variables Globales */
int opcion = 0; // Opcion del menu
int total_puntos = 0; // Numero total de puntos ingresados
int x_inicial = 0; // Valor inicial de x
int y_inicial = 0; // Valor inicial de y
int colorPuntosControl=3; // Color de los puntos de control
int colorPuntosCurva=1; // Color de los puntos de la Curva
Point puntos_control[max_puntos]; // Arreglo para los puntos de control
Point temp[max_puntos]; // Estructura temporal
float w[max_puntos]; // Arreglo de pesos
bool mostrarCurva= true, mostrarPuntos= true;

int main(int argc, char *argv[]) {
	Init(argc,argv);
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutMouseFunc(click);
    menu_opciones();
    glutKeyboardFunc(keyboard);
	glutMainLoop();
	return 0;
}

void Init(int argc, char *argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(1200, 800);
	glutCreateWindow("Algoritmo Relacional de Bezier | Tarea 03 - Oroche Quispe - Pacora Vega");
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glColor3f(1.0, 1.0, 1.0);
    glViewport(0, 0, 1080, 900);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, 1080, 0, 900);
}

void reshape(int x, int y) {
	glViewport(0, 0, x, y);
}

void display(void) {
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	const double w = glutGet(GLUT_WINDOW_WIDTH);
    const double h = glutGet(GLUT_WINDOW_HEIGHT);
	gluOrtho2D(0.0, w, 0.0, h);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	DibujarContenido(h);
	glFlush();
}

void DibujarContenido(const double h) {
	dibujar_puntos_control(h);
    if (mostrarPuntos) {glCallList(miListaPuntosControl);}
    dibujar_curva(h);
    if (mostrarCurva) {glCallList(miListaCurva);}
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
		case 'l': // l para Limpiar Ventana
			Modo(6); // si se presiona l el modo pasara al estado 4 el cual indica que se reinicie los puntos de control
	        break;
		case 27:
			exit(0); // si se presiona esc se cerrara el programa
			break;
	}
}

/* Evento al realizar un click en la ventana */
void click(int button, int state, int x, int y) {
	/*Si realiza un click izquierdo  */
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		if (opcion == 1) {
			Agregar_punto_control(x, y);
		} else if (opcion == 2) {
			Eliminar_punto_control(x, y);
		} else if (opcion == 3) {
			/* se asigna los valores para poder realizar el movimiento de un punto */
			x_inicial  = x;
			y_inicial  = y;
		} else if (opcion == 4) {
			Subir_peso(x, y);
		} else if (opcion == 5) {
			Bajar_peso(x, y);
		}
	}
	/*Si realiza un click izquierdo y se mantiene para realizar el movimeinto de un punto */
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		for (int i = 0; i < total_puntos; i++) {
			if (abs(x_inicial  - puntos_control[i].x) < 10 && abs(puntos_control[i].y - y_inicial ) < 10) {
				puntos_control[i].x = x;
				puntos_control[i].y = y;
				break;
			}
		}
	}
	glutPostRedisplay();
}

/* Menu de opciones */
void menu_opciones(void) {
	glutCreateMenu(Modo);
	glutAddMenuEntry("|___SIN ASIGNACION...___|", 0);
	glutAddMenuEntry("|1) Agregar Punto       |", 1);
	glutAddMenuEntry("|2) Eliminar Punto      |", 2);
	glutAddMenuEntry("|3) Mover Punto         |", 3);
	glutAddMenuEntry("|4) Subir Peso          |", 4);
	glutAddMenuEntry("|5) Bajar Peso          |", 5);
	glutAddMenuEntry("|6) Limpiar Ventana     |", 6);
	glutAddMenuEntry("|7) Ocultar Curva       |", 7);
	glutAddMenuEntry("|8) Cambiar Color Curva |", 8);
	glutAddMenuEntry("|9) Ocultar Puntos      |", 9);
	glutAddMenuEntry("|10)Cambiar Color Puntos|", 10);
	glutAddMenuEntry("|11)Cerrar Programa     |", 11);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

/* Modo que se esta usando */
void Modo(int n) {
	opcion = n;
	switch(opcion){
		case 0:
			glutChangeToMenuEntry(1,"|___SIN ASIGNACION...___|",0);
			break;
		case 1:
			glutChangeToMenuEntry(1,"MODO: Agregar",0);
			break;
		case 2:
			glutChangeToMenuEntry(1,"MODO: Eliminar",0);
			break;
		case 3:
			glutChangeToMenuEntry(1,"MODO: Mover",0);
			break;
		case 4:
			glutChangeToMenuEntry(1,"MODO: Subir Peso",0);
			break;
		case 5:
			glutChangeToMenuEntry(1,"MODO: Bajar Peso",0);
			break;
		case 6:
			reiniciar();
			glutChangeToMenuEntry(1,"|VENTANA LIMPIADA |",0);
			break;
        case 7:
        	glutChangeToMenuEntry(1,"|___SIN ASIGNACION...___|",0);
        	if(mostrarCurva){
        		mostrarCurva=false;
				glutChangeToMenuEntry(8,"|7) Mostrar Curva       |",7);
			} else {
				mostrarCurva=true;
				glutChangeToMenuEntry(8,"|7) Ocultar Curva       |",7);
			}
			break;
	     case 8:
	    	colorPuntosCurva++;
	    	if(colorPuntosCurva>3) {colorPuntosCurva=1;}
            glutChangeToMenuEntry(1,"|___SIN ASIGNACION...___|",0);
			break;
		case 9:
        	glutChangeToMenuEntry(1,"|___SIN ASIGNACION...___|",0);
        	if(mostrarPuntos){
        		mostrarPuntos=false;
				glutChangeToMenuEntry(10,"|9) Mostrar Puntos      |",9);
			} else {
				mostrarPuntos=true;
				glutChangeToMenuEntry(10,"|9) Ocultar Puntos      |",9);
			}
			break;
        case 10:
            colorPuntosControl++;
	    	if(colorPuntosControl>3) {colorPuntosControl=1;}
            glutChangeToMenuEntry(1,"|___SIN ASIGNACION...___|",0);
			break;
		default:
			exit(0);
	}
	glutPostRedisplay();
}

/* Agregar un nuevo punto de control */
void Agregar_punto_control(int x, int y) {
	puntos_control[total_puntos].x = x;
	puntos_control[total_puntos].y = y;
	w[0]=w[total_puntos]=1;
	if(total_puntos!=0 || total_puntos!=1){
		w[total_puntos-1]=30;
	}
	total_puntos++;
}

/* Eliminar un punto de control seleccionado */
void Eliminar_punto_control(int x, int y) {
	for (int i = 0; i < total_puntos; i++) {
		if (abs(x - puntos_control[i].x) < 15 && abs(puntos_control[i].y - y) < 15) {
			for (int j = i; j < total_puntos - 1; j++) {
				puntos_control[j].x = puntos_control[j + 1].x;
				puntos_control[j].y = puntos_control[j + 1].y;
			}
			total_puntos--;
			break;
		}
	}
}

/* Sube el peso del punto de control seleccionado */
void Subir_peso(int x, int y) {
	for (int i = 0; i < total_puntos; i++) {
		if (abs(x - puntos_control[i].x) < 15 && abs(puntos_control[i].y - y) < 15) {
			if(i!=0 && i!=total_puntos-1){
				if(w[i]>=1){
					w[i]+=1;
				} else {
					w[i]+=0.1;
				}
			}
			break;
		}
	}
}

/* Baja el peso del punto de control seleccionado */
void Bajar_peso(int x, int y) {
	for (int i = 0; i < total_puntos; i++) {
		if (abs(x - puntos_control[i].x) < 15 && abs(puntos_control[i].y - y) < 15) {
			if(i!=0 && i!=total_puntos-1){
				if(w[i]>=1){
					w[i]-=1;
				} else {
					w[i]-=0.1;
				}
			}
			break;
		}
	}
}

/* Calulcar el factorial de "n" */
double factorial(int n) {
    double p = 1;
    for (int i = 1; i <= n; i++) {p = p * (double) i;}
    return p;
}

/* Calcula el coeficiente binomial (n,k) */
double CoeficienteNewton(int n, int k) {
    return factorial(n) / (factorial(k) * factorial(n - k));
}

/* B(t) */
double B(int n, int i, const double t){
	return CoeficienteNewton(n,i)*pow(t,i)*pow((1-t),(n-i));
}

/* Algoritmo Relacional de Bezier */
Point BezierRelacional(const double t){
	Point P;
	double numX = 0, numY = 0, den = 0;
	/* Asignacion del primer y ultimo peso */
	w[0]=w[total_puntos]=1;
	for(int i=0;i<total_puntos;i++){
		temp[i] = puntos_control[i];
	}
	/* Sumatoria del numerador */
	for(int i=0;i<total_puntos;i++){
    	numX = numX + (temp[i].x*B((total_puntos-1),i,t)*w[i]);
    	numY = numY + (temp[i].y*B((total_puntos-1),i,t)*w[i]);
	}
	/* Sumatoria del denominador */
	for(int i=0;i<total_puntos;i++){
    	den = den + (B((total_puntos-1),i,t)*w[i]);
	}
	P.x=numX/den;
	P.y=numY/den;
	return P;
}

/* Dibuja la curva utilizando el algoritmo de CastelJau */
void dibujar_curva(double h){
	glNewList(miListaCurva,GL_COMPILE);
		/* Asigna el color de la curva */
		int color=colorPuntosCurva;
		switch(color){
			case 1:
				glColor3f(1, 0, 0);
				break;
			case 2:
				glColor3f(0, 1, 0);
				break;
			case 3:
				glColor3f(0, 0, 1);
				break;
		}
		/* Dibuja la curva */
		for (int i = 0; i < 10000; i++) {
			if (total_puntos < 1){break;}
			double t = (double)i/9999.0;
			Point p = BezierRelacional(t);
			glPointSize(2.0f);
			glBegin(GL_POINTS);
				int x = p.x, y = p.y;
				glVertex2i(x, h - y);
			glEnd();
		}
	glEndList();
}

/* Dibuja los puntos de control igresados */
void dibujar_puntos_control(int h){
	glNewList(miListaPuntosControl, GL_COMPILE);
		/* Asigna el color de los puntos de control y sus uniones */
		int color=colorPuntosControl;
		switch(color){
			case 1:
				glColor3f(1, 0, 0);
				break;
			case 2:
				glColor3f(0, 1, 0);
				break;
			case 3:
				glColor3f(0, 0, 1);
				break;
		}
		/* Se dibuja los puntos de control */
		glPointSize(15.0f);
		glBegin(GL_POINTS);
			for (int i = 0; i < total_puntos; i++) {
				int x = puntos_control[i].x;
				int y = puntos_control[i].y;
				glVertex2i(x, h - y);
			}
		glEnd();
		/* Se dibuja las uniones de los puntos de control */
		glBegin(GL_LINE_STRIP);
			for (int i = 0; i < total_puntos; i++) {
				int x = puntos_control[i].x;
				int y = puntos_control[i].y;
				glVertex2i(x, h - y);
			}
		glEnd();
	glEndList();
}

/* Reinicia la cantidad de puntos de control a 0, limpiando la ventana */
void reiniciar(void) {
    total_puntos=0;
}
