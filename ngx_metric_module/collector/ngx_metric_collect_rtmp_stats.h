/*
 *  ngx_metric_collect_rtmp_stats.h
 *
 *  Created on: Sep 11, 2019
 *      Author: namnd
 */

#ifndef METRIC_COLLECTOR_RTMP_H_
#define METRIC_COLLECTOR_RTMP_H_

#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>
#include <libxml2/libxml/parser.h>
#include "ngx_rtmp.h"
#include "global.h"

#define RTMP_INTRO "# collect RTMP from worker\n"

#define RTMP_HEADER "{ngx_version=\"%s\",rtmp_version=\"%s\"}"
#define RTMP_LINE_UL_FORMAT "node_rtmp_%s%s %ul\n"
#define RTMP_LINE_D_FORMAT "node_rtmp_%s%s %d\n"
#define RTMP_LINE_UNIT_UL_FORMAT "node_rtmp_%s_%s%s %ul\n"
#define RTMP_LINE_UNIT_UZ_FORMAT "node_rtmp_%s_%s%s %uz\n"

#define STREAM_HEADER "{pid=\"%ul\",app_name=\"%s\",session_id =\"\",stream_name=\"%s\"}"
#define STREAM_LINE_UNIT_UZ_FORMAT "node_rtmp_stream_%s_%s%s %uz\n"
#define STREAM_LINE_D_FORMAT "node_rtmp_stream_%s%s %d\n"

#define CLIENT_HEADER "{pid=\"%ul\",app_name=\"%s\",session_id =\"\",stream_name=\"%s\",client=\"%s\"}"
#define CLIENT_LINE_UNIT_FORMAT "node_rtmp_client_%s_%s%s %ul\n"
#define CLIENT_LINE_LD_FORMAT "node_rtmp_client_%s%s %ul\n"
#define CLIENT_LINE_D_FORMAT "node_rtmp_client_%s%s %d\n"

#define VIDEO_HEADER "{pid=\"%ul\",app_name=\"%s\",session_id =\"\",stream_name=\"%s\",codec=\"%s\",res=\"%dx%d\"}"
#define VIDEO_LINE_UNIT_FORMAT "node_rtmp_meta_video_%s_%s%s %ul\n"
#define VIDEO_LINE_UZ_FORMAT "node_rtmp_meta_video_%s%s %uz\n"
#define VIDEO_LINE_D_FORMAT "node_rtmp_meta_video_%s%s %d\n"

#define AUDIO_HEADER "{pid=\"%ul\",app_name=\"%s\",session_id =\"\",stream_name=\"%s\",codec=\"%s\",channels=\"%d\"}"
#define AUDIO_LINE_UNIT_UZ_FORMAT "node_rtmp_meta_audio_%s_%s%s %uz\n"
#define AUDIO_LINE_LD_FORMAT "node_rtmp_meta_audio_%s%s %ul\n"
#define AUDIO_LINE_D_FORMAT "node_rtmp_meta_audio_%s%s %d\n"

#define RTMP_CHILD_LINE_FORMAT "node_rtmp_%s_%s %s\n"

u_char *ngx_curl(ngx_log_t *log, char *inurl);

unsigned int xmlIsNode(const u_char *nodeName, const char *str);

rtmp_t getRTMP(u_char *buffer);

u_char *ngx_http_get_rtmp_stats(ngx_http_request_t *r, u_char *buf, char *rtmpUrl);

#endif /* METRIC_COLLECTOR_RTMP_H_ */
