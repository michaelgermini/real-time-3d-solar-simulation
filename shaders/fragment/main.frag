#version 430 core

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;

out vec4 FragColor;

// Material properties
struct Material {
    vec3 albedo;
    float metallic;
    float roughness;
    float ao;
    sampler2D albedoMap;
    sampler2D normalMap;
    sampler2D metallicMap;
    sampler2D roughnessMap;
    sampler2D aoMap;
};

// Light properties
struct Light {
    int type;
    vec3 position;
    vec3 direction;
    vec3 color;
    float intensity;
    float ambient;
    float diffuse;
    float specular;
    float constant;
    float linear;
    float quadratic;
    float cutOff;
    float outerCutOff;
};

uniform Material material;
uniform Light lights[16];
uniform int numLights;
uniform vec3 viewPos;
uniform vec3 ambientLight;
uniform sampler2D shadowMap;

// Constants
const float PI = 3.14159265359;

// PBR functions
vec3 getNormalFromMap() {
    vec3 tangentNormal = texture(material.normalMap, fs_in.TexCoords).xyz * 2.0 - 1.0;
    
    vec3 Q1  = dFdx(fs_in.FragPos);
    vec3 Q2  = dFdy(fs_in.FragPos);
    vec2 st1 = dFdx(fs_in.TexCoords);
    vec2 st2 = dFdy(fs_in.TexCoords);
    
    vec3 N   = normalize(fs_in.Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);
    
    return normalize(TBN * tangentNormal);
}

float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
    
    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    
    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;
    
    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
    
    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
    
    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float ShadowCalculation(vec4 fragPosLightSpace) {
    // Perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    
    // Transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    
    // Get closest depth value from light's perspective
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    
    // Get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    
    // Check whether current frag pos is in shadow
    float bias = 0.005;
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
    
    return shadow;
}

vec3 calculateLighting(Light light, vec3 N, vec3 V, vec3 albedo, float metallic, float roughness, vec3 F0) {
    vec3 L;
    float attenuation = 1.0;
    
    if (light.type == 0) { // Directional light
        L = normalize(-light.direction);
    } else if (light.type == 1) { // Point light
        L = normalize(light.position - fs_in.FragPos);
        float distance = length(light.position - fs_in.FragPos);
        attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);
    } else if (light.type == 2) { // Spot light
        L = normalize(light.position - fs_in.FragPos);
        float distance = length(light.position - fs_in.FragPos);
        attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);
        
        float theta = dot(L, normalize(-light.direction));
        float epsilon = light.cutOff - light.outerCutOff;
        float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
        attenuation *= intensity;
    }
    
    vec3 H = normalize(V + L);
    float NdotL = max(dot(N, L), 0.0);
    
    // Cook-Torrance BRDF
    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
    
    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * NdotL + 0.0001;
    vec3 specular = numerator / denominator;
    
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;
    
    vec3 Lo = (kD * albedo / PI + specular) * light.color * light.intensity * NdotL * attenuation;
    
    return Lo;
}

void main() {
    // Get material properties
    vec3 albedo = texture(material.albedoMap, fs_in.TexCoords).rgb * material.albedo;
    float metallic = texture(material.metallicMap, fs_in.TexCoords).r * material.metallic;
    float roughness = texture(material.roughnessMap, fs_in.TexCoords).r * material.roughness;
    float ao = texture(material.aoMap, fs_in.TexCoords).r * material.ao;
    
    // Get normal
    vec3 N = getNormalFromMap();
    vec3 V = normalize(viewPos - fs_in.FragPos);
    
    // Calculate reflectance at normal incidence
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);
    
    // Calculate lighting
    vec3 Lo = vec3(0.0);
    for (int i = 0; i < numLights; i++) {
        Lo += calculateLighting(lights[i], N, V, albedo, metallic, roughness, F0);
    }
    
    // Ambient lighting
    vec3 ambient = ambientLight * albedo * ao;
    
    // Calculate shadow
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace);
    
    // Final color
    vec3 color = ambient + Lo * (1.0 - shadow);
    
    // HDR tonemapping
    color = color / (color + vec3(1.0));
    
    // Gamma correction
    color = pow(color, vec3(1.0/2.2));
    
    FragColor = vec4(color, 1.0);
}
