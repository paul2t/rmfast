#include <windows.h>
#include <cstdio>
#include "directory.h"

// Global variable to preserve stack space
// We only need one of these at a time
// It will be more cache friendly
static WIN32_FIND_DATAA find_data;


// @return the length of the new dst
inline size_t str_copy(char* dst, size_t dst_size, char* src)
{
	size_t i = 0;
	for (; i < dst_size; ++i)
	{
		dst[i] = src[i];
		if (dst[i] == 0) break;
	}
	dst[i] = 0;
	return i;
}

// @return the length of the new dst
inline size_t str_append(char* dst, size_t dst_len, size_t dst_size, char* src)
{
	size_t i = 0;
	for (; i < dst_size; ++i)
	{
		dst[dst_len+i] = src[i];
		if (dst[i] == 0) break;
	}
	dst[i] = 0;
	return i;
}

#define PATH_MAX_SIZE 4096

static void rm_dir(char* path, size_t path_len);

inline void rm(Directory* file, char* path, size_t path_len)
{
	if (!isDir(file))
	{
		BOOL res = DeleteFileA(path);
		if (!res)
		{
			fprintf(stderr, "Failed to delete file \"%s\"\n", path);
		}
	}
	else
	{
		path[path_len] = '\\';
		rm_dir(path, path_len+1);
	}
}

// @param path must end with '\\'
static void rm_dir(char* path, size_t path_len)
{
	path[path_len] = '*';
	path[path_len+1] = 0;
	Directory file;
	file.data = &find_data;
	for (dfind(&file, path);
				file.found;
				dnext(&file))
	{
		// Can we remove this ? It seems that '.' and '..' are found first
		// They are already skipped by dfind()
		// I am afraid this might change and break and remove everything.
		// Removing this could potentially be very dangerous.
		if (isThisOrParentDir(&file)) continue;

		size_t subpath_len = str_append(path, path_len, PATH_MAX_SIZE, dName(&file));
		rm(&file, path, subpath_len);
	}
	dclose_fast(&file);
	path[path_len-1] = 0;
	BOOL res = RemoveDirectoryA(path);
	if (!res)
	{
		fprintf(stderr, "Failed to delete folder \"%s\"\n", path);
	}
}

static void rm(char* path, size_t path_len)
{
	Directory file;
	file.data = &find_data;
	dfind(&file, path);
	for (;
		file.found;
		dnext(&file))
	{
		if (isThisOrParentDir(&file)) continue;
		rm(&file, path, path_len);
	}
	
	dclose_fast(&file);
}

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		return 0;
	}

	char path[PATH_MAX_SIZE];
	
	for (int ai = 1; ai < argc; ++ai)
	{
		size_t path_len = str_copy(path, PATH_MAX_SIZE, argv[ai]);
		rm(path, path_len);
	}

	return 0;
}
