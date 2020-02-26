#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

uniform mat4 Transform;
uniform mat4 ViewProjection;

out vec2 FragPos;
out vec2 TexCoord;
out mat3 TBN;

void main()
{
	gl_Position = ViewProjection * Transform * vec4(aPos, 1.0);
	TexCoord = aTexCoords;
	FragPos = vec2(Transform * vec4(aPos, 1.0));

	vec3 T = normalize(vec3(Transform * vec4(aTangent,   0.0)));
 	vec3 N = normalize(vec3(Transform * vec4(aNormal,    0.0)));
	T = normalize(T - dot(T, N) * N);
    vec3 B = normalize(vec3(Transform * vec4(aBitangent, 0.0)));
    TBN = transpose(mat3(T, B, N));
}