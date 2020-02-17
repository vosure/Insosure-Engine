#version 330 core
layout (location = 0) in vec2 aPosition;
layout (location = 1) in vec2 aTexCoord;

uniform mat4 Transform;
uniform mat4 ViewProjection;

out vec2 FragPos;
out vec2 TexCoord;

void main()
{
	gl_Position = ViewProjection * Transform * vec4(aPosition, 0.0, 1.0);
	TexCoord = aTexCoord;
	FragPos = vec2(Transform * vec4(aPosition, 0.0, 1.0));

	//Normal = mat3(transpose(inverse(Transform))) * vec3(0, 0, 1);
}