// Shader-ul de fragment / Fragment shader  
#version 400

#ifdef GL_ES
precision medium float;
#endif

in vec4 ex_Color;
uniform int codCol;

out vec4 out_Color;
vec2 u_resolution = {800, 600};

void main(void)
{
	switch (codCol) {
	case 0:
	{
		out_Color = ex_Color;
		break;
	}
	case 1:
	{
		out_Color = vec4(0.0, 0.0, 1.0, 0.0);
		break;
	}
	case 2:
	{
		out_Color = vec4(1.0, 0.0, 0.0, 0.0);
		break;
	}
	case 3:
	{
		out_Color = vec4(0.0, 1, 0.0, 0.0);
		break;
	}
	case 4:
	{
		out_Color = vec4(1.0, 1.0, 1.0, 0.0);
		break;
	}
	case 5:  // Gradient case 
	{
		vec2 coord = gl_FragCoord.xy / u_resolution;

		vec3 color = vec3(0.5 * sin(coord.x) + 0.5, 0.5 * sin(coord.y) + 0.5, sin(coord.x + coord.y) + 0.2);
		out_Color = vec4(color, 1);
		break;
	}
	default: 
	{
		out_Color = ex_Color;
		break;
	}
	}
}
