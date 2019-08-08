#include <windows.h>
#include <cstdio>
#include <cstring>
#include "directory.h"

#if APP_INTERNAL
#include <stdint.h>
static uint64_t file_count = 0;
static uint64_t file_error_count = 0;

static uint64_t dir_count = 0;
static uint64_t dir_error_count = 0;
#endif

#define PATH_MAX_SIZE 4096

static bool rm_dir(char* path, size_t path_len);

inline bool rm(Directory* file, char* path, size_t path_len)
{
	bool result;
	if (!isDir(file))
	{
		BOOL res = DeleteFileA(path);
		result = res != 0;
		if (!res)
		{
			fprintf(stderr, "Failed to delete file \"%s\"\n", path);
		}
#if APP_INTERNAL
		else
		{
			fprintf(stderr, "Deleted file   \"%s\"\n", path);
		}
		file_count++;
		file_error_count += (res == 0);
#endif
	}
	else
	{
		path[path_len] = '\\';
		result = rm_dir(path, path_len+1);
	}
	return result;
}

// @param path must be terminated with '\\'
static bool rm_dir(char* path, size_t path_len)
{
	path[path_len] = '*';
	path[path_len+1] = 0;
#if APP_INTERNAL
	printf("searching \"%s\"\n", path);
#endif
	bool result = true;
	Directory file;
	for (dfind(&file, path);
				file.found;
				dnext(&file))
	{
		if (isThisOrParentDir(&file)) continue;
		path[path_len] = 0;
		strcat_s(path, PATH_MAX_SIZE, file.name);
		size_t subpath_len = path_len + strlen(file.name);
		bool res = rm(&file, path, subpath_len);
		result = result && res;
	}
	dclose_fast(&file);
	path[path_len-1] = 0;
	BOOL res = RemoveDirectoryA(path);
	result = result && res != 0;
	if (!res)
	{
		fprintf(stderr, "Failed to delete folder \"%s\"\n", path);
	}
#if APP_INTERNAL
	else
	{
		fprintf(stderr, "Deleted folder \"%s\"\n", path);
	}
	dir_count++;
	dir_error_count += (res == 0);
#endif
	return result;
}

static bool rm(char* path, size_t path_len)
{
	bool found = false;
	Directory file;
	dfind(&file, path);
	bool result = true;
	for (;
		file.found;
		dnext(&file))
	{
		if (isThisOrParentDir(&file)) continue;
		bool res = rm(&file, path, path_len);
		result = result && res;
		found = true;
	}
	result = result && found;
	
#if APP_INTERNAL
	if (!found)
	{
		fprintf(stderr, "Failed to delete \"%s\"\n", path);
	}
	else if (!result)
	{
		fprintf(stderr, "Paritally deleted \"%s\"\n", path);
	}
	#endif

	dclose_fast(&file);
	return result;
}

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		fprintf(stderr, "Please specify a folder or a file to delete.\n");
		return 1;
	}

	char* path = (char*)malloc(PATH_MAX_SIZE);
	
	bool result = true;
	for (int ai = 1; ai < argc; ++ai)
	{
		strcpy_s(path, PATH_MAX_SIZE, argv[ai]);
#if APP_INTERNAL
		fprintf(stderr, "Path \"%s\"\n", path);
#endif
		bool res = rm(path, strlen(path));
		result = result && res;
	}

#if APP_INTERNAL
	printf("%llu/%llu files failed\n%llu/%llu folders failed\n", file_error_count, file_count, dir_error_count, dir_count);
#endif
}
