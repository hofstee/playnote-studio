#include "ppm.h"

int ppmInit(ppm_ctx_t *ctx, u8 *ppm, int len)
{
	u8 *start = ppm;
	
	if (len < sizeof(ppm_header_t))
		return 0;

	memcpy(&ctx->hdr, ppm, sizeof(ppm_header_t));
	ppm += sizeof(ppm_header_t);
	
	if (strncmp(ctx->hdr.magic, "PARA", 4) != 0)
		return 1;
	
	if (ppm - start + sizeof(ctx->thumbnail) >= len)
		return 2;
	
	memcpy(ctx->thumbnail, ppm, sizeof(ctx->thumbnail));
	ppm += sizeof(ctx->thumbnail);
	
	if (ppm - start + sizeof(ppm_animation_header_t) >= len)
		return 3;
	
	memcpy(&ctx->animHdr, ppm, sizeof(ppm_animation_header_t));
	ppm += sizeof(ppm_animation_header_t);

	ctx->hdr.numFrames++;

	if (ppm - start + (sizeof(u32) * ctx->hdr.numFrames) >= len)
		return 4;

	ctx->videoOffsets = malloc(sizeof(u32) * ctx->hdr.numFrames);
	memcpy(ctx->videoOffsets, ppm, sizeof(u32) * ctx->hdr.numFrames);
	ppm += sizeof(u32) * ctx->hdr.numFrames;
	
	if (ppm - start + ctx->hdr.animationLength - sizeof(ppm_animation_header_t) - (sizeof(u32) * ctx->hdr.numFrames) >= len)
	{
		free(ctx->videoOffsets);	
		return 5;
	}
	
	ctx->videoData = malloc(ctx->hdr.animationLength - sizeof(ppm_animation_header_t) - (sizeof(u32) * ctx->hdr.numFrames));
	memcpy(ctx->videoData, ppm, ctx->hdr.animationLength - sizeof(ppm_animation_header_t) - (sizeof(u32) * ctx->hdr.numFrames));
	ppm += ctx->hdr.animationLength - sizeof(ppm_animation_header_t) - (sizeof(u32) * ctx->hdr.numFrames);

	if (ppm - start + ctx->hdr.numFrames >= len)
	{
		free(ctx->videoOffsets);
		free(ctx->videoData);	
		return 6;
	}
	
	ctx->audioFrames = malloc(ctx->hdr.numFrames);
	memcpy(ctx->audioFrames, ppm, ctx->hdr.numFrames);

	// TODO: fixme
	// ppm = ROUND_UP_4((int)ppm);
	// tmp hack:
	if ((int)ppm % 4 != 0)
		ppm = (int)ppm + (4 - ((int)ppm % 4));

	if (ppm - start + sizeof(ppm_sound_header_t) >= len)
	{
		free(ctx->videoOffsets);
		free(ctx->videoData);
		free(ctx->audioFrames);
		return 7;
	}

	memcpy(&ctx->sndHdr, ppm, sizeof(ppm_sound_header_t));
	ppm += sizeof(ppm_sound_header_t);

	if (ppm - start + ctx->sndHdr.bgmLength >= len)
	{
		free(ctx->videoOffsets);
		free(ctx->videoData);
		free(ctx->audioFrames);
		return 8;
	}
	
	ctx->bgmData = malloc(ctx->sndHdr.bgmLength);
	memcpy(ctx->bgmData, ppm, ctx->sndHdr.bgmLength);

	if (ppm - start + ctx->sndHdr.seLength[0] + ctx->sndHdr.seLength[1] + ctx->sndHdr.seLength[2] >= len)
	{
		free(ctx->videoOffsets);
		free(ctx->videoData);
		free(ctx->audioFrames);
		free(ctx->bgmData);
		return 9;
	}

	for (u8 i = 0; i < SE_CHANNELS; i++)
	{
		ctx->seData[i] = malloc(ctx->sndHdr.seLength[i]);
		memcpy(ctx->seData[i], ppm, ctx->sndHdr.seLength[i]);
	}

	for (u8 i = 0; i < LAYERS; i++)
	{
		ctx->layers[i]     = calloc(SCREEN_SIZE, 1);
		ctx->prevLayers[i] = calloc(SCREEN_SIZE, 1);
	}

	ctx->prevFrame = 0;

	ctx->frameRate    = speedTable[8 - ctx->sndHdr.playbackSpeed];
	ctx->bgmFrameRate = speedTable[8 - ctx->sndHdr.recordedSpeed];
	
	return -1;
}

void ppmGetThumbnail(ppm_ctx_t *ctx, u32 *out)
{
	u8 *rawData = ctx->thumbnail;

	for (int y = 0; y < 48; y += 8)
	for (int x = 0; x < 64; x += 8)
	for (int l = 0; l <  8; l += 1)
	for (int p = 0; p <  8; p += 2)
	{
		out[(y + l) * 64 + (x + p + 0)] = ppmThumbnailPalette[*rawData  & 0xf];
		out[(y + l) * 64 + (x + p + 1)] = ppmThumbnailPalette[*rawData++ >> 4];
	}
}

void ppmDone(ppm_ctx_t *ctx)
{
	free(ctx->audioFrames);

	free(ctx->videoOffsets);

	free(ctx->bgmData);

	for (u8 i = 0; i < SE_CHANNELS; i++)
		free(ctx->seData[i]);

	free(ctx->videoData);

	for (u8 i = 0; i < LAYERS; i++)
	{
		free(ctx->layers[i]);
		free(ctx->prevLayers[i]);
	}
}