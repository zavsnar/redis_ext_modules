#include "redismodule.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define REDIS_MODULE_NAME "extzsets"


/*
* <MULTIZREVRANK> key [member1, ...] 
* Reply: an array of ranks.
*/
int MultiZRevRank_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
  if (argc < 3) {
    return RedisModule_WrongArity(ctx);
  }
  RedisModule_AutoMemory(ctx);

  RedisModuleKey *key = RedisModule_OpenKey(ctx, argv[1], REDISMODULE_READ);

  if (RedisModule_KeyType(key) == REDISMODULE_KEYTYPE_EMPTY) {
    RedisModule_ReplyWithNull(ctx);
    return REDISMODULE_OK;
  }
  if (RedisModule_KeyType(key) != REDISMODULE_KEYTYPE_ZSET) {
    RedisModule_ReplyWithError(ctx, REDISMODULE_ERRORMSG_WRONGTYPE);
    return REDISMODULE_ERR;
  }

  int i;
  int response_size = argc - 2;
  RedisModule_ReplyWithArray(ctx, response_size);
  for (i = 2; i < argc; i++) {
    RedisModuleCallReply *repl = RedisModule_Call(ctx, "zrevrank", "ss", argv[1], argv[i]);
    // TODO: process errors

    switch (RedisModule_CallReplyType(repl)) {
      case REDISMODULE_REPLY_NULL:
        RedisModule_ReplyWithNull(ctx);
        break;
      case REDISMODULE_REPLY_INTEGER:
        RedisModule_ReplyWithLongLong(ctx, RedisModule_CallReplyInteger(repl));
        break;
      case REDISMODULE_REPLY_STRING:
        RedisModule_ReplyWithString(ctx, RedisModule_CreateStringFromCallReply(repl));
        break;
    }
  }

  return REDISMODULE_OK;
}

int RedisModule_OnLoad(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
  if (RedisModule_Init(ctx, REDIS_MODULE_NAME, 1, REDISMODULE_APIVER_1) ==
      REDISMODULE_ERR)
    return REDISMODULE_ERR;
  
  /* Log the list of parameters passing loading the module. */
	for (int j = 0; j < argc; j++) {
    const char *s = RedisModule_StringPtrLen(argv[j],NULL);
    printf("Module loaded with ARGV[%d] = %s\n", j, s);
	}

  if (RedisModule_CreateCommand(ctx, "multizrevrank", MultiZRevRank_RedisCommand,
                                "readonly fast", 1, 1, 1) == REDISMODULE_ERR)
    return REDISMODULE_ERR;

  return REDISMODULE_OK;
}