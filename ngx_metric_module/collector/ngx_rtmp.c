/*
 * ngx_rtmp.c
 *
 *  Created on: Sep 13, 2019
 *      Author: namnd
 */

#include "ngx_rtmp.h"

void init_audio(audio_t *a)
{
	a->codec = (char *)calloc(256, sizeof(char));
	a->profile = (char *)calloc(256, sizeof(char));
	a->channels = 0;
	a->sample_rate = 0;
}

void init_video(video_t *v)
{
	v->codec = (char *)calloc(256, sizeof(char));
	v->profile = (char *)calloc(256, sizeof(char));
	v->level = (char *)calloc(256, sizeof(char));
	v->width = 0;
	v->height = 0;
	v->frame_rate = 0;
	v->compat = 0;
}

void init_meta(meta_t *m)
{
	init_video(&m->video);
	init_audio(&m->audio);
}

void init_client(client_t *c)
{
	c->address = (char *)calloc(256, sizeof(char));
	c->flashver = (char *)calloc(256, sizeof(char));
	c->swfurl = (char *)calloc(256, sizeof(char));
	c->id = 0L;
	c->time = 0L;
	c->dropped = 0;
	c->avsync = 0;
	c->timestamp = 0L;
}

void free_client(client_t *c)
{
	free(c->address);
	free(c->flashver);
	free(c->swfurl);
}

void init_stream(stream_t *s)
{
	s->name = (char *)calloc(256, sizeof(char));
	init_client(&s->client);
	s->time = 0L;
	s->bw_in = 0L;
	s->bytes_in = 0L;
	s->bw_out = 0L;
	s->bytes_out = 0L;
	s->bw_audio = 0L;
	s->bw_video = 0L;
	s->nclients = 0;
}

void free_stream(stream_t *s)
{
	free(s->name);
	free_client(&s->client);
}

void init_app(app_t *a)
{
	a->name = (char *)calloc(256, sizeof(char));
	a->streams = NULL;
	a->nstreams = 0;
}

void free_app(app_t *a)
{
	free(a->name);
	if(a->streams){
		free(a->streams);
	}
}

void init_rtmp(rtmp_t *r)
{
	r->ngx_version = (char *)calloc(256, sizeof(char));
	r->ngx_rtmp_version = (char *)calloc(256, sizeof(char));
	r->compiler = (char *)calloc(256, sizeof(char));
	r->built = (char *)calloc(256, sizeof(char));
	r->pid = 0L;
	r->uptime = 0L;
	r->naccepted = 0;
	r->apps = (app_t *)calloc(1, sizeof(app_t));
	r->bw_in = 0L;
	r->bytes_in = 0L;
	r->bw_out = 0L;
	r->bytes_out = 0L;
	r->napps = 0;
}

void free_rtmp(rtmp_t *r) 
{
	free(r->ngx_version);
	free(r->ngx_rtmp_version);
	free(r->compiler);
	free(r->built);
	free(r->apps);
}

char *m_strcpy(char *dest, char *source)
{
	dest = realloc(dest, (strlen(source) + 1L) * sizeof(char));
	return strcpy(dest, source);
}

char *m_strcat(char *dest, char *source)
{
	dest = realloc(dest, (strlen(dest) + strlen(source) + 24L) * sizeof(char));
	return strcat(dest, source);
}
