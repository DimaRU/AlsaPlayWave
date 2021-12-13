//
// WaveHeader.h
//

typedef struct __attribute__((__packed__)) {
    char id[4];
    unsigned int size;
} ChunkHeader;

typedef struct __attribute__((__packed__)) {
    unsigned short  audioFormat;    // PCM = 1
    unsigned short  numChannels;    // Количество каналов. Моно = 1, Стерео = 2 и т.д.
    unsigned int    sampleRate;     // Частота дискретизации. 8000 Гц, 44100 Гц и т.д.
    unsigned int    byteRate;       // sampleRate * numChannels * bitsPerSample/8
    unsigned short  blockAlign;     // numChannels * bitsPerSample/8
    unsigned short  bitsPerSample;  // 8 бит, 16 бит и т.д.
} WaveFormat;
