#version 460

in vec2 textCoord;
in vec3 vPos;
in vec3 T;
in vec3 B;
in vec3 N;
in vec4 fragPosLight[2];


out vec4 fragmentColour;

uniform sampler2D shadowMap0;
uniform sampler2D shadowMap1;
uniform samplerCube shadowCubemap0;
uniform samplerCube shadowCubemap1;
uniform samplerCube shadowCubemap2;
uniform samplerCube shadowCubemap3;
uniform samplerCube shadowCubemap4;
uniform sampler2D ambientTexture;
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D normalTexture;
uniform sampler2D roughnessTexture;
uniform sampler2D metalTexture;
uniform vec3 cameraPos;
uniform int hasNormal;
uniform float farPlane;

uniform int lightNr;
uniform vec3 lightPos[5];
uniform vec3 lightColor[5];
uniform int lightType[5];

vec3 ambientStrength = vec3(0.2f, 0.2f, 0.2f);
vec3 ambient;
vec3 diffSpec;
float lightCos;

float PI = 3.14159f;

float distribution(float roughness, vec3 normal, vec3 halfVector){
    float numerator = roughness * roughness;
    float dotProduct = max(dot(normal, halfVector), 0.0f);
    float denominator = dotProduct * dotProduct * (numerator - 1.0f) + 1.0f;
    denominator = max(PI * denominator * denominator, 0.000001f);

    return numerator / denominator;
}

float geomAux(float roughness, vec3 normal, vec3 x){
    float numerator = max(dot(normal, x), 0.0f);
    float k = roughness / 2.0f;
    float denominator = max(numerator * (1.0f - k) + k, 0.000001f);
    return numerator / denominator;
}

float geom(float roughness, vec3 normal, vec3 view, vec3 light){
    return geomAux(roughness, normal, view) * geomAux(roughness, normal, light);
}

vec3 fresnel(vec3 f, vec3 view, vec3 halfVector){
    return f + (vec3(1.0f) - f) * pow(1.0f - max(dot(view, halfVector), 0.0f), 5.0f);
}

void PBR(vec3 light, vec3 lightColor, float att){
    vec3 normal = normalize(N);
    mat3 TBN = mat3(normalize(T), normalize(B), normal);
    normal = hasNormal == 1 ? normalize( TBN * (texture(normalTexture, textCoord).rgb * 2.0f - 1.0f)) : normal;
    vec3 view = normalize(cameraPos-vPos);
    vec3 halfVector = normalize(view + light);
    vec4 albedo = texture(diffuseTexture, textCoord);
    float roughness = texture(roughnessTexture, textCoord).r;
    lightCos = max(dot(light, normal), 0.0f);
    float baseRefl = 0.05f;
    float metal = texture(metalTexture, textCoord).r;
    baseRefl = baseRefl * (1.0f - metal) + (albedo.r + albedo.g + albedo.b) / 3.0f * metal;

    vec3 Ks = fresnel(vec3(baseRefl), view, halfVector);
    vec3 Kd = (vec3(1.0f) - Ks) * (1.0f - metal);

    vec3 lambert = albedo.rgb;

    vec3 cookNumerator = distribution(roughness, normal, halfVector) * geom(roughness, normal, view, light) * Ks;
    float cookDenominator = max(4.0f * max(dot(view, normal), 0.0f) * lightCos, 0.0000001f);
    vec3 cook = cookNumerator / cookDenominator;

    vec3 BRDF = Kd * lambert + cook;

    ambient = albedo.rgb * ambientStrength * (1.0f - metal) * att;
    diffSpec = BRDF * lightCos * lightColor * att;
}

vec3 directionalShadow(vec4 fragLight, int nrDir){
    float shadow = 0.0f;
    vec3 lightCoords = fragLight.xyz / fragLight.w;
    if(lightCoords.z <= 1.0f){
        lightCoords = (lightCoords + 1.0f)/2.0f;

        float currentDepth = lightCoords.z;

        float bias = max(0.025f * (1.0f - lightCos), 0.0005f);
        int sampleRadius = 2;
        vec2 pixelSize;
        if(nrDir == 0)
            pixelSize = 1.0f / textureSize(shadowMap0, 0) * 4.0f;
        else
            pixelSize = 1.0f / textureSize(shadowMap1, 0) * 4.0f;
        for(int j=-sampleRadius; j<=sampleRadius; j++)
            for(int i=-sampleRadius; i<=sampleRadius; i++){
                float closestDepth;
                if(nrDir == 0)
                    closestDepth = texture(shadowMap0, lightCoords.xy + vec2(i, j) * pixelSize).r;
                else
                    closestDepth = texture(shadowMap1, lightCoords.xy + vec2(i, j) * pixelSize).r;
                if(currentDepth > closestDepth + bias)
                    shadow += 1.0f;
            }
        shadow /= pow((sampleRadius * 2.0f + 1.0f),2);
    }
    return ambient + (1.0f - shadow) * diffSpec;
}

vec4 getTexture(vec3 pos, int nrPoint){
    if(nrPoint == 0)
        return texture(shadowCubemap0, pos);
    else if(nrPoint == 1)
        return texture(shadowCubemap1, pos);
    else if(nrPoint == 2)
        return texture(shadowCubemap2, pos);
    else if(nrPoint == 3)
        return texture(shadowCubemap3, pos);
    else if(nrPoint == 4)
        return texture(shadowCubemap4, pos);
}

vec3 pointShadow(vec3 lightPosition, int nrPoint){
    vec3 sampleOffsetDirections[12] = vec3[](
       vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
       vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
       vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
    );  
    int samples  = 12;

    float shadow = 0.0f;
    float bias   = 0.15f;
    float diskRadius = 0.05f;
    vec3 fragToLight = vPos - lightPosition;
    float currentDepth = length(fragToLight);

    for(int i = 0; i < samples; ++i){
        float closestDepth = getTexture((fragToLight + sampleOffsetDirections[i] * diskRadius), nrPoint).r * farPlane;
        if(currentDepth -  bias > closestDepth)
            shadow += 1.0f;
    }

    shadow /= float(samples); 


    return ambient + (1.0f - shadow) * diffSpec;
}

void main() {
    vec3 color = vec3(0.0f);

    int nrDir = 0, nrPoint = 0;
    for(int i=0; i<lightNr; i++){
        vec3 pointLightDir = lightType[i] == 0 ? lightPos[i] : lightPos[i] - vPos.xyz;
        float dist = max(length(pointLightDir),0.000001f);
        float att = lightType[i] == 0 ? 1.0f : 1.0f/(dist*dist);
        PBR(normalize(pointLightDir), lightColor[i], att);
        if(lightType[i] == 0){
            color += directionalShadow(fragPosLight[nrDir], nrDir);
            nrDir++;
        }
        else{
            color += pointShadow(lightPos[i], nrPoint);
            nrPoint++;
        }
    }

    fragmentColour = vec4(color,1.0f);
}