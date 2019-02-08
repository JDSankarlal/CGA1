#include "Game.h"
#include "ResourceManager.h"
#include "TextureCube.h"
#include "UI.h"
#include "Light.h"

#include <vector>
#include <string>
#include <fstream>
#include <random>

Game::Game()
{
	updateTimer = new Timer();
}

Game::~Game()
{
	delete updateTimer;
}

int activeToonRamp = 0;
bool toonActive = false;
bool rimActive = false;
bool ambientActive = false;
bool specularActive = false;

void Game::initializeGame()
{
	ShaderProgram::initDefault();
	Framebuffer::initFrameBuffers();
	framebuffer.addDepthTarget();
	framebuffer.addColorTarget(GL_RGB8);
	framebuffer.init(windowWidth, windowHeight);
	framebufferTV.addDepthTarget();
	framebufferTV.addColorTarget(GL_RGB8);
	framebufferTV.init(128, 128);
	meshIsland.LoadFromObj("island.obj");
	meshTree.LoadFromObj("tree.obj");
	meshLeaves.LoadFromObj("leaves.obj");
	meshSphere.initMeshSphere(32U, 32U);
	meshSkybox.initMeshSphere(32U, 32U, true);
	meshLight.initMeshSphere(6U, 6U);
	
	shaderBasic.load("shader.vert", "shader.frag");
	shaderTexture.load("shader.vert", "shaderTexture.frag");
	shaderRim.load("shader.vs", "shader.fs");
	shaderSky.load("shaderSky.vert", "shaderSky.frag");

	ResourceManager::Shaders.push_back(&shaderBasic);
	ResourceManager::Shaders.push_back(&shaderTexture);
	ResourceManager::Shaders.push_back(&shaderRim);
	ResourceManager::Shaders.push_back(&shaderSky);

	uniformBufferCamera.allocateMemory(sizeof(mat4) * 2);
	uniformBufferCamera.bind(0);
	uniformBufferTime.allocateMemory(sizeof(float));
	uniformBufferTime.bind(1);
	uniformBufferLightScene.allocateMemory(sizeof(vec4));
	uniformBufferLightScene.bind(2);
	light.m_pUBO.bind(3);							
	uniformBufferToon.allocateMemory(sizeof(int)); // TOON
	uniformBufferToon.bind(5);
	uniformBufferRim.allocateMemory(sizeof(int)); // RIM
	uniformBufferRim.bind(6);
	uniformBufferAmbient.allocateMemory(sizeof(int)); // AMBIENT
	uniformBufferAmbient.bind(7);
	uniformBufferSpecular.allocateMemory(sizeof(int)); // SPECULAR
	uniformBufferSpecular.bind(8);
	
	uniformBufferToon.sendBool(false, 0);
	uniformBufferLightScene.sendVector(vec3(0.2f), 0);
	uniformBufferRim.sendBool(false, 0);
	uniformBufferLightScene.sendVector(vec3(0.2f), 0);
	uniformBufferAmbient.sendBool(false, 0);
	uniformBufferLightScene.sendVector(vec3(0.2f), 0);
	uniformBufferSpecular.sendBool(false, 0);
	uniformBufferLightScene.sendVector(vec3(0.2f), 0);

	Texture* texBlack = new Texture("black.png");
	Texture* texYellow = new Texture("yellow.png");
	Texture* texTreeAlbedo = new Texture("TreeAlbedo.png");
	Texture* texTreeSpecular = new Texture("TreeSpecular.png");
	Texture* texIslandAlbedo = new Texture("IslandAlbedo.png");
	Texture* texIslandSpecular = new Texture("IslandSpecular.png");
	Texture* texLeavesAlbedo = new Texture("LeavesAlbedo.png");
	Texture* texLeavesSpecular = new Texture("LeavesSpecular.png");

	// TODO: Load toon texture ramp
	textureToonRamp.push_back(new Texture("TF2.jpg", false));
	textureToonRamp[0]->setWrapParameters(GL_CLAMP_TO_EDGE);
	textureToonRamp[0]->setFilterParameters(GL_NEAREST, GL_NEAREST);
	textureToonRamp[0]->sendTexParameters();
	textureToonRamp.push_back(new Texture("toonramp1.png", false));
	textureToonRamp[1]->setWrapParameters(GL_CLAMP_TO_EDGE);
	textureToonRamp[1]->sendTexParameters();
	textureToonRamp.push_back(new Texture("toonramp2.png", false));
	textureToonRamp[2]->setWrapParameters(GL_CLAMP_TO_EDGE);
	textureToonRamp[2]->sendTexParameters();

	std::vector<Texture*> texTree = { texTreeAlbedo, texBlack, texTreeSpecular };
	std::vector<Texture*> texIsland = { texIslandAlbedo, texBlack, texIslandSpecular };
	std::vector<Texture*> texSun = { texBlack, texYellow, texBlack };
	std::vector<Texture*> texLeaves = { texLeavesAlbedo, texBlack, texLeavesSpecular };

	goSun = GameObject(&meshSphere, texSun);
	goSun.addChild(&light);
	goTree = GameObject(&meshTree, texTree);
	goIsland = GameObject(&meshIsland, texIsland);
	goLeaves = GameObject(&meshLeaves, texLeaves);

	std::vector<std::string> skyboxTex;
	skyboxTex.push_back("sky2/sky_c00.bmp");
	skyboxTex.push_back("sky2/sky_c01.bmp");
	skyboxTex.push_back("sky2/sky_c02.bmp");
	skyboxTex.push_back("sky2/sky_c03.bmp");
	skyboxTex.push_back("sky2/sky_c04.bmp");
	skyboxTex.push_back("sky2/sky_c05.bmp");
	goSkybox = GameObject(&meshSkybox, new TextureCube(skyboxTex));
	//goSkybox = GameObject(&meshSkybox, new TextureCube("Sky/Skybox.png"));
	goSkybox.setShaderProgram(&shaderSky);

	ResourceManager::addEntity(&goSun);
	ResourceManager::addEntity(&goTree);
	ResourceManager::addEntity(&goIsland);
	ResourceManager::addEntity(&goLeaves);

	goSun.setLocalPos(vec3(4, 5, 0));
	goTree.setLocalPos(vec3(0, 0, 0));
	goIsland.setLocalPos(vec3(0, 0, 0));
	goLeaves.setLocalPos(vec3(0, 0, 0));

	goSun.setScale(1.f);
	goTree.setScale(1.f);
	goIsland.setScale(1.f);
	goLeaves.setScale(1.f);

	goSun.setShaderProgram(&shaderTexture);
	goTree.setShaderProgram(&shaderTexture);
	goIsland.setShaderProgram(&shaderTexture);
	goLeaves.setShaderProgram(&shaderTexture);

	   	 
	// These Render flags can be set once at the start (No reason to waste time calling these functions every frame).
	// Tells OpenGL to respect the depth of the scene. Fragments will not render when they are behind other geometry.
	glEnable(GL_DEPTH_TEST); 
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	   
	// Basic clear color used by glClear().
	glClearColor(0, 0, 0, 0); // Black.

	// Setup our main scene objects...
	float aspect = static_cast<float>(windowWidth) / static_cast<float>(windowHeight);
	camera.perspective(90.0f, aspect, 0.05f, 1000.0f);
	camera.setLocalPos(vec3(0.0f, 4.0f, 4.0f));
	camera.setLocalRotX(-15.0f);
	camera.attachFrameBuffer(&framebuffer);


}

void Game::update()
{
	// update our clock so we have the delta time since the last update
	updateTimer->tick();

	float deltaTime = updateTimer->getElapsedTimeSeconds();
	TotalGameTime += deltaTime;

#pragma region movementCode
	float cameraSpeedMult = 2.0f;
	float cameraRotateSpeed = 90.0f;
	if (input.shiftL || input.shiftR)
	{
		cameraSpeedMult *= 4.0f;
	}

	if (input.ctrlL || input.ctrlR)
	{
		cameraSpeedMult *= 0.5f;
	}
	if (input.moveUp)
	{
		camera.m_pLocalPosition.y += cameraSpeedMult * deltaTime;
	}
	if (input.moveDown)
	{
		camera.m_pLocalPosition.y -= cameraSpeedMult * deltaTime;
	}
	if (input.moveForward)
	{
		camera.m_pLocalPosition -= camera.m_pLocalRotation.GetForward() * cameraSpeedMult * deltaTime;
	}
	if (input.moveBackward)
	{
		camera.m_pLocalPosition += camera.m_pLocalRotation.GetForward() * cameraSpeedMult * deltaTime;
	}
	if (input.moveRight)
	{
		camera.m_pLocalPosition += camera.m_pLocalRotation.GetRight() *  cameraSpeedMult * deltaTime;
	}
	if (input.moveLeft)
	{
		camera.m_pLocalPosition -= camera.m_pLocalRotation.GetRight() * cameraSpeedMult * deltaTime;
	}
	if (input.rotateUp)
	{
		camera.m_pLocalRotationEuler.x += cameraRotateSpeed * deltaTime;
		camera.m_pLocalRotationEuler.x = min(camera.m_pLocalRotationEuler.x, 85.0f);
	}
	if (input.rotateDown)
	{
		camera.m_pLocalRotationEuler.x -= cameraRotateSpeed * deltaTime;
		camera.m_pLocalRotationEuler.x = max(camera.m_pLocalRotationEuler.x, -85.0f);
	}
	if (input.rotateRight)
	{
		camera.m_pLocalRotationEuler.y -= cameraRotateSpeed * deltaTime;
	}
	if (input.rotateLeft)
	{
		camera.m_pLocalRotationEuler.y += cameraRotateSpeed * deltaTime;
	}
#pragma endregion movementCode

	// Give the earth some motion over time.
	goLeaves.setLocalRotY(TotalGameTime * 70.0f);

	light.position = camera.getView() * vec4(goSun.getWorldPos(), 1.0f);
	light.update(deltaTime);
	// Give our Transforms a chance to compute the latest matrices
	camera.update(deltaTime);
	for (Transform* object : ResourceManager::Transforms)
	{
		object->update(deltaTime);
	}
	goSkybox.update(deltaTime);
}

void Game::draw()
{
	// TODO: Bind toon texture

	textureToonRamp[activeToonRamp]->bind(31);

	uniformBufferCamera.sendMatrix(camera.getLocalToWorld(), sizeof(mat4));
	goSkybox.draw();
	glClear(GL_DEPTH_BUFFER_BIT);		
	
	uniformBufferTime.sendFloat(TotalGameTime, 0);
	uniformBufferCamera.sendMatrix(camera.getProjection(), 0);
	uniformBufferCamera.sendMatrix(camera.getView(), sizeof(mat4));

	shaderTexture.bind();
	shaderTexture.unbind();
	
	light.position = camera.getView() * vec4(goSun.getWorldPos(), 1.0f);
	
	camera.render();

	framebuffer.bindColorAsTexture(0, 0);
	glViewport(0, 0, windowWidth, windowHeight);
	framebuffer.drawFSQ();
	ShaderProgram::unbind();
	framebuffer.unbindTexture(0);

	if(guiEnabled)
		GUI();

	// Commit the Back-Buffer to swap with the Front-Buffer and be displayed on the monitor.
	glutSwapBuffers();
}

int minCurrMode = 5;
int magCurrMode = 1;
void Game::GUI()
{
	UI::Start(windowWidth, windowHeight);
		
	// TODO: Add imgui controls to texture ramps
	if (ImGui::Checkbox("Toon Shading Active", &toonActive))
	{
		uniformBufferToon.sendBool(toonActive, 0);
	}
	if (ImGui::Checkbox("Rim Shading Active", &rimActive))
	{
		uniformBufferRim.sendBool(rimActive, 0);
	}
	if (ImGui::Checkbox("Ambient Light Active", &ambientActive))
	{
		uniformBufferAmbient.sendBool(ambientActive, 0);
	}
	if (ImGui::Checkbox("Specular Shading Active", &specularActive))
	{
		uniformBufferSpecular.sendBool(specularActive, 0);
	}

	if (ImGui::SliderInt("Toon Ramp Selection", &activeToonRamp, 0, (int)textureToonRamp.size()-1))
	{

	}

	if (ImGui::SliderFloat3("Attenuation", &light.attenConst, 0.0001f, 1.0f, "%.3f", 2.0f))
	{
		//clamp luminanceMin
	}

	static vec3 lightPosition = goSun.getLocalPos();
	if (ImGui::SliderFloat3("Light Position", &lightPosition[0], 10.f, -10.f))
	{
		goSun.setLocalPos(lightPosition);
	}

	ImGui::SliderFloat3("Light Color", &light.color[0], 0.f, 1.f);
	
	ImGui::Text("Radius: %f", light.radius);

	UI::End();
}

void Game::keyboardDown(unsigned char key, int mouseX, int mouseY)
{
	if (guiEnabled)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.KeysDown[(int)key] = true;
		io.AddInputCharacter((int)key); // this is what makes keyboard input work in imgui
		// This is what makes the backspace button work
		int keyModifier = glutGetModifiers();
		switch (keyModifier)
		{
		case GLUT_ACTIVE_SHIFT:
			io.KeyShift = true;
			break;

		case GLUT_ACTIVE_CTRL:
			io.KeyCtrl = true;
			break;

		case GLUT_ACTIVE_ALT:
			io.KeyAlt = true;
			break;
		}
	}

	switch(key)
	{
	case 27: // the escape key
		break;
	case 'w':
	case 'W':
	case 'w' - 96:
		input.moveForward = true;
		break;
	case 's':
	case 'S':
	case 's' - 96:
		input.moveBackward = true;
		break;
	case 'd':
	case 'D':
	case 'd' - 96:
		input.moveRight = true;
		break;
	case 'a':
	case 'A':
	case 'a' - 96:
		input.moveLeft = true;
		break;
	case 'e':
	case 'E':
	case 'e' - 96:
		input.moveUp = true;
		break;
	case 'q':
	case 'Q':
	case 'q' - 96:
		input.moveDown = true;
		break;
	case 'l':
	case 'L':
	case 'l' - 96:
		input.rotateRight = true;
		break;
	case 'j':
	case 'J':
	case 'j' - 96:
		input.rotateLeft = true;
		break;
	case 'i':
	case 'I':
	case 'i' - 96:
		input.rotateUp = true;
		break;
	case 'k':
	case 'K':
	case 'k' - 96:
		input.rotateDown = true;
		break;
	case '1':
		if (rimActive)
		{
			uniformBufferRim.sendBool(false, 0);
			rimActive = false;
		}
		if (ambientActive)
		{
			uniformBufferAmbient.sendBool(false, 0);
			ambientActive = false;
		}
		if (specularActive)
		{
			uniformBufferSpecular.sendBool(false, 0);
			specularActive = false;
		}
		break;
	case '2':
		if (rimActive)
		{
			uniformBufferRim.sendBool(false, 0);
			rimActive = false;
		}
		if (!ambientActive)
		{
			uniformBufferAmbient.sendBool(true, 0);
			ambientActive = true;
		}
		if (specularActive)
		{
			uniformBufferSpecular.sendBool(false, 0);
			specularActive = false;
		}
		break;
	case '3':
		if (rimActive)
		{
			uniformBufferRim.sendBool(false, 0);
			rimActive = false;
		}
		if (ambientActive)
		{
			uniformBufferAmbient.sendBool(false, 0);
			ambientActive = false;
		}
		if (!specularActive)
		{
			uniformBufferSpecular.sendBool(true, 0);
			specularActive = true;
		}
		break;
	case '4':
		if (!rimActive)
		{
			uniformBufferRim.sendBool(true, 0);
			rimActive = true;
		}
		if (ambientActive)
		{
			uniformBufferAmbient.sendBool(false, 0);
			ambientActive = false;
		}
		if (!specularActive)
		{
			uniformBufferSpecular.sendBool(true, 0);
			specularActive = true;
		}
		break;
	case '5':
		if (!rimActive)
		{
			uniformBufferRim.sendBool(true, 0);
			rimActive = true;
		}
		if (!ambientActive)
		{
			uniformBufferAmbient.sendBool(true, 0);
			ambientActive = true;
		}
		if (!specularActive)
		{
			uniformBufferSpecular.sendBool(true, 0);
			specularActive = true;
		}
		break;
	case '6':
		if (!toonActive)
		{
			uniformBufferToon.sendBool(true, 0);
			toonActive = true;
		}
		else if (toonActive)
		{
			uniformBufferToon.sendBool(false, 0);
			toonActive = false;
		}
		if (ambientActive)
		{
			uniformBufferAmbient.sendBool(false, 0);
			ambientActive = false;
		}
		else if (!ambientActive)
		{
			uniformBufferAmbient.sendBool(true, 0);
			ambientActive = true;
		}
		break;
	case '7':
		if (!toonActive)
		{
			uniformBufferToon.sendBool(true, 0);
			toonActive = true;
		}
		else if (toonActive)
		{
			uniformBufferToon.sendBool(false, 0);
			toonActive = false;
		}
		if (specularActive)
		{
			uniformBufferSpecular.sendBool(false, 0);
			specularActive = false;
		}
		else if (!specularActive)
		{
			uniformBufferAmbient.sendBool(true, 0);
			specularActive = true;
		}
		break;
	case '8':
		break;
	case '9':
		break;
	case '0':
		break;
	}
}

void Game::keyboardUp(unsigned char key, int mouseX, int mouseY)
{
	if (guiEnabled)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.KeysDown[key] = false;

		int keyModifier = glutGetModifiers();
		io.KeyShift = false;
		io.KeyCtrl = false;
		io.KeyAlt = false;
		switch (keyModifier)
		{
		case GLUT_ACTIVE_SHIFT:
			io.KeyShift = true;
			break;

		case GLUT_ACTIVE_CTRL:
			io.KeyCtrl = true;
			break;

		case GLUT_ACTIVE_ALT:
			io.KeyAlt = true;
			break;
		}
	}

	switch(key)
	{
	case 32: // the space bar
		camera.cullingActive = !camera.cullingActive;
		break;
	case 27: // the escape key
		exit(1);
		break;
	case 'w':
	case 'W':
	case 'w' - 96:
		input.moveForward = false;
		break;
	case 's':
	case 'S':
	case 's' - 96:
		input.moveBackward = false;
		break;
	case 'd':
	case 'D':
	case 'd' - 96:
		input.moveRight = false;
		break;
	case 'a':
	case 'A':
	case 'a' - 96:
		input.moveLeft = false;
		break;
	case 'e':
	case 'E':
	case 'e' - 96:
		input.moveUp = false;
		break;
	case 'q':
	case 'Q':
	case 'q' - 96:
		input.moveDown = false;
		break;
	case 'l':
	case 'L':
	case 'l' - 96:
		input.rotateRight = false;
		break;
	case 'j':
	case 'J':
	case 'j' - 96:
		input.rotateLeft = false;
		break;
	case 'i':
	case 'I':
	case 'i' - 96:
		input.rotateUp = false;
		break;
	case 'k':
	case 'K':
	case 'k' - 96:
		input.rotateDown = false;
		break;
	case '1':
		break;
	case '2':
		break;
	case '3':
		break;
	case '4':
		break;
	case '5':
		break;
	case '6':
		break;
	case '7':
		break;
	case '8':
		break;
	case '9':
		break;
	case '0':
		break;
	}
}

void Game::keyboardSpecialDown(int key, int mouseX, int mouseY)
{
	switch (key)
	{
	case GLUT_KEY_F1:
		guiEnabled = !guiEnabled;
		if (!UI::isInit)
		{
			UI::InitImGUI();
		}
		break;
	case GLUT_KEY_F5:
		for (ShaderProgram* shader : ResourceManager::Shaders)
		{
			shader->reload();
		}
		break;
	case GLUT_KEY_CTRL_L:
		input.ctrlL = true;
		break;
	case GLUT_KEY_CTRL_R:
		input.ctrlR = true;
		break;
	case GLUT_KEY_SHIFT_L:
		input.shiftL = true;
		break;
	case GLUT_KEY_SHIFT_R:
		input.shiftR = true;
		break;
	case GLUT_KEY_ALT_L:
		input.altL = true;
		break;
	case GLUT_KEY_ALT_R:
		input.altR = true;
		break;
	case GLUT_KEY_UP:
		input.moveForward = true;
		break;
	case GLUT_KEY_DOWN:
		input.moveBackward = true;
		break;
	case GLUT_KEY_RIGHT:
		input.moveRight = true;
		break;
	case GLUT_KEY_LEFT:
		input.moveLeft = true;
		break;
	case GLUT_KEY_PAGE_UP:
		input.moveUp = true;
		break;
	case GLUT_KEY_PAGE_DOWN:
		input.moveDown = true;
		break;
	case GLUT_KEY_END:
		exit(1);
		break;
	}
}

void Game::keyboardSpecialUp(int key, int mouseX, int mouseY)
{
	switch (key)
	{
	case GLUT_KEY_CTRL_L:
		input.ctrlL = false;
		break;
	case GLUT_KEY_CTRL_R:
		input.ctrlR = false;
		break;
	case GLUT_KEY_SHIFT_L:
		input.shiftL = false;
		break;
	case GLUT_KEY_SHIFT_R:
		input.shiftR = false;
		break;
	case GLUT_KEY_ALT_L:
		input.altL = false;
		break;
	case GLUT_KEY_ALT_R:
		input.altR = false;
		break;
	case GLUT_KEY_UP:
		input.moveForward = false;
		break;
	case GLUT_KEY_DOWN:
		input.moveBackward = false;
		break;
	case GLUT_KEY_RIGHT:
		input.moveRight = false;
		break;
	case GLUT_KEY_LEFT:
		input.moveLeft = false;
		break;
	case GLUT_KEY_PAGE_UP:
		input.moveUp = false;
		break;
	case GLUT_KEY_PAGE_DOWN:
		input.moveDown = false;
		break;
	}
}

void Game::mouseClicked(int button, int state, int x, int y)
{
	if (guiEnabled)
	{
		ImGui::GetIO().MousePos = ImVec2((float)x, (float)y);
		ImGui::GetIO().MouseDown[0] = !state;
	}

	if(state == GLUT_DOWN) 
	{
		switch(button)
		{
		case GLUT_LEFT_BUTTON:

			break;
		case GLUT_RIGHT_BUTTON:
		
			break;
		case GLUT_MIDDLE_BUTTON:

			break;
		}
	}
	else
	{

	}
}

/*
 * mouseMoved(x,y)
 * - this occurs only when the mouse is pressed down
 *   and the mouse has moved.  you are given the x,y locations
 *   in window coordinates (from the top left corner) and thus 
 *   must be converted to screen coordinates using the screen to window pixels ratio
 *   and the y must be flipped to make the bottom left corner the origin.
 */
void Game::mouseMoved(int x, int y)
{
	if (guiEnabled)
	{
		ImGui::GetIO().MousePos = ImVec2((float)x, (float)y);

		if (!ImGui::GetIO().WantCaptureMouse)
		{

		}
	}
}

void Game::reshapeWindow(int w, int h)
{
	windowWidth = w;
	windowHeight = h;

	float aspect = static_cast<float>(windowWidth) / static_cast<float>(windowHeight);
	camera.perspective(90.0f, aspect, 0.05f, 1000.0f);
	glViewport(0, 0, w, h);
	framebuffer.reshape(w, h);

}
