/*
 * Copyright (c) 2006-Present, Redis Ltd.
 * All rights reserved.
 *
 * Licensed under your choice of the Redis Source Available License 2.0
 * (RSALv2); or (b) the Server Side Public License v1 (SSPLv1); or (c) the
 * GNU Affero General Public License v3 (AGPLv3).
*/
#pragma once
#include "redismodule.h"

void Backup_Globals();
void Restore_Globals();
void Discard_Globals_Backup();

// For rdb short read

#define LoadStringBufferAlloc_IOErrors(rdb, ptr, len, exclude_null_delimiter_from_len, cleanup_exp)  \
do {                                                                \
  size_t tmp_len;                                                   \
  size_t *tmp_len_ptr = len ? len : &tmp_len;                       \
  char *oldbuf = RedisModule_LoadStringBuffer(rdb, tmp_len_ptr);    \
  if (RedisModule_IsIOError(rdb)) {                                 \
    cleanup_exp;                                                    \
  }                                                                 \
  RS_ASSERT(oldbuf);                                                \
  RedisModule_Assert(*tmp_len_ptr);                                 \
  ptr = rm_malloc(*tmp_len_ptr);                                    \
  memcpy(ptr, oldbuf, *tmp_len_ptr);                                \
  *tmp_len_ptr -= exclude_null_delimiter_from_len;                  \
  RedisModule_Free(oldbuf);                                         \
} while (0)

#define LoadStringBuffer_IOError(rdb, len_ptr, cleanup_exp)     \
  ({                                                            \
    char *res = RedisModule_LoadStringBuffer((rdb), (len_ptr)); \
    if (RedisModule_IsIOError(rdb)) {                           \
      cleanup_exp;                                              \
    }                                                           \
    (res);                                                      \
  })

#define LoadUnsigned_IOError(rdb, cleanup_exp)      \
  ({                                                \
    uint64_t res = RedisModule_LoadUnsigned((rdb)); \
    if (RedisModule_IsIOError(rdb)) {               \
      cleanup_exp;                                  \
    }                                               \
    (res);                                          \
  })

#define LoadSigned_IOError(rdb, cleanup_exp)     \
  ({                                             \
    int64_t res = RedisModule_LoadSigned((rdb)); \
    if (RedisModule_IsIOError(rdb)) {            \
      cleanup_exp;                               \
    }                                            \
    (res);                                       \
  })

#define LoadDouble_IOError(rdb, cleanup_exp)    \
  ({                                            \
    double res = RedisModule_LoadDouble((rdb)); \
    if (RedisModule_IsIOError(rdb)) {           \
      cleanup_exp;                              \
    }                                           \
    (res);                                      \
  })
