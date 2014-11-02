#ifndef H264PARSER_H
#define H264PARSER_H


int  OpenBitstreamFile (char *fn);

void rtp_unpackage(char *bufIn,int len);

#endif//H264PARSER_H