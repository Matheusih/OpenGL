#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/filesystem.h>
#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
float processInput(std::vector<Camera> &cameras, GLFWwindow *window, unsigned short &cur_cam);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    // -------------------------
    Shader ourShader(FileSystem::getPath("resources/cg_ufpel.vs").c_str(), FileSystem::getPath("resources/cg_ufpel.fs").c_str());

    // load models
    // -----------
    Model ourModel(FileSystem::getPath("resources/objects/nanosuit/nanosuit.obj"));

    
    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    // -----------
	std::vector<Camera> my_Cameras;
	unsigned short cur_cam = 0;
	

	Camera my_camera(glm::vec3(3.0f, 2.0f, 3.0f));
	my_camera.Front = glm::normalize(glm::vec3(0, 0, 0) - my_camera.Position); //makes camera face origin
	my_camera.Right = glm::normalize(glm::cross( my_camera.WorldUp, my_camera.Front));
	my_camera.Up = glm::normalize(glm::cross(my_camera.Front, my_camera.Right));
	
	my_Cameras.push_back(camera);
	my_Cameras.push_back(my_camera);
	float press = 0;
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		// -----
		if((currentFrame - press ) > .03f)
			press = processInput(my_Cameras, window, cur_cam);

		//Animate
		// -----
		if (my_Cameras.size() > 0) {
			if(my_Cameras[cur_cam].animations.size() > 0)
				my_Cameras[cur_cam].Animate();
			// render
			// ------
			glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// don't forget to enable shader before setting uniforms
			ourShader.use();

			// view/projection transformations
			glm::mat4 projection = glm::perspective(glm::radians(my_Cameras[cur_cam].Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
			glm::mat4 view = my_Cameras[cur_cam].GetViewMatrix();
			//view = glm::lookAt(camera.Position, glm::vec3(0, 0, 0), camera.WorldUp);
			ourShader.setMat4("projection", projection);
			ourShader.setMat4("view", view);

			// render the loaded model
			glm::mat4 model;
			model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f)); // translate it down so it's at the center of the scene
			model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	// it's a bit too big for our scene, so scale it down
			ourShader.setMat4("model", model);
			ourModel.Draw(ourShader);


			// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
			// -------------------------------------------------------------------------------

			glfwSwapBuffers(window);
			glfwPollEvents();
		}
	}

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
float processInput(std::vector<Camera> &cameras, GLFWwindow *window, unsigned short &cur_cam)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameras[cur_cam].ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameras[cur_cam].ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameras[cur_cam].ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameras[cur_cam].ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
		cameras[cur_cam].animations.push_back(Animations::TRANSLATEMZ);
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
		cameras[cur_cam].animations.push_back(Animations::TRANSLATER);
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
		cameras[cur_cam].animations.push_back(Animations::TRANSLATEL);
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
		cameras[cur_cam].animations.push_back(Animations::TRANSLATEZ);
	if (glfwGetKey(window, GLFW_KEY_F9) == GLFW_PRESS)
		cameras[cur_cam].animations.push_back(Animations::BSPLINE);
	if (glfwGetKey(window, GLFW_KEY_F10) == GLFW_PRESS)
		cameras[cur_cam].animations.push_back(Animations::BEZIER);

	if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS) {
		//if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_RELEASE) {
			printf("Queued rotation\n");
			cameras[cur_cam].animations.push_back(Animations::ROTATE);
		//}
	}
	if (glfwGetKey(window, GLFW_KEY_F7) == GLFW_PRESS) {
		//if (glfwGetKey(window, GLFW_KEY_F7) == GLFW_RELEASE) {
		printf("Queued rotation\n");
		cameras[cur_cam].animations.push_back(Animations::ROTATE_POINT);
		//}
	}

	// 1: Creates new camera at random position with xyz ranging [0,3] looking at origin
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
		glm::vec3 Position;
		for (int i = 0; i < 3; ++i)
			Position[i] = (float) (rand() % 3 + 0);

		Camera new_cam(Position);
		new_cam.LookAt(glm::vec3(0, 0, 0));
		cameras.push_back(new_cam);
	}

	if (glfwGetKey(window, GLFW_KEY_DELETE) == GLFW_PRESS) {
		if (cameras.size() > 1)
			cameras.erase(cameras.begin() + cur_cam);
		if (cur_cam > cameras.size() - 1) cur_cam = 0;
	}
	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) {
		++cur_cam;
		if (cur_cam > cameras.size() - 1) cur_cam = 0;
	}

	return glfwGetTime();
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}
