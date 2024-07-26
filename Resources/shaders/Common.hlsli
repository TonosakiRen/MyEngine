#define RTWidth 1280
#define RTHeight 720

struct Material {
	float32_t4 materialColor;
	float32_t4x4 uvTransfrom;
	int32_t enableLighting;
};

struct TileInformation {
	uint32_t pointLightNum;
	uint32_t spotLightNum;
	uint32_t shadowSpotLightNum;
	uint32_t pointLightOffset;
	uint32_t spotLightOffset;
	uint32_t shadowSpotLightOffset;
};