uniform sampler2D source;
uniform vec4 mask;
uniform vec2 direction;

void main() {
	vec2 textureCoordinates = gl_TexCoord[0].xy;

	vec4 color = vec4(0.0);
	color += texture2D(source, textureCoordinates - 4.0 * direction) * 0.0162162162;
	color += texture2D(source, textureCoordinates - 3.0 * direction) * 0.0540540541;
	color += texture2D(source, textureCoordinates - 2.0 * direction) * 0.1216216216;
	color += texture2D(source, textureCoordinates - direction) * 0.1945945946;
	color += texture2D(source, textureCoordinates) * 0.2270270270;
	color += texture2D(source, textureCoordinates + direction) * 0.1945945946;
	color += texture2D(source, textureCoordinates + 2.0 * direction) * 0.1216216216;
	color += texture2D(source, textureCoordinates + 3.0 * direction) * 0.0540540541;
	color += texture2D(source, textureCoordinates + 4.0 * direction) * 0.0162162162;
	color = mask * vec4(100.0, 100.0, 100.0, color[3]) / 100.0;

	gl_FragColor = color;
}