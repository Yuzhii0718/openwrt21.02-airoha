#ifndef __UTIL_PLUGIN_CFG_H__
#define __UTIL_PLUGIN_CFG_H__


/** Get the file handle of data file for plugin!
  * @Param: msgHandle - for get dir from cpm by message
  * @Param: fileName - the data config file name
  * @Param: mode - open mode, is equal to fopen's mode, "r", "r+", "w", and so on;
  *
  * @Reuturn: the file's handlers
  */
FILE *UTIL_pluginGetDataFile(void *msgHandle, const char *fileName, const char *mode);
FILE *UTIL_pluginGetTmpFile(void *msgHandle, const char *fileName, const char *mode);


#endif /* __UTIL_PLUGIN_CFG_H__ */
