//--------------------------------------------------------------------------------------
// File: DX11 Framework.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
cbuffer ConstantBuffer : register( b0 )
{
	matrix  World;
	matrix  View;
	matrix  Projection;

    float4  DiffuseMtrl;
    float4  DiffuseLight;
    float3  LightVecW;

    float4  AmbientMtrl;
    float4  AmbientLight;

    float4  SpecularMtrl;
    float4  SpecularLight;
    float   SpecularPower;
    float3  EyePosW;
}

//--------------------------------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float3 Norm : NORMAL;
    float3 PosW : POSITION;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS( float4 Pos : POSITION, float3 NormalL : NORMAL )
{
    VS_OUTPUT output = (VS_OUTPUT)0;

    //converts from model to world space
    output.Pos = mul( Pos, World );

    output.PosW = normalize(EyePosW.xyz - output.Pos.xyz);

    //converts from camera space from world space
    output.Pos = mul( output.Pos, View );

    //converts from view space to projection
    output.Pos = mul( output.Pos, Projection );

    float3 normalW = mul(float4(NormalL, 0.0f), World).xyz;
    output.Norm = normalize(normalW);

    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( VS_OUTPUT input ) : SV_Target
{
    float diffuseAmount = max(dot(LightVecW, input.Norm), 0.0f);
    float3 diffuse = diffuseAmount * (DiffuseMtrl * DiffuseLight).rgb;

    float r = reflect(-LightVecW, input.Norm);
    float specularAmount = pow(max(dot(r, input.PosW), 0.0f), SpecularPower);
    float3 specular = specularAmount * (SpecularMtrl * SpecularLight).rgb;

    float3 ambient = AmbientMtrl * AmbientLight;

    float4 outColour;
    outColour.rgb = diffuse + ambient + clamp(specular, 0, 1);
    outColour.a = DiffuseMtrl.a;
    return outColour;
}