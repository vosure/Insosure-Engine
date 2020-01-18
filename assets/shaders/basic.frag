#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

uniform vec3 CustomColor;

void main()
{
    FragColor = vec4(CustomColor, 1.0);

	float Brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0222));
    if (Brightness > 1.0)
        BrightColor = vec4(FragColor.rgb, 1.0);
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 0.0);
}