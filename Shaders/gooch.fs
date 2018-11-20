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
	// init consts and vectors

	vec3 normalVec = normalize(Normal);
	vec3 lightVec = normalize( light.position - FragPos);
	vec3 cameraDir = normalize( viewPos - FragPos );
	float DiffuseCool = 0.3;
	float DiffuseWarm = 0.3;
	vec3 Cool = vec3(0, 0, 0.6);
	vec3 Warm = vec3(0.6, 0, 0);
	float NdotL = (dot(lightVec, normalVec) + 1.0) * 0.5;

	vec3 vColor = texture(texture_diffuse1, TexCoords).rgb;
	vec3 kcool = min(Cool + DiffuseCool * vColor, 1.0);
	vec3 kwarm = min(Warm + DiffuseWarm * vColor, 1.0);
	vec3 kfinal = mix(kcool, kwarm, NdotL);
    
	vec3 ReflectVec = normalize(reflect(-light.position, normalVec));
	vec3 nRefl = normalize(ReflectVec);
	vec3 nview = cameraDir;
	float spec = pow(max(dot(nRefl, nview), 0.0), 32.0);

	vec4 result;
	if (gl_FrontFacing) {
		result = vec4(min(kfinal + spec, 1.0), 1.0);
	} else {
		result = vec4(0, 0, 0, 1);
	}
	FragColor = result;
}


/*
void main()
{   
	// init consts and vectors

	vec3 normalVec = normalize(Normal);
	vec3 lightVec = normalize( light.position - FragPos);
	vec3 cameraDir = normalize( viewPos - FragPos );
	float DiffuseCool = 0.3;
	float DiffuseWarm = 0.3;
	vec3 Cool = vec3(0, 0, 0.6);
	vec3 Warm = vec3(0.6, 0, 0);
	float NdotL = (dot(lightVec, normalVec) + 1.0) * 0.5;

	vec3 vColor = texture(texture_diffuse1, TexCoords).rgb;
	vec3 kcool = min(Cool + DiffuseCool * vColor, 1.0);
	vec3 kwarm = min(Warm + DiffuseWarm * vColor, 1.0);
	vec3 kfinal = mix(kcool, kwarm, NdotL);
    
	vec3 ReflectVec = normalize(reflect(-light.position, normalVec));
	vec3 nRefl = normalize(ReflectVec);
	vec3 nview = cameraDir;
	float spec = pow(max(dot(nRefl, nview), 0.0), 32.0);

	vec4 result;
	if (gl_FrontFacing) {
		result = vec4(min(kfinal + spec, 1.0), 1.0);
	} else {
		result = vec4(0, 0, 0, 1);
	}
	FragColor = result;
}
*/

/*  IMP 2
	// normalizes lighting vectors
	vec3 normalVec = normalize(Normal);
	vec3 lightVec = normalize( light.position - FragPos);
	vec4 mesh_color = texture(texture_diffuse1, TexCoords).rgba;
	// calculates diffuse lighting
	float diffuse = dot( normalVec,lightVec);
	// convert intensity of light from [-1,1] to [0,1]
	float intensity = (1.0f + diffuse) / 2;

	vec3 coolColor = vec3(159.0f/255, 148.0f/255, 255.0f/255) + vec3(1,1,1) * 0.25;

	vec3 warmColor = vec3(255.0f/255, 75.0f/255, 75.0f/255); + vec3(1,1,1) * 0.5;

	vec3 colorOut = mix(coolColor, warmColor, intensity);


	FragColor.rgb = colorOut;
	FragColor.a = 1;
*/

/*  IMP 1
	// Calculates warm & cold colors
	float kd = 1, a = 0.2, b = 0.6;
	vec3 normal = normalize(Normal);
	vec3 lightDir = normalize(light.position - FragPos);
    vec3 cameraDir = normalize( viewPos - FragPos );
	vec4 mesh_color = texture(texture_diffuse1, TexCoords).rgba;
    
    
	float NL = dot(normalize(normal), normalize(lightDir));

	float it = ((1 + NL) / 2);

	vec3 color = (1-it) * (vec3(0,0,0.4) + a * mesh_color.xyz)
				  + it * (vec3(0.4,0.4,0) + b * mesh_color.xyz);

    //FragColor = vec4(color, 1.0f);
    
    // Adds highlights
    vec3 R = reflect( - normalize(lightDir),  normalize(normal) );
    float ER = clamp( dot( normalize(cameraDir), normalize(R) ), 0, 1 );
    vec4 spec = vec4(1) * pow(ER, material.shininess);
    
    FragColor = vec4(color + spec.xyz, mesh_color.a);
*/


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