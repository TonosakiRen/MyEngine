static const uint32_t kMaxParticles = 1024;
RWStructuredBuffer<Particle> particles : register(u0);
[numthreads(1024, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	uint32_t particleIndex = Dtid.x;
	if (particleIndex < kMaxParticles) {
		// Particle構造体の全要素を0で埋めるという書き方
		particles[particleIndex] = (Particle)0;
	}
}