// Shader-ul de fragment / Fragment shader  
#version 400

#ifdef GL_ES
precision medium float;
#endif

in vec4 ex_Color;
out vec4 out_Color;


uniform int codCol;
uniform int windowHeight;
uniform int windowWidth;
vec2 u_resolution = { windowHeight, windowWidth };

const vec4 red = vec4(1.0, 0.0, 0.0, 0.0);
const vec4 blue = vec4(0.0, 0.0, 1.0, 0.0);
const vec4 green = vec4(0.0, 1.0, 0.0, 0.0);
const vec4 white = vec4(1.0, 1.0, 1.0, 0.0);
const vec4 black = vec4(0.0, 0.0, 0.0, 0.0);

void main(void)
{
	switch (codCol) {
	case 0:
		out_Color = ex_Color;
		break;

	case 1:
		out_Color = blue;
		break;

	case 2:
		out_Color = red;
		break;

	case 3:
		out_Color = green;
		break;

	case 4:
		out_Color = white;
		break;

	case 5:
		out_Color = black;
		break;

	case 6:  // Gradient case 
		vec2 coord = gl_FragCoord.xy / u_resolution;

		vec3 color = vec3(0.5 * sin(coord.x) + 0.5, 0.5 * sin(coord.y) + 0.5, sin(coord.x + coord.y) + 0.2);
		out_Color = vec4(color, 1);
		break;

	default:
		out_Color = ex_Color;
		break;
	}
}
