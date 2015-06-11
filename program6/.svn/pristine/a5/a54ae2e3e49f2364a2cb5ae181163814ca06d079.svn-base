//Replace with your usernames!
#define printusers() printf("Program by Fryjc and Veatchje\n");

#define GLEW_STATIC
#include <stdio.h>
#include <stdlib.h>
#include "glew/glew.h"
#include <SFML/Window.hpp>
#define _USE_MATH_DEFINES
#include <math.h>
#include "ShaderManager.h"
#include "GLHelper.h"
#include "RenderEngine.h"
#include "Maze.h"
#include "MazeModel.h"

#define RESOLUTION 1080
#define TARGET_FPS 30              // controls spin update rate
#define TIME_WINDOW 3                // number of frames motion is valid after release


class Program4
{
public:
	glm::vec3 location;

	float directionX;
	float directionY;

	bool out;
	glm::mat4 rotate;
	glm::mat4 translate;

	Program4(unsigned int const & w, unsigned int const & h)
	{
		angle = 0;
		App = new sf::Window(sf::VideoMode(RESOLUTION, RESOLUTION, 32), "program5");
		render.init(w, h);
		size = render.model.getUnitSize();
		step = 2;
		location = glm::vec3(-5.0f*size, 0.5*render.h*size, 0.5*size);
		directionX = 1.0f;
		directionY = 0.0f;
		out = false;
		rotate = glm::mat4(1);
		translate = glm::mat4(1);

		while (App->IsOpened())
		{
			App->SetActive();
			float targetFrameTime = 1.0f / (float)TARGET_FPS;
			float sleepTime = targetFrameTime - App->GetFrameTime();
			if (sleepTime > 0)
				sf::Sleep(sleepTime);
			render.setTransform(glm::translate(glm::mat4(1), glm::vec3(0, 0, -1.9)));
			render.display();
			App->Display();
			handleEvents();
		}
	}

	// *glm::rotate(translate*rotate*glm::mat4(1), (float)-90., glm::vec3(1, 0, 0))*glm::rotate(glm::mat4(1), (float)-90., glm::vec3(0, 1, 0))
	~Program4()
	{
	}
	void toggleView(){
		if (out){
			render.setView(glm::lookAt(location, glm::vec3(location.x + directionX, location.y + directionY, location.z), glm::vec3(0.0f, 0.0f, 1.0f)));
			render.setProjection(glm::perspective(90.0f, 1.0f, 0.1f*size, 2.0f*render.w*size));
		}
		else{
			render.setView(glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0)));
			render.setProjection(glm::ortho(-size, (render.w + 1)*size, -size, (render.h + 1)*size));
		}
		out = !out;
	}
	void turn(){
		angle = (-position) / (RESOLUTION / 2) * 3 * M_PI;
		directionX = cos(angle);
		directionY = sin(angle);
		render.setView(glm::lookAt(location, glm::vec3(location.x + directionX, location.y + directionY, location.z), glm::vec3(0.0f, 0.0f, 1.0f)));
	}
	void clipForward(){
		int x, y;
		float directionXClip = directionX;
		float directionYClip = directionY;
		Maze maze = render.maze;
		if (location.x >= 0 && location.x < (render.w*size))
			x = floor((location.x) / size);
		else{
			if (location.x < 0)
				x = 0;
			else
				x = render.w - 1;
		}
		if (location.y >= 0 && location.y < (render.h*size))
			y = floor((location.y) / size);
		else{
			if (location.y < 0)
				y = 0;
			else
				y = render.h - 1;
		}
		if (fmodf((location.y), size) < (size / 8)){
			if ((directionX>0)&&(x != (render.w - 1))&&maze.bottomBlocked(x + 1, y))
				directionXClip = 0;
			if ((directionX<0)&&(x != 0)&&maze.bottomBlocked(x - 1, y))
				directionXClip = 0;
		}
		if (fmodf((location.y), size) > (7*size / 8)){
			if ((directionX > 0)&&(x != (render.w - 1))&&maze.topBlocked(x + 1, y))
				directionXClip = 0;
			if ((directionX < 0)&&(x != 0)&&maze.topBlocked(x - 1, y))
				directionXClip = 0;
		}
		if (fmodf((location.x), size) < (size / 8)){
			if ((directionY>0)&&(y != (render.h - 1))&&(maze.leftBlocked(x, y + 1)))
				directionYClip = 0;
			if ((directionY<0)&&(y != 0)&&(maze.leftBlocked(x, y - 1)))
				directionYClip = 0;
		}
		if (fmodf((location.x), size) >(7 * size / 8)){
			if ((directionY>0)&&(y != (render.h - 1))&&(maze.rightBlocked(x, y + 1)))
				directionYClip = 0;
			if ((directionY < 0)&&(y != 0)&&(maze.rightBlocked(x, y - 1)))
				directionYClip = 0;
		}
		printf("direction %i %i ", x,y);
		if ((testTop && maze.topBlocked(x, y))||(testBot && maze.bottomBlocked(x,y)))
			directionYClip = 0;
		if ((testLeft && maze.leftBlocked(x, y)) || (testRight && maze.rightBlocked(x, y)))
			directionXClip = 0;
		location = glm::vec3(location.x + directionXClip*size*0.05, location.y + directionYClip*size*0.05, location.z);
	}
	void forward(){
		xMargin = fmodf((location.x+size*100), size);
		yMargin = fmodf((location.y+size*100), size);
		testTop = (directionY>0 && yMargin>(3*size/4)&&location.y>0)||(location.y>(size*render.h)&&directionY<0);
		testBot = (directionY < 0 && yMargin < (1 * size / 4))||(location.y<0&&directionY>0);
		testRight = (directionX > 0 && xMargin > (3 * size / 4)&&location.x>0)||(location.x>(size*render.w)&&directionX<0);
		testLeft = (directionX < 0 && xMargin <(1 * size / 4))||(location.x<0&&directionX>0);
		if ((testTop || testBot || testRight || testLeft) && (location.x>(-size / 4)) && (location.x < (size*render.w + size / 8)) && (location.y>(-size / 4))&& (location.y < (size*render.h + size / 8)))
			clipForward();
		else{
		location = glm::vec3(location.x + this->directionX*size*0.05, location.y + this->directionY*size*0.05, location.z);
		}
	}
	
private:
	float size;
	bool testTop, testBot, testRight, testLeft;
	float xMargin;
	float yMargin;
	float angle;
	float position;
	sf::Window *App;
	RenderEngine render;
	unsigned int step;

	void handleEvents()
	{
		const sf::Input& Input = App->GetInput();
		bool shiftDown = Input.IsKeyDown(sf::Key::LShift) || Input.IsKeyDown(sf::Key::RShift);
		sf::Event Event;
		while (App->GetEvent(Event))
		{
			if (Event.Type == sf::Event::Closed)
				App->Close();
			if ((Event.Type == sf::Event::KeyPressed) && (Event.Key.Code == sf::Key::Escape))
				App->Close();
			
			// This is for grading your code. DO NOT REMOVE
			if(Event.Type == sf::Event::KeyPressed && Event.Key.Code == sf::Key::Equal)
			{
				unsigned char *dest;
				unsigned int w = App->GetWidth();
				unsigned int h = App->GetHeight();
				glPixelStorei(GL_PACK_ALIGNMENT, 1);
				dest = (unsigned char *) malloc( sizeof(unsigned char)*w*h*3);
				glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, dest);
				
				FILE *file;
				file = fopen("_program1.ppm", "wb");
				fprintf(file, "P6\n%i %i\n%i\n", w, h, 255);
				for(int i=h-1; i>=0; i--)
					fwrite(&dest[i*w*3], sizeof(unsigned char), w*3, file);
				fclose(file);
				free(dest);
			}
			
			if (Event.Type == sf::Event::Resized)
			{ glViewport(0, 0, Event.Size.Width, Event.Size.Height); }

			if (Event.Type == sf::Event::MouseMoved){
				position = Event.MouseMove.X - RESOLUTION / 2;
				if (!out)
					turn();
			}
			if (Event.Type == sf::Event::KeyPressed && Event.Key.Code == sf::Key::W)
			{
				forward();
				if (!out){
					render.setView(glm::lookAt(location, glm::vec3(location.x + directionX, location.y + directionY, location.z), glm::vec3(0.0f, 0.0f, 1.0f)));
				}
			}
			if (Event.Type == sf::Event::KeyPressed && Event.Key.Code == sf::Key::LControl)
			{
				this->toggleView();
			}
			if(Event.Type == sf::Event::KeyPressed && Event.Key.Code == sf::Key::Space)
			{
				render.generateMaze(step);
				step++;
				location = glm::vec3(-5.0f*size, 0.5*render.h*size, 0.5*size);
			}

		}
	}
};

int main(int argc, char ** argv)
{
	printusers();
	
	/* check that there are sufficient arguments */
	unsigned int w;
	unsigned int h;
	if (argc < 3) {
		w = 6;
		h = 6;
		fprintf(stderr, "The width and height can be specified as command line arguments. Defaulting to %i %i\n", w, h);
	}
	else {
		w = atoi(argv[1]);
		h = atoi(argv[2]);
	}
	
	Program4 prog(w, h);
	
    return EXIT_SUCCESS;
}
