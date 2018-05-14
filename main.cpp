#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <SOIL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"
#include "Camera.h"
using namespace std;

// 窗口大小。
const GLuint screenWidth = 800, screenHeight = 800;

// 箱子的位置。
glm::vec3 boxPositions[] = {
	glm::vec3( 0.0f,  0.0f,  0.0f),
	glm::vec3( 20.5f,  5.0f, -15.0f),
	glm::vec3(-10.5f, -2.2f, -2.5f),
	
	glm::vec3( 20.4f, -0.4f, -30.5f),
	glm::vec3(-10.7f,  3.0f, -30.5f),
	glm::vec3( 10.3f, -5.0f, -5.5f),

	glm::vec3( 10.5f,  20.0f, -2.5f),
	glm::vec3( 1.5f,  0.2f, -1.5f),
	glm::vec3(-1.3f,  10.0f, -1.5f),

	glm::vec3( 10.5f,  2.0f, -1.5f),
	glm::vec3( 10.5f,  5.2f, -10.5f),
	glm::vec3(-3.8f, -15.0f, -12.3f)
};

// 相机属性。
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

// 键盘回调。
void key_callback (GLFWwindow* window, int key, int scancode, int action, int mode) {
	// 当用户按下ESC键,我们设置window窗口的WindowShouldClose属性为true，关闭应用程序。
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		 glfwSetWindowShouldClose(window, GL_TRUE);

	if (key >= 0 && key < 1024) {
        if(action == GLFW_PRESS)
            keys[key] = true;
        else if(action == GLFW_RELEASE)
            keys[key] = false;	
    }
}

// 鼠标滑动回调。
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if(firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos;
    
    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// 鼠标滚轮回调。
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(yoffset);
}

// 键盘控制相机移动。
void moveCamera () {
    if(keys[GLFW_KEY_W])
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if(keys[GLFW_KEY_S])
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if(keys[GLFW_KEY_A])
        camera.ProcessKeyboard(LEFT, deltaTime);
    if(keys[GLFW_KEY_D])
        camera.ProcessKeyboard(RIGHT, deltaTime);
	if(keys[GLFW_KEY_C])
        camera.ProcessKeyboard(DOWN, deltaTime);
	if(keys[GLFW_KEY_SPACE])
        camera.ProcessKeyboard(UP, deltaTime);
	
	boxPositions[0] = camera.Position + camera.Front + camera.Front + camera.Front;
}

GLuint loadCubeMap (vector<const GLchar*> faces) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glActiveTexture(GL_TEXTURE0);

    int imageWidth, imageHeight;
    unsigned char* image;

    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    for (GLuint i = 0; i < faces.size(); i++) {
        image = SOIL_load_image(faces[i], &imageWidth, &imageHeight, 0, SOIL_LOAD_RGB);
        glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    }

	// 设置纹理过滤。
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// 设置纹理环绕方式。
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    return textureID;
}

GLuint loadTexture (GLchar* path) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    int imageWidth, imageHeight;
    unsigned char* image = SOIL_load_image(path, &imageWidth, &imageHeight, 0, SOIL_LOAD_RGB);
    // Assign texture to ID
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);	

    // 设置纹理环绕方式。。
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// 设置纹理过滤。
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// 释放资源。
    glBindTexture(GL_TEXTURE_2D, 0);
    SOIL_free_image_data(image);
    return textureID;
}

int main () {
	// 初始化 GLFW
	glfwInit();
	// 将OpenGL的主版本号(Major)和次版本号(Minor)都设为3，模式为核心模式，窗口大小不可调。
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// 创建窗口。
	GLFWwindow* window = glfwCreateWindow(800, 800, "2015130257_T-ming_final", nullptr, nullptr);
	if (window == nullptr) {
		cout << "窗口创建失败" << endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// 设置回调。
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

	// 隐藏和抓取光标，提供虚拟和无限的光标移动。
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// 让GLEW知道使用现代方法来检索函数指针和扩展。
	glewExperimental = GL_TRUE;
	// 初始化GLEW来设置OpenGL函数指针。
	glewInit();
	
	// 打开深度测试。
	glEnable(GL_DEPTH_TEST);

	// 设置窗口的维度。
	glViewport(0, 0, screenWidth, screenHeight);

	// 顶点坐标。
	float boxVertex[] = {
		// 位置                  // 纹理      // 法向量
		// 后面
		-0.25f, -0.25f, -0.25f,  0.0f, 0.0f,  0.0f,  0.0f, -1.0f,
		 0.25f, -0.25f, -0.25f,  1.0f, 0.0f,  0.0f,  0.0f, -1.0f,
		 0.25f,  0.25f, -0.25f,  1.0f, 1.0f,  0.0f,  0.0f, -1.0f,
		 0.25f,  0.25f, -0.25f,  1.0f, 1.0f,  0.0f,  0.0f, -1.0f,
		-0.25f,  0.25f, -0.25f,  0.0f, 1.0f,  0.0f,  0.0f, -1.0f,
		-0.25f, -0.25f, -0.25f,  0.0f, 0.0f,  0.0f,  0.0f, -1.0f,

		// 前面
		-0.25f, -0.25f,  0.25f,  0.0f, 0.0f,  0.0f,  0.0f,  1.0f,
		 0.25f, -0.25f,  0.25f,  1.0f, 0.0f,  0.0f,  0.0f,  1.0f,
		 0.25f,  0.25f,  0.25f,  1.0f, 1.0f,  0.0f,  0.0f,  1.0f,
		 0.25f,  0.25f,  0.25f,  1.0f, 1.0f,  0.0f,  0.0f,  1.0f,
		-0.25f,  0.25f,  0.25f,  0.0f, 1.0f,  0.0f,  0.0f,  1.0f,
		-0.25f, -0.25f,  0.25f,  0.0f, 0.0f,  0.0f,  0.0f,  1.0f,

		// 左面
		-0.25f,  0.25f,  0.25f,  1.0f, 0.0f, -1.0f,  0.0f,  0.0f,
		-0.25f,  0.25f, -0.25f,  1.0f, 1.0f, -1.0f,  0.0f,  0.0f,
		-0.25f, -0.25f, -0.25f,  0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
		-0.25f, -0.25f, -0.25f,  0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
		-0.25f, -0.25f,  0.25f,  0.0f, 0.0f, -1.0f,  0.0f,  0.0f,
		-0.25f,  0.25f,  0.25f,  1.0f, 0.0f, -1.0f,  0.0f,  0.0f,

		// 右面
		 0.25f,  0.25f,  0.25f,  1.0f, 0.0f,  1.0f,  0.0f,  0.0f,
		 0.25f,  0.25f, -0.25f,  1.0f, 1.0f,  1.0f,  0.0f,  0.0f,
		 0.25f, -0.25f, -0.25f,  0.0f, 1.0f,  1.0f,  0.0f,  0.0f,
		 0.25f, -0.25f, -0.25f,  0.0f, 1.0f,  1.0f,  0.0f,  0.0f,
		 0.25f, -0.25f,  0.25f,  0.0f, 0.0f,  1.0f,  0.0f,  0.0f,
		 0.25f,  0.25f,  0.25f,  1.0f, 0.0f,  1.0f,  0.0f,  0.0f,

		// 底面
		-0.25f, -0.25f, -0.25f,  0.0f, 1.0f,  0.0f, -1.0f,  0.0f,
		 0.25f, -0.25f, -0.25f,  1.0f, 1.0f,  0.0f, -1.0f,  0.0f,
		 0.25f, -0.25f,  0.25f,  1.0f, 0.0f,  0.0f, -1.0f,  0.0f,
		 0.25f, -0.25f,  0.25f,  1.0f, 0.0f,  0.0f, -1.0f,  0.0f,
		-0.25f, -0.25f,  0.25f,  0.0f, 0.0f,  0.0f, -1.0f,  0.0f,
		-0.25f, -0.25f, -0.25f,  0.0f, 1.0f,  0.0f, -1.0f,  0.0f,

		// 顶面
		-0.25f,  0.25f, -0.25f,  0.0f, 1.0f,  0.0f,  1.0f,  0.0f,
		 0.25f,  0.25f, -0.25f,  1.0f, 1.0f,  0.0f,  1.0f,  0.0f,
		 0.25f,  0.25f,  0.25f,  1.0f, 0.0f,  0.0f,  1.0f,  0.0f,
		 0.25f,  0.25f,  0.25f,  1.0f, 0.0f,  0.0f,  1.0f,  0.0f,
		-0.25f,  0.25f,  0.25f,  0.0f, 0.0f,  0.0f,  1.0f,  0.0f,
		-0.25f,  0.25f, -0.25f,  0.0f, 1.0f,  0.0f,  1.0f,  0.0f
	};
	GLfloat skyBoxVertex[] = {
        // 位置         
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

	// 天空盒
	// 创建并初始化顶点数组对象、顶点缓存对象。
	GLuint skyBoxVAO, skyBoxVBO;
    glGenVertexArrays(1, &skyBoxVAO);
    glGenBuffers(1, &skyBoxVBO);
	// 绑定skyBoxVAO。
    glBindVertexArray(skyBoxVAO);
	// 复制顶点数组到顶点缓冲中供OpenGL使用。
    glBindBuffer(GL_ARRAY_BUFFER, skyBoxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyBoxVertex), &skyBoxVertex, GL_STATIC_DRAW);
	// 设置顶点位置属性指针
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// 解绑skyBoxVAO。
    glBindVertexArray(0);

	// 箱子
	// 创建并初始化顶点数组对象、顶点缓存对象。
	GLuint boxVAO, boxVBO;
	glGenVertexArrays(1, &boxVAO);
	glGenBuffers(1, &boxVBO);
	// 绑定boxVAO。
	glBindVertexArray(boxVAO);
	// 复制顶点数组到顶点缓冲中供OpenGL使用。
	glBindBuffer(GL_ARRAY_BUFFER, boxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(boxVertex), boxVertex, GL_STATIC_DRAW);
	// 设置顶点位置属性指针
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
    // 设置纹理位置属性指针
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
	// // 设置法向量属性指针
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(5 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
	// 解绑boxVAO。
	glBindVertexArray(0);

	// 加载天空盒纹理。
	vector<const GLchar*> faces;
    faces.push_back("./textures/skyBox/right.jpg");
    faces.push_back("./textures/skyBox/left.jpg");
    faces.push_back("./textures/skyBox/top.jpg");
    faces.push_back("./textures/skyBox/bottom.jpg");
    faces.push_back("./textures/skyBox/back.jpg");
    faces.push_back("./textures/skyBox/front.jpg");
    GLuint skyBoxTexture = loadCubeMap(faces);

    // 加载箱子纹理。
	GLuint mainBoxTexture = loadTexture("./textures/surprisedFace.png");
    GLuint boxTexture = loadTexture("./textures/box.png");

    // 创建并编译着色器。
	Shader boxShader("./shaders/box.vs", "./shaders/box.fs");
	Shader skyBoxShader("./shaders/skyBox.vs", "./shaders/skyBox.fs");

	// 游戏循环。
	while (!glfwWindowShouldClose(window)) {
		GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

		// 检查事件。
		glfwPollEvents();
		moveCamera();

		// 渲染指令。
		// 将窗口背景色设为浅蓝色。
		glClearColor(0.12f, 0.63f, 1.0f, 1.0f);
		// 清除颜色缓存与深度缓冲区。
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 天空盒
		glDepthMask(GL_FALSE);
		glDepthFunc(GL_LEQUAL);

		skyBoxShader.Use();

		// 创建观察矩阵、投影矩阵。
		glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
		glm::mat4 projection = glm::perspective(camera.Zoom, (GLfloat)screenWidth / (GLfloat)screenHeight, 0.1f, 100.0f);

		// 将观察矩阵、投影矩阵传入着色器。	
		glUniformMatrix4fv(glGetUniformLocation(skyBoxShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(skyBoxShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        glBindVertexArray(skyBoxVAO);
		glActiveTexture(GL_TEXTURE0);
        glUniform1i(glGetUniformLocation(boxShader.Program, "skyBox"), 0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyBoxTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthMask(GL_TRUE);

		// 箱子
		boxShader.Use();

		// 创建观察矩阵、投影矩阵。
		view = camera.GetViewMatrix();
		projection = glm::perspective(camera.Zoom, (GLfloat)screenWidth / (GLfloat)screenHeight, 0.1f, 100.0f);

		// 将观察矩阵、投影矩阵传入着色器。		
		glUniformMatrix4fv(glGetUniformLocation(boxShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(boxShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		// 将相机位置传入着色器。
		glUniform3f(glGetUniformLocation(boxShader.Program, "cameraPosition"), camera.Position.x, camera.Position.y, camera.Position.z);

		glBindVertexArray(boxVAO);

		for (GLuint i = 0; i < 12; ++i) {
			if (i == 0) {
				glBindTexture(GL_TEXTURE_2D, mainBoxTexture);
			} else {
				glBindTexture(GL_TEXTURE_2D, boxTexture);
			}
			// 创建模型矩阵。
			glm::mat4 model;
			model = glm::translate(model, boxPositions[i]); 
			model = glm::rotate(model, (GLfloat)glfwGetTime() * 50.0f, glm::vec3(0.5f, 1.0f, 0.0f));
			// 将模型矩阵传入着色器。
			glUniformMatrix4fv(glGetUniformLocation(boxShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		glBindVertexArray(0);

		// 交换缓冲。
		glfwSwapBuffers(window);
	}

	// 释放资源。
	glDeleteVertexArrays(1, &boxVAO);
	glDeleteBuffers(1, &boxVBO);
	// 释放GLFW分配的内存。
	glfwTerminate();
	return 0;
}