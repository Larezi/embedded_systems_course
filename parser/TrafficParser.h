#ifndef TRAFFICPARSER_H
#define TRAFFICPARSER_H

// Error codes
#define TRAFFIC_LEN_ERROR    -10
#define TRAFFIC_COLOR_ERROR  -11
#define TRAFFIC_FORMAT_ERROR -12
#define TRAFFIC_NULL_ERROR   -13

int traffic_parse(char *sequence);

#endif