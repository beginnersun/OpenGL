#version 330 core
out vec4 FragColor
in VS_OUT{
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
}fs_in;

uniform sample2D floorTexture;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform bool blinn;

void main(){
    vec3 color = texture(floorTexture,fs_in.TexCoords).rgb;

    //环境光
    vec3 ambient = color * 0.2;

    //漫反射  （法线向量与光源向量）影响比例 * 光的颜色
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    vec3 normal = normalize(fs_in.Normal);
    float diff = max(dot(lightDir,normal),0.0);
    vec3 diffuse = diff * color;

    //半程反射（半程向量）
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir,normal);
    float spec = 0.0;

    //
    if(blinn){
        //向量相加 A<-B B->C  两向量相加得到中间向量(半程向量)B->D 
        vec3 halfwayDir = normalize(reflectDir,normal);
        spec = pow(max(dot(normal,halfwayDir),0.0),32.0);//32代表反光度
    } else {
        spec = pow(max(dot(viewDir,reflectDir),0.0),8.0);
    }
    vec3 specular = lightColor * spec;
    FragColor = vec4(ambient+diffuse+specular,1.0);
}