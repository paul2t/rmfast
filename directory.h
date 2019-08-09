#ifndef _directory_h_
#define _directory_h_


struct Directory
{
    int found = false;
    HANDLE _findHandle = 0;
    WIN32_FIND_DATAA* data = 0;
};

inline char* dName(Directory* d)
{
    return d->data->cFileName;
}

inline bool isThisOrParentDir(Directory* d)
{
    char* name = dName(d);
    return name[0] == '.' && (name[1] == '\0' || (name[1] == '.' && name[2] == '\0'));
}

inline int isDir(Directory* d)
{
    return (d->data->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
}

inline int isFile(Directory* d)
{
    return !isDir(d);
}

inline int isReadOnly(Directory* d)
{
    return (d->data->dwFileAttributes & FILE_ATTRIBUTE_READONLY);
}

inline void dnext(Directory* d)
{
    d->found = FindNextFile(d->_findHandle, d->data);
}

inline void dnextFile(Directory* d)
{
    do
    {
        dnext(d);
    }
    while (d->found && !isFile(d));
}

inline void dnextDir(Directory* d)
{
    do
    {
        dnext(d);
    }
    while (d->found && !isDir(d));
}


// Global variable to preserve stack space
// We only need one of these at a time
// It will be more cache friendly
static WIN32_FIND_DATAA g_find_data;

inline void dfind_(Directory* d, char* searchPath)
{
    if (!d->data) d->data = &g_find_data;
    d->_findHandle = FindFirstFileA(searchPath, d->data);
    d->found = (d->_findHandle != INVALID_HANDLE_VALUE);
}

inline void dfind(Directory* d, char* searchPath)
{
    dfind_(d, searchPath);
    while(d->found && isDir(d) && isThisOrParentDir(d))
    {
        dnext(d);
    }
}

inline void dfindDir(Directory* d, char* searchPath)
{
    dfind_(d, searchPath);
    while(d->found && (!isDir(d) || isThisOrParentDir(d)))
    {
        dnext(d);
    }
}

inline void dfindFile(Directory* d, char* searchPath)
{
    dfind_(d, searchPath);
    while(d->found && isDir(d))
    {
        dnext(d);
    }
}

inline void dclose_fast(Directory* d)
{
    FindClose(d->_findHandle);
}

inline void dclose(Directory* d)
{
    dclose_fast(d);
    *d = {};
}

static bool PathFileExists(char* path)
{
    bool result = false;
#if _WIN32
    WIN32_FIND_DATAA ffd;
    HANDLE hFind = FindFirstFileA(path, &ffd);
    if (INVALID_HANDLE_VALUE != hFind)
    {
        result = true;
        FindClose(hFind);
    }
#else
    if (FILE* file = fopen(path, "r"))
    {
        result = true;
        fclose(file);
    }
#endif
    return result;
}

#endif
