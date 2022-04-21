#include "AABB.h"

#include "GLCommon.h"

#include <algorithm>

AABB::AABB(const glm::vec3& min, const glm::vec3 max) : 
	min(min), 
	max(max)
{
//	this->vertexArray->Bind();
//	float* vertices = this->GetVertices();
//	this->vertexBuffer = CreateRef<VertexBuffer>(vertices, 72 * sizeof(float));
//	this->vertexArray->AddVertexBuffer(this->vertexBuffer);
//
//	uint32_t* indices = new uint32_t[12];
//	for (int i = 0; i < 12; i++)
//	{
//		indices[i] = i;
//	}
//
//	this->indexBuffer = CreateRef<IndexBuffer>(indices, 12);
//	this->vertexArray->SetIndexBuffer(this->indexBuffer);
//	
//	delete vertices;
//	delete indices;
}

AABB::~AABB()
{

}

void AABB::Draw(Ref<Shader> shader, const glm::mat4& transform) const
{
	shader->Bind();
	shader->SetMat4x4("matModel", transform);
	shader->SetMat4x4("matModelInverseTranspose", glm::inverse(transform));

	shader->SetFloat("isOverrideColor", (float) GL_TRUE);
	shader->SetFloat("isIgnoreLighting", (float)GL_TRUE);
	shader->SetFloat4("colorOverride", glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));

	glLineWidth(2.0f);
	glBegin(GL_LINES);

	glVertex3f(this->min.x, this->min.y, this->min.z);
	glVertex3f(this->min.x, this->min.y, this->max.z);

	glVertex3f(this->min.x, this->min.y, this->min.z);
	glVertex3f(this->max.x, this->min.y, this->min.z);

	glVertex3f(this->max.x, this->min.y, this->min.z);
	glVertex3f(this->max.x, this->min.y, this->max.z);

	glVertex3f(this->min.x, this->min.y, this->max.z);
	glVertex3f(this->max.x, this->min.y, this->max.z);

	// Draw connecting lines up
	glVertex3f(this->min.x, this->min.y, this->min.z);
	glVertex3f(this->min.x, this->max.y, this->min.z);

	glVertex3f(this->max.x, this->min.y, this->min.z);
	glVertex3f(this->max.x, this->max.y, this->min.z);

	glVertex3f(this->max.x, this->min.y, this->max.z);
	glVertex3f(this->max.x, this->max.y, this->max.z);

	glVertex3f(this->min.x, this->min.y, this->max.z);
	glVertex3f(this->min.x, this->max.y, this->max.z);

	// Draw top square
	glVertex3f(this->min.x, this->max.y, this->min.z);
	glVertex3f(this->min.x, this->max.y, this->max.z);

	glVertex3f(this->min.x, this->max.y, this->min.z);
	glVertex3f(this->max.x, this->max.y, this->min.z);

	glVertex3f(this->max.x, this->max.y, this->min.z);
	glVertex3f(this->max.x, this->max.y, this->max.z);

	glVertex3f(this->min.x, this->max.y, this->max.z);
	glVertex3f(this->max.x, this->max.y, this->max.z);

	glEnd();

	shader->SetFloat("isOverrideColor", (float)GL_FALSE);
	shader->SetFloat("isIgnoreLighting", (float)GL_FALSE);
}

float* AABB::GetVertices() const
{
	float* vertexData = new float[72];

	// Draw bottom square
	vertexData[0] = min.x;
	vertexData[1] = min.y;
	vertexData[2] = min.z;
	vertexData[3] = min.x;
	vertexData[4] = min.y;
	vertexData[5] = max.z;

	vertexData[6] = min.x;
	vertexData[7] = min.y;
	vertexData[8] = min.z;
	vertexData[9] = max.x;
	vertexData[10] = min.y;
	vertexData[11] = min.z;

	vertexData[12] = max.x;
	vertexData[13] = min.y;
	vertexData[14] = min.z;
	vertexData[15] = max.x;
	vertexData[16] = min.y;
	vertexData[17] = max.z;

	vertexData[18] = min.x;
	vertexData[19] = min.y;
	vertexData[20] = max.z;
	vertexData[21] = max.x;
	vertexData[22] = min.y;
	vertexData[23] = max.z;

	// Draw connecting lines up
	vertexData[24] = min.x;
	vertexData[25] = min.y;
	vertexData[26] = min.z;
	vertexData[27] = min.x;
	vertexData[28] = max.y;
	vertexData[29] = min.z;

	vertexData[30] = max.x;
	vertexData[31] = min.y;
	vertexData[32] = min.z;
	vertexData[33] = max.x;
	vertexData[34] = max.y;
	vertexData[35] = min.z;

	vertexData[36] = max.x;
	vertexData[37] = min.y;
	vertexData[38] = max.z;
	vertexData[39] = max.x;
	vertexData[40] = max.y;
	vertexData[41] = max.z;

	vertexData[42] = min.x;
	vertexData[43] = min.y;
	vertexData[44] = max.z;
	vertexData[45] = min.x;
	vertexData[46] = max.y;
	vertexData[47] = max.z;

	// Draw top square
	vertexData[48] = min.x;
	vertexData[49] = max.y;
	vertexData[50] = min.z;
	vertexData[51] = min.x;
	vertexData[52] = max.y;
	vertexData[53] = max.z;

	vertexData[54] = min.x;
	vertexData[55] = max.y;
	vertexData[56] = min.z;
	vertexData[57] = max.x;
	vertexData[58] = max.y;
	vertexData[59] = min.z;

	vertexData[60] = max.x;
	vertexData[61] = max.y;
	vertexData[62] = min.z;
	vertexData[63] = max.x;
	vertexData[64] = max.y;
	vertexData[65] = max.z;

	vertexData[66] = min.x;
	vertexData[67] = max.y;
	vertexData[68] = max.z;
	vertexData[69] = max.x;
	vertexData[70] = max.y;
	vertexData[71] = max.z;
	return vertexData;
}

bool AABB::IsInside(const glm::mat4& transform, const glm::vec3& position)
{
	glm::vec4 transformedMin = transform * glm::vec4(this->min, 1.0f);
	glm::vec4 transformedMax = transform * glm::vec4(this->max, 1.0f);
	return std::min(transformedMax.x, std::max(transformedMin.x, position.x)) == position.x
		&& std::min(transformedMax.y, std::max(transformedMin.y, position.y)) == position.y
		&& std::min(transformedMax.z, std::max(transformedMin.z, position.z)) == position.z;
}