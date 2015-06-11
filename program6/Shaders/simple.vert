//#version 130

uniform mat4 P;
uniform mat4 V;
uniform float time;
varying vec4 fColor;
attribute vec3 norm;
attribute vec3 pos;
varying float d;
varying float s;
//in vec2 pos;

vec4 lightPos= vec4(4,0,4,1);

mat4 rotateY(float a){
    return mat4(
        cos(a), -sin(a),0, 0,
        0, 1, 0, 0,
        sin(a), cos(a),0, 0,
        0, 0, 0, 1);
}
void main()
{	
	vec4 p = vec4(pos, 1);
      mat4 R = rotateY(time/360);
      
      //diffuse
      vec4 lPos = R * lightPos+ vec4(4,4,0,0);
      p = p;
      vec3 l = normalize(lPos.xyz - p.xyz);
      vec3 n = (R * vec4(norm, 0)).xyz;
      d = abs(dot(l, n));
      
      //specular
      vec3 camPos = vec3(0);
	  p=P*V*p;
      vec3 v = normalize(camPos - p.xyz);
      vec3 r = reflect(l, n);
      s = abs(dot(v,r));
      s = pow(s, 25);
      

	gl_Position = p;
}
