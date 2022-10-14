#version 430 core

layout(local_size_x = 2, local_size_y = 2, local_size_z = 1) in;

uniform sampler2D uInputDepth;
uniform restrict writeonly image2D uOutputDepth;
uniform int uPreLevel;

shared uint sMinDepth;

void main()
{
	if (gl_LocalInvocationIndex == 0)
	{
		// Initialize to 1.0 (max depth) before performing atomicMin
		sMinDepth = floatBitsToUint(1.0);
	}

	memoryBarrierShared();
	barrier();

	float depth = texelFetch(uInputDepth, ivec2(gl_GlobalInvocationID.xy), uPreLevel).x;
	//float depth = texture2D(uInputDepth, vec2(gl_GlobalInvocationID.xy) / resolution).x;
	atomicMin(sMinDepth, floatBitsToUint(depth));

	memoryBarrierShared();
	barrier();

	// Thread 0 writes workgroup-wide min to image
	if (gl_LocalInvocationIndex == 0)
	{
		float res = uintBitsToFloat(sMinDepth);
		imageStore(uOutputDepth, ivec2(gl_WorkGroupID.xy), vec4(res));
	}
}