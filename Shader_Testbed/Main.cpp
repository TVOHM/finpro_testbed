#include <GL\glew.h>
#include <GL\freeglut.h>

#include <string>
#include <stdio.h>
#include <time.h>
#include <vector>
#include <random> //using MT RNG
using namespace std;

bool Keys[256];

struct Camera
{
	Camera()
	{
		x = 0.0f; y = 0.0f; z = -200.0f;
	}
	void Execute()
	{
		if(Keys['w'])
			z+=1.0f;
		if(Keys['s'])
			z-=1.0f;
		if(Keys['a'])
			x+=1.0f;
		if(Keys['d'])
			x-=1.0f;
		if(Keys['q'])
			y+=1.0f;
		if(Keys['e'])
			y-=1.0f;
		glTranslatef(x, y, z);
	}
	float x, y, z, xrot, yrot, zrot;
};

Camera c;

GLuint programObject;


GLuint vertexShader;
const char* vertexShaderProgram =
	"void main(void)\n"
	"{\n"
	"	gl_Position = ftransform();\n"
	"}\n";

GLuint fragmentShader;
const char* fragmentShaderProgram =
	"void main(void)\n"
	"{\n"
	"gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);\n"
	"}\n";

GLuint geometryShader;
const char* geometryShaderProgram =
	"#version 150\n"
	"layout (triangles) in;\n"
	"layout (triangle_strip, max_vertices = 3) out;\n"
	"void main(void)\n"
	"{\n"
	"	for(int i=0; i<3; i++)\n"
	"	{\n"
	"		gl_Position = gl_in[i].gl_Position;\n"
	"		EmitVertex();\n"
	"	}\n"
	"	EndPrimitive();\n"
	"}\n";

float vertexPositions[] = {
	//								Font Triangle
	1.0f, 1.0f, 1.0f, 1.0f,		//Top
	1.0f, -1.0f, 1.0f, 1.0f,		//Right
	-1.0f, -1.0f, 1.0f, 1.0f,	//Left
	//								Font Triangle
	-1.0f, 1.0f, 1.0f, 1.0f,		//Top
	1.0f, 1.0f, 1.0f, 1.0f,		//Right
	-1.0f, -1.0f, 1.0f, 1.0f,	//Left

	//								Rear Triangle
	1.0f, 1.0f, -1.0f, 1.0f,		//Top
	1.0f, -1.0f, -1.0f, 1.0f,		//Right
	-1.0f, -1.0f, -1.0f, 1.0f,	//Left
	//								Rear Triangle
	-1.0f, 1.0f, -1.0f, 1.0f,		//Top
	1.0f, 1.0f, -1.0f, 1.0f,		//Right
	-1.0f, -1.0f, -1.0f, 1.0f,	//Left

	//								Top Triangle
	1.0f, 1.0f, 1.0f, 1.0f,		//Top
	1.0f, 1.0f, -1.0f, 1.0f,		//Right
	-1.0f, 1.0f, -1.0f, 1.0f,	//Left
	//								Top Triangle
	-1.0f, 1.0f, 1.0f, 1.0f,		//Top
	1.0f, 1.0f, 1.0f, 1.0f,		//Right
	-1.0f, 1.0f, -1.0f, 1.0f,	//Left

	//								Bottom Triangle
	1.0f, -1.0f, 1.0f, 1.0f,		//Top
	1.0f, -1.0f, -1.0f, 1.0f,		//Right
	-1.0f, -1.0f, -1.0f, 1.0f,	//Left
	//								Bottom Triangle
	-1.0f, -1.0f, 1.0f, 1.0f,		//Top
	1.0f, -1.0f, 1.0f, 1.0f,		//Right
	-1.0f, -1.0f, -1.0f, 1.0f,	//Left

	//								Right Triangle
	1.0f, 1.0f, 1.0f, 1.0f,		//Top
	1.0f, 1.0f, -1.0f, 1.0f,		//Right
	1.0f, -1.0f, 1.0f, 1.0f,	//Left
	//								Right Triangle
	-1.0f, -1.0f, 1.0f, 1.0f,		//Top
	1.0f, -1.0f, 1.0f, 1.0f,		//Right
	-1.0f, -1.0f, -1.0f, 1.0f,	//Left

	//								Left Triangle
	-1.0f, 1.0f, 1.0f, 1.0f,		//Top
	-1.0f, 1.0f, -1.0f, 1.0f,		//Right
	-1.0f, -1.0f, 1.0f, 1.0f,	//Left
	//								Left Triangle
	-1.0f, -1.0f, 1.0f, 1.0f,		//Top
	-1.0f, -1.0f, 1.0f, 1.0f,		//Right
	-1.0f, -1.0f, -1.0f, 1.0f,	//Left
};

int Number_Of_Boids = 1000;
float HalfWidth_Of_Boid = 1;
vector<float> vertexVector;
vector<float> positions;
vector<float> velocities;

void Initialise()
{
	srand((unsigned int)time(NULL));
	positions = vector<float>(Number_Of_Boids * 4, 1.0f);
	for(int i = 0; i < Number_Of_Boids * 4; i++)
		positions.at(i) = rand() % 250;
	velocities = vector<float>(Number_Of_Boids * 4, 1.0f);
	for(int i = 0; i < Number_Of_Boids * 4; i++)
		velocities.at(i) = rand() % 10;
	vertexVector = vector<float>(Number_Of_Boids * 36 * 4, 1.0f);
	for(int i = 0; i < Number_Of_Boids; i++)
		for(int j = 0; j < 36; j++)
			for(int k = 0; k < 3; k++)
				vertexVector.at((i*(36*4)) + (j*4) + k) = positions.at(i * 4 + k) + vertexPositions[j*4 + k];

}

GLuint positionBufferObject;
GLuint vao;

unsigned int last_redraw = 0;

void Display()
{

	unsigned int the_time = glutGet(GLUT_ELAPSED_TIME);
	unsigned int time_since_last_redraw = the_time - last_redraw;
	last_redraw = the_time;
	for(int i = 0; i < Number_Of_Boids; i++)
		for(int j = 0; j < 3; j++)
			positions.at(i * 4 + j) += velocities.at(i * 4 + j) * (time_since_last_redraw / 1000.0f);
	for(int i = 0; i < Number_Of_Boids; i++)
		for(int j = 0; j < 36; j++)
			for(int k = 0; k < 3; k++)
				vertexVector.at((i*(36*4)) + (j*4) + k) = positions.at(i * 4 + k) + vertexPositions[j*4 + k];

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();   	//saves the current matrix on the top of the matrix stack
    //glTranslatef(0,0,-200); //translates the current matrix 0 in x, 0 in y and -100 in z
	c.Execute();

	//glRotatef(120, 0,1,0);
	//glRotatef(pitch, 1,0,0);
	//glRotatef(rollTotal, 0,0,1);

	glUseProgram(programObject);

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * vertexVector.size(), &vertexVector[0]);

	glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glDrawArrays(GL_TRIANGLES, 0, 36 * Number_Of_Boids);

	glDisableVertexAttribArray(0);
	glUseProgram(0);

	//TODO: Printing text to a screen, how can I get this in say the top left?
	//glRasterPos3f (0, 0, 0);
	//glutBitmapCharacter(GLUT_BITMAP_8_BY_13, 'H');
	//glutBitmapCharacter(GLUT_BITMAP_8_BY_13, 'I');
	//glutBitmapCharacter(GLUT_BITMAP_8_BY_13, '!');

    glPopMatrix();      	//retrieves our saved matrix from the top of the matrix stack
    glutSwapBuffers();  	//swaps the front and back buffers
	glutPostRedisplay();
}

void Idle()
{

}

void Keyboard(unsigned char key, int x, int y)
{
	Keys[key] = true;
}

void KeyUp(unsigned char key, int x, int y)
{
	Keys[key] = false;
}

int main(int argc, char* argv[])
{
	Initialise();
	memset(Keys, false, sizeof(bool) * 256);
    glutInit(&argc,argv);                                     	//initializes the GLUT framework
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);   //sets up the display mode
	glutInitWindowSize(800, 800);
    glutCreateWindow("GL Testbed");              	//creates a window
	glewInit();

    glutDisplayFunc(Display);
	glutIdleFunc(Idle);
	glutKeyboardFunc(Keyboard);
	glutKeyboardUpFunc(KeyUp);

	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE ); //Lines!

	glEnable(GL_DEPTH_BUFFER);

    glMatrixMode(GL_PROJECTION);   //changes the current matrix to the projection matrix

    //sets up the projection matrix for a perspective transform
    gluPerspective(    45,   	 //view angle
   				 1.0,	//aspect ratio
   				 10.0,   //near clip
   				 800.0); //far clip

    glMatrixMode(GL_MODELVIEW);   //changes the current matrix to the modelview matrix

	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderProgram, NULL);
	glCompileShader(vertexShader);
	GLint vertextShaderCompiled = 0;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vertextShaderCompiled);
	if(!vertextShaderCompiled)
	{
		printf("Vertex shader compile FAILED");
	}
	else
		printf("Vertext shader compile OK");

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, (const GLchar**)&fragmentShaderProgram, NULL);
	glCompileShader(fragmentShader);
	GLint fragmentShaderCompiled = 0;
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fragmentShaderCompiled);
	if(!fragmentShaderCompiled)
		printf("Fragment shader compile FAILED");
	else
		printf("Fragment shader compile OK");

	geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
	glShaderSource(geometryShader, 1, (const GLchar**)&geometryShaderProgram, NULL);
	glCompileShader(geometryShader);
	GLint geometryShaderCompiled = 0;
	glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &geometryShaderCompiled);
	if(!geometryShaderCompiled)
		printf("Geometry shader compile FAILED");
	else
		printf("Geometry shader compile OK");

	programObject = glCreateProgram();
	glAttachShader(programObject, vertexShader);
	glAttachShader(programObject, fragmentShader);
	glAttachShader(programObject, geometryShader);
	glLinkProgram(programObject);

	GLint status;
	glGetProgramiv (programObject, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetProgramInfoLog(programObject, infoLogLength, NULL, strInfoLog);
		fprintf(stderr, "Linker failure: %s\n", strInfoLog);
		delete[] strInfoLog;
	}
	else
		printf("Link OK!");

	glGenBuffers(1, &positionBufferObject);

	glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertexVector.size(), &vertexVector[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

    glutMainLoop();           	//the main loop of the GLUT framework

    return 0;
}