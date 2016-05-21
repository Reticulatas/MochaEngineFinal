#include "Shadow.hlsl"

#define DIRECTIONAL 0
#define POINT 1
#define SPOTLIGHT 2

struct LightCount
 {
   int Dir;
   int Point;
   int Spot;
   int Total;
 };

struct Light
{
  float4 Diffuse;

  float3 Position;
  float Range;

  float3 Direction;
  float Att;

  float InnerAngle;
  float OuterAngle;
  float Spot;
  
  int Type;
};

struct Material
{
  float4 Ambient;
  float4 Diffuse;
  float4 Specular; // w = SpecPower
};

//---------------------------------------------------------------------------------------
// Computes the ambient, diffuse, and specular terms in the lighting equation
// from a directional light.  We need to output the terms separately because
// later we will modify the individual terms.
//---------------------------------------------------------------------------------------
float4 ComputeDirectionalLight(Material mat, Light L, float3 normal, float3 toEye)
{
  // Initialize outputs.
  float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
  float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
  float4 spec    = float4(0.0f, 0.0f, 0.0f, 0.0f);

  // The light vector aims opposite the direction the light rays travel.
  float3 lightVec = -L.Direction;

  // Add ambient term.
  //ambient = mat.Ambient * L.Ambient;	

  // Add diffuse and specular term, provided the surface is in 
  // the line of site of the light.
  
  float diffuseFactor = dot(lightVec, normal);

  // Flatten to avoid dynamic branching.
  [flatten]
  if( diffuseFactor > 0.0f )
  {
    float3 v         = reflect(-lightVec, normal);
    v = normalize(v);
    float specFactor = pow(max(dot(v, toEye), 0.0f), mat.Specular.w);
          
    diffuse = diffuseFactor * mat.Diffuse * L.Diffuse;
    spec    = specFactor * mat.Specular;
  }

  return saturate(diffuse + spec);
}

float4 ComputePointLight(Material mat, Light L, float3 pos, float3 normal, float3 toEye)
{
  // Initialize outputs.
  float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
  float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
  float4 spec    = float4(0.0f, 0.0f, 0.0f, 0.0f);

  // The vector from the surface to the light.
  float3 lightVec = L.Position - pos;
    
  // The distance from surface to light.
  float d = length(lightVec);
    
  // Range test.
  if (d > L.Range)
    return diffuse;

  // Normalize the light vector.
  lightVec /= d; 
  
  // Ambient term.
  //ambient = mat.Ambient * L.Ambient;	

  // Add diffuse and specular term, provided the surface is in 
  // the line of site of the light.

  float diffuseFactor = dot(lightVec, normal);

  // Flatten to avoid dynamic branching.
  [flatten]
  if( diffuseFactor > 0.0f )
  {
    float3 v         = reflect(-lightVec, normal);
    v = normalize(v);
    float specFactor = pow(max(dot(v, toEye), 0.0f), mat.Specular.w);
          
    diffuse = diffuseFactor * mat.Diffuse * L.Diffuse;
    spec = specFactor * mat.Specular;
  }

  // Normalize distance with light range
  //float att = 1.0f / dot(L.Att, float3(1.0f, d, d*d));
  //att = min(att, 1.f);
  float distToLightNorm = 1.f - saturate(d / L.Range);
  float att = distToLightNorm * distToLightNorm * L.Att;

  diffuse *= att;
  spec    *= att;

  return saturate(diffuse + spec);
}

float4 ComputeSpotLight(Material mat, Light L, float3 pos, float3 normal, float3 toEye)
{
  // Initialize outputs.
  float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
  float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
  float4 spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

  // The vector from the surface to the light.
  float3 lightVec = L.Position - pos;
    
  // The distance from surface to light.
  float d = length(lightVec);
  
  // Range test.
  if( d > L.Range )
   return diffuse;
    
  // Normalize the light vector.
  lightVec /= d; 
 
  // Ambient term.
  //ambient = mat.Ambient * L.Ambient;	

  // Add diffuse and specular term, provided the surface is in 
  // the line of site of the light.

  float diffuseFactor = dot(lightVec, normal);

  // Flatten to avoid dynamic branching.
  [flatten]
  if( diffuseFactor > 0.0f )
  {
    float3 v         = reflect(-lightVec, normal);
    v = normalize(v);
    float specFactor = pow(max(dot(v, toEye), 0.0f), mat.Specular.w);
          
    diffuse = diffuseFactor * mat.Diffuse * L.Diffuse;
    spec    = specFactor * mat.Specular;
  }

  // Calculate inner and outer angle
  float3 dirVec = normalize(L.Direction);
  float outAng = L.OuterAngle;
  float inAng = L.InnerAngle;

  // calculate spot factor
  float spot = (dot(-lightVec, dirVec) - outAng) / (inAng - outAng);
  spot = pow(max(spot, 0.f), L.Spot);

  // Normalize distance with light range
  float distToLightNorm = 1.f - saturate(d / L.Range);
  float att = distToLightNorm * distToLightNorm * L.Att;
  //att = min(att, 1.f);
  att *= spot;

  diffuse *= att;
  spec    *= att;

  return saturate(diffuse + spec);
}




/////// PBR ///////

#define PI 3.14f

float NormalDistribution_GGX(float a, float NdH)
{
	float a2 = a * a;
	float NdH2 = NdH * NdH;
	float den = NdH2 * (a2 - 1.0f) + 1.0f;

	return a2 / (PI * (den * den));
}

float Specular_D(float a, float NdH)
{
	return NormalDistribution_GGX(a, NdH);
}

float3 Fresnel_Schlick(float3 specularColor, float3 h, float3 v)
{
	return (specularColor + (1.0f - specularColor) * pow((1.0f - saturate(dot(v, h))), 5));
}

float Geometric_Schlick_GGX(float a, float NdV)
{
	float k = a * 0.5f;

	return (NdV / (NdV * (1.0f - k) + k));
}

float3 Specular_F(float3 specularColor, float3 h, float3 v)
{
	return Fresnel_Schlick(specularColor, h, v);
}

float3 Specular_F_Roughness(float3 specularColor, float a, float3 h, float3 v)
{
	return (specularColor + (max(1.0f - a, specularColor) - specularColor) * pow((1 - saturate(dot(v, h))), 5));
}

float Specular_G(float a, float NdV, float NdL, float NdH, float VdH, float LdV)
{
	return Geometric_Schlick_GGX(a, NdV);
}

float3 Specular(float3 specularColor, float3 h, float3 v, float3 l, float a, float NdL, float NdV, float NdH, float VdH, float LdV)
{
	return ((Specular_D(a, NdH) * Specular_G(a, NdV, NdL, NdH, VdH, LdV)) * Specular_F(specularColor, v, h)) / (4.0f * NdL * NdV + 0.0001f);
}

float3 Diffuse(float3 pAlbedo)
{
	return pAlbedo / PI;
}

float3 ComputeLight(float3 albedoColor, float3 specularColor, float3 normal, float roughness, float3 lightPosition, float3 lightColor, float3 lightDir, float3 viewDir)
{
	// Compute some useful values.
	float NdL = saturate(dot(normal, lightDir));
	float NdV = saturate(dot(normal, viewDir));
	float3 h = normalize(lightDir + viewDir);
	float NdH = saturate(dot(normal, h));
	float VdH = saturate(dot(viewDir, h));
	float LdV = saturate(dot(lightDir, viewDir));
	float a = max(0.002f, roughness * roughness);

	float3 cDiff = Diffuse(albedoColor);
	float3 cSpec = Specular(specularColor, h, viewDir, lightDir, a, NdL, NdV, NdH, VdH, LdV);
	//cSpec = saturate(cSpec);

	return lightColor * NdL * (cDiff * (1.0f - cSpec) + cSpec);
}

float4 ComputeDirLightPBR(Material mat, Light L, float3 pos, float3 normal, float3 viewDirection, float3 envColor, float roughness, float metallic)
{
  float3 lightColor = L.Diffuse.xyz;
  float3 albedoColor = mat.Diffuse.xyz;
  float3 specularColor = mat.Specular.xyz;

  // The vector from the surface to the light.
  float3 lightVec = -L.Direction;
  lightVec = normalize(lightVec);

  // Gamma correction.
  //albedoColor = pow(albedoColor.rgb, 2.2f);

  // Lerp with metallic value to find the good diffuse and specular.
  float3 realAlbedo = albedoColor - albedoColor * metallic;

  // 0.03 default specular value for dielectric.
  float3 realSpecularColor = lerp(0.02f, albedoColor, metallic);

  float3 light = ComputeLight(realAlbedo, realSpecularColor, normal, roughness, L.Position.xyz, L.Diffuse.rgb, lightVec, viewDirection);
  // The distance from surface to light.

  float3 envFresnel = Specular_F_Roughness(realSpecularColor, roughness * roughness, normal, viewDirection);

  // realAlbedo = saturate(realAlbedo);
  float Ndl = saturate(dot(normal, lightVec));

  return float4((light + envFresnel * envColor + Ndl * realAlbedo), 1.0f);
  //return float4(light, 1);
}


float4 ComputePointLightPBR(Material mat, Light L, float3 pos, float3 normal, float3 viewDirection, float3 envColor, float roughness, float metallic)
{
	////////////////////////////////////////////////////////////
  float3 lightColor = L.Diffuse.xyz;
  float3 albedoColor = mat.Diffuse.xyz;
	float3 specularColor = mat.Specular.xyz;

	// The vector from the surface to the light.
	float3 lightVec = L.Position - pos;

	// The distance from surface to light.
	float d = length(lightVec);

	// Range test.
  if (d > L.Range)
    return float4(0, 0, 0, 0);

  //if (roughness == 0 && metallic == 0)
  //  return mat.Diffuse;

	lightVec /= d;
	
	//albedoColor = albedoColor / PI;
	// Gamma correction.
  // Lerp with metallic value to find the good diffuse and specular.
	float3 realAlbedo = albedoColor - albedoColor * metallic;

	// 0.03 default specular value for dielectric.
	float3 realSpecularColor = lerp(0.02f, albedoColor, metallic);
	//float3 realSpecularColor = lerp(0.04f, albedoColor, 1 - roughness);
	//realSpecularColor = albedoColor;

	float3 light = ComputeLight(realAlbedo, realSpecularColor, normal, roughness, L.Position.xyz, L.Diffuse.rgb, lightVec, viewDirection);

	// environment fresnel
	//float3 envFresnel = Specular_F_RoughnessF0(realSpecularColor, roughness * roughness, normal, viewDirection, max(0.03f, metallic));
	float3 envFresnel = Specular_F_Roughness(realSpecularColor, roughness * roughness, normal, viewDirection);
	//envFresnel = pow(envFresnel, 2.2);

	float Ndl = saturate(dot(normal, lightVec));
	float distToLightNorm = 1.f - saturate(d / L.Range);
	float attenuation = distToLightNorm * distToLightNorm * L.Att;

	return float4(attenuation * (light + envFresnel * envColor + Ndl * realAlbedo * L.Diffuse.rgb), 1.0f);
}

float4 ComputeSpotLightPBR(Material mat, Light L, float3 pos, float3 normal, float3 viewDirection, float3 envColor, float roughness, float metallic)
{
	// Initialize outputs.
	float3 albedoColor = mat.Diffuse.xyz;

	// The vector from the surface to the light.
	float3 lightVec = L.Position - pos;

	// The distance from surface to light.
	float d = length(lightVec);

	// Range test.
	if (d > L.Range)
		return float4(0,0,0,0);

	// Normalize the light vector.
	lightVec /= d;

	//albedoColor = albedoColor / PI;
	// Ambient term.
	//ambient = mat.Ambient * L.Ambient;	
	// Gamma correction.
	//albedoColor = pow(albedoColor.rgb, 2.2f);
	// Lerp with metallic value to find the good diffuse and specular.
	float3 realAlbedo = albedoColor - albedoColor * metallic;

	// 0.03 default specular value for dielectric.
	// float3 realSpecularColor = lerp(0.02f, specularColor, metallic);
	float3 realSpecularColor = lerp(0.02f, albedoColor, metallic);

	float3 envFresnel = Specular_F_Roughness(realSpecularColor, roughness * roughness, normal, viewDirection);
	//envFresnel = pow(envFresnel, 2.2);
	// Add diffuse and specular term, provided the surface is in 
	// the line of site of the light.
	float3 light = ComputeLight(realAlbedo, realSpecularColor, normal, roughness, L.Position.xyz, L.Diffuse.rgb, lightVec, viewDirection);

	// Calculate inner and outer angle
	float3 dirVec = normalize(L.Direction);
	float outAng = L.OuterAngle;
	float inAng = L.InnerAngle;

	// calculate spot factor
	float spot = (dot(-lightVec, dirVec) - outAng) / (inAng - outAng);
	spot = pow(max(spot, 0.f), L.Spot);

	// Normalize distance with light range
	float distToLightNorm = 1.f - saturate(d / L.Range);
	float att = distToLightNorm * distToLightNorm * L.Att;
	//att = min(att, 1.f);
	//realAlbedo = realAlbedo / PI;
	att *= spot;

	float Ndl = saturate(dot(normal, lightVec));

	return float4(att * (light + envFresnel * envColor + Ndl * realAlbedo * L.Diffuse.rgb), 1.0f);
}

float4 ComputeDirLightPhong(Light L, float3 posW, float3 normal, float3 viewDirection)
{

}

float4 ComputePointLightPhong(Light L, float3 posW, float3 normal, float3 viewDirection)
{
    float dist = distance(posW, L.Position);

    if (dist > L.Range)
        return float4(0, 0, 0, 0);
    else
    {

        float ratio = (dist / L.Range);
        float attenuation = saturate(0.25f *(dist / L.Range));
        float4 finalCol = L.Diffuse;
        finalCol.w = attenuation;
        return finalCol;

    }
    

}

float4 ComputeSpotLightPhong(Light L, float3 posW, float3 normal, float3 viewDirection)
{

}

// Parallax correction cube map
float3 BPCEM(float3 camPos, float3 bumpNormal, float3 worldPos, float3 probePos, float3 EnvBoxDim)
{
  float3 EnvBoxMax = probePos + EnvBoxDim;
  float3 EnvBoxMin = probePos - EnvBoxDim;

  float3 dir = worldPos - camPos;
  float3 rdir = reflect(dir, bumpNormal);

  //BPCEM
  float3 nrdir = normalize(rdir);
  float3 rbmax = (EnvBoxMax - worldPos) / nrdir;
  float3 rbmin = (EnvBoxMin - worldPos) / nrdir;

  float3 rbminmax = (nrdir>0.0f) ? rbmax : rbmin;
  float fa = min(min(rbminmax.x, rbminmax.y), rbminmax.z);

  float3 posonbox = worldPos + nrdir*fa;
    rdir = posonbox - probePos;
  return rdir;
}

/////////////////////////////////////
//Original Author: Kenneth Tan
//Modifications:
//	Date	-	Change