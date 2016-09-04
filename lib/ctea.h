/*  
**Created on 2014-06-08
**@summary:  tea header
**@author: fiefdx
*/

#include <stdint.h>

void tea_encrypt (uint32_t* v, uint32_t* k);
void tea_decrypt (uint32_t* v, uint32_t* k);
void tea_str_encrypt(uint32_t* v, uint32_t* k, uint32_t length);
uint32_t tea_str_decrypt(uint32_t* v, uint32_t* k, uint32_t length);
void tea_str_pointer_encrypt(uint32_t* v, uint32_t* k, uint32_t length);
uint32_t tea_str_pointer_decrypt(uint32_t* v, uint32_t* k, uint32_t length); 