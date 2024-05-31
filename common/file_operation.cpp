#include "file_operation.h"
#include "global.h"

unsigned int file_operation_read(const char *pszFileName, unsigned char *pszBuf,
                                 int len, unsigned int *uiReadpos)
{
    FILE *pFile = NULL;
    int num = 0;

    if (NULL == pszBuf) {
        return 0;
    }
    pFile = fopen(pszFileName, "rb");
    if (NULL == pFile) {
        perror("pszFileName");
        return 0;
    }

    if (-1 == fseek(pFile, *uiReadpos, SEEK_SET)) {
        fclose(pFile);
        return 0;
    }

    num = fread(pszBuf, 1, len, pFile);

	//add2print_queue("num=%d len=%d pos=%d\n", num, len, *uiReadpos);
	// if ( num != len) {
	// 	add2print_queue("num=%d len=%d pos=%d", num, len, uiReadpos);
	// }

    *uiReadpos += num;

    fclose(pFile);

    return num;
}

char *file_operation_read_malloc(char *file, int *len)
{
    FILE *f;
    char *data;

    f = fopen(file, "rb");
    if (f == NULL) {
        fprintf(stderr, "open file %s failed\n", file);
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    *len = ftell(f);
    data = (char *)malloc((*len + 1) * sizeof(char));
    rewind(f);
    *len = fread(data, 1, *len, f);
    data[*len] = '\0';
    fclose(f);

    return data;
}

void file_operation_read_free(char **pdata)
{
    if (NULL != *pdata) {
        free(*pdata);
        *pdata = NULL;
    }
}