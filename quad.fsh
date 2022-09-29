varying highp vec2 TexCoord;

uniform sampler2D texture;

void main(void)
{
    gl_FragColor = texture2D(texture, TexCoord.st);
}
