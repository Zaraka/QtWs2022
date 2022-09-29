attribute highp vec3 verCoord;
attribute highp vec2 texCoord;
attribute highp vec3 normal;

uniform highp mat4 model;
uniform highp mat4 view;
uniform highp mat4 projection;
uniform highp mat3 trans_inv_model;

varying highp vec2 TexCoord;
varying highp vec3 FragPos;
varying highp vec3 Normal;


void main(void)
{
    FragPos = vec3(model * vec4(verCoord.xyz, 1.0));
    Normal = trans_inv_model * normal;
    TexCoord = texCoord;

    gl_Position = projection * view * vec4(FragPos, 1.0);
}
