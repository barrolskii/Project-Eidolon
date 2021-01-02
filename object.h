#ifndef __PHANTOM_OBJECT_H_
#define __PHANTOM_OBJECT_H_

typedef enum {
    OBJ_VAL_LONG,
    OBJ_VAL_STR,
} object_value_t;

typedef struct {
    object_value_t type;
    union {
        long long_num;
        double double_num;
        char *str;
    } as;

} object_t;

#endif // __PHANTOM_OBJECT_H_
