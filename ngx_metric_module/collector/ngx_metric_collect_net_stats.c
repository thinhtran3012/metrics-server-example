#include "ngx_metric_collect_net_stats.h"

int match_tcp_ext(char *name)
{
	return start_with(name, "Listen") || start_with(name, "Syncookies") || (strcmp(name, "TCPSynRetrans") == 0) || (strcmp(name, "InErrors") == 0);
}

int match_ip_ext(char *name)
{
	return (strcmp(name, "InOctets") == 0) || (strcmp(name, "OutOctets") == 0);
}

int match_icmp(char *name)
{
	return (strcmp(name, "InErrors") == 0) || (strcmp(name, "InMsgs") == 0) || (strcmp(name, "OutMsgs") == 0);
}

int match_tcp(char *name)
{
	return (strcmp(name, "ActiveOpens") == 0) || (strcmp(name, "InSegs") == 0) || (strcmp(name, "OutSegs") == 0) || (strcmp(name, "PassiveOpens") == 0) || (strcmp(name, "RetransSegs") == 0) || (strcmp(name, "CurrEstab") == 0) || (strcmp(name, "InErrors") == 0) || (strcmp(name, "InErrs") == 0);
}

int match_udp(char *name)
{
	return (strcmp(name, "InErrors") == 0) || (strcmp(name, "InDatagrams") == 0) || (strcmp(name, "NoPortss") == 0) || (strcmp(name, "OutDatagrams") == 0);
}

u_char *parse_netstas_key_value(u_char *buf, char *nameString, char *valueString, char *key, ngx_http_request_t *r)
{
	node_array_t nameParts = str_split(nameString, ' ');
	node_array_t valueParts = str_split(valueString, ' ');
	int minLength = (int)ngx_min(nameParts.len, valueParts.len);
	char **nameData = nameParts.data;
	char **valueData = valueParts.data;
	int i = 0;
	for (i = 1; i < minLength; i++)
	{
		char *name = *(nameData + i);
		char *value = *(valueData + i);
		if ((strcmp(key, "TcpExt") == 0) && match_tcp_ext(name))
		{
			buf = ngx_sprintf(buf, TCP_EXT_FORMAT, name, name, name, name, atol(value));
		}
		else if ((strcmp(key, "IpExt") == 0) && match_ip_ext(name))
		{
			buf = ngx_sprintf(buf, IP_EXT_FORMAT, name, name, name, name, atol(value));
		}
		
		if (name)
		{
			free(name);
		}
		if (value)
		{
			free(value);
		}
	}
	if (nameData)
	{
		free(nameData);
	}
	if (valueData)
	{
		free(valueData);
	}
	return buf;
}

u_char *parse_snmp_key_value(u_char *buf, char *nameString, char *valueString, char *key)
{
	node_array_t nameParts = str_split(nameString, ' ');
	node_array_t valueParts = str_split(valueString, ' ');
	size_t minLength = ngx_min(nameParts.len, valueParts.len);
	char **nameData = nameParts.data;
	char **valueData = valueParts.data;
	int i = 0;
	int lengFor = (int)minLength;
	for (i = 1; i < lengFor; i++)
	{
		char *name = *(nameData + i);
		char *value = *(valueData + i);
		if ((strcmp(key, "Ip") == 0) && (strcmp(name, "Forwarding") == 0))
		{
			buf = ngx_sprintf(buf, IP_FORMAT, name, name, name, name, atol(value));
		}
		else if ((strcmp(key, "Icmp") == 0) && match_icmp(name))
		{
			buf = ngx_sprintf(buf, ICMP_FORMAT, name, name, name, name, atol(value));
		}
		else if ((strcmp(key, "Tcp") == 0) && match_tcp(name))
		{
			buf = ngx_sprintf(buf, TCP_FORMAT, name, name, name, name, atol(value));
		}
		else if ((strcmp(key, "Udp") == 0) && match_udp(name))
		{
			buf = ngx_sprintf(buf, UDP_FORMAT, name, name, name, name, atol(value));
		}
		else if ((strcmp(key, "UdpLite") == 0) && (strcmp(name, "InErrors") == 0))
		{
			buf = ngx_sprintf(buf, UDP_LITE_FORMAT, name, name, name, name, atol(value));
		}
		
		if (name)
		{
			free(name);
		}
		if (value)
		{
			free(value);
		}
	}
	if (nameData)
	{
		free(nameData);
	}
	if (valueData)
	{
		free(valueData);
	}
	return buf;
}

u_char *read_net_stat_in_file(u_char *buf, char *fileName, ngx_http_request_t *r)
{
	char *content = read_proc_file(fileName, r);
	if (content != NULL)
	{
		node_array_t tokens = str_split(content, '\n');
		int lengFor = (int)tokens.len;
		if (lengFor > 0)
		{
			char **data = tokens.data;
			int i;
			for (i = 0; i < lengFor; i++)
			{
				// read name line
				char *nameString = *(data + i);
				i++;
				// read value line
				char *valueString = *(data + i);
				//
				if (start_with(nameString, "TcpExt:"))
				{
					buf = parse_netstas_key_value(buf, nameString, valueString, "TcpExt", r);
				}
				else if (start_with(nameString, "IpExt:"))
				{
					buf = parse_netstas_key_value(buf, nameString, valueString, "IpExt", r);
				}
				else if (start_with(nameString, "Tcp:"))
				{
					buf = parse_snmp_key_value(buf, nameString, valueString, "Tcp");
				}
				else if (start_with(nameString, "Ip:"))
				{
					buf = parse_snmp_key_value(buf, nameString, valueString, "Ip");
				}
				else if (start_with(nameString, "Icmp:"))
				{
					buf = parse_snmp_key_value(buf, nameString, valueString, "Icmp");
				}
				else if (start_with(nameString, "Udp:"))
				{
					buf = parse_snmp_key_value(buf, nameString, valueString, "Udp");
				}
				else if (start_with(nameString, "UdpLite:"))
				{
					buf = parse_snmp_key_value(buf, nameString, valueString, "UdpLite");
				}

				if (nameString)
				{
					free(nameString);
				}
				if (valueString)
				{
					free(valueString);
				}
			}
			if (data)
			{
				free(data);
			}
		}
		free(content);
	}
	return buf;
}

u_char *ngx_http_get_net_stats(ngx_http_request_t *r, u_char *buf)
{
	// read file /proc/net/netstat
	buf = read_net_stat_in_file(buf, NET_STAT_FILE, r);
	// read file /proc/net/snmp
	buf = read_net_stat_in_file(buf, SNMP_STAT_FILE, r);
	
	ngx_log_error(NGX_LOG_INFO, r->connection->log, 0, "NET STATS DONE!!!");
	return buf;
}