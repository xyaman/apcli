#include <portaudio.h>
#include <stdio.h>

#include "include/youtube.h"

#define SAMPLE_RATE 44100
#define BUFFER_SIZE 2048
#define CHANNELS 2

// FFMPEG OUTPUT (progress)
/*size=     720KiB time=00:00:02.29 bitrate=2565.8kbits/s speed= 2.3x
 * bitrate=2565.8kbits/s*/
/*total_size=737280*/
/*out_time_us=2298776*/
/*out_time_ms=2298776*/
/*out_time=00:00:02.298776*/
/*dup_frames=0*/
/*drop_frames=0*/
/*speed= 2.3x*/
/*progress=continue*/

int main(int argc, char **argv) {

    if (argc < 2) {
        printf("You need to specify a youtube url\n");
        return 1;
    }

    /*FILE *ytdl_pipe = popen(*/
    /*    "yt-dlp -o - URL 2> /dev/null | ffmpeg -i pipe:0 -ac 2 -ar 44100 -f f32le pipe:1 -progress pipe:2 2> ffmpeg.txt","r");*/


    char command[512];
    //  download]   0.0% of  156.12MiB at  Unknown B/s ETA 
    snprintf(command, 512, "yt-dlp -o - %s 2> yt-dlp.out | ffmpeg -i pipe:0 -ac %d -ar %d -f f32le pipe:1 2> /dev/null", argv[1], CHANNELS, SAMPLE_RATE);
    printf("Starting yt-dlp and ffmpeg\n");
    FILE *ytdl_proc = popen(command, "r");
    if (!ytdl_proc) {
        printf("Can't open ytdl\n");
        return 1;
    }

    yt_track_info info = yt_get_info(argv[1]);
    printf("Title: %s\n", info.title);
    printf("Duration: %s\n", info.string_duration);

    PaError err;

    // avoid alsa errors (at least in my pc)
    FILE *og_stderr = stderr;
    freopen("/dev/null", "w", stderr);

    printf("Initializing PortAudio\n");
    err = Pa_Initialize();
    if (err != paNoError) {
        stderr = og_stderr;
        printf("PortAudio error: %s\n", Pa_GetErrorText(err));
        return 1;
    }

    stderr = og_stderr;

    PaStream *stream;
    err = Pa_OpenDefaultStream(&stream, 0, CHANNELS, paFloat32, SAMPLE_RATE,
                               BUFFER_SIZE, NULL, NULL);
    if (err != paNoError) {
        Pa_Terminate();
        printf("PortAudio error: %s\n", Pa_GetErrorText(err));
        return 1;
    }

    printf("Starting PortAudio stream..\n");
    err = Pa_StartStream(stream);
    if (err != paNoError) {
        Pa_Terminate();
        printf("[StartStream] PortAudio error: %s\n", Pa_GetErrorText(err));
        return 1;
    }

    float buffer[BUFFER_SIZE * CHANNELS];
    size_t bytes_read;
    printf("Playing...\n");
    while ((bytes_read = fread(buffer, sizeof(float), BUFFER_SIZE * CHANNELS,
                               ytdl_proc)) > 0) {
        err = Pa_WriteStream(stream, buffer, bytes_read / CHANNELS);
        if (err) {
            printf("[WriteStream] PortAudio error: %s\n", Pa_GetErrorText(err));
        }
    }

    pclose(ytdl_proc);
    err = Pa_CloseStream(stream);
    if (err != paNoError) {
        Pa_Terminate();
        printf("PortAudio error: %s\n", Pa_GetErrorText(err));
        return 1;
    }

    err = Pa_Terminate();
    if (err != paNoError) {
        printf("PortAudio error: %s\n", Pa_GetErrorText(err));

        return 1;
    }

    return 0;
}
