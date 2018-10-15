#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/spline.hpp>
#include <vector>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

enum Animations {
	TRANSLATEZ,
	TRANSLATER,
	TRANSLATEL,
	TRANSLATEMZ,
	ROTATE_POINT,
	ROTATE,
	BSPLINE,
	BEZIER
};

enum Anim_Orientations {
	X, Y, Z
};

// Default camera values
const float YAW         = -90.0f;
const float PITCH       =  0.0f;
const float SPEED       =  2.5f;
const float SENSITIVITY =  0.1f;
const float ZOOM        =  45.0f;
const float ROTATION_SPEED = glm::radians(60.0f) * 12.0f;
const float STEP = 3.5f;



// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
    // Camera Attributes
	glm::vec3 Position;
	glm::vec3 start_Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
	glm::vec3 start_Front;
	glm::vec3 start_Up;
	glm::vec3 start_Right;
    glm::vec3 WorldUp;

	std::vector<glm::vec3> mypath;

    // Euler Angles
    float Yaw;
    float Pitch;
    // Camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;
	float anim_start = 0.0;
	float anim_end;
	float anim_time = 2.0f;
	float offset = 0.0f;
	float t_spline = 0.0f;

	bool anim_Started = false;
	std::vector<Animations> animations;

    // Constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
		mypath.push_back(glm::vec3(0, 0, 3));
		mypath.push_back(glm::vec3(3, 3, 0));
		mypath.push_back(glm::vec3(0, 0, -3));
		mypath.push_back(glm::vec3(-3, -3, 0));
		mypath.push_back(glm::vec3(0, 0, 3));
    }
    // Constructor with scalar values
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
		//path to camera do
		mypath.push_back(glm::vec3(0, 0, 3));
		mypath.push_back(glm::vec3(3, 3, 0));
		mypath.push_back(glm::vec3(0, 0, -3));
		mypath.push_back(glm::vec3(-3, -3, 0));
		mypath.push_back(glm::vec3(0, 0, 3));
    }

    // Returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    // Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        if (direction == FORWARD)
            Position += Front * velocity;
        if (direction == BACKWARD)
            Position -= Front * velocity;
        if (direction == LEFT)
            Position -= Right * velocity;
        if (direction == RIGHT)
            Position += Right * velocity;
    }

    // Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw   += xoffset;
        Pitch += yoffset;

        // Make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        // Update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors();
    }

    // Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset)
    {
        if (Zoom >= 1.0f && Zoom <= 45.0f)
            Zoom -= yoffset;
        if (Zoom <= 1.0f)
            Zoom = 1.0f;
        if (Zoom >= 45.0f)
            Zoom = 45.0f;
    }
	void LookAt(glm::vec3 target) {
		Front = glm::normalize(target - Position);
		Right = glm::normalize(glm::cross(WorldUp, Front));
		Up = glm::normalize(glm::cross( Front, Right ));
	}
	void Animate() {
		glm::mat4 res_m(1.0f);
		float deltaTime;
		float curentTime;
		
		unsigned short end_anim_flag = 0;
		bool anim_ended = false;

		if (anim_Started == false && animations.size() > 0) {
			anim_Started = true;
			anim_start = glfwGetTime();  //-offset
			anim_end = anim_start + anim_time;
			start_Right = Right;
			start_Up = Up;
			start_Front = Front;
			start_Position = Position;
		}

		curentTime = glfwGetTime();
		deltaTime = ((curentTime - anim_start) / (anim_end - anim_start)) + offset;
		offset = 0.0f;
		
		if (curentTime >= anim_end) {
			end_anim_flag = 1;
			anim_ended = true;
			deltaTime = 1.0f;
		} 
		printf("cur time: %.3f\n", curentTime);
		printf("cur end: %.3f\n", anim_end);
		printf("cur start: %.3f\n\n", anim_start);
		//printf("delta time %.2f\n", deltaTime);
		//printf("Position: %.3f, %.3f, %.3f\n", Position.x, Position.y, Position.z);
		if (animations.size() > 0) {
			switch (animations[0]) {
			case ROTATE_POINT:
				//res_m = rotate_about(glm::vec3(0.0f, -1.75f, 0.0f), deltaTime); //model pos 0.0f, -1.75f, 0.0f
				res_m = rotate_about(glm::vec3(0.0f, 0.f, 0.f), deltaTime); //model pos 0.0f, -1.75f, 0.0f
				break;
			case ROTATE:
				res_m = Rotate(deltaTime);
				break;
			case BSPLINE:
				if (t_spline < 4);
					B_Spline(deltaTime);
				break;
			case BEZIER:
				LookAt(glm::vec3(0, 0, 0));
				mybez(deltaTime, mypath, anim_ended);
				break;
			case TRANSLATEZ:
				TranslateZ(deltaTime);
				break;
			case TRANSLATEMZ:
				TranslateMZ(deltaTime);
				break;
			case TRANSLATER:
				TranslateR(deltaTime);
				break;
			case TRANSLATEL:
				TranslateL(deltaTime);
				break;
			}
			
			
			
		}

		if (end_anim_flag == 1 && animations.size() > 0) {  //if animation has ended update anim queue
			anim_Started = false;
			animations.erase(animations.begin());
			//if ( animations.size() < 1 )
			anim_start = offset;
			t_spline = 0.0f;
			//updateDirections(res_m); //update Right ,Up , Front vectors of camera
		}
			
	}

private:
	glm::vec3 mybez(float t, std::vector<glm::vec3>& path, bool ended) {
		printf("t = %.2f\n", t);
		if (path.size() < 2) return glm::vec3(0, 0, 0);

		float c1, c2, c3;
		glm::vec3 p;
		bool linear = false;

		c1 = pow(1 - t, 2);
		c2 = 2 * t * (1 - t);
		c3 = pow(t, 2);

		if (path.size() > 2) { //quad
			p = c1 * path[0] + c2 * path[1] + c3 * path[2];

			//printf("Quad\n");
		}
		else { //linear
			p = (1 - t)*path[0] + t * path[1];
			printf("Linear\n");
			linear = true;
		}
		if (ended == true) {
			if (linear)
				path.erase(path.begin(), path.end());
			else
				path.erase(path.begin(), path.begin() + 2);
		}

		Position = p;

		return p;

	}
    // Calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors()
    {
        // Calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        // Also re-calculate the Right and Up vector
        Right = glm::normalize(glm::cross(Front, WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up    = glm::normalize(glm::cross(Right, Front));
    }


	//model pos 0.0f, -1.75f, 0.0f

	glm::mat4 rotate_about(glm::vec3 point, float t) {
		glm::mat4 view(1.0f);
		
		view = glm::translate(view,  point);
		view = glm::rotate(view, ROTATION_SPEED*t, glm::vec3(0, 1, 0));
		view = glm::translate(view, start_Position - point);


		Front = glm::normalize(point - Position);
		Right = glm::normalize(glm::cross(WorldUp, Front));
		//Up = glm::normalize(glm::cross(Front, Right));

		Position.x = view[3][0];
		Position.y = view[3][1];
		Position.z = view[3][2];
		
		return view;
	}
	glm::mat4 Rotate(float t) {
		glm::mat4 rot_m = getMatrix(start_Right, start_Up, start_Front, start_Position); //builds rotation matrix from camera vectors
		
		glm::mat4 res_m = glm::rotate(rot_m, ROTATION_SPEED * t, glm::vec3(0, 0, 1)); //rotates 

		updateDirections(res_m); //updates Right,Up,Front vectors of camera
		return res_m;
	}
	void B_Spline(float t) {
		float tspline = t * 4; //normalizes deltaTime to range 0 -> n of cps
		printf("tspline : %.2f\n", tspline);
		std::vector<glm::vec3> cp;
		cp.push_back(glm::vec3(0, 0, 3));
		cp.push_back(glm::vec3(3, 3, 0));
		cp.push_back(glm::vec3(0, 0, -3));
		cp.push_back(glm::vec3(-3, -3, 0));
		cp.push_back(glm::vec3(0, 0, 3));

		glm::vec3 res = catmull_rom_spline(cp,tspline);
		Position = res;

		Front = glm::normalize(glm::vec3(0,0,0) - Position); //camera always face origin
		Right = glm::normalize(glm::cross(WorldUp, Front));
		Up = glm::normalize(glm::cross(Front, Right));

		t_spline += 0.01f;
	}

	void TranslateZ(float t) {
		Position.z = start_Position.z + STEP * t;

		Front = glm::normalize(glm::vec3(0, 0, 0) - Position); //camera always face origin
		Right = glm::normalize(glm::cross(WorldUp, Front));
		Up = glm::normalize(glm::cross(Front, Right));
	}
	void TranslateR(float t) {
		Position.x = start_Position.x + STEP * t;

		Front = glm::normalize(glm::vec3(0, 0, 0) - Position); //camera always face origin
		Right = glm::normalize(glm::cross(WorldUp, Front));
		Up = glm::normalize(glm::cross(Front, Right));
	}
	void TranslateL(float t) {
		Position.x = start_Position.x - STEP * t;

		Front = glm::normalize(glm::vec3(0, 0, 0) - Position); //camera always face origin
		Right = glm::normalize(glm::cross(WorldUp, Front));
		Up = glm::normalize(glm::cross(Front, Right));
	}
	void TranslateMZ(float t) {
		Position.z = start_Position.z - STEP * t;

		Front = glm::normalize(glm::vec3(0, 0, 0) - Position); //camera always face origin
		Right = glm::normalize(glm::cross(WorldUp, Front));
		Up = glm::normalize(glm::cross(Front, Right));
	}


	glm::mat4 getMatrix(glm::vec3 right, glm::vec3 up, glm::vec3 front, glm::vec3 position) {
		glm::mat4 mat;
		mat[0][0] = right.x;
		mat[0][1] = right.y;
		mat[0][2] = right.z;

		mat[1][0] = up.x;
		mat[1][1] = up.y;
		mat[1][2] = up.z;

		mat[2][0] = front.x;
		mat[2][1] = front.y;
		mat[2][2] = front.z;

		mat[3][0] = position.x;
		mat[3][1] = position.y;
		mat[3][2] = position.z;
		return mat;
	}

	void updateDirections(glm::mat4 mat) {
		Right.x = mat[0][0];
		Right.y = mat[0][1];
		Right.z = mat[0][2];

		Up.x = mat[1][0];
		Up.y = mat[1][1];
		Up.z = mat[1][2];

		Front.x = mat[2][0];
		Front.y = mat[2][1];
		Front.z = mat[2][2];

		Position.x = mat[3][0];
		Position.y = mat[3][1];
		Position.z = mat[3][2];
	}

	glm::vec3 catmull_rom_spline(const std::vector<glm::vec3>& cp, float t)
	{
		// indices of the relevant control points
		int i0 = (int)glm::clamp(t - 1, 0.0f, (float)cp.size() - 1);
		int i1 = (int)glm::clamp(t, 0.0f, (float)cp.size() - 1);
		int i2 = (int)glm::clamp(t + 1, 0.0f, (float)cp.size() - 1);
		int i3 = (int)glm::clamp(t + 2, 0.0f, (float)cp.size() - 1);

		// parameter on the local curve interval
		float local_t = glm::fract(t);

		return glm::catmullRom(cp[i0], cp[i1], cp[i2], cp[i3], local_t);
	}

};
#endif