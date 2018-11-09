#include <Bela.h>
#include "gen_exported.h"

CommonState* gState = NULL;

void Bela_userSettings(BelaInitSettings *settings)
{
	settings->uniformSampleRate = 1;
	settings->interleave = 0;
	settings->analogOutputsPersist = 0;
}

bool setup(BelaContext *context, void *userData)
{
	if(context->flags & BELA_FLAG_INTERLEAVED)
	{
		fprintf(stderr, "You need a non-interleaved buffer\n");
		return 0;
	}
	if(context->analogSampleRate != context->audioSampleRate)
	{
		fprintf(stderr, "You need the analog and audio channels to have the same sampling rate\n");
		return 0;
	}
	gState = (CommonState*)gen_exported::create(context->audioSampleRate, context->audioFrames);
	return true;
}

void render(BelaContext *context, void *userData)
{
	int nFrames = context->audioFrames;

	int nInChannels = context->audioInChannels + context->analogInChannels;
	// taking pointers to the input channels
	t_sample* ip[nInChannels];
	for (unsigned int i = 0; i < context->audioInChannels; i++) {
		ip[i] = (t_sample*)context->audioIn + (i * nFrames);
	}
	for (unsigned int i = 0; i < context->audioInChannels; i++) {
		ip[i + context->audioInChannels] = (t_sample*)context->analogIn + (i * nFrames);
	}

	int nOutChannels = context->audioOutChannels + context->analogOutChannels;
	// taking pointers to the output channels
	t_sample* op[nOutChannels];
	for (unsigned int i = 0; i < context->audioOutChannels; i++) {
		op[i] = (t_sample*)context->audioOut + (i * nFrames);
	}
	for (unsigned int i = 0; i < context->analogOutChannels; i++) {
		op[i + context->audioOutChannels] = (t_sample*)context->analogOut + (i * nFrames);
	}

	gen_exported::perform(gState, ip, nInChannels, op, nOutChannels, nFrames);
}

void cleanup(BelaContext *context, void *userData)
{
	if (gState) {
		gen_exported::destroy(gState);
	}
}
