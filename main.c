#include <portaudio.h>
#include <stdio.h>

#define SAMPLE_RATE 44100
#define BUFFER_SIZE 1024
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

    snprintf(command, 512, "yt-dlp -o - %s 2> /dev/null | ffmpeg -i pipe:0 -ac 2 -ar 44100 -f f32le pipe:1 2> /dev/null", argv[1]);
    
    FILE *ytdl_pipe = popen(command, "r");

    if (!ytdl_pipe) {
        printf("Can't open ytdl\n");
        return 1;
    }

    /*FILE *file_d = popen("ffmpeg", "r");*/
    /*if (!file_d) {*/
    /*    printf("error\n");*/
    /*    return 1;*/
    /*}*/

    PaError err;
    err = Pa_Initialize();
    if (err != paNoError) {
        printf("PortAudio error: %s\n", Pa_GetErrorText(err));

        return 1;
    }

    PaStream *stream;

    /*Pa_OpenDefaultStream(PaStream **stream, int numInputChannels, int
     * numOutputChannels, PaSampleFormat sampleFormat, double sampleRate,
     * unsigned long framesPerBuffer, PaStreamCallback *streamCallback, void
     * *userData)*/
    err = Pa_OpenDefaultStream(&stream, 0, CHANNELS, paFloat32, SAMPLE_RATE,
                               BUFFER_SIZE, NULL, NULL);
    if (err != paNoError) {
        Pa_Terminate();
        printf("PortAudio error: %s\n", Pa_GetErrorText(err));
        return 1;
    }

    printf("Starting stream..\n");
    err = Pa_StartStream(stream);
    if (err != paNoError) {
        Pa_Terminate();
        printf("[StartStream] PortAudio error: %s\n", Pa_GetErrorText(err));
        return 1;
    }

    float buffer[BUFFER_SIZE * CHANNELS];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, sizeof(float), BUFFER_SIZE * CHANNELS,
                               ytdl_pipe)) > 0) {
        err = Pa_WriteStream(stream, buffer, bytes_read / CHANNELS);
        if (err) {
            printf("[WriteStream] PortAudio error: %s\n", Pa_GetErrorText(err));
        }
    }

    pclose(ytdl_pipe);
    /*pclose(file_d);*/
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
