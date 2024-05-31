/**
 *   \file file_operation.h
 *   \brief 一些文件的操作
 */
#ifndef _FILE_OPERATION_H_
#define _FILE_OPERATION_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <ftw.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif
/**
 *  \brief 读文件
 *  \param 文件名称
 *  \param 数据
 *  \param 长度
 *  \param 位置
 *  \return 读出长度
 */
unsigned int file_operation_read(const char *pszFileName, unsigned char *pszBuf,
                                 int len, unsigned int *uiReadpos);

/**
 *  \brief 打开文件并将所有数据取出来
 *  \param file 文件名
 *  \param len 需要返回的数据长度
 *  \return 文件数据
 */
char *file_operation_read_malloc(char *file, int *len);

/**
 *  \brief 删除数据
 *  \param 已经malloc的数据
 *  \return void
 */
void file_operation_read_free(char **pdata);

/**
 *  \brief 获取文件大小
 *  \param 文件
 *  \return 大小
 */
unsigned long file_operation_get_size(const char *pchFileName);

/**
 *  \brief 文件是否存在
 *  \param 文件名
 *  \return true
 */
bool file_operation_exist(const char *pchFileName);

/**
 *  \brief 文件夹是否存在
 *  \param 文件夹名
 *  \return true
 */
bool file_operation_dir_exist(const char *pchDirPath);

/**
 *  \brief 生成完整的文件路径
 *  \param 文件夹路径
 *  \param 文件名称（不含路径）
 *  \param 全路径
 *  \return void
 */
void file_operation_get_total_path(const char *path, const char *file_name,
                                   char *file_path);

/**
 *  \brief 修改文件权限
 *  \param 文件名称
 *  \param 文件权限
 *  \return bool
 */
bool file_operation_change_mode(char *pszFileName, int imode);

/**
 *  \brief 获取最新的json文件按文件名字
 *  \param path
 *  \return <=0 无文件
 *  \return >0 有文件
 */
int file_operation_get_json_file_list(char *path, char plist[][128], int max);

/**
 *  \brief 获取最新的json文件按文件名字
 *  \param path
 *  \return char *
 */
char *file_operation_get_latest_json_file(char *path);

/**
 *  \brief 获取最旧的json文件按文件名字
 *  \param path
 *  \return char *
 */
char *file_operation_get_oldest_json_file(char *path);

#ifdef __cplusplus
}
#endif

#endif /* _FILE_OPERATION_H_ */

// This file is set to c + + mode when you set the following content to the bottom and reopen the file
// Local Variables:
// mode: c++
// End:
