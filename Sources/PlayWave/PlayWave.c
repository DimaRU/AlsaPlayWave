//
// PlayWave.c
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
	if (format == NULL ||
		strncmp(format, "WAVE", 4)) {
			printf("Bad wav format\n");
			return -1;
	}
	ptr = format + 4;
	const WaveFormat *wf = getChunkData(ptr, "fmt ");
	if (wf == NULL ||
		wf->audioFormat != 1) {
			printf("Bad wav format1\n");
			return -1;
	}
	const void *wavData;
	do {
		ptr = nextChunk(ptr);
		wavData = getChunkData(ptr, "data");
	} while (wavData == NULL);

	unsigned int wavSize = ((ChunkHeader *)ptr)->size;
	unsigned int numSamples = wavSize / (wf->bitsPerSample / 8 * wf->numChannels);
	printf("Samples: %d\n", numSamples);
	return playWaveData(audioDevice, wf, numSamples, wavData);
}

int playWaveData(const char *audioDevice, const WaveFormat *wf, unsigned int numSamples, const void *buff) {
	snd_pcm_t *pcm_handle;
	snd_pcm_hw_params_t *params;
	int err;
	unsigned int rate = wf->sampleRate;

	err = snd_pcm_open(&pcm_handle, audioDevice, SND_PCM_STREAM_PLAYBACK, 0);
	if (err < 0) printf("ERROR: Can't open \"%s\" PCM device. %s\n", audioDevice, snd_strerror(err));

	snd_pcm_hw_params_alloca(&params);
	snd_pcm_hw_params_any(pcm_handle, params);

	err = snd_pcm_hw_params_set_access(pcm_handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
	if (err < 0) printf("ERROR: Can't set interleaved mode. %s\n", snd_strerror(err));

	snd_pcm_format_t format;
	if (wf->bitsPerSample == 8) {
		format = SND_PCM_FORMAT_S8;
	} else if (wf->bitsPerSample == 16) {
		format = SND_PCM_FORMAT_S16;
	} else if (wf->bitsPerSample == 24) {
		format = SND_PCM_FORMAT_S24;
	}
	err = snd_pcm_hw_params_set_format(pcm_handle, params, format);
	if (err < 0) printf("ERROR: Can't set format. %s\n", snd_strerror(err));

	err = snd_pcm_hw_params_set_channels(pcm_handle, params, wf->numChannels);
	if (err < 0) printf("ERROR: Can't set channels number. %s\n", snd_strerror(err));

	err = snd_pcm_hw_params_set_rate_near(pcm_handle, params, &rate, 0);
	if (err < 0) printf("ERROR: Can't set rate. %s\n", snd_strerror(err));

	/* Write parameters */
	err = snd_pcm_hw_params(pcm_handle, params);
	if (err < 0) printf("ERROR: Can't set harware parameters. %s\n", snd_strerror(err));

	printf("PCM name: '%s'\n", snd_pcm_name(pcm_handle));
	printf("PCM state: %s\n", snd_pcm_state_name(snd_pcm_state(pcm_handle)));
	unsigned int tmp;
	snd_pcm_hw_params_get_channels(params, &tmp);
	printf("channels: %i %s\n ", tmp, tmp == 1 ? "(mono)": "(stereo)");
	snd_pcm_hw_params_get_rate(params, &tmp, 0);
	printf("rate: %d bps\n", tmp);
	printf("Samples: %d\n", numSamples);

	err = snd_pcm_writei(pcm_handle, buff, numSamples);
	if (err == -EPIPE) {
		snd_pcm_prepare(pcm_handle);
	} else if (err < 0) {
		printf("ERROR. Can't write to PCM device. %s\n", snd_strerror(err));
	}
	snd_pcm_drain(pcm_handle);
	snd_pcm_close(pcm_handle);

	return 0;
}

void listAlsadev(const char *devname)
{
    char** hints;
    int    err;
    char** curHint;
    char*  name;
    char*  desc;
    char*  ioid;

    /* Enumerate sound devices */
    err = snd_device_name_hint(-1, devname, (void***)&hints);
    if (err != 0) {
        printf("Cannot get device names\n");
    }
    curHint = hints;
    while (*curHint != NULL) {
        name = snd_device_name_get_hint(*curHint, "NAME");
        desc = snd_device_name_get_hint(*curHint, "DESC");
        ioid = snd_device_name_get_hint(*curHint, "IOID");

        printf("Name of device: %s\n", name);
        printf("Description of device: %s\n", desc);
        printf("I/O type of device: %s\n", ioid);
        printf("\n");

        if (name && strcmp("null", name)) free(name);
        if (desc && strcmp("null", desc)) free(desc);
        if (ioid && strcmp("null", ioid)) free(ioid);
        curHint++;
    }

    snd_device_name_free_hint((void**)hints);
}
