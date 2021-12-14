#include "WAD.h"
#include "Shader.h"
#include "Texture.h"
#include "Mesh.h"
#include "camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>

#include <SDL.h>
#ifdef _WIN32
#	undef main
#endif

#include <map>
#include <string>

glm::ivec2 mouse;
Camera camera;
glm::mat4 model;
Shader shader;

Shader wireframeShader;

std::map<std::string, Texture> textures;
Texture texture;

std::vector<Mesh> meshes;
Mesh* currentMesh;

bool wireframe = true;

bool keyboard[SDL_NUM_SCANCODES]{};

std::string CleanFilename(std::string& name)
{
	std::string result = name;
	for (size_t i = 0; i < result.size(); i++)
	{
		if (result[i] <= 33 || result[i] > 126)
		{
			result[i] = 0;
		}
	}
	return std::string(result);
}

void LoadTextures()
{
	for (int i = 0; i < numsectors; i++)
	{
		msector_t* s = &msectors[i];
		char cfloor[9]{};
		char cceil[9]{};

		strncpy_s(cfloor, s->floortex, 8);
		strncpy_s(cceil, s->ceiltex, 8);

		std::string floor = cfloor;
		std::string ceil = cceil;

		if (textures.find(floor) == textures.end())
		{
			if (floor.find("-") == std::string::npos)
			{
				Texture texture = CreateTextureFromFile(std::string("Textures/") + floor + std::string(".png"));
				textures.insert(std::make_pair(floor, texture));
			}
		}

		if (textures.find(ceil) == textures.end())
		{
			if (ceil.find("-") == std::string::npos)
			{
				Texture texture = CreateTextureFromFile(std::string("Textures/") + ceil + std::string(".png"));
				textures.insert(std::make_pair(ceil, texture));
			}
		}
	}

	for (int i = 0; i < numsidefs; i++)
	{
		msidedef_t* sidedef = &msidedefs[i];

		char cmid[9]{};
		char cbottom[9]{};
		char ctop[9]{};

		strncpy_s(cmid, sidedef->midtex, 8);
		strncpy_s(cbottom, sidedef->bottomtex, 8);
		strncpy_s(ctop, sidedef->toptex, 8);

		std::string mid = cmid;
		std::string bottom = cbottom;
		std::string top = ctop;

		if (textures.find(mid) == textures.end())
		{
			if (mid.find("-") == std::string::npos)
			{
				Texture texture = CreateTextureFromFile(std::string("Textures/") + mid + std::string(".png"));
				textures.insert(std::make_pair(mid, texture));
			}
		}

		if (textures.find(bottom) == textures.end())
		{
			if (bottom.find("-") == std::string::npos)
			{
				std::string filepath = "Textures/";
				filepath+= bottom;
				filepath+= ".png";
				Texture texture = CreateTextureFromFile(std::string("Textures/") + bottom + std::string(".png"));
				textures.insert(std::make_pair(bottom, texture));
			}
		}


		if (textures.find(top) == textures.end())
		{
			if (top.find("-") == std::string::npos)
			{
				Texture texture = CreateTextureFromFile(std::string("Textures/") + top + std::string(".png"));
				textures.insert(std::make_pair(top, texture));
			}
		}
	}
}


void CalculateNormals(mlinedef_t* l, std::vector<Vertex>& vertices, int num)
{
	glm::i16vec2 start = glm::vec2(mvertices[l->v1].x, mvertices[l->v1].y);
	glm::i16vec2 end = glm::vec2(mvertices[l->v2].x, mvertices[l->v2].y);
	glm::i16vec2 dir = end - start;
	glm::i16vec2 n = glm::i16vec2(dir.y, -dir.x);

	for (int i = 0; i < num; i++)
	{
		vertices[i].normal = glm::normalize(glm::vec3(n.x, 0, n.y));
	}
}

void CalculateTexcoord(std::vector<Vertex>& vertices, Texture* texture = nullptr)
{
	for (Vertex& v : vertices)
	{
		glm::vec3 vAxis = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::vec3 uAxis = glm::cross(v.normal, vAxis);

		// Floor / Ceiling
		if (glm::abs(v.normal.y) > 0.9f)
		{
			vAxis = glm::vec3(0.0f, 0.0f, -1.0f);
			uAxis = glm::cross(v.normal, vAxis);
		}

		float w = texture ? texture->width : 128.0f;
		float h = texture ? texture->height : 128.0f;

		v.uv.x = glm::dot(v.position / w, uAxis);
		v.uv.y = glm::dot(v.position / h, vAxis);
	}
}

void BuildWalls()
{
	for (int i = 0; i < numlinedefs; i++)
	{
		mlinedef_t* l = &mlinedefs[i];

		// only draw line defs with no backside
		if (l->sidenum[1] & 0x8000)
		{
			msidedef_t* side = &msidedefs[l->sidenum[0]];
			msector_t* sec = &msectors[side->sector];

			std::vector<Vertex> vertices(4);
			vertices[0].position = glm::vec3(mvertices[l->v1].x,  sec->floorheight, mvertices[l->v1].y);
			vertices[1].position = glm::vec3(mvertices[l->v2].x,  sec->floorheight, mvertices[l->v2].y);
			vertices[2].position = glm::vec3(mvertices[l->v2].x, sec->ceilheight, mvertices[l->v2].y);
			vertices[3].position = glm::vec3(mvertices[l->v1].x, sec->ceilheight, mvertices[l->v1].y);

			char texname[9]{};
			strncpy_s(texname, side->midtex, 8);
			Texture* texture = &textures[texname];
			CalculateNormals(l, vertices, 4);
			CalculateTexcoord(vertices, texture);
			Mesh mesh = CreateMesh(vertices, GL_TRIANGLE_FAN);
			mesh.texture = texture;
			meshes.push_back(mesh);
		}
		else
		{
			msidedef_t* fside = &msidedefs[l->sidenum[0]];
			msidedef_t* bside = &msidedefs[l->sidenum[1]];
			msector_t* fsec = &msectors[fside->sector];
			msector_t* bsec = &msectors[bside->sector];

			short lower = 0;
			short higher= 0;

			if (fsec->floorheight < bsec->floorheight)
			{
				lower = fsec->floorheight;
				higher = bsec->floorheight;
			}
			else
			{
				lower = bsec->floorheight;
				higher = fsec->floorheight;
			}

			std::vector<Vertex> vertices(4);
			vertices[0].position = glm::vec3(mvertices[l->v1].x, lower, mvertices[l->v1].y);
			vertices[1].position = glm::vec3(mvertices[l->v2].x, lower, mvertices[l->v2].y);
			vertices[2].position = glm::vec3(mvertices[l->v2].x, higher, mvertices[l->v2].y);
			vertices[3].position = glm::vec3(mvertices[l->v1].x, higher, mvertices[l->v1].y);
			
			char texname[9]{};
			strncpy_s(texname, fside->bottomtex, 8);
			Texture* texture = &textures[texname];
			CalculateNormals(l, vertices, 4);
			CalculateTexcoord(vertices, texture);
			Mesh mesh = CreateMesh(vertices, GL_TRIANGLE_FAN);
			mesh.texture = texture;
			meshes.push_back(mesh);

			if (fsec->ceilheight < bsec->ceilheight)
			{
				lower = fsec->ceilheight;
				higher = bsec->ceilheight;
			}
			else
			{
				lower = bsec->ceilheight;
				higher = fsec->ceilheight;
			}

			vertices[0].position = glm::vec3(mvertices[l->v1].x, lower, mvertices[l->v1].y);
			vertices[1].position = glm::vec3(mvertices[l->v2].x, lower, mvertices[l->v2].y);
			vertices[2].position = glm::vec3(mvertices[l->v2].x, higher, mvertices[l->v2].y);
			vertices[3].position = glm::vec3(mvertices[l->v1].x, higher, mvertices[l->v1].y);

			strncpy_s(texname, fside->toptex, 8);
			texture = &textures[texname];
			CalculateNormals(l, vertices, 4);
			CalculateTexcoord(vertices, texture);
			Mesh mesh2 = CreateMesh(vertices, GL_TRIANGLE_FAN);
			mesh2.texture = texture;
			meshes.push_back(mesh2);
		}
	}
}

void BuildFloors()
{
	for (int i = 0; i < numglssectors; i++)
	{
		std::vector<Vertex> vertices(glssectors[i].count);
		short floorheight = 0;
		short ceilheight = 128;
		char floorName[9]{};
		char ceilName[9]{};

		for (int j = 0; j < glssectors[i].count; j++)
		{
			glseg_t* seg = &glsegs[glssectors[i].firstseg + j];
			

			if (seg->linedef != 0xffff)
			{
				int sector = msidedefs[mlinedefs[seg->linedef].sidenum[seg->side]].sector;
				if (floorheight == 0)
				{
					floorheight = msectors[sector].floorheight;
					strncpy_s(floorName, msectors[sector].floortex, 8);
				}
				if (ceilheight == 128)
				{
					ceilheight = msectors[sector].ceilheight;
					strncpy_s(ceilName, msectors[sector].ceiltex, 8);
				}
			}

			if (seg->v1 & 0x8000)
			{
				int v1 = seg->v1 & (~0x8000);
				float x = glvertices[v1].x / 65536.0f;
				float y = glvertices[v1].y / 65536.0f;

				vertices[j].position = glm::vec3(x, 0.0f, y);
			}
			else
			{
				float x = (float)mvertices[seg->v1].x;
				float y = (float)mvertices[seg->v1].y;
				vertices[j].position = glm::vec3(x, 0.0f, y);
			}
			// floor always points up
			vertices[j].normal = glm::vec3(0.0f, 1.0f, 0.0f);
		}

		// create ceiling
		for (Vertex& v : vertices)
			v.position.y = (float)ceilheight;
		
		Texture* texture = &textures[ceilName];
		CalculateTexcoord(vertices, texture);
		Mesh mesh = CreateMesh(vertices, GL_TRIANGLE_FAN);
		mesh.texture = texture;
		meshes.push_back(mesh);
		

		// create floor
		for (Vertex& v : vertices)
			v.position.y = (float)floorheight;
		std::reverse(vertices.begin(), vertices.end());
		texture = &textures[floorName];
		CalculateTexcoord(vertices, texture);
		Mesh mesh2 = CreateMesh(vertices, GL_TRIANGLE_FAN);
		mesh2.texture = texture;
		meshes.push_back(mesh2);
	}
}

void Init()
{
	WADLoadFromFile("Maps/MAPGL.wad");
	MapLoadLevel("E1M2");

	shader = CreateShaderFromFile("Shaders/Default.glsl");

	wireframeShader = CreateShaderFromFile("Shaders/Color.glsl");

	texture = CreateTextureFromFile("Textures/DoomDummy.png");

	camera.position = glm::vec3(0.0f, 48.0f, 0.0f);
	camera.rotation.y = glm::pi<float>();
	CreateCamera(&camera, 70.0f, 16.0f / 9.0f, 0.1f, 100000.0f);
	model = glm::mat4(1.0f);

	LoadTextures();
	BuildWalls();
	BuildFloors();
}

void Update(float dt)
{
	float speed = 320 * dt;
	float speedBoost = 2.0f;

	if (keyboard[SDL_SCANCODE_LSHIFT])
	{
		speed *= speedBoost;
	}

	if (keyboard[SDL_SCANCODE_W])
	{
		CameraMoveForward(&camera, speed);
	}
	if (keyboard[SDL_SCANCODE_S])
	{
		CameraMoveForward(&camera, -speed);
	}
	if (keyboard[SDL_SCANCODE_A])
	{
		CameraMoveRight(&camera, -speed);
	}
	if (keyboard[SDL_SCANCODE_D])
	{
		CameraMoveRight(&camera, speed);
	}
	if (keyboard[SDL_SCANCODE_E])
	{
		CameraMoveUp(&camera, speed);
	}
	if (keyboard[SDL_SCANCODE_Q])
	{
		CameraMoveUp(&camera, -speed);
	}

	if (keyboard[SDL_SCANCODE_F1])
	{
		wireframe = !wireframe;
	}
	
	CameraRotateOnAxis(&camera, glm::vec3(0.0f, 1.0f, 0.0f), glm::radians((float)(-mouse.x) * dt * 40));
	CameraRotateOnAxis(&camera, glm::vec3(1.0f, 0.0f, 0.0f), glm::radians((float)(-mouse.y) * dt * 40));
}

void Draw()
{
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	texture.Bind();

	wireframeShader.Bind();
	wireframeShader.SetUniformMat4(wireframeShader.GetUniformLocation("u_projection"), camera.projection);
	wireframeShader.SetUniformMat4(wireframeShader.GetUniformLocation("u_view"), camera.view);
	wireframeShader.SetUniformVec3(wireframeShader.GetUniformLocation("u_color"), glm::vec3(0.019f, 0.658f, 0.321f));

	if (wireframe)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		for (const auto& mesh : meshes)
		{
			mesh.Draw(wireframeShader, model);
		}

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0f, 1.0f);

	shader.Bind();
	shader.SetUniformMat4(shader.GetUniformLocation("u_projection"), camera.projection);
	shader.SetUniformMat4(shader.GetUniformLocation("u_view"), camera.view);

	for (const auto& mesh : meshes)
	{
		mesh.Draw(shader, model);
	}
	
	glDisable(GL_POLYGON_OFFSET_FILL);
}

int main(int argc, char** argv)
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		printf("%s\n", SDL_GetError());
		return -1;
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	SDL_Window* window = SDL_CreateWindow(
		"Harm",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		1920, 1080,
		SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL
	);

	SDL_SetRelativeMouseMode(SDL_TRUE);

	if (!window)
	{
		printf("%s\n", SDL_GetError());
		return -1;
	}

	SDL_GLContext context = SDL_GL_CreateContext(window);
	
	if (!context)
	{
		printf("%s\n", SDL_GetError());
		return -1;
	}

	SDL_GL_MakeCurrent(window, context);

	gladLoadGL();

	printf("OpenGL Version: %s\n", glGetString(GL_VERSION));

	SDL_Event ev = {};
	bool running = true;

	Init();

	Uint64 old_time = SDL_GetPerformanceCounter();
	Uint64 elapsed_time = 0;
	float dt = 0.0f;

	while (running)
	{
		elapsed_time = SDL_GetPerformanceCounter() - old_time;
		old_time = SDL_GetPerformanceCounter();
		dt = (float)elapsed_time / SDL_GetPerformanceFrequency();

		mouse = glm::ivec2(0);
		keyboard[SDL_SCANCODE_F1] = false;

		while (SDL_PollEvent(&ev))
		{
			if (ev.type == SDL_QUIT)
			{
				running = false;
				break;
			}
			else if (ev.type == SDL_KEYDOWN)
			{
				keyboard[ev.key.keysym.scancode] = true;
			}
			else if (ev.type == SDL_KEYUP)
			{
				keyboard[ev.key.keysym.scancode] = false;
			}
			else if (ev.type == SDL_MOUSEMOTION)
			{
				mouse.x = ev.motion.xrel;
				mouse.y = ev.motion.yrel;
			}
		}

		Update(dt);
		Draw();

		SDL_GL_SwapWindow(window);
	}

	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}