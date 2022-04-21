// Vertex shader
#version 420

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTextureCoordinates;

uniform mat4 matModel;
uniform mat4 matView;
uniform mat4 matProjection;

uniform sampler2D heightMapTexture;
uniform vec3 heightMapUVOffsetRotation;
uniform float heightMapScale;

void main()
{	
	// Apply transforms to this vertex
	mat4 MVP = matProjection * matView * matModel;
	
	vec4 vertexPos = vec4(vPosition, 1.0f);
	
	vec2 sampleUV = vTextureCoordinates.xy + heightMapUVOffsetRotation.xy;
	float rotationOffset = heightMapUVOffsetRotation.z;
	sampleUV.xy -= vec2(0.5f, 0.5f); // Go back to the origin
		
		
	mat2 matRotate2D = mat2( vec2( cos(rotationOffset), sin(rotationOffset) ), 	// 1st column
					  vec2( -sin(rotationOffset), cos(rotationOffset) ) );	// 2nd column
	sampleUV.xy = matRotate2D * sampleUV.xy;
	sampleUV.xy += vec2(0.5f, 0.5f);
		
	float heightSample = texture(heightMapTexture, sampleUV.xy ).r;
	vertexPos.y += (heightSample * heightMapScale);
	
	
    gl_Position = MVP * vertexPos;	
};
