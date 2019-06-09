#version 330 core
in vec3 FragPos;
in vec2 Texcoord;
in vec3 Normal;
in vec3 Color;
in vec4 FragPosLightSpace;

struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform bool receiveShadow;
uniform vec3 cameraPos;
uniform DirLight dirLight;
uniform sampler2D image;
uniform sampler2D depthMap;

out vec4 fragColor;

float shadowCalculation(vec4 fragPosLightSpace, float bias);

void main(){
	// diffuse texture.
	vec3 color = texture(image,Texcoord).rgb;
    // ambient
    vec3 ambient = dirLight.ambient * color;
    // diffuse
    vec3 lightDir = dirLight.direction;
    vec3 normal = normalize(Normal);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * dirLight.diffuse * color;
    // specular
    vec3 viewDir = normalize(cameraPos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 8.0);

    vec3 specular = dirLight.specular * color * spec;

	// shadow
	//ambient = vec3(0.15);
	if(receiveShadow)
	{
		float shadow = 1.0f - shadowCalculation(FragPosLightSpace, 0.0f);
		fragColor = vec4(ambient + shadow * (diffuse + specular), 1.0f);
	}
	else
	{
		fragColor = vec4(ambient + diffuse + specular, 1.0f);
	}
	
	//fragColor = clamp(fragColor, vec4(0.1), vec4(1));
	// gamma correction.
	const float gamma = 2.2f;
	fragColor.rgb = pow(fragColor.rgb,vec3(1.0f/gamma));
}

const vec2 poissonDisk[16] = vec2[]( 
   vec2( -0.94201624, -0.39906216 ), 
   vec2( 0.94558609, -0.76890725 ), 
   vec2( -0.094184101, -0.92938870 ), 
   vec2( 0.34495938, 0.29387760 ), 
   vec2( -0.91588581, 0.45771432 ), 
   vec2( -0.81544232, -0.87912464 ), 
   vec2( -0.38277543, 0.27676845 ), 
   vec2( 0.97484398, 0.75648379 ), 
   vec2( 0.44323325, -0.97511554 ), 
   vec2( 0.53742981, -0.47373420 ), 
   vec2( -0.26496911, -0.41893023 ), 
   vec2( 0.79197514, 0.19090188 ), 
   vec2( -0.24188840, 0.99706507 ), 
   vec2( -0.81409955, 0.91437590 ), 
   vec2( 0.19984126, 0.78641367 ), 
   vec2( 0.14383161, -0.14100790 ) 
);

float random(vec3 seed, int i){
	vec4 seed4 = vec4(seed,i);
	float dot_product = dot(seed4, vec4(12.9898, 78.233, 45.164, 94.673));
	return fract(sin(dot_product) * 43758.5453);
}

float shadowCalculation(vec4 fragPosLightSpace, float bias)
{
	// perspective division.
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	projCoords = projCoords * 0.5 + 0.5;

    if(projCoords.z > 1.0)
        return 1.0f;

	// poision sampling.
	/*
	int randround = 4;
	float shadowFactor = 1.0;
	vec2 texelSize = 1.0 / textureSize(depthMap, 0);
	float roundmag = 1.0 / float(randround);
	for (int i = 0; i < randround; i++) {
		int index = int(16.0 * random(gl_FragCoord.xyy, i)) % 16;
		float factor = texture(depthMap, projCoords.xy + poissonDisk[index] * texelSize).r > (projCoords.z - bias) ? 1.0 : 0.0;
		shadowFactor -= roundmag * (1.0 - factor);
	}*/
	float shadowFactor = 0.0f;
	float currentDepth = projCoords.z;
	vec2 texelSize = 1.0 / textureSize(depthMap, 0);
	for(int x = -2; x <= 2; ++x)
	{
    		for(int y = -2; y <= 2; ++y)
    		{
        		float pcfDepth = texture(depthMap, projCoords.xy + vec2(x, y) * texelSize).r; 
        		shadowFactor += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
    		}    
	}
	shadowFactor /= 16.0;
	return shadowFactor;
}