// Fragment shader
#version 420

in vec2 fTextureCoordinates;
in vec4 fNormal;
in vec4 fVertWorldLocation;

out vec4 Color;

uniform float alphaTransparency;

uniform sampler2D texture0;
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D texture3;
uniform sampler2D texture4;
uniform sampler2D texture5;
uniform sampler2D texture6;
uniform sampler2D texture7;
uniform vec2 textureRatioScale0;
uniform vec2 textureRatioScale1;
uniform vec2 textureRatioScale2;
uniform vec2 textureRatioScale3;
uniform vec2 textureRatioScale4;
uniform vec2 textureRatioScale5;
uniform vec2 textureRatioScale6;
uniform vec2 textureRatioScale7;

uniform samplerCube cubeMap0;
uniform samplerCube cubeMap1;
uniform samplerCube cubeMap2;
uniform samplerCube cubeMap3;
uniform vec4 cubeMapRatios;
uniform bool isSkyBox;

uniform vec4 cameraPosition;
uniform bool isIgnoreLighting;
uniform bool isOverrideColor;
uniform vec4 colorOverride;

uniform sampler2D discardTexture;
uniform bool isDiscardTexture;

uniform sampler2D forSomeReasonOtherNamesDontWork;
uniform bool isAlphaTexture;
uniform float aTexScale;

struct sLight
{
	vec4 position;			
	vec4 diffuse;	
	vec4 specular;	// rgb = highlight colour, w = power
	vec4 attenuation; // x = constant, y = linear, z = quadratic, w = DistanceCutOff
	vec4 direction;	// Spot, directional lights
	vec4 param1;	// x = lightType, y = inner angle, z = outer angle, w = TBD
	                // 0 = pointlight
					// 1 = spot light
					// 2 = directional light
	vec4 param2;	// x = 0 for off, 1 for on
};

const int POINT_LIGHT_TYPE = 0;
const int SPOT_LIGHT_TYPE = 1;
const int DIRECTIONAL_LIGHT_TYPE = 2;

const int MAX_LIGHTS = 10;
uniform sLight lightArray[MAX_LIGHTS];

vec4 calculateLightContrib(vec4 vertexColor, vec3 vertexNormal, vec3 vertexWorldPos, vec4 vertexSpecular);

void main()
{	
	if(isSkyBox)
	{
		if ( cubeMapRatios.x > 0.0f )
		{
			Color.rgb += texture( cubeMap0, fNormal.xyz ).rgb * cubeMapRatios.x;
		}
		if ( cubeMapRatios.y > 0.0f )
		{
			Color.rgb += texture( cubeMap1, fNormal.xyz ).rgb * cubeMapRatios.y;
		}
		if ( cubeMapRatios.z > 0.0f )
		{
			Color.rgb += texture( cubeMap2, fNormal.xyz ).rgb * cubeMapRatios.z;
		}
		if ( cubeMapRatios.w > 0.0f )
		{
			Color.rgb += texture( cubeMap3, fNormal.xyz ).rgb * cubeMapRatios.w;
		}

		return;
	}
	
	if(isDiscardTexture)
	{
		vec3 discardSample = texture(discardTexture, fTextureCoordinates.xy).rgb;
		float discardSampleValue = (discardSample.r + discardSample.g + discardSample.b);
		if(discardSampleValue <= 0.0f)
		{
			discard;
		}
	}
	
	vec4 diffuse = vec4(1.0f, 1.0f, 1.0f, alphaTransparency);
	
	if(isOverrideColor)
	{
		diffuse = colorOverride;
	}
	else
	{
		diffuse = texture(texture0, fTextureCoordinates * textureRatioScale0.y) * textureRatioScale0.x;
		diffuse += texture(texture1, fTextureCoordinates * textureRatioScale1.y) * textureRatioScale1.x;	
		diffuse += texture(texture2, fTextureCoordinates * textureRatioScale2.y) * textureRatioScale2.x;
		diffuse += texture(texture3, fTextureCoordinates * textureRatioScale3.y) * textureRatioScale3.x;
		diffuse += texture(texture4, fTextureCoordinates * textureRatioScale4.y) * textureRatioScale4.x;
		diffuse += texture(texture5, fTextureCoordinates * textureRatioScale5.y) * textureRatioScale5.x;	
		diffuse += texture(texture6, fTextureCoordinates * textureRatioScale6.y) * textureRatioScale6.x;
		diffuse += texture(texture7, fTextureCoordinates * textureRatioScale7.y) * textureRatioScale7.x;

		diffuse.a = alphaTransparency;
	}
	
	if(isAlphaTexture)
	{
		vec3 alphaSample = texture(forSomeReasonOtherNamesDontWork, fTextureCoordinates.xy * aTexScale).rgb;
		float alphaSampleValue = (alphaSample.r + alphaSample.g + alphaSample.b);
		if(alphaSampleValue <= 0.2f)
		{
			diffuse.a = alphaSampleValue * 5.0f;
		}
	}
	
	if(isIgnoreLighting)
	{
		Color = diffuse;
		return;
	}
	
	vec4 specularColor = vec4(1.0f, 1.0f, 1.0f, 1.0f); // White
	Color = calculateLightContrib(diffuse, fNormal.xyz, fVertWorldLocation.xyz, specularColor.rgba);
}

vec4 calculateLightContrib(vec4 vertexColor, vec3 vertexNormal, vec3 vertexWorldPos, vec4 vertexSpecular)
{
	vec3 normal = normalize(vertexNormal);
	vec4 vertexColorResult = vec4(vertexColor.x * 0.1f, vertexColor.y * 0.1f, vertexColor.z * 0.1f, vertexColor.a);
	
	for(int i = 0; i < MAX_LIGHTS; i++)
	{
		if(lightArray[i].param2.x == 0.0f) // Light is off, do nothing
		{
			continue;
		}
		
		int lightTypeInt = int(lightArray[i].param1.x);
		if(lightTypeInt == DIRECTIONAL_LIGHT_TYPE) // Do directional light first because it has no attenuation
		{
			vec3 lightContrib = lightArray[i].diffuse.rgb;
			
			float dotProduct = dot(-lightArray[i].direction.xyz, normalize(normal.xyz)); // -1 to 1

			dotProduct = max(0.0f, dotProduct);
			
			lightContrib *= dotProduct;
			
			vertexColorResult.rgb += (vertexColor.rgb * lightArray[i].diffuse.rgb * lightContrib);
			
			continue;
		}
		
		vec3 lightDirection = lightArray[i].position.xyz - vertexWorldPos.xyz;
		float lightDistance = length(lightDirection);
		lightDirection = normalize(lightDirection);
		float dotProduct = dot(lightDirection, vertexNormal.xyz);
		dotProduct = max(0.0f, dotProduct);
		
		vec3 lightDiffuseContrib = dotProduct * lightArray[i].diffuse.rgb;
		
		vec3 reflectVector = reflect(-lightDirection, normalize(normal.xyz));
		vec3 cameraVector = normalize(cameraPosition.xyz - vertexWorldPos.xyz);
		
		vec3 lightSpecularContrib = pow(max(0.0f, dot(cameraVector, reflectVector)), vertexSpecular.w) * lightArray[i].specular.rgb;
		
		// Attenuation
		float attenuation = 1.0f / (lightArray[i].attenuation.x + lightArray[i].attenuation.y * lightDistance + lightArray[i].attenuation.z * lightDistance * lightDistance);
		
		lightDiffuseContrib *= attenuation;
		lightSpecularContrib *= attenuation;
		
		if(lightTypeInt == SPOT_LIGHT_TYPE)
		{
			vec3 directionToLight = normalize(vertexWorldPos.xyz - lightArray[i].position.xyz);
			
			float currentLightRayAngle = max(0.0f, dot(directionToLight.xyz, lightArray[i].direction.xyz));
			
			float outerConeAngleCos = cos(radians(lightArray[i].param1.z));
			float innerConeAngleCos = cos(radians(lightArray[i].param1.y));
			
			if(currentLightRayAngle < outerConeAngleCos) // We aren't in the spot light
			{
				lightDiffuseContrib = vec3(0.0f, 0.0f, 0.0f);
				lightSpecularContrib = vec3(0.0f, 0.0f, 0.0f);
			}
			else if(currentLightRayAngle < innerConeAngleCos) // Angle is between the inner and outer cone
			{
				float penumbraRatio = (currentLightRayAngle - outerConeAngleCos) / (innerConeAngleCos - outerConeAngleCos);
				
				lightDiffuseContrib *= penumbraRatio;
				lightSpecularContrib *= penumbraRatio;
			}
		}
		
		vertexColorResult.rgb += (vertexColor.rgb * lightDiffuseContrib.rgb) + (vertexSpecular.rgb * lightSpecularContrib.rgb);
	}

	return vertexColorResult;
}

// LIGHTING NOTES
// Directional Lights have no position (only direction) and are the cheapest to calculate