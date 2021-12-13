//
// AlsaPlayWave.c
//

#include <alsa/asoundlib.h>
#include "WaveHeader.h"
#include <strings.h>

static int playWaveData(const char *audioDevice, const WaveFormat *wf, unsigned int numSamples, const void *buff);

static const void *getChunkData(const void *ptr, char *id) {
	if (strncmp(ptr, id, 4) == 0) 
		return ptr + sizeof(ChunkHeader);
	else
		return NULL;
}

static const void *nextChunk(const void *ptr) {
	const ChunkHeader *header = ptr;
	return ptr + header->size + sizeof(ChunkHeader);
}

int playWave(const void *data, const char *audioDevice) {
	const void *ptr = data;
	const void *format = getChunkData(ptr, "RIFF");
	if (format == NULL || strncmp(format, "WAVE", 4)) {
			return -1;
	}
	ptr = format + 4;
	const WaveFormat *wf = getChunkData(ptr, "fmt ");
	if (wf == NULL || wf->audioFormat != 1) {
			return -1;
	}
	const void *wavData;
	do {
		ptr = nextChunk(ptr);
		wavData = getChunkData(ptr, "data");
	} while (wavData == NULL);

	unsigned int wavSize = ((ChunkHeader *)ptr)->size;
	unsigned int numSamples = wavSize / (wf->bitsPerSample / 8 * wf->numChannels);
	return playWaveData(audioDevice, wf, numSamples, wavData);
}

int playWaveData(const char *audioDevice, const WaveFormat *wf, unsigned int numSamples, const void *buff) {
	snd_pcm_t *pcm_handle;
	snd_pcm_hw_params_t *params;
	int err;
	unsigned int rate = wf->sampleRate;

	err = snd_pcm_open(&pcm_handle, audioDevice, SND_PCM_STREAM_PLAYBACK, 0);
	if (err < 0) return err;

	snd_pcm_hw_params_alloca(&params);
	snd_pcm_hw_params_any(pcm_handle, params);

	err = snd_pcm_hw_params_set_access(pcm_handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
	if (err < 0) return err;

	snd_pcm_format_t format;
	if (wf->bitsPerSample == 8) {
		format = SND_PCM_FORMAT_U8;
	} else if (wf->bitsPerSample == 16) {
		format = SND_PCM_FORMAT_S16;
	} else if (wf->bitsPerSample == 24) {
		format = SND_PCM_FORMAT_S24;
	}
	err = snd_pcm_hw_params_set_format(pcm_handle, params, format);
	if (err < 0) return err;

	err = snd_pcm_hw_params_set_channels(pcm_handle, params, wf->numChannels);
	if (err < 0) return err;

	err = snd_pcm_hw_params_set_rate_near(pcm_handle, params, &rate, 0);
	if (err < 0) return err;

	/* Write parameters */
	err = snd_pcm_hw_params(pcm_handle, params);
	if (err < 0) return err;

	err = snd_pcm_writei(pcm_handle, buff, numSamples);
	if (err == -EPIPE)
		snd_pcm_prepare(pcm_handle);
	snd_pcm_drain(pcm_handle);
	snd_pcm_close(pcm_handle);

	return err;
}

const char *getErrorMessage(int error) 
{
	return snd_strerror(error);
}
