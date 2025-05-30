/*
 * Copyright (c) 2006-Present, Redis Ltd.
 * All rights reserved.
 *
 * Licensed under your choice of the Redis Source Available License 2.0
 * (RSALv2); or (b) the Server Side Public License v1 (SSPLv1); or (c) the
 * GNU Affero General Public License v3 (AGPLv3).
*/
#ifndef INDEXER_H
#define INDEXER_H

#include "document.h"
#include "concurrent_ctx.h"
#include "util/arr.h"
#include "geometry_index.h"

extern bool g_isLoading;

// Preprocessors can store field data to this location
typedef struct FieldIndexerData {
  int isMulti;
  int isNull;
  struct {
    // This is a struct and not a union since when FieldSpec options is `FieldSpec_Dynamic`:
    // it can store data as several types, e.g., as numeric and as tag)

    // Single value
    double numeric;  // i.e. the numeric value of the field
    arrayof(char*) tags;
    struct {
      const void *vector;
      size_t vecLen;
      size_t numVec;
    };

    // Multi value
    arrayof(double) arrNumeric;

    struct {
      const char *str;
      size_t strlen;
      GEOMETRY_FORMAT format;
    };
    // struct {
    //   arrayof(GEOMETRY) arrGeometry;
    // };
  };

} FieldIndexerData;


/**
 * Add a document to the indexing queue. If successful, the indexer now takes
 * ownership of the document context (until it DocumentAddCtx_Finish).
 */
int IndexDocument(RSAddDocumentCtx *aCtx);

/**
 * Function to preprocess field data. This should do as much stateless processing
 * as possible on the field - this means things like input validation and normalization.
 *
 * The `fdata` field is used to contain the result of the processing, which is then
 * actually written to the index at a later point in time.
 *
 * This function is called with the GIL released.
 */
typedef int (*PreprocessorFunc)(RSAddDocumentCtx *aCtx, RedisSearchCtx *sctx, DocumentField *field,
                                const FieldSpec *fs, FieldIndexerData *fdata, QueryError *status);

/**
 * Function to write the entry for the field into the actual index. This is called
 * with the GIL locked, and it should therefore only write data, and nothing more.
 */
typedef int (*IndexerFunc)(RSAddDocumentCtx *aCtx, RedisSearchCtx *ctx, const DocumentField *field,
                           const FieldSpec *fs, FieldIndexerData *fdata, QueryError *status);

int IndexerBulkAdd(RSAddDocumentCtx *cur, RedisSearchCtx *sctx,
                   const DocumentField *field, const FieldSpec *fs, FieldIndexerData *fdata,
                   QueryError *status);

/**
 * Yield to Redis after a certain number of operations during indexing while loading.
 * This helps keep Redis responsive during long indexing operations.
 * @param ctx The Redis context
 */
static void IndexerYieldWhileLoading(RedisModuleCtx *ctx);

#endif
