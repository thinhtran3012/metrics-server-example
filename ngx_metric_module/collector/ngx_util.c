// C program for implementation of ftoa()
#include "ngx_util.h"

// replace char by char
int replace_char(char *str, char find, char replace)
{
    int len = strlen(str);
    int i;
    for (i = 0; i <= len; i++)
    {
        if (str[i] == find)
        {
            str[i] = replace;
        }
    }
    return 0;
}

// remove space
int trim(char *str)
{
    int count = 0;
    int index = 0;
    int prev = -1;
    while (str[index])
    {
        if ((str[index] == ' ' || str[index] == '\t') && (prev < 0 || str[prev] == ' '))
        {
            count++; // 2 ' ' liên tiếp hoăc ' ' ở đầu chuỗi => tăng số kí tự bị xoá
        }
        else
        {
            str[index - count] = str[index];
            prev = index;
        }
        index++;
    }
    index -= count; // i is new length
                    //trim right
    if (index && (str[index - 1] == ' ' || str[index - 1] == '\t'))
        str[index - 1] = 0, count++;
    else
        str[index] = 0;
    return count;
}

// replace string by another string
char *replace_str(char *string, const char *substr, const char *replacement)
{
    char *tok = NULL;
    char *newstr = NULL;
    char *oldstr = NULL;
    int oldstr_len = 0;
    int substr_len = 0;
    int replacement_len = 0;

    newstr = strdup(string);
    substr_len = strlen(substr);
    replacement_len = strlen(replacement);

    if (substr == NULL || replacement == NULL)
    {
        return newstr;
    }

    while ((tok = strstr(newstr, substr)))
    {
        oldstr = newstr;
        oldstr_len = strlen(oldstr);
        newstr = (char *)calloc(oldstr_len - substr_len + replacement_len + 1, sizeof(char));

        if (newstr == NULL)
        {
            free(oldstr);
            return NULL;
        }

        memcpy(newstr, oldstr, tok - oldstr);
        memcpy(newstr + (tok - oldstr), replacement, replacement_len);
        memcpy(newstr + (tok - oldstr) + replacement_len, tok + substr_len, oldstr_len - substr_len - (tok - oldstr));
        memset(newstr + oldstr_len - substr_len + replacement_len, 0, 1);

        free(oldstr);
    }

    return newstr;
}

// remove char from string
int remove_char(char *str, char c)
{
    int indexBase, indexResult, n = strlen(str);
    for (indexBase = indexResult = 0; indexBase < n; indexBase++)
        if (str[indexBase] != c)
            str[indexResult++] = str[indexBase];
    str[indexResult] = '\0';
    return 0;
}

char *insert_char(const char *source, const char add, int pos, int headOnly)
{
    int source_len = strlen(source);
    if (source_len < pos)
    {
        return strdup(source);
    }
    char *dest = (char *)calloc(source_len + 1, sizeof(char));
    int i = 0;
    for (i = 0; i < source_len + 1; i++)
    {
        if ((source[i] == ' ' || source[i] == '\t') && headOnly)
        {
            break;
        }
        if (pos >= 0)
        {
            if (i < pos)
            {
                dest[i] = source[i];
            }
            else if (i == pos)
            {
                dest[i] = add;
            }
            else
            {
                dest[i] = source[i - 1];
            }
        }
        else
        {
            dest[i] = source[i];
        }
    }
    return dest;
}

int start_with(const char *str, const char *prefix)
{
    if (!str || !prefix)
        return 0;
    size_t lenstr = strlen(str);
    size_t lenprefix = strlen(prefix);
    if (lenprefix > lenstr)
        return 0;
    return strncmp(prefix, str, lenprefix) == 0;
}

int end_with(const char *str, const char *suffix)
{
    if (!str || !suffix)
        return 0;
    size_t lenstr = strlen(str);
    size_t lensuffix = strlen(suffix);
    if (lensuffix > lenstr)
        return 0;
    return strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
}

node_array_t str_split(char *a_str, const char a_delim)
{
    node_array_t array;
    size_t count = 0;
    char *tmp = a_str;
    char *last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;
    /* Count how many elements will be extracted. */
    while (*tmp)
    {
        if (a_delim == *tmp)
        {
            count++;
            last_comma = tmp;
        }
        tmp++;
    }
    /* Add space for trailing token. */
    count += last_comma < (a_str + strlen(a_str) - 1);
    /* Add space for terminating null string so caller
       knows where the list of returned strings ends. */
    count++;
    char **data = (char **)calloc(count, sizeof(*data));
    if (data != NULL)
    {
        size_t idx = 0;
        char *token = strtok(a_str, delim);
        do
        {
            if (idx < count)
            {
                *(data + idx++) = strdup(token);
            }
        } while (token = strtok(NULL, delim));
    }
    array.data = data;
    array.len = count - 1;
    return array;
}

char *read_proc_file(char *filename, ngx_http_request_t *r)
{
    char *result = (char *)calloc(256, sizeof(char));
    FILE *fp;
    if ((fp = fopen(filename, READ_ONLY_MODE)) == NULL)
    {
        ngx_log_error(NGX_LOG_INFO, r->connection->log, 0, OPEN_FILE_FAILED);
        return NULL;
    }
    if (fp)
    {
        do
        {
            char *buffer = (char *)calloc(256, sizeof(char));
            if (fgets(buffer, 256, fp))
            {
                result = (char *)realloc(result, (strlen(buffer) + strlen(result) + 1L) * sizeof(char));
                strcat(result, buffer);
            }
            free(buffer);
        } while (!feof(fp));
        // Always remember to close the file.
        fclose(fp);
    }
    return result;
}

void *remove_all_characters(char *inputStr)
{
    int j = 0;
    int i;
    for (i = 0; inputStr[i]; i++)
    {
        if (inputStr[i] >= '0' && inputStr[i] <= '9')
        {
            inputStr[j] = inputStr[i];
            j++;
        }
    }
    inputStr[j] = '\0';
}