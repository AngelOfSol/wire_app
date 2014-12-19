#include "shader_data.h"


namespace shaders
{
	namespace tool_path
	{
		std::string vertex =
			R"(
#version 330

layout (location = 0) in vec2 position;

uniform mat4 mvp;

void main()
{
	gl_Position = mvp * vec4(position, 0, 1);
}
)";
		std::string fragment =
			R"(
#version 330

void main(void) 
{
	gl_FragColor = vec4(0.6, 0.2, 0.6, 1);
}
)";

	}

	namespace tip_model
	{
		std::string vertex =
			R"(
#version 330

out vec4 geo_model_position;

void main()
{
	gl_Position = vec4(0, 0, 0, 1);
}
)";
		std::string geometry = R"(

#version 330

uniform mat4 mvp;
uniform mat4 model;
uniform mat4 vp;

layout(points) in;
layout(triangle_strip, max_vertices = 3) out;

void main() 
{
	vec4 pos = gl_in[0].gl_Position;

	gl_Position = mvp * pos;
	EmitVertex();

	gl_Position = vp * (model * pos + vec4(-0.5, 1, 0, 0));
	EmitVertex();

	gl_Position = vp * (model * pos + vec4(0.5, 1, 0, 0));
	EmitVertex();

		
	EndPrimitive();
}
)";
		std::string fragment =
			R"(
#version 330

void main(void) 
{
	gl_FragColor = vec4(0.8, 0.2, 0.2, 1);
}
)";
	}







namespace slice_model
{
	std::string vertex =
		R"(
#version 330

layout (location = 0) in vec3 position;

uniform mat4 mvp;
uniform mat4 model;
uniform int half_way;
out vec4 geo_model_position;
out int switch_order;

void main()
{
	gl_Position = mvp * vec4(position, 1);
	geo_model_position = model * vec4( position, 1);
	switch_order = (gl_VertexID >= half_way) ? 1 : 0;
}
)";
	std::string geometry = R"(

#version 330

in vec4 geo_model_position[];
in int switch_order[];
out vec3 frag_normal;
out vec4 frag_model_position;


layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

void main() {
	
	vec4 pos0 = gl_in[0].gl_Position;
	vec4 pos1 = gl_in[1].gl_Position;
	vec4 pos2 = gl_in[2].gl_Position;

	vec3 normal = cross( geo_model_position[1].xyz - geo_model_position[2].xyz, geo_model_position[0].xyz - geo_model_position[1].xyz);

	if(switch_order[0] == 1)
	{
		normal = cross( geo_model_position[0].xyz - geo_model_position[1].xyz, geo_model_position[1].xyz - geo_model_position[2].xyz);
	}
	
	gl_Position = pos0;
	frag_normal = normal;
	frag_model_position = geo_model_position[0];
	EmitVertex();

	gl_Position = pos1;
	frag_normal = normal;
	frag_model_position = geo_model_position[1];
	EmitVertex();

	gl_Position = pos2;
	frag_normal = normal;
	frag_model_position = geo_model_position[2];
	EmitVertex();

		
	EndPrimitive();
}
)";
	std::string fragment =
		R"(
#version 330

in vec3 frag_normal;
in vec4 frag_model_position;

uniform vec4 color;

void main(void) 
{
	vec4 diff = normalize(vec4(-1, -1, -1, 1));
	float diffuse = 0.2;
	diffuse += clamp(dot(normalize(frag_normal), normalize(diff.xyz)), 0.0, 1.0) * 0.5;

	diff = normalize(vec4(1, 1, 1, 1));

	diffuse += clamp(dot(normalize(frag_normal), normalize(diff.xyz)), 0.0, 1.0);

	gl_FragColor =  diffuse * color;
}
)";
}






namespace text
{
	std::string vertex = 
R"(
#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;

out vec2 gTexCoord;

void main()
{
	gl_Position = vec4(position, 1);
	gTexCoord = texCoord;
}
)";
	std::string geometry =
R"(
#version 330

in vec2 gTexCoord [];

out vec2 fTexCoord;

uniform float textWidth;
uniform float textHeight;
uniform float textureWidth;
uniform float textureHeight;

uniform mat4 transformMat;

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

void main() {
	gl_Position = transformMat * (gl_in[0].gl_Position + vec4(textWidth, textHeight, 0, 0));
	fTexCoord = gTexCoord[0] + vec2(textureWidth, textureHeight);
	EmitVertex();

	gl_Position = transformMat * (gl_in[0].gl_Position + vec4(0, textHeight, 0, 0));
	fTexCoord = gTexCoord[0] + vec2(0, textureHeight);
	EmitVertex();

	gl_Position = transformMat * (gl_in[0].gl_Position + vec4(textWidth, 0, 0, 0));
	fTexCoord = gTexCoord[0] + vec2(textureWidth, 0);
	EmitVertex();
	
	gl_Position = transformMat * gl_in[0].gl_Position;
	fTexCoord = gTexCoord[0] + vec2(0, 0);
	EmitVertex();


	EndPrimitive();
}
)";
	std::string fragment =
		R"(
#version 330

varying vec2 fTexCoord;

uniform sampler2D fontBuffer;

void main(void) 
{
	gl_FragColor = vec4(1, 1, 1, 1);
}
)";
}


namespace axis_model
{
	std::string vertex =
		R"(
#version 330

uniform vec4 color[3];

out vec4 dir;
out vec4 gColor;

void main()
{
	gl_Position = vec4(0, 0, 0, 1);
	dir = vec4(0, 0, 0, 1);
	if(gl_VertexID == 0)
		dir = vec4(1, 0, 0, 1);
	if(gl_VertexID == 1)
		dir = vec4(0, 1, 0, 1);
	if(gl_VertexID == 2)
		dir = vec4(0, 0, 1, 1);
	gColor = color[gl_VertexID];
}
)";
	std::string geometry = R"(

#version 330

in vec4 dir[];
in vec4 gColor[];

out vec4 fColor;

uniform mat4 mvp;

layout(points) in;
layout(line_strip, max_vertices = 2) out;

void main() {
	gl_Position = mvp * gl_in[0].gl_Position;
	fColor = gColor[0];
	EmitVertex();


	gl_Position = mvp * (gl_in[0].gl_Position + dir[0]);
	fColor = gColor[0];
	EmitVertex();

	EndPrimitive();
}
)";
	std::string fragment =
		R"(
#version 330


in vec4 fColor;

void main(void) 
{
	gl_FragColor = fColor;
}
)";
}



}