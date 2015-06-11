#ifndef __RENDERENGINE
#define __RENDERENGINE

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "GLHelper.h"
#include "MazeModel.h"

class RenderEngine
{
public:
	Maze maze;
	MazeModel model;
	unsigned int w;
	unsigned int h;
	RenderEngine()
	{

		initialized = false;
		this->P = glm::ortho(-1, 1, -1, 1);

	}

	~RenderEngine()
	{
		if(initialized)
		{
			// Clean up the buffers
			glDeleteBuffers(1, &positionBuffer);
			glDeleteBuffers(1, &elementBuffer);
			glDeleteVertexArrays(1, &vertexArray);
		}
	}

	void init(unsigned int const & w, unsigned int const & h)
	{
		time = 0;
		this->w = w;
		this->h = h;
		
		M=glm::mat4(1);

		setupGlew();
		setupShader();
		generateMaze();
		initialized = true;
	}

	void display(bool pickingEnabled=false)
	{
		time++;
		glEnable(GL_DEPTH_TEST);
		//clear the old frame
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		//use shader
		glUseProgram(shaderProg);
		glUniform1f(timeSlot, time);
		glUniformMatrix4fv(matSlot1, 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(matSlot2, 1, GL_FALSE, &V[0][0]);
		//draw
		glBindVertexArray(vertexArray);
		glDrawElements(GL_TRIANGLES, model.getElementCount(), GL_UNSIGNED_INT,0);
		//cleanup
		glBindVertexArray(0);
		glUseProgram(0);
		checkGLError("display");
	}
	
	void reshape(int const & newWidth, int const & newHeight)
	{
		glViewport(0, 0, newWidth, newHeight);
	}
	void setTransform(glm::mat4 trans){
		this->M = trans;
	}
	void setView(glm::mat4 view){
		this->V = view;
	}
	void setProjection(glm::mat4 project){
		this->P = project;
	}
	void generateMaze(unsigned int const & seed = 1)
	{
		Maze mazeLayout(w, h, seed);
		maze = mazeLayout;
		model = MazeModel(mazeLayout);
		
		this->P = glm::perspective(90.0f, 1.0f, 0.1f*model.getUnitSize(), 2.0f*w*model.getUnitSize());
		
		this->V = glm::lookAt(glm::vec3(-5.0f*model.getUnitSize(), 0.5*h*model.getUnitSize(), 0.5*model.getUnitSize()), glm::vec3(0.0f, 0.5*h*model.getUnitSize(), 0.5*model.getUnitSize()), glm::vec3(0.0f, 0.0f, 1.0f));

		unsigned int x, y;
		mazeLayout.getLeftOpening(x, y);
		float yfloat = y*1.0;
		int width = w;
		if(initialized)
			rebuildBuffers();
		else
			setupBuffers();
	}

private:

	bool initialized;

	GLuint shaderProg;
	GLfloat time;
	GLuint wallBuffer;
	GLuint positionBuffer;
	GLuint elementBuffer;
	GLuint vertexArray;
	GLuint normalBuffer;

	GLint positionSlot;
	GLint normalSlot;
	GLint timeSlot;
	GLint matSlot1;
	GLint matSlot2;
	

	
	glm::mat4 P;
	glm::mat4 V;
	glm::mat4 M;
	void setupGlew()
	{
		glewExperimental = GL_TRUE;
		GLenum err = glewInit();
		if (GLEW_OK != err)
		{
			fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
			exit(1);
		}
		printf("Using GLEW %s\n", glewGetString(GLEW_VERSION));
	}

	void setupShader()
	{
		char const * vertPath = "Shaders/simple.vert";
		char const * fragPath = "Shaders/simple.frag";

		shaderProg = ShaderManager::shaderFromFile(&vertPath, &fragPath, 1, 1);

		// Find out where the shader expects the data
		positionSlot = glGetAttribLocation(shaderProg, "pos");
		normalSlot = glGetAttribLocation(shaderProg, "norm");
		matSlot1 =      glGetUniformLocation(shaderProg, "P");
		matSlot2 = glGetUniformLocation(shaderProg, "V");
		timeSlot = glGetUniformLocation(shaderProg, "time");
		checkGLError("shader");
	}

	void setupBuffers()
	{
		checkGLError("setup0");
		//setup position buffer
		glGenBuffers(1, &positionBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
		glBufferData(GL_ARRAY_BUFFER, model.getPositionBytes(), model.getPosition(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		checkGLError("setup1");
		glGenBuffers(1, &normalBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
		glBufferData(GL_ARRAY_BUFFER, model.getNormalBytes(), model.getNormals(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		checkGLError("setup2");
		// now the elements
		glGenBuffers(1, &elementBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, model.getElementBytes(), model.getElements(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	
		//setup the vertex array
		glGenVertexArrays(1, &vertexArray);
		checkGLError("setup3");
		glBindVertexArray(vertexArray);
		glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
		glEnableVertexAttribArray(positionSlot);
		glVertexAttribPointer(positionSlot, 3, GL_FLOAT, GL_FALSE, 0, 0);
		checkGLError("setup4");
		glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
		checkGLError("setup5");
		glEnableVertexAttribArray(normalSlot);
		checkGLError("setup6");
		glVertexAttribPointer(normalSlot, 3, GL_FLOAT, GL_FALSE, 0, 0);
		checkGLError("setup7");
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
		checkGLError("setup8");
		glBindVertexArray(0);
		checkGLError("setup9");

		checkGLError("setup");
	}
	
	void rebuildBuffers()
	{
		
		glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, model.getPositionBytes(), model.getPosition());
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, model.getElementBytes(), model.getElements());
		
		
		glBindVertexArray(vertexArray);	
		glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
		glEnableVertexAttribArray(positionSlot);
		glVertexAttribPointer(positionSlot, 3, GL_FLOAT, GL_FALSE, 0, 0);
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
		glBindVertexArray(0);
		
		checkGLError("rebuild");
	}
};

#endif