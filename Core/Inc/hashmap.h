/*
 * hashmap.h
 *
 *  Created on: 2023年4月18日
 *      Author: 陈骏骏
 */

#ifndef INC_HASHMAP_H_
#define INC_HASHMAP_H_

#include <uthash.h>

//结构体中的key与value的类型可以根据情况自己定义
typedef struct s_hashmap{
    int key;
    int value;
    UT_hash_handle hh; // make this structure hashable
}map;

void hashMapAdd(map* hashMap,int key,int value);
int hashMapFind(map* hashMap,int key);

#endif /* INC_HASHMAP_H_ */
