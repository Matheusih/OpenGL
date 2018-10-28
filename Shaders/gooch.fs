#version 330 core

out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Material{
	vec3 specular;
	float shininess;
};

uniform vec3 viewPos;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
//uniform sampler2D texture_normal1;
uniform Light light;
uniform Material material;

// Gooch Shading
void main()
{   
	// Calculates warm & cold colors
	float kd = 1, a = 0.2, b = 0.6;
	vec3 normal = normalize(Normal);
	vec3 lightDir = normalize(light.position - FragPos);
    vec3 cameraDir = normalize( FragPos - viewPos );
	vec4 mesh_color = texture(texture_diffuse1, TexCoords).rgba;
    
    
	float NL = dot(normal, lightDir);

	float it = ((1 + NL) / 2);

	vec3 color = (1-it) * (vec3(0,0,0.4) + a * vec3(mesh_color))
				  + it * (vec3(0.4,0.4,0) + b * vec3(mesh_color));

    //FragColor = vec4(color, 1.0f);
    
    // Adds highlights
    vec3 R = reflect( -lightDir, normal );
    float ER = clamp( dot( cameraDir, normalize(R) ), 0, 1 );
    vec4 spec = vec4(1.0f) * pow(ER, material.shininess);
    
    FragColor = vec4(color + spec.xyz, mesh_color.a);
    
}




/*   // Basic lighting
void main()
{   
	// ambient
	vec3 ambient = light.ambient * texture(texture_diffuse1, TexCoords).rgb;
	
	// diffuse
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(light.position - FragPos);
	float diff = max(dot(lightDir, viewPos), 0.0f);
	vec3 diffuse = light.diffuse * diff * texture(texture_diffuse1, TexCoords).rgb;

	// specular
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specular * (spec * texture_specular1); 

	vec3 result = ambient + diffuse + specular;

    FragColor = vec4(result, 1.0f);
}
*/