// Vertex shader
#version 420

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTextureCoordinates;
layout (location = 3) in vec3 vBiNormal;
layout (location = 4) in vec3 vTangent;

uniform mat4 matModel;
uniform mat4 matView;
uniform mat4 matProjection;
uniform mat4 matModelInverseTranspose;	// For normal calculation
	
out vec4 fVertWorldLocation;
out vec4 fNormal;
out vec2 fTextureCoordinates;

uniform sampler2D heightMapTexture;
uniform bool useHeightMap;
uniform vec3 heightMapUVOffsetRotation;
uniform float heightMapScale;

out float heightMapVertex; // For our transform feedback

void main()
{	
	// Apply transforms to this vertex
	mat4 MVP = matProjection * matView * matModel;
	
	vec4 vertexPos = vec4(vPosition, 1.0f);
	if(useHeightMap)
	{
		vec2 sampleUV = vTextureCoordinates.xy + heightMapUVOffsetRotation.xy;
		float rotationOffset = heightMapUVOffsetRotation.z;
		sampleUV.xy -= vec2(0.5f, 0.5f); // Go back to the origin
		
		mat2 matRotate2D = mat2( vec2( cos(rotationOffset), sin(rotationOffset) ), 	// 1st column
					  vec2( -sin(rotationOffset), cos(rotationOffset) ) );	// 2nd column
		sampleUV.xy = matRotate2D * sampleUV.xy;
		sampleUV.xy += vec2(0.5f, 0.5f);
		
		float heightSample = texture(heightMapTexture, sampleUV.xy ).r;
		vertexPos.y += (heightSample * heightMapScale);
		//heightMapVertex = vertexPos.y;
	}
	
    gl_Position = MVP * vertexPos;	
	
	// Make sure we pass the texture coord so the fragment shader can sample it
	fTextureCoordinates = vTextureCoordinates;
	
	// The location of the vertex in "world" space (not screen space)
	fVertWorldLocation = matModel * vec4(vPosition, 1.0f);
	
	// Calculate the normal based on any rotation we've applied.
	// This inverse transpose removes scaling and translation (movement) 
	// 	from the matrix.
	fNormal = matModelInverseTranspose * normalize(vec4(vNormal, 1.0f));
	fNormal = normalize(fNormal); // Make sure normal is a unit vector
};
