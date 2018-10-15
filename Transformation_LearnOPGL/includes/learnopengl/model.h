#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/spline.hpp>
#include <stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/gtx/vector_angle.hpp>
#include <learnopengl/mesh.h>
#include <learnopengl/shader.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
using namespace std;


enum Animations {
	ROTATEX,
	ROTATEY,
	ROTATEZ,
	ROTATE_ABOUT,
	TRANSLATE,
	SCALE_UP, SCALE_DOWN,
	BSPLINE,
	BEZIER
};
enum Directions {
	mFORWARD, mBACKWARD,
	mLEFT, mRIGHT, mUP, mDOWN,
	X, Y, Z
};

const float STEP = 0.5f;
const float ROTATION_SPEED = glm::radians(60.0f);

unsigned int TextureFromFile(const char *path, const string &directory, bool gamma = false);

class Model 
{
public:
    /*  Model Data */
    vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    vector<Mesh> meshes;
    string directory;
    bool gammaCorrection;

	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Right;
	glm::vec3 Up;
	glm::vec3 start_Front;
	glm::vec3 start_Right;
	glm::vec3 start_Up;
	glm::vec3 start_Position;
	glm::vec3 WorldUp;
	glm::mat4 Matrix;

	std::vector<glm::vec3> mypath;
	glm::vec3 radius = glm::vec3(2, 0, 0);

	float anim_start = 0.0;
	float anim_end;
	float anim_time = 2.0f;
	float t_spline = 0.0f;
	float scale;
	float start_scale;
	float offset = 0.0f;

	float angleX = 0.0f;
	float angleY = 0.0f;
	float angleZ = 0.0f;

	bool anim_Started = false;

	std::vector<Animations> animations;
	std::vector<Directions> directions;
	/*  Animations    */
	void Animate() {
		glm::mat4 res(1.0f);
		float deltaTime, currentTime;
		bool anim_ended = false;

		if (anim_Started == false && animations.size() > 0) {
			anim_Started = true;
			anim_start = glfwGetTime();
			anim_end = anim_start + anim_time;
			start_Right = Right;
			start_Up = Up;
			start_Front = Front;
			start_Position = Position;
			start_scale = scale;
		}

		currentTime = glfwGetTime();
		deltaTime = ( (currentTime - anim_start) / (anim_end - anim_start) ) + offset;
		offset = 0.0f;

		if (currentTime >= anim_end) {
			anim_ended = true;
			offset = (currentTime - anim_end);
			deltaTime = deltaTime - offset;
		}

		if (animations.size() > 0) {
			switch (animations[0])
			{
			case ROTATEX:
				rotateX(deltaTime, anim_ended);
				break;
			case ROTATEY:
				rotateY(deltaTime, anim_ended);
				break;
			case ROTATEZ:
				rotateZ(deltaTime, anim_ended);
				break;
			case ROTATE_ABOUT:
				rotate_about(deltaTime, glm::vec3(0, 0, 0), anim_ended);
				break;
			case TRANSLATE:
				if(directions.size() > 0)
					translate(deltaTime, directions[0], anim_ended);
				break;
			case BSPLINE:
				catmull(deltaTime, anim_ended);
				break;
			case BEZIER:
				//bezier(deltaTime, anim_ended);
				mybez(deltaTime, mypath, anim_ended);
				break;
			case SCALE_UP:
				scale_model_UP(deltaTime, anim_ended);
				break;
			case SCALE_DOWN:
				scale_model_DOWN(deltaTime, anim_ended);
				break;
			default:
				break;
			}
		}

		if (anim_ended == true) {
			anim_Started = false;
			if(animations.size() > 0)
				animations.erase(animations.begin());
			if(directions.size() > 0)
				directions.erase(directions.begin());
			anim_start = offset;
		}

	}
	void scale_model_UP(float deltaTime, bool ended) {
		float new_scale;
		new_scale = start_scale + (deltaTime*0.1);
			
		glm::mat4 scale_m = glm::scale(glm::mat4(1.0f), glm::vec3(new_scale, new_scale, new_scale));
		glm::mat4 model_m = getMatrix(Right, Up, Front, Position);

		Matrix = model_m * scale_m;

		if (ended == true) scale = new_scale;
	}
	void scale_model_DOWN(float deltaTime, bool ended) {
		float new_scale;
		new_scale = start_scale - deltaTime * 0.1;

		glm::mat4 scale_m = glm::scale(glm::mat4(1.0f), glm::vec3(new_scale, new_scale, new_scale));
		glm::mat4 model_m = getMatrix(Right, Up, Front, Position);

		Matrix = model_m * scale_m;

		if (ended == true) scale = new_scale;
	}
	void bezier(float deltaTime, bool ended) {
		glm::vec2 points[3];
		points[0].x = start_Position.x; points[0].y = start_Position.y;
		points[1] = glm::vec2(start_Position.x, 5);
		points[2] = glm::vec2(start_Position.x + 3, 0);
		glm::vec2 ans = getBezierPoint(points,3, deltaTime);
		Matrix[3][0] = ans[0];
		Matrix[3][1] = ans[1];
		if (ended == true) {
			Position.x = ans.x;
			Position.y = ans.y;
		}
	}
	glm::vec2 getBezierPoint(glm::vec2* points, int numPoints, float t) {
		glm::vec2* tmp = new glm::vec2[numPoints];
		memcpy(tmp, points, numPoints * sizeof(glm::vec2));
		int i = numPoints - 1;
		while (i > 0) {
			for (int k = 0; k < i; k++)
				tmp[k] = tmp[k] + t * (tmp[k + 1] - tmp[k]);
			i--;
		}
		glm::vec2 answer = tmp[0];
		delete[] tmp;
		return answer;
	}
	glm::vec3 mybez(float t, std::vector<glm::vec3>& path, bool ended) {
		if (path.size() < 2) return glm::vec3(0, 0, 0);

		float c1, c2, c3;
		glm::vec3 p;
		bool linear = false;

		c1 = pow(1 - t, 2);
		c2 = 2 * t * (1 - t);
		c3 = pow(t, 2);

		if (path.size() > 2) { //quad
			p = c1 * path[0] + c2 * path[1] + c3 * path[2];
			
			printf("Quad\n");
		}
		else { //linear
			p = (1 - t)*path[0] + t * path[1];
			printf("Linear\n");
			linear = true;
		}
		if (ended == true) { 
			if(linear) 
				path.erase(path.begin(), path.end());
			else
				path.erase(path.begin(), path.begin() + 2);
		}

		Matrix[3][0] = p.x;
		Matrix[3][1] = p.y;
		Matrix[3][2] = p.z;

		return p;

	}
	void catmull(float deltaTime, bool ended) {
		
		std::vector<glm::vec3> cp;
		cp.push_back(start_Position);
		cp.push_back(start_Position + glm::vec3(0.1,1.2,0));
		cp.push_back(start_Position + glm::vec3(1.2, 1.4, 0));
		cp.push_back(start_Position + glm::vec3(1.3, 0, 0));

		float t = deltaTime * (mypath.size() - 1);

		glm::vec3 ans = catmull_rom_spline(mypath, t);
		if (ended == true) Position = ans;
		Matrix[3][0] = ans.x;
		Matrix[3][1] = ans.y;
		Matrix[3][2] = ans.z;
	}
	// Rotates object around point p
	void rotate_about(float deltaTime, glm::vec3 p, bool ended) {

		glm::vec3 axis = glm::vec3(0, 1, 0); //rotation axis

		glm::mat4 model = getMatrix(Right,Up,Front, glm::vec3(0,0,0));

		model = glm::translate(model, p);

		float new_angle = angleX + (6.28f * deltaTime);

		if (new_angle >= 6.28) new_angle -= 6.28f;

		model = glm::rotate(model, new_angle, axis);

		model = glm::translate(model, -p);
		model = glm::translate(model, Position);

		model = glm::scale(model, glm::vec3(scale));

		/*  look at point p  */                  /*
		glm::vec3 look_direction = Position - p;
		glm::vec3 front = glm::vec3(model[2]);
		float angle = glm::angle(front, look_direction);

		model = glm::rotate(model, - angle, axis);*/

		if (ended == true) {
			//Position = glm::vec3(Matrix[3]);
			angleX = new_angle;
		}

		
		Matrix = model;
	}

	void rotateX(float deltaTime, bool ended) {
		glm::vec3 axis;
		glm::mat4 scale_m = glm::scale(glm::mat4(1.0f), glm::vec3(scale));
		glm::mat4 rotation = getMatrix(start_Right,start_Up,start_Front,start_Position);

		axis = glm::vec3(1, 0, 0);

		rotation = glm::rotate(rotation, ROTATION_SPEED * deltaTime, axis);

		//saves vectors and position if its last iteration
		if (ended == true) updateVectors(rotation);

		Matrix = rotation * scale_m;
		return;
	}
	void rotateY(float deltaTime, bool ended) {
		glm::vec3 axis;
		glm::mat4 scale_m = glm::scale(glm::mat4(1.0f), glm::vec3(scale));
		glm::mat4 rotation = getMatrix(start_Right, start_Up, start_Front, start_Position);

		axis = glm::vec3(0, 1, 0);
		rotation = glm::rotate(rotation, ROTATION_SPEED * deltaTime, axis);

		//saves vectors and position if its last iteration
		if (ended == true) updateVectors(rotation);

		Matrix = rotation * scale_m;
		return;
	}
	void rotateZ(float deltaTime, bool ended) {
		glm::vec3 axis;
		glm::mat4 scale_m = glm::scale(glm::mat4(1.0f), glm::vec3(scale));
		glm::mat4 rotation = getMatrix(start_Right, start_Up, start_Front, start_Position);
		axis = glm::vec3(0, 0, 1);

		rotation = glm::rotate(rotation, ROTATION_SPEED * deltaTime, axis);

		//saves vectors and position if its last iteration
		if (ended == true) updateVectors(rotation);

		Matrix = rotation * scale_m;
		return;
	}

	void translate(float deltaTime, Directions dir, bool ended) {
		
		glm::mat4 scale_m = glm::scale(glm::mat4(1.0f), glm::vec3(scale, scale, scale));
		glm::mat4 model_matrix = getMatrix(start_Right, start_Up, start_Front, start_Position);
		glm::vec3 tmp = start_Position;
		
		if (dir == mFORWARD)
			tmp.z += STEP * deltaTime;
		if (dir == mBACKWARD)
			tmp.z -= STEP * deltaTime;
		if (dir == mRIGHT)
			tmp.x += STEP * deltaTime;
		if (dir == mLEFT)
			tmp.x -= STEP * deltaTime;
		if (dir == mUP)
			tmp.y += STEP * deltaTime;
		if (dir == mDOWN)
			tmp.y -= STEP * deltaTime;

		model_matrix[3][0] = tmp.x;
		model_matrix[3][1] = tmp.y;
		model_matrix[3][2] = tmp.z;

		if (ended == true) Position = tmp;

		Matrix = model_matrix * scale_m;
		return;
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
		glm::vec3 front(mat[2][0], mat[2][1], mat[3][1]);
		Front = glm::normalize(front);
		Right = glm::normalize(glm::cross(WorldUp, Front));
		Up = glm::normalize(glm::cross(Right, Front));
		Position = glm::vec3(mat[3][0], mat[3][1], mat[3][2]);
		
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

	void updateVectors(glm::mat4 m) {
		Right = glm::vec3(m[0][0], m[0][1], m[0][2]);
		Up = glm::vec3(m[1][0], m[1][1], m[1][2]);
		Front = glm::vec3(m[2][0], m[2][1], m[2][2]);
	}

    /*  Functions   */
    // constructor, expects a filepath to a 3D model.
    Model(string const &path, bool gamma = false, glm::vec3 pos = glm::vec3(0,0,0), glm::vec3 mscale = glm::vec3(1,1,1)) : gammaCorrection(gamma)
    {
        loadModel(path);
		Position = pos;
		Front = glm::vec3(0., 0., 1.);
		Up = glm::vec3(0., 1., 0.);
		Right = glm::normalize(glm::cross(Up, Front));
		Matrix = getMatrix(Right, Up, Front, Position);
		glm::mat4 scale_m = glm::scale(glm::mat4(1.0f), mscale);
		Matrix = Matrix * scale_m;
    }
	Model(string const &path,glm::vec3 pos = glm::vec3(0, 0, 0), glm::vec3 mscale = glm::vec3(1, 1, 1))
	{
		loadModel(path);
		Position = pos;
		Front = glm::vec3(0., 0., 1.);
		Up = glm::vec3(0., 1., 0.);
		Right = glm::normalize(glm::cross(Up, Front));
		scale = mscale.x;
		Matrix = getMatrix(Right, Up, Front, Position);
		glm::mat4 scale_m = glm::scale(glm::mat4(1.0f), mscale);
		Matrix = Matrix * scale_m;
		mypath.push_back(glm::vec3(0, 0, 3));
		mypath.push_back(glm::vec3(3, 3, 0));
		mypath.push_back(glm::vec3(0, 0, -3));
		mypath.push_back(glm::vec3(-3, -3, 0));
		mypath.push_back(glm::vec3(0, 0, 3));
	}

    // draws the model, and thus all its meshes
    void Draw(Shader shader)
    {
        for(unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].Draw(shader);
    }
    
private:
    /*  Functions   */
    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void loadModel(string const &path)
    {
        // read file via ASSIMP
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
        // check for errors
        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
        {
            cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
            return;
        }
        // retrieve the directory path of the filepath
        directory = path.substr(0, path.find_last_of('/'));

        // process ASSIMP's root node recursively
        processNode(scene->mRootNode, scene);
    }

    // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void processNode(aiNode *node, const aiScene *scene)
    {
        // process each mesh located at the current node
        for(unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            // the node object only contains indices to index the actual objects in the scene. 
            // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene));
        }
        // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
        for(unsigned int i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene);
        }

    }

    Mesh processMesh(aiMesh *mesh, const aiScene *scene)
    {
        // data to fill
        vector<Vertex> vertices;
        vector<unsigned int> indices;
        vector<Texture> textures;

        // Walk through each of the mesh's vertices
        for(unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
            glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
            // positions
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;
            // normals
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;
            // texture coordinates
            if(mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
            {
                glm::vec2 vec;
                // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
                // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
                vec.x = mesh->mTextureCoords[0][i].x; 
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;
            }
            else
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);
            // tangent
            vector.x = mesh->mTangents[i].x;
            vector.y = mesh->mTangents[i].y;
            vector.z = mesh->mTangents[i].z;
            vertex.Tangent = vector;
            // bitangent
            vector.x = mesh->mBitangents[i].x;
            vector.y = mesh->mBitangents[i].y;
            vector.z = mesh->mBitangents[i].z;
            vertex.Bitangent = vector;
            vertices.push_back(vertex);
        }
        // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
        for(unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            // retrieve all indices of the face and store them in the indices vector
            for(unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }
        // process materials
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];    
        // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
        // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
        // Same applies to other texture as the following list summarizes:
        // diffuse: texture_diffuseN
        // specular: texture_specularN
        // normal: texture_normalN

        // 1. diffuse maps
        vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        // 2. specular maps
        vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        // 3. normal maps
        std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        // 4. height maps
        std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
        
        // return a mesh object created from the extracted mesh data
        return Mesh(vertices, indices, textures);
    }

    // checks all material textures of a given type and loads the textures if they're not loaded yet.
    // the required info is returned as a Texture struct.
    vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName)
    {
        vector<Texture> textures;
        for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);
            // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
            bool skip = false;
            for(unsigned int j = 0; j < textures_loaded.size(); j++)
            {
                if(std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
                {
                    textures.push_back(textures_loaded[j]);
                    skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                    break;
                }
            }
            if(!skip)
            {   // if texture hasn't been loaded already, load it
                Texture texture;
                texture.id = TextureFromFile(str.C_Str(), this->directory);
                texture.type = typeName;
                texture.path = str.C_Str();
                textures.push_back(texture);
                textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
            }
        }
        return textures;
    }
};


unsigned int TextureFromFile(const char *path, const string &directory, bool gamma)
{
    string filename = string(path);
    filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

#endif
