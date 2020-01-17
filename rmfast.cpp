#include <windows.h>
#include <cstdio>
#include "directory.h"


inline bool is_path_separator(char c)
{
	return c == '\\' || c == '/';
}

// @return the length of the new dst
inline size_t str_copy_escape(char* dst, size_t dst_size, char* src)
{
	size_t i = 0;
	size_t si = 0;
	for (; i < dst_size; ++i, ++si)
	{
		// TODO: need to check dst_size
		dst[i] = src[si];

#if _WIN32
		// Replace ".*" with "*.<" (for Windows)
		if (src[si] == '*' && si > 0 && src[si-1] == '.')
		{
			dst[i-1] = '*';
			dst[i] = '.';
			dst[++i] = '<';
		}
#endif

		if (dst[i] == 0) break;
	}
	dst[i] = 0;
	return i;
}

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

inline void disable_read_only(Directory* d, char* path)
{
	DWORD attributes = d->data->dwFileAttributes;
	if (attributes & FILE_ATTRIBUTE_READONLY)
	{
		attributes = (attributes & (~FILE_ATTRIBUTE_READONLY));
		SetFileAttributesA(path, attributes);
	}
}

#define PATH_MAX_SIZE 32767*2

static void rm_dir(char* path, size_t path_len);

inline void rm(Directory* file, char* path, size_t path_len)
{
	disable_read_only(file, path);
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
	// TODO: need to check dst_size
	path[path_len] = '*';
	path[path_len+1] = 0;

	Directory file = {};
	for (dfind(&file, path);
		file.found;
		dnext(&file))
	{
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
	// remove trailing '\\' or '/'
	while (path_len > 0 && is_path_separator(path[path_len-1])) --path_len;
	path[path_len] = 0;

	// Find the root folder of the search
	size_t folder_len = path_len;
	while (folder_len > 0 && !is_path_separator(path[folder_len-1])) --folder_len;
	if (folder_len) folder_len++;

	Directory file = {};
	for (dfind(&file, path);
		file.found;
		dnext(&file))
	{
		size_t file_len = str_append(path, folder_len, PATH_MAX_SIZE, dName(&file));
		rm(&file, path, file_len);
	}
	
	dclose_fast(&file);
}

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		fprintf(stderr, "usage: rmfast path [path2 ...]\n");
		return 0;
	}

	char* path = (char*)malloc(PATH_MAX_SIZE+1);
	
	for (int ai = 1; ai < argc; ++ai)
	{
		size_t path_len = str_copy_escape(path, PATH_MAX_SIZE, argv[ai]);
		rm(path, path_len);
	}

	return 0;
}
