#ifndef __OBJECT_H_
#define __OBJECT_H_

typedef enum {
    OBJ_VAL_LONG,
    OBJ_VAL_DOUBLE,
    OBJ_VAL_STR,
    OBJ_VAL_BOOL,
} object_val_t;

typedef struct {
    object_val_t type;
    union {
        long long_num;
        double double_num;
        char *str;
    } as;

} object_t;

#endif // __OBJECT_H_
