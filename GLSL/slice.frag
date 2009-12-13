uniform sampler3D slice_sampler;
uniform sampler2D transfer_sampler;

void main() {
	vec4 intensity = texture3D(slice_sampler,vec3(gl_TexCoord[0]) );
	if (intensity.x < 0.05) {
		discard;
	} else {
		gl_FragColor = texture2D(transfer_sampler,vec2(intensity.x, intensity.x)/1.5);
	}
}