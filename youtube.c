#include <stdio.h>
#include <string.h>

#include "include/youtube.h"

/* Blocking function. The return value will be empty in case of error */
yt_track_info yt_get_info(char *url) {
    yt_track_info info;

    // TODO: sanitize url (remove playlists/info params etc)

    char command[512];
    snprintf(command, 512, "yt-dlp --print 'T%%(title)s\nD%%(duration_string)s' %s --no-download", url);
    FILE *ytdl = popen(command, "r");

    if (!ytdl) {
        return info;
    }

    char buffer[128];
    int i = 0;
    while(!feof(ytdl)){
        while (fgets(buffer, 128, ytdl)) {
            // Replace \n character with \0
            int nl = strlen(buffer) - 1;
            buffer[nl] = '\0';

            switch (buffer[0]) {
                case 'T': 
                    // we want to ignore the identifier
                    strcpy(info.title, &buffer[1]);
                    break;

                case 'D': 
                    // we want to ignore the identifier
                    strcpy(info.string_duration, &buffer[1]);
                    break;
                
                default:
                    printf("Unkown param\n");
                break;
            }
        }
    }

    pclose(ytdl);
    return info;
}

/*int main(void) {*/
/*    yt_get_info("'https://www.youtube.com/watch?v=jmKRgqWGrWc'");*/
/*}*/
