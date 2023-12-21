#ifndef VFS_H
#define VFS_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define VFS_MAXPATHLENGTH 255

typedef struct vfsNode_s
{
    struct vfsNode_s* next; // Next file node on same level, otherwise NULL
    char* name;      // Name of the node
    void* data;      // Points to child node / NULL if directory, otherwise to vfsFile
    bool  isDir;     // Is directory or file?
} vfsNode_t;

typedef struct vfs_s
{
    vfsNode_t root;
} vfs_t;

typedef struct vfsFile_s
{
    void*  data;
    size_t len;
    char   access;
    int    ownerId;
} vfsFile_t;

// FS operations
void Vfs_Init(vfs_t* vfs);
void Vfs_Close(vfs_t* vfs);
int  Vfs_SaveToDisk(vfs_t* vfs, char* path);
// Checks
bool Vfs_Exists(vfs_t* vfs, char* path);
bool Vfs_IsFile(vfs_t* vfs, char* path);
// File operations
int  Vfs_GetFile(vfs_t* vfs, vfsFile_t* f, char* path);
// Directory operations
int  Vfs_CreateDirectory(vfs_t* vfs, char* path, char* name);
// For both
int  Vfs_DeleteObject(vfs_t* vfs, char* path);
// Node operations (used internally)
int  Vfs_GetNodeByPath(vfs_t* vfs, vfsNode_t** node, vfsNode_t** prev, vfsNode_t** parent, char* path);
int  Vfs_AddChildNode(vfs_t* vfs, vfsNode_t* parent, vfsNode_t* node);
int  Vfs_DeleteNode(vfs_t* vfs, char* path);

#endif