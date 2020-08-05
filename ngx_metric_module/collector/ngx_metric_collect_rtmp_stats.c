/*
 *  ngx_metric_collect_rtmp_stats.c
 *
 *  Created on: Sep 11, 2019
 *      Author: namnd
 */
#include "ngx_metric_collect_rtmp_stats.h"

void init_string(ngx_str_t *s)
{
	s->len = 0;
	s->data = (u_char *)calloc(1, sizeof(u_char));
	if (s->data == NULL)
	{
		fprintf(stderr, "calloc() failed\n");
		return;
	}
	s->data[0] = '\0';
}

unsigned int xmlIsNode(const u_char *nodeName, const char *str)
{
	return xmlStrcmp(nodeName, (const u_char *)str) == 0;
}

// for curl
static size_t writefunc(void *contents, size_t size, size_t nmemb, ngx_str_t *s)
{
	size_t realsize = s->len + size * nmemb;
	s->data = realloc(s->data, (realsize + 1) * sizeof(u_char));
	if (s->data == NULL)
	{
		fprintf(stderr, "realloc() failed\n");
		return 0;
	}
	memcpy(s->data + s->len, contents, size * nmemb);
	s->data[realsize] = '\0';
	s->len = realsize;
	return size * nmemb;
}

u_char *ngx_curl(ngx_log_t *log, char *inurl)
{
	//	getNetStats();
	CURL *curl;
	CURLcode res;

	curl = curl_easy_init();
	if (curl)
	{
		ngx_str_t s;
		init_string(&s);
		curl_easy_setopt(curl, CURLOPT_URL, inurl);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);
		res = curl_easy_perform(curl);
		if (res != CURLE_OK)
		{
			ngx_log_error(NGX_LOG_INFO, log, 0, "url: %s", inurl);
			ngx_log_error(NGX_LOG_INFO, log, 0, "libcurl said %s", curl_easy_strerror(res));
		}
		/* always cleanup */
		curl_easy_cleanup(curl);
		return s.data;
	}
	return NULL;
}

/*  parse xml */

char *getChildNodeName(const u_char *name)
{
	if (xmlIsNode(name, "server") == 0)
	{
		return "server";
	}
	else if (xmlIsNode(name, "application") == 0)
	{
		return "application";
	}
	else if (xmlIsNode(name, "live") == 0)
	{
		return "live";
	}
	else if (xmlIsNode(name, "stream") == 0)
	{
		return "stream";
	}
	else if (xmlIsNode(name, "client") == 0)
	{
		return "client";
	}
	else if (xmlIsNode(name, "meta") == 0)
	{
		return "meta";
	}
	else if (xmlIsNode(name, "video") == 0)
	{
		return "meta_video";
	}
	else if (xmlIsNode(name, "audio") == 0)
	{
		return "meta_audio";
	}
	else
	{
		return NULL;
	}
}

xmlNodePtr getChildNode(xmlNodePtr node)
{
	if (xmlNodeIsText(node))
	{
		node = node->next;
	}
	char *chName = getChildNodeName(node->name);
	if (chName)
	{
		if (xmlNodeIsText(node->children))
		{
			return node->children->next;
		}
		else
		{
			return node->children;
		}
	}
	else
	{
		return node;
	}
}

//parser a audio
audio_t getAudio(xmlNodePtr root)
{
	audio_t audio; // = (audio_t*)malloc(sizeof(audio_t));
	init_audio(&audio);
	xmlNodePtr node = getChildNode(root);
	while (node != NULL)
	{
		if (!xmlNodeIsText(node))
		{
			char *content = (char *)xmlNodeGetContent(node->children);
			if (xmlIsNode(node->name, "codec"))
			{
				audio.codec = strdup(content);
			}
			else if (xmlIsNode(node->name, "profile"))
			{
				audio.profile = strdup(content);
			}
			else if (xmlIsNode(node->name, "channels"))
			{
				audio.channels = atoi(content);
			}
			else if (xmlIsNode(node->name, "sample_rate"))
			{
				audio.sample_rate = atol(content);
			}
			free(content);
		}
		node = node->next;
	}
	return audio;
}

//parser a video
video_t getVideo(xmlNodePtr root)
{
	video_t video; // = (video_t*)malloc(sizeof(video_t));
	init_video(&video);
	xmlNodePtr node = getChildNode(root);
	while (node != NULL)
	{
		if (!xmlNodeIsText(node))
		{
			char *content = (char *)xmlNodeGetContent(node->children);
			if (xmlIsNode(node->name, "width"))
			{
				video.width = atoi(content);
			}
			else if (xmlIsNode(node->name, "height"))
			{
				video.height = atoi(content);
			}
			else if (xmlIsNode(node->name, "codec"))
			{
				video.codec = strdup(content);
			}
			else if (xmlIsNode(node->name, "profile"))
			{
				video.profile = strdup(content);
			}
			else if (xmlIsNode(node->name, "level"))
			{
				video.level = strdup(content);
			}
			else if (xmlIsNode(node->name, "compat"))
			{
				video.compat = atoi(content);
			}
			else if (xmlIsNode(node->name, "frame_rate"))
			{
				video.frame_rate = atol(content);
			}
			free(content);
		}
		node = node->next;
	}
	//	printf("video: %s, %d, %d\n", video.codec, video.width, video.height);
	return video;
}

//parser a meta
meta_t getMeta(xmlNodePtr root)
{
	meta_t meta; // (meta_t*)malloc(sizeof(meta_t));
	init_meta(&meta);
	xmlNodePtr node = getChildNode(root);
	while (node != NULL)
	{
		if (!xmlNodeIsText(node))
		{
			if (xmlIsNode(node->name, "audio"))
			{
				meta.audio = getAudio(node);
			}
			else if (xmlIsNode(node->name, "video"))
			{
				meta.video = getVideo(node);
			}
		}
		node = node->next;
	}
	return meta;
}

//parser a client
client_t getClient(xmlNodePtr root)
{
	client_t client; // = (client_t*)malloc(sizeof(client_t));
	init_client(&client);
	xmlNodePtr node = getChildNode(root);
	while (node != NULL)
	{
		if (!xmlNodeIsText(node))
		{
			char *content = (char *)xmlNodeGetContent(node->children);
			if (xmlIsNode(node->name, "id"))
			{
				client.id = atol(content);
			}
			else if (xmlIsNode(node->name, "address"))
			{
				client.address = strdup(content);
			}
			else if (xmlIsNode(node->name, "time"))
			{
				client.time = atol(content);
			}
			else if (xmlIsNode(node->name, "flashver"))
			{
				client.flashver = strdup(content);
			}
			else if (xmlIsNode(node->name, "swfurl"))
			{
				client.swfurl = strdup(content);
			}
			else if (xmlIsNode(node->name, "dropped"))
			{
				client.dropped = atoi(content);
			}
			else if (xmlIsNode(node->name, "avsync"))
			{
				client.avsync = atoi(content);
			}
			else if (xmlIsNode(node->name, "timestamp"))
			{
				client.timestamp = atoi(content);
			}
			free(content);
		}
		node = node->next;
	}
	return client;
}

//parser a stream
stream_t getStream(xmlNodePtr root)
{
	stream_t stream; // =(stream_t*) malloc(sizeof(stream_t));
	init_stream(&stream);
	xmlNodePtr node = getChildNode(root);
	while (node != NULL)
	{
		if (!xmlNodeIsText(node))
		{
			char *content = (char *)xmlNodeGetContent(node->children);
			if (xmlIsNode(node->name, "name"))
			{
				stream.name = strdup(content);
			}
			else if (xmlIsNode(node->name, "time"))
			{
				stream.time = atol(content);
			}
			else if (xmlIsNode(node->name, "bw_in"))
			{
				stream.bw_in = atol(content);
			}
			else if (xmlIsNode(node->name, "bytes_in"))
			{
				stream.bytes_in = atol(content);
			}
			else if (xmlIsNode(node->name, "bw_out"))
			{
				stream.bw_out = atol(content);
			}
			else if (xmlIsNode(node->name, "bytes_out"))
			{
				stream.bytes_out = atol(content);
			}
			else if (xmlIsNode(node->name, "bw_audio"))
			{
				stream.bw_audio = atol(content);
			}
			else if (xmlIsNode(node->name, "bw_video"))
			{
				stream.bw_video = atol(content);
			}
			else if (xmlIsNode(node->name, "client"))
			{
				stream.client = getClient(node);
			}
			else if (xmlIsNode(node->name, "meta"))
			{
				stream.meta = getMeta(node);
			}
			else if (xmlIsNode(node->name, "nclients"))
			{
				stream.nclients = atoi(content);
			}
			free(content);
		}
		node = node->next;
	}
	//	printf("stream: { %s, %s, %d, %d, %d }\n", stream.name, (stream.clients[stream.nclients - 1]).address, stream.meta.video.width, stream.meta.video.height, stream.nclients);
	return stream;
}

//parser an application
app_t getApp(xmlNodePtr root)
{

	app_t app; // = (app_t*)malloc(sizeof(app_t));
	init_app(&app);
	xmlNodePtr node = getChildNode(root);
	while (node != NULL)
	{
		if (!xmlNodeIsText(node))
		{
			if (xmlIsNode(node->name, "name"))
			{
				char *content = (char *)xmlNodeGetContent(node->children);
				app.name = strdup(content);
			}
			else if (xmlIsNode(node->name, "live"))
			{
				// streams
				xmlNodePtr cnode = getChildNode(node);
				int si = 0; //stream index
				while (cnode != NULL)
				{
					if (!xmlNodeIsText(cnode))
					{
						if (xmlIsNode(cnode->name, "stream"))
						{
							if (si == 0)
							{
								app.streams = (stream_t *)calloc(1, sizeof(stream_t));
							}
							if (si > 1)
							{
								app.streams = (stream_t *)realloc(app.streams, (si + 1) * sizeof(stream_t));
							}
							*(app.streams + si) = getStream(cnode);
							si++;
						}
					}
					cnode = cnode->next;
				}
				app.nstreams = si;
			}
		}
		node = node->next;
	}
	return app;
}

// parser Applications
int getApps(app_t *apps, xmlNodePtr root)
{
	// get all application in <server></server>
	int ai = 0; // application index
	xmlNodePtr node = getChildNode(root);
	while (node != NULL)
	{
		if (!xmlNodeIsText(node))
		{
			if (xmlIsNode(node->name, "application"))
			{
				if (ai > 1)
				{
					apps = (app_t *)realloc(apps, (ai + 1) * sizeof(app_t));
				}
				*(apps + ai) = getApp(node);
				ai++;
			}
		}
		node = node->next;
	}
	return ai;
}

rtmp_t getRTMP(u_char *buffer)
{
	rtmp_t rtmp;
	init_rtmp(&rtmp);
	xmlDocPtr doc;
	xmlNodePtr cur;
	doc = xmlParseDoc(buffer);
	cur = xmlDocGetRootElement(doc);
	cur = cur->children;
	while (cur != NULL)
	{
		if (!xmlNodeIsText(cur))
		{
			char *ct = (char *)xmlNodeGetContent(cur->children);
			if (xmlIsNode(cur->name, "server"))
			{
				rtmp.napps = getApps(rtmp.apps, cur);
			}
			else if (xmlIsNode(cur->name, "nginx_version"))
			{
				rtmp.ngx_version = strdup(ct);
			}
			else if (xmlIsNode(cur->name, "nginx_rtmp_version"))
			{
				rtmp.ngx_rtmp_version = strdup(ct);
			}
			else if (xmlIsNode(cur->name, "compiler"))
			{
				rtmp.compiler = strdup(ct);
			}
			else if (xmlIsNode(cur->name, "built"))
			{
				rtmp.built = strdup(ct);
			}
			else if (xmlIsNode(cur->name, "pid"))
			{
				rtmp.pid = atol(ct);
			}
			else if (xmlIsNode(cur->name, "uptime"))
			{
				rtmp.uptime = atol(ct);
			}
			else if (xmlIsNode(cur->name, "naccepted"))
			{
				rtmp.naccepted = atoi(ct);
			}
			else if (xmlIsNode(cur->name, "bw_in"))
			{
				rtmp.bw_in = atol(ct);
			}
			else if (xmlIsNode(cur->name, "bytes_in"))
			{
				rtmp.bytes_in = atol(ct);
			}
			else if (xmlIsNode(cur->name, "bw_out"))
			{
				rtmp.bw_out = atol(ct);
			}
			else if (xmlIsNode(cur->name, "bytes_out"))
			{
				rtmp.bytes_out = atol(ct);
			}
			free(ct);
		}
		cur = cur->next;
	}
	xmlFreeDoc(doc);
	return rtmp;
}

u_char *process_xml(ngx_http_request_t *r, u_char* buf, u_char *xmlContent)
{
	u_char *tmpBuf = buf;
	tmpBuf = ngx_sprintf(tmpBuf, RTMP_INTRO);
	rtmp_t rtmp = getRTMP(xmlContent);
	// RTMP
	size_t leng = ngx_strlen(RTMP_HEADER) + ngx_strlen(rtmp.ngx_version) + ngx_strlen(rtmp.ngx_rtmp_version) + 1;
	u_char *rtmpHeader = ngx_pcalloc(r->pool, leng);
	if (rtmpHeader == NULL)
	{
		return buf;
	}
	ngx_sprintf(rtmpHeader, RTMP_HEADER, rtmp.ngx_version, rtmp.ngx_rtmp_version);
	// append
	tmpBuf = ngx_sprintf(tmpBuf, RTMP_LINE_UL_FORMAT, "pid", rtmpHeader, rtmp.pid);
	tmpBuf = ngx_sprintf(tmpBuf, RTMP_LINE_UNIT_UL_FORMAT, "uptime", "seconds", rtmpHeader, rtmp.uptime);
	tmpBuf = ngx_sprintf(tmpBuf, RTMP_LINE_D_FORMAT, "naccepted", rtmpHeader, rtmp.naccepted);
	tmpBuf = ngx_sprintf(tmpBuf, RTMP_LINE_UNIT_UZ_FORMAT, "bw_in", "bits", rtmpHeader, rtmp.bw_in);
	tmpBuf = ngx_sprintf(tmpBuf, RTMP_LINE_UNIT_UZ_FORMAT, "bytes_in", "bytes", rtmpHeader, rtmp.bytes_in);
	tmpBuf = ngx_sprintf(tmpBuf, RTMP_LINE_UNIT_UZ_FORMAT, "bw_out", "bits", rtmpHeader, rtmp.bw_out);
	tmpBuf = ngx_sprintf(tmpBuf, RTMP_LINE_UNIT_UZ_FORMAT, "bytes_out", "bytes", rtmpHeader, rtmp.bytes_out);
	ngx_pfree(r->pool, rtmpHeader);
	//Server/Application/Live/Stream
	app_t *apps = rtmp.apps;
	int ai = 0;
	for (ai = 0; ai < rtmp.napps; ai++)
	{
		app_t app = *(apps + ai);
		if (app.streams != NULL)
		{
			stream_t *streams = app.streams;
			int si = 0;
			for (si = 0; si < app.nstreams; si++)
			{
				stream_t stream = *(streams + si);
				size_t shead_len = ngx_strlen(STREAM_HEADER) + ngx_strlen(app.name) + ngx_strlen(stream.name);
				u_char *sheader = ngx_pcalloc(r->pool, shead_len);

				if (sheader != NULL)
				{
					ngx_sprintf(sheader, STREAM_HEADER, rtmp.pid, app.name, stream.name);
					//
					tmpBuf = ngx_sprintf(tmpBuf, STREAM_LINE_UNIT_UZ_FORMAT, "time", "seconds", sheader, stream.time);
					tmpBuf = ngx_sprintf(tmpBuf, STREAM_LINE_UNIT_UZ_FORMAT, "bw_in", "bits", sheader, stream.bw_in);
					tmpBuf = ngx_sprintf(tmpBuf, STREAM_LINE_UNIT_UZ_FORMAT, "bytes_in", "bytes", sheader, stream.bytes_in);
					tmpBuf = ngx_sprintf(tmpBuf, STREAM_LINE_UNIT_UZ_FORMAT, "bw_out", "bits", sheader, stream.bw_out);
					tmpBuf = ngx_sprintf(tmpBuf, STREAM_LINE_UNIT_UZ_FORMAT, "bytes_out", "bytes", sheader, stream.bytes_out);
					tmpBuf = ngx_sprintf(tmpBuf, STREAM_LINE_UNIT_UZ_FORMAT, "bw_audio", "bits", sheader, stream.bw_audio);
					tmpBuf = ngx_sprintf(tmpBuf, STREAM_LINE_UNIT_UZ_FORMAT, "bw_video", "bits", sheader, stream.bw_video);
					tmpBuf = ngx_sprintf(tmpBuf, STREAM_LINE_D_FORMAT, "nclients", sheader, stream.nclients);
					ngx_pfree(r->pool, sheader);
				}

				// client
				client_t client = stream.client;
				size_t chead_len = ngx_strlen(CLIENT_HEADER) + ngx_strlen(app.name) + ngx_strlen(stream.name) + ngx_strlen(client.address);
				u_char *cheader = ngx_pcalloc(r->pool, chead_len);
				if (cheader != NULL)
				{
					ngx_sprintf(cheader, CLIENT_HEADER, rtmp.pid, app.name, stream.name, client.address);
					//
					tmpBuf = ngx_sprintf(tmpBuf, CLIENT_LINE_LD_FORMAT, "id", cheader, client.id);
					tmpBuf = ngx_sprintf(tmpBuf, CLIENT_LINE_UNIT_FORMAT, "time", "seconds", cheader, client.time);
					tmpBuf = ngx_sprintf(tmpBuf, CLIENT_LINE_D_FORMAT, "dropped", cheader, client.dropped);
					tmpBuf = ngx_sprintf(tmpBuf, CLIENT_LINE_D_FORMAT, "avsync", cheader, client.avsync);
					tmpBuf = ngx_sprintf(tmpBuf, CLIENT_LINE_UNIT_FORMAT, "timestamp", "seconds", cheader, client.timestamp);
					ngx_pfree(r->pool, cheader);
				}
				// video
				video_t video = stream.meta.video;
				size_t vhead_len = ngx_strlen(VIDEO_HEADER) + ngx_strlen(app.name) + ngx_strlen(stream.name) + ngx_strlen(video.codec);
				u_char *vheader = ngx_pcalloc(r->pool, vhead_len);
				if (vheader != NULL)
				{
					ngx_sprintf(vheader, VIDEO_HEADER, rtmp.pid, app.name, stream.name, video.codec, video.width, video.height);
					//
					tmpBuf = ngx_sprintf(tmpBuf, VIDEO_LINE_UZ_FORMAT, "fps", vheader, video.frame_rate);
					tmpBuf = ngx_sprintf(tmpBuf, VIDEO_LINE_D_FORMAT, "compat", vheader, video.compat);
					ngx_pfree(r->pool, vheader);
				}
				//audio
				audio_t audio = stream.meta.audio;
				size_t ahead_len = ngx_strlen(AUDIO_HEADER) + ngx_strlen(app.name) + ngx_strlen(stream.name) + ngx_strlen(audio.codec);
				u_char *aheader = ngx_pcalloc(r->pool, ahead_len);
				if (aheader != NULL)
				{
					ngx_sprintf(aheader, AUDIO_HEADER, rtmp.pid, app.name, stream.name, audio.codec, audio.channels);
					//
					tmpBuf = ngx_sprintf(tmpBuf, AUDIO_LINE_UNIT_UZ_FORMAT, "freq", "Hz", aheader, audio.sample_rate);
					ngx_pfree(r->pool, aheader);
				}
				free_stream(&stream);
			}
		}
		free_app(&app);
	}
	free_rtmp(&rtmp);
	buf = tmpBuf;
	return buf;
}

u_char *ngx_http_get_rtmp_stats(ngx_http_request_t *r, u_char *buf, char *rtmpUrl)
{
	u_char *content = ngx_curl(r->connection->log, rtmpUrl);
	if (content != NULL)
	{
		if(start_with(content, "<?xml")){
			// procress xml format
			buf = process_xml(r, buf, content);
		} else {
			// cat prometheus format
			buf = ngx_sprintf(buf, content);
		}
		free(content);
	}
	ngx_log_error(NGX_LOG_INFO, r->connection->log, 0, "RTMP STATS DONE!!!");
	return buf;
}