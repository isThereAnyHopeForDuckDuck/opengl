#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;
uniform vec3 lightPos;

in vec3 FragPos;
in vec3 Normal;



void main()
{    

    vec3 light_ambient = vec3(0.2f, 0.2f, 0.2f);
    vec3 light_diffuse = vec3(0.5f, 0.5f, 0.5f);
    vec3 light_specular = vec3(1.0f, 1.0f, 1.0f);

    vec3 material_ambient = vec3(1.0f, 1.0f, 1.0f);
    vec3 material_diffuse = vec3(1.0f, 1.0f, 1.0f);
    vec3 material_specular = vec3(1.0f, 1.0f, 1.0f);

    //ambient ������ = ϵ�� * �����ɫ;        RGB������������ϵ�� ��ת���� �� vec(0.1, 0.2, 0.3) * lightColor(vec3(r, g, b))
    vec3 ambient = material_ambient * light_ambient;

    //diffuse �������              ��������ɫ = ������ǿ�� * �����ɫ * ����ϵ��
        //�����������������һ��
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);

        //���ݼнǣ�����������ǿ��
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * light_diffuse * material_diffuse;

    //specular           specularStrength����û��ɲ���ϵ�� 
    vec3 viewDir = normalize(vec3(0, 0, 13.0f) - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = material_specular * spec * light_specular;

    // �������� + ������ �� * ������ɫ
    vec3 result = (ambient + diffuse + specular) * vec3(1.0f, 0.5f, 0.2f);
    //FragColor = vec4(result, 1.0);

    //FragColor = texture(texture_diffuse1, TexCoords) * vec4((ambient + diffuse), 1.0);
    FragColor = texture(texture_diffuse1, TexCoords) * vec4((ambient + diffuse + specular), 1.0);
    //FragColor = texture(texture_diffuse1, TexCoords);
}