attribute highp vec2 texCoord;
attribute highp vec2 verCoord;

varying highp vec2 TexCoord;

void main(void)
{
    gl_Position = vec4(verCoord.x, verCoord.y, 0, 1);
    TexCoord = texCoord;
}
