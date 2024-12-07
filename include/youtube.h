typedef struct yt_track_info {
    char title[128];
    char string_duration[16]; // "00:00:00"
} yt_track_info;

yt_track_info yt_get_info(char *url);
