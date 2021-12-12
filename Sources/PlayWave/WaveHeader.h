//
// CWavHeader.h
//

typedef struct __attribute__((__packed__)) {
    char id[4];
    unsigned int size;
} ChunkHeader;

typedef struct __attribute__((__packed__)) {
    unsigned short  audioFormat;    // Аудио формат, полный список можно получить здесь http://audiocoding.ru/wav_formats.txt
    unsigned short  numChannels;    // Количество каналов. Моно = 1, Стерео = 2 и т.д.
    unsigned int    sampleRate;     // Частота дискретизации. 8000 Гц, 44100 Гц и т.д.
    unsigned int    byteRate;       // sampleRate * numChannels * bitsPerSample/8
    unsigned short  blockAlign;     // numChannels * bitsPerSample/8
    unsigned short  bitsPerSample;  // 8 бит, 16 бит и т.д.
} WaveFormat;

typedef struct __attribute__((__packed__)) {
    ChunkHeader riffChunk;          // "RIFF" (0x52494646 в big-endian представлении)
    char format[4];                 // "WAVE" (0x57415645 в big-endian представлении)
    // Формат "WAVE" состоит из двух подцепочек: "fmt " и "data":
    // Подцепочка "fmt " описывает формат звуковых данных:
    ChunkHeader frmChunk;           // "fmt " (0x666d7420 в big-endian представлении)
    WaveFormat  waveFormat;
    ChunkHeader dataChunk;          // "data" (0x64617461 в big-endian представлении)
} WaveHeader;
