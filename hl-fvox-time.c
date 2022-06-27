#include <stdio.h>
#include <time.h>
#include <SDL2/SDL.h>
#include "sfx.h"

SDL_AudioDeviceID deviceId;
SDL_AudioSpec wavSpec;
SDL_RWops *rw;
Uint32 wavLength;
Uint8 *wavBuffer;

void loadAndQueue(void *file, unsigned int len)
{
    rw = SDL_RWFromConstMem(file, len);
    SDL_LoadWAV_RW(rw, 1, &wavSpec, &wavBuffer, &wavLength);
    SDL_QueueAudio(deviceId, wavBuffer, wavLength);
};

void queueDecimal(int num)
{
    // FIXME: No sound file for zero, so I guess we just say fuck that noise and exit?
    if (num == 0)
        exit(1);
    if (num < 20)
    {
        loadAndQueue(sfx[num], sfx_len[num]);
    }
    else
    {
        loadAndQueue(sfx[num / 10 + 18], sfx_len[num / 10 + 18]);
        loadAndQueue(sfx[num % 10], sfx_len[num % 10]);
    }
}

int main(int argc, char **argv)
{
    SDL_Init(SDL_INIT_AUDIO);

    rw = SDL_RWFromConstMem(time_is_now_wav, time_is_now_wav_len);
    SDL_LoadWAV_RW(rw, 1, &wavSpec, &wavBuffer, &wavLength);
    deviceId = SDL_OpenAudioDevice(NULL, 0, &wavSpec, NULL, 0);
    SDL_QueueAudio(deviceId, wavBuffer, wavLength);

    time_t my_time;
    struct tm *timeinfo;
    time(&my_time);
    timeinfo = localtime(&my_time);

    queueDecimal(timeinfo->tm_hour);
    queueDecimal(timeinfo->tm_min);
    loadAndQueue(period_wav, period_wav_len);

    SDL_PauseAudioDevice(deviceId, 0);

    while (SDL_GetQueuedAudioSize(deviceId) > 0)
    {
        SDL_Delay(100);
    }

    SDL_CloseAudioDevice(deviceId);
    SDL_FreeWAV(wavBuffer);
    SDL_Quit();

    return 0;
}