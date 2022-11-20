/************************************************************************
	 File:        TrainView.cpp

	 Author:
				  Michael Gleicher, gleicher@cs.wisc.edu

	 Modifier
				  Yu-Chi Lai, yu-chi@cs.wisc.edu

	 Comment:
						The TrainView is the window that actually shows the
						train. Its a
						GL display canvas (Fl_Gl_Window).  It is held within
						a TrainWindow
						that is the outer window with all the widgets.
						The TrainView needs
						to be aware of the window - since it might need to
						check the widgets to see how to draw

	  Note:        we need to have pointers to this, but maybe not know
						about it (beware circular references)

	 Platform:    Visio Studio.Net 2003/2005

*************************************************************************/

#include <iostream>
#include <Fl/fl.h>
#include <vector>
// we will need OpenGL, and OpenGL needs windows.h
#include <windows.h>
//#include "GL/gl.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <GL/glu.h>

#include "TrainView.H"
#include "TrainWindow.H"
#include "Utilities/3DUtils.H"



#ifdef EXAMPLE_SOLUTION
#	include "TrainExample/TrainExample.H"
#endif


//************************************************************************
//
// * Constructor to set up the GL window
//========================================================================
TrainView::
TrainView(int x, int y, int w, int h, const char* l)
	: Fl_Gl_Window(x, y, w, h, l)
	//========================================================================
{
	mode(FL_RGB | FL_ALPHA | FL_DOUBLE | FL_STENCIL);
	water_simulator = water_simulation(30, nullptr);
	// water_simulator.surfaces[5][5].height = 3;
	//this->water_wave = new VAO;
	resetArcball();
}

//************************************************************************
//
// * Reset the camera to look at the world
//========================================================================
void TrainView::
resetArcball()
//========================================================================
{
	// Set up the camera to look at the world
	// these parameters might seem magical, and they kindof are
	// a little trial and error goes a long way
	arcball.setup(this, 40, 250, .2f, .4f, 0);
}

//************************************************************************
//
// * FlTk Event handler for the window
//########################################################################
// TODO: 
//       if you want to make the train respond to other events 
//       (like key presses), you might want to hack this.
//########################################################################
//========================================================================
int TrainView::handle(int event)
{
	// see if the ArcBall will handle the event - if it does, 
	// then we're done
	// note: the arcball only gets the event if we're in world view
	if (tw->worldCam->value())
		if (arcball.handle(event))
			return 1;

	// remember what button was used
	static int last_push;

	switch (event) {
		// Mouse button being pushed event
	case FL_PUSH:
		last_push = Fl::event_button();
		// if the left button be pushed is left mouse button
		if (last_push == FL_LEFT_MOUSE) {
			doPick();
			damage(1);
			return 1;
		};
		break;

		// Mouse button release event
	case FL_RELEASE: // button release
		damage(1);
		last_push = 0;
		return 1;

		// Mouse button drag event
	case FL_DRAG:

		// Compute the new control point position
		if ((last_push == FL_LEFT_MOUSE) && (selectedCube >= 0)) {
			ControlPoint* cp = &m_pTrack->points[selectedCube];

			double r1x, r1y, r1z, r2x, r2y, r2z;
			getMouseLine(r1x, r1y, r1z, r2x, r2y, r2z);

			double rx, ry, rz;
			mousePoleGo(r1x, r1y, r1z, r2x, r2y, r2z,
				static_cast<double>(cp->pos.x),
				static_cast<double>(cp->pos.y),
				static_cast<double>(cp->pos.z),
				rx, ry, rz,
				(Fl::event_state() & FL_CTRL) != 0);

			cp->pos.x = (float)rx;
			cp->pos.y = (float)ry;
			cp->pos.z = (float)rz;
			damage(1);
		}
		break;

		// in order to get keyboard events, we need to accept focus
	case FL_FOCUS:
		return 1;

		// every time the mouse enters this window, aggressively take focus
	case FL_ENTER:
		focus(this);
		break;

	case FL_KEYBOARD:
		int k = Fl::event_key();
		int ks = Fl::event_state();
		if (k == 'p') {
			// Print out the selected control point information
			if (selectedCube >= 0)
				printf("Selected(%d) (%g %g %g) (%g %g %g)\n",
					selectedCube,
					m_pTrack->points[selectedCube].pos.x,
					m_pTrack->points[selectedCube].pos.y,
					m_pTrack->points[selectedCube].pos.z,
					m_pTrack->points[selectedCube].orient.x,
					m_pTrack->points[selectedCube].orient.y,
					m_pTrack->points[selectedCube].orient.z);
			else
				printf("Nothing Selected\n");

			return 1;
		};
		break;
	}

	return Fl_Gl_Window::handle(event);
}
void  TrainView::draw_everything_in_gl1_0() {

}
//************************************************************************
//
// * this is the code that actually draws the window
//   it puts a lot of the work into other routines to simplify things
//========================================================================
GLfloat water_vertices[5400 * 3];
void TrainView::draw()
{

	//*********************************************************************
	//
	// * Set up basic opengl informaiton
	//
	//**********************************************************************
	//initialized glad

	//draw_everything_in_gl1_0();
	vector<triangle_t> triangles;
	if (gladLoadGL())
	{

		//initiailize VAO, VBO, Shader...
		
		//if (!this->shader)
		//	this->shader = new
		//	Shader(
		//		PROJECT_DIR "/src/shaders/simple.vert",
		//		nullptr, nullptr, nullptr,
		//		PROJECT_DIR "/src/shaders/simple.frag");
		if (!this->water_shader) {
			this->water_shader = new
				Shader(
					PROJECT_DIR "/src/shaders/water.vert",
					nullptr, nullptr, nullptr,
					PROJECT_DIR "/src/shaders/water.frag");
		}
		if (!this->skybox_shader) {
			this->skybox_shader = new
				Shader(
					PROJECT_DIR "/src/shaders/skybox.vert",
					nullptr, nullptr, nullptr,
					PROJECT_DIR "/src/shaders/skybox.frag");
		}

		if (!this->commom_matrices)
			this->commom_matrices = new UBO();
		this->commom_matrices->size = 2 * sizeof(glm::mat4);
		glGenBuffers(1, &this->commom_matrices->ubo);
		glBindBuffer(GL_UNIFORM_BUFFER, this->commom_matrices->ubo);
		glBufferData(GL_UNIFORM_BUFFER, this->commom_matrices->size, NULL, GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		//if (!this->plane) {
		//	GLfloat wall_vertices[] = {
		//		10.0f, 15.0f, -10.0f,  //the first wall
		//		-10.0f, 15.0f, -10.0f,
		//		-10.0f, -0.5f, -10.0f,
		//		10.0f, -0.5f, -10.0f,

		//		-10.0f, 15.0f, 10.0f,  //the second wall
		//		-10.0f, 15.0f, -10.0f,
		//		-10.0f, -0.5f, -10.0f,
		//		-10.0f, -0.5f, 10.0f,

		//		10.0f, -0.5f, 10.0f,  //floor
		//		10.0f, -0.5f, -10.0f,
		//		-10.0f, -0.5f, -10.0f,
		//		-10.0f, -0.5f, 10.0f,

		//		10.0f, 15.0f, 10.0f,  //the third wall
		//		10.0f, 15.0f, -10.0f,
		//		10.0f, -0.5f, -10.0f,
		//		10.0f, -0.5f, 10.0f,

		//		10.0f, 15.0f, 10.0f,  //the forth wall
		//		-10.0f, 15.0f, 10.0f,
		//		-10.0f, -0.5f, 10.0f,
		//		10.0f, -0.5f, 10.0f
		//	};
		//	float wall_color_sample[3] = { 1.0f, 1.0f, 1.0f };
		//	//float wall_color_sample[3] = { 0.4, 0.7, 0.8 };
		//	static GLfloat wall_color[60];
		//	for (int i = 0; i < 20; ++i) {
		//		for (int j = 0; j < 3; ++j)
		//			wall_color[i * 3 + j] = wall_color_sample[j];
		//	}
		//	GLfloat  normal[] = {
		//		0.0f, 1.0f, 0.0f,
		//		0.0f, 1.0f, 0.0f,
		//		0.0f, 1.0f, 0.0f,
		//		0.0f, 1.0f, 0.0f };
		//	//GLfloat  texture_coordinate[] = {
		//	//	0.0f, 0.0f,
		//	//	1.0f, 0.0f,
		//	//	1.0f, 1.0f,
		//	//	0.0f, 1.0f };

		//	GLfloat  wall_texture_coordinate[] = {
		//		0.0f, 1.0f,
		//		0.0f, 0.0f,
		//		1.0f, 0.0f,
		//		1.0f, 1.0f,

		//		0.0f, 1.0f,
		//		0.0f, 0.0f,
		//		1.0f, 0.0f,
		//		1.0f, 1.0f,

		//		0.0f, 1.0f,
		//		0.0f, 0.0f,
		//		1.0f, 0.0f,
		//		1.0f, 1.0f,

		//		0.0f, 1.0f,
		//		0.0f, 0.0f,
		//		1.0f, 0.0f,
		//		1.0f, 1.0f,

		//		0.0f, 1.0f,
		//		0.0f, 0.0f,
		//		1.0f, 0.0f,
		//		1.0f, 1.0f
		//	};
		//	GLuint element[12];
		//	for (int i = 0; i < 12; ++i) { element[i] = i; }

		//	this->plane = new VAO;
		//	this->plane->element_amount = sizeof(element) / sizeof(GLuint);
		//	glGenVertexArrays(1, &this->plane->vao);
		//	glGenBuffers(3, this->plane->vbo);
		//	glGenBuffers(1, &this->plane->ebo);

		//	glBindVertexArray(this->plane->vao);

		//	// Position attribute
		//	//glBindBuffer(GL_ARRAY_BUFFER, this->plane->vbo[0]);
		//	//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		//	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		//	//glEnableVertexAttribArray(0);

		//	glBindBuffer(GL_ARRAY_BUFFER, this->plane->vbo[0]);
		//	glBufferData(GL_ARRAY_BUFFER, sizeof(wall_vertices), wall_vertices, GL_STATIC_DRAW);
		//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0 * sizeof(GLfloat), (void*)0);
		//	glEnableVertexAttribArray(0);

		//	glBindBuffer(GL_ARRAY_BUFFER, this->plane->vbo[1]);
		//	glBufferData(GL_ARRAY_BUFFER, sizeof(wall_color), wall_color, GL_STATIC_DRAW);
		//	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0 * sizeof(GLfloat), (void*)0);
		//	glEnableVertexAttribArray(3);

		//	////Normal attribute
		//	//glBindBuffer(GL_ARRAY_BUFFER, this->plane->vbo[1]);
		//	//glBufferData(GL_ARRAY_BUFFER, sizeof(normal), normal, GL_STATIC_DRAW);
		//	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		//	//glEnableVertexAttribArray(1);

		//	// Texture Coordinate attribute
		//	glBindBuffer(GL_ARRAY_BUFFER, this->plane->vbo[2]);
		//	glBufferData(GL_ARRAY_BUFFER, sizeof(wall_texture_coordinate), wall_texture_coordinate, GL_STATIC_DRAW);
		//	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0 * sizeof(GLfloat), (GLvoid*)0);
		//	glEnableVertexAttribArray(2);

		//	//Element attribute
		//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->plane->ebo);
		//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(element), element, GL_STATIC_DRAW);

		//	// Unbind VAO
		//	glBindVertexArray(0);
		//}
		if (!this->skybox) {
			GLuint element[36];
			for (int i = 0; i < 36; ++i) { element[i] = i; }
			GLfloat skyboxVertices[] = {
				// positions          
				-1.0f,  1.0f, -1.0f,
				-1.0f, -1.0f, -1.0f,
				 1.0f, -1.0f, -1.0f,
				 1.0f, -1.0f, -1.0f,
				 1.0f,  1.0f, -1.0f,
				-1.0f,  1.0f, -1.0f,

				-1.0f, -1.0f,  1.0f,
				-1.0f, -1.0f, -1.0f,
				-1.0f,  1.0f, -1.0f,
				-1.0f,  1.0f, -1.0f,
				-1.0f,  1.0f,  1.0f,
				-1.0f, -1.0f,  1.0f,

				 1.0f, -1.0f, -1.0f,
				 1.0f, -1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f, -1.0f,
				 1.0f, -1.0f, -1.0f,

				-1.0f, -1.0f,  1.0f,
				-1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f, -1.0f,  1.0f,
				-1.0f, -1.0f,  1.0f,

				-1.0f,  1.0f, -1.0f,
				 1.0f,  1.0f, -1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				-1.0f,  1.0f,  1.0f,
				-1.0f,  1.0f, -1.0f,

				-1.0f, -1.0f, -1.0f,
				-1.0f, -1.0f,  1.0f,
				 1.0f, -1.0f, -1.0f,
				 1.0f, -1.0f, -1.0f,
				-1.0f, -1.0f,  1.0f,
				 1.0f, -1.0f,  1.0f
			};
			//for(int i= 0; i<36*3; ++i){
			//	skyboxVertices[i] *= 10;
			//}
			this->skybox = new VAO;
			this->skybox->element_amount = sizeof(element) / sizeof(GLuint);
			glGenVertexArrays(1, &this->skybox->vao);
			glGenBuffers(1, this->skybox->vbo);
			glGenBuffers(1, &this->skybox->ebo);
			glBindVertexArray(this->skybox->vao);

			glBindBuffer(GL_ARRAY_BUFFER, this->skybox->vbo[0]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
			glEnableVertexAttribArray(0);

			//Element attribute
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->skybox->ebo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(element), element, GL_STATIC_DRAW);

			// Unbind VAO
			glBindVertexArray(0);
		}
		if (!this->water_wave) {
			float edge_length = 20.0f;
			float divide_n = 20.0f;
			float stride = edge_length / divide_n;
			float amptitude = 3;

			//vector<triangle_t> triangles;
			//water_simulator.get_matrix(triangles);
			//for (int i = 0; i < edge_length; i += stride) {
			//	for (int j = 0; j < edge_length; j += stride) {
			//		/*  a   d
			//		*     _
			//		*	 |_|
			//		*	b   c
			//		*/
			//		float x = i-10, y = j-10;
			//		glm::vec3 a(x, 12.0, y );
			//		glm::vec3 b( x+1, 12.0, y);
			//		glm::vec3 c( x+1, 12.0, y+1 );
			//		glm::vec3 d( x, 12.0, y+1 );
			//		//lower left
			//		triangle_t lower_left;
			//		lower_left.vertices = { a, b, c };
			//		
			//		//top right
			//		triangle_t upper_right;
			//		upper_right.vertices = { a, d, c };

			//		triangles.push_back(lower_left);
			//		triangles.push_back(upper_right);
			//	}
			//}

			water_simulator.get_matrix(triangles);
			//GLfloat* water_vertices = new GLfloat[5400*3];
			//water_simulator.get_matrix(triangles);
			//int water_vertices_n = 0;
			//std::cout << triangles.size() << " " << triangles[0].vertices.size() << std::endl;

			//for (auto& triangle : triangles) { //200
			//	for (auto& vertice : triangle.vertices) { //3
			//		water_vertices[water_vertices_n++] = vertice.x;
			//		water_vertices[water_vertices_n++] = vertice.y;
			//		water_vertices[water_vertices_n++] = vertice.z;
			//	}
			//}

			float water_color_sample[3] = { 0.4, 0.7, 0.8 };
			static GLfloat water_color[60];
			for (int i = 0; i < 20; ++i) {
				for (int j = 0; j < 3; ++j)
					water_color[i * 3 + j] = water_color_sample[j];
			}
			GLfloat  normal[] = {
				0.0f, 1.0f, 0.0f,
				0.0f, 1.0f, 0.0f,
				0.0f, 1.0f, 0.0f,
				0.0f, 1.0f, 0.0f };

			GLuint element[1800 * 3];
			for (int i = 0; i < 1800 * 3; ++i) { element[i] = i; }

			this->water_wave = new VAO;
			this->water_wave->element_amount = sizeof(element) / sizeof(GLuint);
			glGenVertexArrays(1, &this->water_wave->vao);
			glGenBuffers(3, this->water_wave->vbo);
			glGenBuffers(1, &this->water_wave->ebo);

			glBindVertexArray(this->water_wave->vao);

			// Position attribute
			//glBindBuffer(GL_ARRAY_BUFFER, this->plane->vbo[0]);
			//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
			//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
			//glEnableVertexAttribArray(0);

			glBindBuffer(GL_ARRAY_BUFFER, this->water_wave->vbo[0]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(water_vertices), water_vertices, GL_DYNAMIC_DRAW);
			//glBufferData(GL_ARRAY_BUFFER, 9*sizeof(GLfloat), water_vertices, GL_DYNAMIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0 * sizeof(GLfloat), (void*)0);
			glEnableVertexAttribArray(0);

			//glBindBuffer(GL_ARRAY_BUFFER, this->plane->vbo[1]);
			//glBufferData(GL_ARRAY_BUFFER, sizeof(water_wave), wall_color, GL_STATIC_DRAW);
			//glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0*sizeof(GLfloat), (void*)0);
			//glEnableVertexAttribArray(3);

			////Normal attribute
			//glBindBuffer(GL_ARRAY_BUFFER, this->plane->vbo[1]);
			//glBufferData(GL_ARRAY_BUFFER, sizeof(normal), normal, GL_STATIC_DRAW);
			//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
			//glEnableVertexAttribArray(1);

			// Texture Coordinate attribute
			GLfloat water_wave_coordinate[5400 * 2] = { 0, 0, 0, 1, 1, 0 };
			for (int i = 0; i < 900; ++i) {
				water_wave_coordinate[i * 12 + 0] = 0;
				water_wave_coordinate[i * 12 + 1] = 0;
				water_wave_coordinate[i * 12 + 2] = 0;
				water_wave_coordinate[i * 12 + 3] = 1;
				water_wave_coordinate[i * 12 + 4] = 1;
				water_wave_coordinate[i * 12 + 5] = 0;

				water_wave_coordinate[i * 12 + 6] = 0;
				water_wave_coordinate[i * 12 + 7] = 0;
				water_wave_coordinate[i * 12 + 8] = 0;
				water_wave_coordinate[i * 12 + 9] = 1;
				water_wave_coordinate[i * 12 + 10] = 1;
				water_wave_coordinate[i * 12 + 11] = 1;

			}
			//glBindBuffer(GL_ARRAY_BUFFER, this->water_wave->vbo[2]);
			//glBufferData(GL_ARRAY_BUFFER, sizeof(water_wave_coordinate), water_wave_coordinate, GL_STATIC_DRAW);
			//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0* sizeof(GLfloat), (GLvoid*)0);
			//glEnableVertexAttribArray(2);

			//Element attribute
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->water_wave->ebo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(element), element, GL_STATIC_DRAW);

			// Unbind VAO

			glBindBuffer(GL_ARRAY_BUFFER, this->water_wave->vbo[0]);
			//glBindVertexArray(0);

		}
		vector<string> skybox_images_path = {
			PROJECT_DIR "/Images/skybox/right.jpg",
			PROJECT_DIR "/Images/skybox/left.jpg",
			PROJECT_DIR "/Images/skybox/top.jpg",
			PROJECT_DIR "/Images/skybox/bottom.jpg",
			PROJECT_DIR "/Images/skybox/front.jpg",
			PROJECT_DIR "/Images/skybox/back.jpg"
		};

		//if (!this->texture)
		//	this->texture = new Texture2D(PROJECT_DIR "/Images/tiles.jpg");
		if (!this->skybox_texture)
			this->skybox_texture = new skybox_t(skybox_images_path);
		if (!this->device) {
			//Tutorial: https://ffainelli.github.io/openal-example/
			this->device = alcOpenDevice(NULL);
			if (!this->device)
				puts("ERROR::NO_AUDIO_DEVICE");

			ALboolean enumeration = alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT");
			if (enumeration == AL_FALSE)
				puts("Enumeration not supported");
			else
				puts("Enumeration supported");

			this->context = alcCreateContext(this->device, NULL);
			if (!alcMakeContextCurrent(context))
				puts("Failed to make context current");

			this->source_pos = glm::vec3(0.0f, 5.0f, 0.0f);

			ALfloat listenerOri[] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f };
			alListener3f(AL_POSITION, source_pos.x, source_pos.y, source_pos.z);
			alListener3f(AL_VELOCITY, 0, 0, 0);
			alListenerfv(AL_ORIENTATION, listenerOri);

			alGenSources((ALuint)1, &this->source);
			alSourcef(this->source, AL_PITCH, 1);
			alSourcef(this->source, AL_GAIN, 1.0f);
			alSource3f(this->source, AL_POSITION, source_pos.x, source_pos.y, source_pos.z);
			alSource3f(this->source, AL_VELOCITY, 0, 0, 0);
			alSourcei(this->source, AL_LOOPING, AL_TRUE);

			alGenBuffers((ALuint)1, &this->buffer);

			ALsizei size, freq;
			ALenum format;
			ALvoid* data;
			ALboolean loop = AL_TRUE;

			//Material from: ThinMatrix
			alutLoadWAVFile((ALbyte*)PROJECT_DIR "/Audios/bounce.wav", &format, &data, &size, &freq, &loop);
			alBufferData(this->buffer, format, data, size, freq);
			alSourcei(this->source, AL_BUFFER, this->buffer);

			if (format == AL_FORMAT_STEREO16 || format == AL_FORMAT_STEREO8)
				puts("TYPE::STEREO");
			else if (format == AL_FORMAT_MONO16 || format == AL_FORMAT_MONO8)
				puts("TYPE::MONO");

			alSourcePlay(this->source);

			// cleanup context
			//alDeleteSources(1, &source);
			//alDeleteBuffers(1, &buffer);
			//device = alcGetContextsDevice(context);
			//alcMakeContextCurrent(NULL);
			//alcDestroyContext(context);
			//alcCloseDevice(device);
		}

		wall.bind();
	}
	else
		throw std::runtime_error("Could not initialize GLAD!");

	// Set up the view port		
	glViewport(0, 0, w(), h());

	// clear the window, be sure to clear the Z-Buffer too
	glClearColor(0, 0, .3f, 0);		// background should be blue

	// we need to clear out the stencil buffer since we'll use
	// it for shadows
	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_DEPTH);

	// Blayne prefers GL_DIFFUSE
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	// prepare for projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	setProjection();		// put the code to set up matrices here

	//######################################################################
	// TODO: 
	// you might want to set the lighting up differently. if you do, 
	// we need to set up the lights AFTER setting up the projection
	//######################################################################
	// enable the lighting
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	// top view only needs one light
	if (tw->topCam->value()) {
		glDisable(GL_LIGHT1);
		glDisable(GL_LIGHT2);
	}
	else {
		glEnable(GL_LIGHT1);
		glEnable(GL_LIGHT2);
	}

	//*********************************************************************
	//
	// * set the light parameters
	//
	//**********************************************************************
	GLfloat lightPosition1[] = { 0,1,1,0 }; // {50, 200.0, 50, 1.0};
	GLfloat lightPosition2[] = { 1, 0, 0, 0 };
	GLfloat lightPosition3[] = { 0, -1, 0, 0 };
	GLfloat yellowLight[] = { 0.5f, 0.5f, .1f, 1.0 };
	GLfloat whiteLight[] = { 1.0f, 1.0f, 1.0f, 1.0 };
	GLfloat blueLight[] = { .1f,.1f,.3f,1.0 };
	GLfloat grayLight[] = { .3f, .3f, .3f, 1.0 };

	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition1);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, whiteLight);
	glLightfv(GL_LIGHT0, GL_AMBIENT, grayLight);

	glLightfv(GL_LIGHT1, GL_POSITION, lightPosition2);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, yellowLight);

	glLightfv(GL_LIGHT2, GL_POSITION, lightPosition3);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, blueLight);

	// set linstener position 
	if (selectedCube >= 0)
		alListener3f(AL_POSITION,
			m_pTrack->points[selectedCube].pos.x,
			m_pTrack->points[selectedCube].pos.y,
			m_pTrack->points[selectedCube].pos.z);
	else
		alListener3f(AL_POSITION,
			this->source_pos.x,
			this->source_pos.y,
			this->source_pos.z);


	//*********************************************************************
	// now draw the ground plane
	//*********************************************************************
	// set to opengl fixed pipeline(use opengl 1.x draw function)

	glm::mat4 model_matrix = glm::mat4();
	model_matrix = glm::translate(model_matrix, this->source_pos);
	model_matrix = glm::scale(model_matrix, glm::vec3(10.0f, 10.0f, 10.0f));

	glUseProgram(0);

	glDisable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	setUBO();
	HMatrix modelMatrix; //�A���o��? �o������ok 
	arcball.getMatrix(modelMatrix); //�� �צn�F ������z=-250 �A�o��get�o�� �쩳�O���̪�
	//glGetFloatv(GL_MODELVIEW_MATRIX, &view_matrix[0][0]); //�o�̭��L�F��:(�o��tranf�L�F 
	glBindBuffer(GL_UNIFORM_BUFFER, this->commom_matrices->ubo);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), &modelMatrix);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBufferRange(
		GL_UNIFORM_BUFFER, /*binding point*/0, this->commom_matrices->ubo, 0, this->commom_matrices->size);

	//bind shader


	this->skybox_shader->Use();


	glUniformMatrix4fv(
		glGetUniformLocation(this->skybox_shader->Program, "u_model"), 1, GL_FALSE, &model_matrix[0][0]);
	glUniform3fv(
		glGetUniformLocation(this->skybox_shader->Program, "u_color"), 1, &glm::vec3(0.0f, 1.0f, 0.0f)[0]);
	glUniform3fv(
		glGetUniformLocation(this->skybox_shader->Program, "light_color"), 1, &glm::vec3(1.0f, 0.7f, 0.7f)[0]);
	/*this->texture->bind(0);
	glUniform1i(glGetUniformLocation(this->skybox_shader->Program, "u_texture"), 0);*/

	this->skybox_texture->bind(0);
	glUniform1i(glGetUniformLocation(this->skybox_shader->Program, "u_texture"), 0);

	//bind VAO
	glBindVertexArray(this->skybox->vao);
	glDrawElements(GL_TRIANGLES, this->skybox->element_amount, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
	glDepthFunc(GL_LESS); // set depth function back to default
	glUseProgram(0);

	glEnable(GL_DEPTH_TEST);
	glUseProgram(0);

	setupFloor();
	glDisable(GL_LIGHTING);
	//drawFloor(200,10);


	//*********************************************************************
	// now draw the object and we need to do it twice
	// once for real, and then once for shadows
	//*********************************************************************
	glEnable(GL_LIGHTING);
	setupObjects();

	drawStuff();

	// this time drawing is for shadows (except for top view)
	if (!tw->topCam->value()) {
		setupShadows();
		drawStuff(true);
		unsetupShadows();
	}







	setUBO();
	glBindBufferRange(
		GL_UNIFORM_BUFFER, /*binding point*/0, this->commom_matrices->ubo, 0, this->commom_matrices->size);

	//bind shader
	//this->shader->Use();


	//glUniformMatrix4fv(
	//	glGetUniformLocation(this->shader->Program, "u_model"), 1, GL_FALSE, &model_matrix[0][0]);
	//glUniform3fv(
	//	glGetUniformLocation(this->shader->Program, "u_color"), 1, &glm::vec3(0.0f, 1.0f, 0.0f)[0]);
	//glUniform3fv(
	//	glGetUniformLocation(this->shader->Program, "light_color"), 1, &glm::vec3(1.0f, 0.7f, 0.7f)[0]);
	//this->texture->bind(0);
	//glUniform1i(glGetUniformLocation(this->shader->Program, "u_texture"), 0);

	////bind VAO
	//glBindVertexArray(this->plane->vao);
	//glDrawElements(GL_QUADS, this->plane->element_amount, GL_UNSIGNED_INT, 0);

	//glBindVertexArray(0);

	//glUseProgram(0);

	wall.draw();


	setUBO();

	// recalculate the water triangles
	int water_vertices_n = 0;
	triangles.clear();
	water_simulator.get_matrix(triangles);
	for (auto& triangle : triangles) { //200
		for (auto& vertice : triangle.vertices) { //3
			water_vertices[water_vertices_n++] = vertice.x;
			water_vertices[water_vertices_n++] = vertice.y;
			water_vertices[water_vertices_n++] = vertice.z;
		}
	}

	//rewrite the triangles martics in buffer
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(water_vertices), water_vertices);
	this->water_shader->Use();


	glUniformMatrix4fv(
		glGetUniformLocation(this->water_shader->Program, "u_model"), 1, GL_FALSE, &model_matrix[0][0]);
	glUniform3fv(
		glGetUniformLocation(this->water_shader->Program, "u_color"),
		1,
		&glm::vec3(0.0f, 1.0f, 0.0f)[0]);
	glUniform3fv(	
		glGetUniformLocation(this->water_shader->Program, "light_color"), 1, &glm::vec3(1.0f, 0.7f, 0.7f)[0]);
	//this->water->bind(0);
	glUniform1i(glGetUniformLocation(this->water_shader->Program, "u_texture"), 0);
	glBindVertexArray(this->water_wave->vao);
	glDrawElements(GL_TRIANGLES, this->water_wave->element_amount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	//glDrawArrays(GL_TRIANGLES, 0, 12 * 3);
	//unbind VAO


	//unbind shader(switch to fixed pipeline)
	glUseProgram(0);




}

//************************************************************************
//
// * This sets up both the Projection and the ModelView matrices
//   HOWEVER: it doesn't clear the projection first (the caller handles
//   that) - its important for picking
//========================================================================
void TrainView::
setProjection()
//========================================================================
{
	// Compute the aspect ratio (we'll need it)
	float aspect = static_cast<float>(w()) / static_cast<float>(h());

	// Check whether we use the world camp
	if (tw->worldCam->value())
		arcball.setProjection(false);
	// Or we use the top cam
	else if (tw->topCam->value()) {
		float wi, he;
		if (aspect >= 1) {
			wi = 110;
			he = wi / aspect;
		}
		else {
			he = 110;
			wi = he * aspect;
		}

		// Set up the top camera drop mode to be orthogonal and set
		// up proper projection matrix
		glMatrixMode(GL_PROJECTION);
		glOrtho(-wi, wi, -he, he, 200, -200);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glRotatef(-90, 1, 0, 0);
	}
	// Or do the train view or other view here
	//####################################################################
	// TODO: 
	// put code for train view projection here!	
	//####################################################################
	else {
#ifdef EXAMPLE_SOLUTION
		trainCamView(this, aspect);
#endif
	}
}

//************************************************************************
//
// * this draws all of the stuff in the world
//
//	NOTE: if you're drawing shadows, DO NOT set colors (otherwise, you get 
//       colored shadows). this gets called twice per draw 
//       -- once for the objects, once for the shadows
//########################################################################
// TODO: 
// if you have other objects in the world, make sure to draw them
//########################################################################
//========================================================================
void TrainView::drawStuff(bool doingShadows)
{
	// Draw the control points
	// don't draw the control points if you're driving 
	// (otherwise you get sea-sick as you drive through them)
	if (!tw->trainCam->value()) {
		for (size_t i = 0; i < m_pTrack->points.size(); ++i) {
			if (!doingShadows) {
				if (((int)i) != selectedCube)
					glColor3ub(240, 60, 60);
				else
					glColor3ub(240, 240, 30);
			}
			m_pTrack->points[i].draw();
		}
	}
	// draw the track
	//####################################################################
	// TODO: 
	// call your own track drawing code
	//####################################################################

#ifdef EXAMPLE_SOLUTION
	drawTrack(this, doingShadows);
#endif

	// draw the train
	//####################################################################
	// TODO: 
	//	call your own train drawing code
	//####################################################################
#ifdef EXAMPLE_SOLUTION
	// don't draw the train if you're looking out the front window
	if (!tw->trainCam->value())
		drawTrain(this, doingShadows);
#endif
}

// 
//************************************************************************
//
// * this tries to see which control point is under the mouse
//	  (for when the mouse is clicked)
//		it uses OpenGL picking - which is always a trick
//########################################################################
// TODO: 
//		if you want to pick things other than control points, or you
//		changed how control points are drawn, you might need to change this
//########################################################################
//========================================================================
void TrainView::
doPick()
//========================================================================
{
	// since we'll need to do some GL stuff so we make this window as 
	// active window
	make_current();

	// where is the mouse?
	int mx = Fl::event_x();
	int my = Fl::event_y();

	// get the viewport - most reliable way to turn mouse coords into GL coords
	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	// Set up the pick matrix on the stack - remember, FlTk is
	// upside down!
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPickMatrix((double)mx, (double)(viewport[3] - my),
		5, 5, viewport);

	// now set up the projection
	setProjection();

	// now draw the objects - but really only see what we hit
	GLuint buf[100];
	glSelectBuffer(100, buf);
	glRenderMode(GL_SELECT);
	glInitNames();
	glPushName(0);

	// draw the cubes, loading the names as we go
	for (size_t i = 0; i < m_pTrack->points.size(); ++i) {
		glLoadName((GLuint)(i + 1));
		m_pTrack->points[i].draw();
	}

	// go back to drawing mode, and see how picking did
	int hits = glRenderMode(GL_RENDER);
	if (hits) {
		// warning; this just grabs the first object hit - if there
		// are multiple objects, you really want to pick the closest
		// one - see the OpenGL manual 
		// remember: we load names that are one more than the index
		selectedCube = buf[3] - 1;
	}
	else // nothing hit, nothing selected
		selectedCube = -1;

	printf("Selected Cube %d\n", selectedCube);
}

void TrainView::setUBO()
{
	float wdt = this->pixel_w();
	float hgt = this->pixel_h();

	glm::mat4 view_matrix;
	glGetFloatv(GL_MODELVIEW_MATRIX, &view_matrix[0][0]);
	//HMatrix view_matrix; 
	//this->arcball.getMatrix(view_matrix);

	glm::mat4 projection_matrix;
	glGetFloatv(GL_PROJECTION_MATRIX, &projection_matrix[0][0]);
	//projection_matrix = glm::perspective(glm::radians(this->arcball.getFoV()), (GLfloat)wdt / (GLfloat)hgt, 0.01f, 1000.0f);

	glBindBuffer(GL_UNIFORM_BUFFER, this->commom_matrices->ubo);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &projection_matrix[0][0]);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), &view_matrix[0][0]);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}