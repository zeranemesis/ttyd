#include "parse.h"
#include <string.h>
#include <stdio.h>

typedef struct ParseWorkInternal {
    const char* ptr;       // 0x0
    s32 stackStart[10];    // 0x4
    s32 stackEnd[10];      // 0x2C
    s32 stackLevel;        // 0x54
} ParseWorkInternal;

static ParseWorkInternal parse;
static char s_parseBuf[128];

void parseInit(void* ptr, u32 size) {
    parse.ptr = (const char*)ptr;
    parse.stackLevel = 0;
    parse.stackStart[0] = 0;
    parse.stackEnd[0] = (s32)size;
}

void parsePop(void) {
    if (parse.stackLevel > 0) {
        parse.stackLevel--;
    }
}

void parsePopNext(void) {
    s32 nextPos;
    if (parse.stackLevel > 0) {
        nextPos = parse.stackEnd[parse.stackLevel];
        parse.stackLevel--;
        parse.stackStart[parse.stackLevel] = nextPos;
    }
}

BOOL parsePush(const char* tag) {
    s32 cur, end, matchStart, matchEnd, state, depth;
    s32 t;
    int bufIdx;
    char c;

    if (!parse.ptr) return FALSE;
    cur = parse.stackStart[parse.stackLevel];
    end = parse.stackEnd[parse.stackLevel];
    matchStart = 0;
    matchEnd = 0;
    state = 0; // 0 = searching open, 1 = searching close, 2 = matched
    depth = 0;

    while (cur < end) {
        if (parse.ptr[cur] == '<') {
            if (cur + 1 < end && parse.ptr[cur + 1] == '/') {
                if (state == 1) {
                    depth--;
                    if (depth == 0) {
                        while (cur < end && parse.ptr[cur] != '>') cur++;
                        if (cur < end && parse.ptr[cur] == '>') cur++;
                        matchEnd = cur;
                        state = 2;
                        break;
                    }
                } else {
                    depth--;
                }
            } else {
                if (state == 0 && depth == 0) {
                    t = cur;
                    bufIdx = 0;
                    while (t < end && bufIdx < 127) {
                        c = parse.ptr[t];
                        if (c != ' ' && c != '\t' && c != '\n' && c != '\r') {
                            s_parseBuf[bufIdx++] = c;
                        }
                        if (c == '>') {
                            t++;
                            break;
                        }
                        t++;
                    }
                    s_parseBuf[bufIdx] = '\0';
                    if (strcmp(s_parseBuf, tag) == 0) {
                        while (cur < end && parse.ptr[cur] != '>') cur++;
                        if (cur < end && parse.ptr[cur] == '>') cur++;
                        matchStart = cur;
                        state = 1;
                    }
                }
                depth++;
            }
        }
        cur++;
    }

    if (state == 2 && parse.stackLevel < 9) {
        parse.stackLevel++;
        parse.stackStart[parse.stackLevel] = matchStart;
        parse.stackEnd[parse.stackLevel] = matchEnd;
        return TRUE;
    }
    return FALSE;
}

BOOL parseGet1Next(s32 type, void* data) {
    BOOL res;
    s32 cur;
    s32 end;
    const char* src;
    char c;
    char* out;
    int val;
    float fval;

    if (!parse.ptr) return FALSE;
    if (type == 3) {
        res = TRUE;
        res &= parseGet1Next(2, data);
        res &= parseGet1Next(2, (void*)((uintptr_t)data + 4));
        res &= parseGet1Next(2, (void*)((uintptr_t)data + 8));
        return res;
    }

    cur = parse.stackStart[parse.stackLevel];
    end = parse.stackEnd[parse.stackLevel];
    src = parse.ptr;

    // Skip comments and leading whitespace
    while (cur < end) {
        if (src[cur] == '/' && cur + 1 < end && src[cur + 1] == '/') {
            while (cur < end && src[cur] != '\n') cur++;
            continue;
        }
        c = src[cur];
        if (c != ' ' && c != '\t' && c != '\n' && c != '\r') break;
        cur++;
    }

    if (cur >= end) return FALSE;

    if (type == 0) {
        // String
        out = (char*)data;
        if (src[cur] == '"') {
            cur++;
            while (cur < end && src[cur] != '"') {
                *out++ = src[cur++];
            }
            if (cur < end && src[cur] == '"') cur++;
        } else {
            while (cur < end && src[cur] != ' ' && src[cur] != '\t' && src[cur] != '\n' && src[cur] != '\r' && src[cur] != '<') {
                *out++ = src[cur++];
            }
        }
        *out = '\0';
        parse.stackStart[parse.stackLevel] = cur;
        return TRUE;
    } else if (type == 1) {
        // Integer
        val = 0;
        if (sscanf(src + cur, "%d", &val) == 1) {
            *(s32*)data = val;
            while (cur < end && src[cur] != ' ' && src[cur] != '\t' && src[cur] != '\n' && src[cur] != '\r' && src[cur] != '<') cur++;
            parse.stackStart[parse.stackLevel] = cur;
            return TRUE;
        }
    } else if (type == 2) {
        // Float
        fval = 0.0f;
        if (sscanf(src + cur, "%f", &fval) == 1) {
            *(f32*)data = fval;
            while (cur < end && src[cur] != ' ' && src[cur] != '\t' && src[cur] != '\n' && src[cur] != '\r' && src[cur] != '<') cur++;
            parse.stackStart[parse.stackLevel] = cur;
            return TRUE;
        }
    }

    return FALSE;
}

BOOL parseTagGet1(const char* tag, s32 type, void* data) {
    BOOL res;
    if (parsePush(tag)) {
        res = parseGet1Next(type, data);
        parsePop();
        return res;
    }
    return FALSE;
}
