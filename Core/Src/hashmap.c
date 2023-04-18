/*
 * hashmap.c
 *
 *  Created on: 2023年4月18日
 *      Author: 陈骏骏
 */

#include "hashmap.h"
#include <stdio.h>

/**
 * 添加元素
 * ​	HASH_FIND_INT(hashMap, &key, s);
​		hashmap：从哪里查找，传入指针
​		key：查找的元素，传入指针
​		s：out，结构存放，指针
	HASH_ADD_INT(hashMap, key, s);
​		hashmap：添加到哪里，传入指针
​		key：添加的元素
​		s：要添加的结构指针
	HASH_ADD_INT表示添加的键值为int类型
	HASH_ADD_STR表示添加的键值为字符串类型
	HASH_ADD_PTR表示添加的键值为指针类型
	HASH_ADD表示添加的键值可以是任意类型
 *
 */
void hashMapAdd(map* hashMap,int key,int value) {
    map* s;
    HASH_FIND_INT(hashMap, &key, s);
    if (s == NULL) {
        s = (map*)malloc(sizeof(map));
        s->key = key;
        HASH_ADD_INT(hashMap, key, s);
    }
    s->value = value;
}

/**
 * 查找元素
 */
int hashMapFind(map* hashMap,int key) {
    map* s;
    s = (map*)malloc(sizeof(map));
    HASH_FIND_INT(hashMap, &key, s);
    return s->value;
}

/**
 * 删除元素
 */
void hashMapDelete(map* hashMap,int key) {
    map* s;
    HASH_FIND_INT(hashMap, &key, s);
    if (s != NULL) {
        HASH_DEL(hashMap, s);
    }
}

/**
 * 清空元素
 */
void  hashMapClean(map* hashMap) {
    map* current, * tmp;
    HASH_ITER(hh, hashMap, current, tmp) {
        HASH_DEL(hashMap, current);
        free(current);
    }

}

/**
 * 遍历元素
 */
void Traverse(map* hashMap) {
    map* s;
    for (s = hashMap; s != NULL; s = s->hh.next) {
        printf("key: % d, value : % d\n", s->key, s->value);
    }
}
