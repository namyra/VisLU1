uniform sampler2D tex_exit;
uniform sampler2D tex_entry;
uniform sampler3D tex_intensity;
uniform sampler2D tex_transfer;
uniform sampler3D tex_gradient;
uniform int lighting;

uniform float lightDirWorldX;
uniform float lightDirWorldY;
uniform float lightDirWorldZ;

varying vec3 view;

void main (void) 
{
	vec4 diffuse, ambient, specular;
	vec4 globalAmbient = vec4(0.05, 0.05, 0.05, 0.0);
	float NdotL, NdotH, gradientMulti;
	vec3 V = normalize(-view);
	vec3 lightDir, normal, grad, H;
	
	const float dz = 0.003;
	const int maxrange = 1000;
	vec4 transferred, intensity;

	vec4 Result = vec4(0.0, 0.0, 0.0, 0.0); 
	vec4 entry_point = texture2D(tex_entry, vec2(gl_TexCoord[0]));
	vec4 exit_point = texture2D(tex_exit, vec2(gl_TexCoord[0]));
	float dist = distance(entry_point, exit_point)/dz;
	int maxiter = int(floor(dist));
	vec3 diff = (exit_point.xyz - entry_point.xyz)/dist;
	vec3 point = entry_point.xyz;
  
	if (entry_point.w == 0.0)
	{
		discard;
	}
	else
	{
		for(int i = 0; i < maxrange; i++)
		{
			intensity = texture3D(tex_intensity, point);
			grad = texture3D(tex_gradient, point.xyz).xyz;
			//grad = (grad * 2.0) - 1.0;
			grad = vec3(gl_NormalMatrix * grad);
			
			transferred = texture2D(tex_transfer, vec2(intensity.x, intensity.x)/1.5);
			if (lighting == 1)
			{
				normal = normalize(grad);
				lightDir = normalize(vec3(gl_ModelViewMatrix * vec4(lightDirWorldX, lightDirWorldY, lightDirWorldZ, 1.0)) - view);

				H = normalize(lightDir + V);
	
				diffuse = 0.5 * transferred * gl_LightSource[0].diffuse;
				ambient = 0.5 * transferred * gl_LightSource[0].ambient;
				specular = 0.5 * transferred * gl_LightSource[0].specular;

				NdotL = max(dot(normal, lightDir), 0.0);
				
				Result = globalAmbient;

				if (NdotL > 0.0) {
					Result += globalAmbient;
					Result += ambient;
					Result += diffuse * NdotL;
					NdotH = max(dot(normal, H), 0.0);
					Result += specular * pow(NdotH, gl_FrontMaterial.shininess);
				}

				Result.xyz += (1.0 - Result.w) * transferred.w * Result.xyz;
				Result.w += (1.0 - Result.w) * transferred.w;
			}
			else
			{	
				Result.xyz += (1.0 - Result.w) * transferred.w * transferred.xyz;
				Result.w += (1.0 - Result.w) * transferred.w;
			}
			point +=diff;
			if ((Result.w >= 1.0) || (i >= maxiter)) break;
		}
	gl_FragColor = Result;
	}
}
