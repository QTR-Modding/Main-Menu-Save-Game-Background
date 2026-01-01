cbuffer BlurCB : register(b0)
{
    float2 texelSize;
    float radius;
    float brightness;
    float saturation;
    float contrast;
    float rMultiply;
    float gMultiply;
    float bMultiply;
    uint inputWidth;
    uint inputHeight;
    uint outputWidth;
    uint outputHeight;
    int offsetX;
    int offsetY;
    float vignetteStrength;
};

Texture2D inputTex : register(t0);
RWTexture2D<float4> outputTex : register(u0);

[numthreads(16, 16, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    if (DTid.x >= outputWidth || DTid.y >= outputHeight)
        return;

    int2 inputCoord = int2(DTid.xy) - int2(offsetX, offsetY);

    if (inputCoord.x < 0 || inputCoord.y < 0 ||
        inputCoord.x >= (int) inputWidth || inputCoord.y >= (int) inputHeight)
    {
        outputTex[DTid.xy] = float4(0, 0, 0, 1);
        return;
    }

    float resolutionFactor = max(outputWidth, outputHeight) / 1920.0f;
    float scaledRadius = radius * resolutionFactor;
    float sigma = max(scaledRadius / 2.0f, 0.01f);
    float twoSigmaSq = 2.0f * sigma * sigma;
    float piSigma = 3.14159265 * twoSigmaSq;
    float weightSum = 0.0f;
    int r = int(scaledRadius);

    float4 sum = float4(0, 0, 0, 0);
    for (int y = -r; y <= r; y++)
    {
        for (int x = -r; x <= r; x++)
        {
            int2 sampleCoord = inputCoord + int2(x, y);
            sampleCoord.x = clamp(sampleCoord.x, 0, (int) inputWidth - 1);
            sampleCoord.y = clamp(sampleCoord.y, 0, (int) inputHeight - 1);

            float weight = exp(-(x * x + y * y) / twoSigmaSq) / piSigma;
            sum += inputTex.Load(int3(sampleCoord, 0)) * weight;
            weightSum += weight;
        }
    }

    float4 blurred = sum / max(weightSum, 0.0001f);
    blurred.rgb *= brightness;
    blurred.rgb = ((blurred.rgb - 0.5f) * contrast) + 0.5f;
    float lum = dot(blurred.rgb, float3(0.299, 0.587, 0.114));
    blurred.rgb = lerp(float3(lum, lum, lum), blurred.rgb, saturation);
    blurred.r *= rMultiply;
    blurred.g *= gMultiply;
    blurred.b *= bMultiply;

    float2 center = float2(outputWidth, outputHeight) * 0.5f;
    float2 pos = float2(DTid.xy);
    float dist = length(pos - center) / length(center);
    blurred.rgb *= 1.0 - vignetteStrength * dist * dist;

    outputTex[DTid.xy] = blurred;
}
