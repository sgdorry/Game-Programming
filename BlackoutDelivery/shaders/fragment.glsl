#version 330

uniform sampler2D texture0;
uniform vec2 lightPosition;
uniform vec2 screenSize;
uniform float lightRadius;

in vec2 fragTexCoord;
in vec2 fragPosition;

out vec4 finalColor;

void main(){
    vec2 screenUV = gl_FragCoord.xy / screenSize;
    screenUV.y = 1.0-screenUV.y;
    vec2 diff = screenUV-lightPosition;
    diff.x *= screenSize.x / screenSize.y;
    float dist = length(diff);

    vec4 texColor = texture(texture0, fragTexCoord);
    if (dist<lightRadius){
        finalColor=texColor;

    }
    else if(dist<lightRadius*1.5){
        float falloff = 1.0-(dist-lightRadius) / (lightRadius*0.5);
        finalColor = texColor*falloff;
    }
    else{
        finalColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
}