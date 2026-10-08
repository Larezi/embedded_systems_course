#include "TrafficParser.h"
#include "TimeParser.h"

#include <string.h>
#include "TrafficParser.h"
#include "TimeParser.h"

int traffic_parse(char *sequence)
{
    // Tarkistetaan NULL
    if (sequence == NULL) {
        return TRAFFIC_NULL_ERROR;
    }

    // "R HHMMSS" = yhteensä 8 merkkiä
    if (strlen(sequence) != 8) {
        return TRAFFIC_LEN_ERROR;
    }

    // Ensimmäinen merkki saa olla vain R, Y tai G
    if (sequence[0] != 'R' &&
        sequence[0] != 'Y' &&
        sequence[0] != 'G') {
        return TRAFFIC_COLOR_ERROR;
    }

    // Toisen merkin pitää olla välilyönti
    if (sequence[1] != ' ') {
        return TRAFFIC_FORMAT_ERROR;
    }

    // Annetaan HHMMSS TimeParserille
    int seconds = time_parse(sequence + 2);

    // Jos TimeParser palautti virheen
    if (seconds < 0) {
        return seconds;
    }

    return seconds;
}