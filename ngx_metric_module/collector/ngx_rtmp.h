/*
 * ngx_rtmp.h
 *
 *  Created on: Sep 13, 2019
 *      Author: namnd
 */

#ifndef NGX_RTMP_H_
#define NGX_RTMP_H_

#include <string.h>
#include <stdlib.h>
typedef struct video_t
{
	int width;
	int height;
	size_t frame_rate;
	char *codec;
	char *profile;
	int compat;
	char *level;
} video_t;

typedef struct audio_t
{
	char *codec;
	char *profile;
	int channels;
	size_t sample_rate;
} audio_t;

typedef struct meta_t
{
	video_t video;
	audio_t audio;
} meta_t;

typedef struct client_t
{
	unsigned long id;
	char *address;
	unsigned long time;
	char *flashver;
	char *swfurl;
	int dropped;
	int avsync;
	unsigned long timestamp;
} client_t;

typedef struct stream_t
{
	char *name;
	unsigned long time;
	size_t bw_in;
	size_t bytes_in;
	size_t bw_out;
	size_t bytes_out;
	size_t bw_audio;
	size_t bw_video;
	client_t client;
	meta_t meta;
	int nclients;
} stream_t;

typedef struct app_t
{
	char *name;
	stream_t *streams;
	int nstreams;
} app_t;

typedef struct rtmp_t
{
	char *ngx_version;
	char *ngx_rtmp_version;
	char *compiler;
	char *built;
	unsigned long pid;
	unsigned long uptime;
	int naccepted;
	size_t bw_in;
	size_t bytes_in;
	size_t bw_out;
	size_t bytes_out;
	app_t *apps;
	int napps;
} rtmp_t;

void init_audio(audio_t *a);

void init_video(video_t *v);

void init_meta(meta_t *m);

void init_client(client_t *c);

void init_stream(stream_t *s);

void free_stream(stream_t *s);

void init_app(app_t *a);

void free_app(app_t *a);

void init_rtmp(rtmp_t *r);

void free_rtmp(rtmp_t *r);

char *m_strcpy(char *dest, char *source);

char *m_strcat(char *dest, char *source);

#endif /* NGX_RTMP_H_ */
