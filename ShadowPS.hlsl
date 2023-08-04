struct PSOutput {
	float32_t4 shadow : SV_TARGET0;
};

PSOutput main()
{
	PSOutput psOutput;
	psOutput.shadow = float32_t4(0.5f, 0.5f, 0.5f, 0.5f);
	return psOutput;
}