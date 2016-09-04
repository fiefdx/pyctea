/*  
**Created on 2014-06-08
**@summary:  tea
**@author: fiefdx
*/

#include <stdio.h>
#include <ctea.h>
 
void tea_encrypt (uint32_t* v, uint32_t* k) {
    uint32_t v0=v[0], v1=v[1], sum=0, i;           /* set up */
    uint32_t delta=0x9e3779b9;                     /* a key schedule constant */
    uint32_t k0=k[0], k1=k[1], k2=k[2], k3=k[3];   /* cache key */
    for (i=0; i < 32; i++) {                       /* basic cycle start */
        sum += delta;
        v0 += ((v1<<4) + k0) ^ (v1 + sum) ^ ((v1>>5) + k1);
        v1 += ((v0<<4) + k2) ^ (v0 + sum) ^ ((v0>>5) + k3);
    }                                              /* end cycle */
    v[0]=v0; v[1]=v1;
}
 
void tea_decrypt (uint32_t* v, uint32_t* k) {
    uint32_t v0=v[0], v1=v[1], sum=0xC6EF3720, i;  /* set up */
    uint32_t delta=0x9e3779b9;                     /* a key schedule constant */
    uint32_t k0=k[0], k1=k[1], k2=k[2], k3=k[3];   /* cache key */
    for (i=0; i<32; i++) {                         /* basic cycle start */
        v1 -= ((v0<<4) + k2) ^ (v0 + sum) ^ ((v0>>5) + k3);
        v0 -= ((v1<<4) + k0) ^ (v1 + sum) ^ ((v1>>5) + k1);
        sum -= delta;
    }                                              /* end cycle */
    v[0]=v0; v[1]=v1;
}

void tea_str_encrypt(uint32_t* v, uint32_t* k, uint32_t length) {
    uint32_t i, i_0, i_1;
    uint32_t v_tmp[2] = {0, 0};
    uint64_t cipertext = 0xffffffffffffffff;
    uint64_t pre_plaintext = 0xffffffffffffffff;
    uint64_t plaintext = 0xffffffffffffffff;
    uint64_t encrypt_text = 0xffffffffffffffff;
    length = length / 2;

    pre_plaintext = v[0];
    pre_plaintext <<= 32;
    pre_plaintext |= v[1];
    v_tmp[0] = v[0];
    v_tmp[1] = v[1];
    tea_encrypt(v_tmp, k);
    v[0] = v_tmp[0];
    v[1] = v_tmp[1];
    // printf("(%u, %u)\n", v[0], v[1]);
    cipertext = v[0];
    cipertext <<= 32;
    cipertext |= v[1];
    for (i = 1; i<length; i++){
        i_0 = i*2;
        i_1 = i_0 + 1;
        plaintext = v[i_0];
        plaintext <<= 32;
        plaintext |= v[i_1];
        plaintext ^= cipertext;
        v_tmp[0] = (uint32_t)((plaintext >> 32) & 0xffffffff);;
        v_tmp[1] = (uint32_t)(plaintext & 0xffffffff);
        tea_encrypt(v_tmp, k);
        v[i_0] = v_tmp[0];
        v[i_1] = v_tmp[1];
        encrypt_text = v[i_0];
        encrypt_text <<= 32;
        encrypt_text |= v[i_1];
        encrypt_text ^= pre_plaintext;
        v[i_0] = (uint32_t)((encrypt_text >> 32) & 0xffffffff);
        v[i_1] = (uint32_t)(encrypt_text & 0xffffffff);
        // printf("(%u, %u)\n", v[i_0], v[i_1]);
        cipertext = encrypt_text;
        pre_plaintext = plaintext;
    }
}

uint32_t tea_str_decrypt(uint32_t* v, uint32_t* k, uint32_t length) {
    uint32_t i, i_0, i_1;
    uint32_t pos = 0;
    uint32_t v_tmp[2] = {0, 0};
    uint64_t cipertext = 0xffffffffffffffff;
    uint64_t cipertext_tmp = 0xffffffffffffffff;
    uint64_t pre_plaintext = 0xffffffffffffffff;
    uint64_t plaintext = 0xffffffffffffffff;
    uint64_t encrypt_text = 0xffffffffffffffff;
    length = length / 2;

    cipertext = v[0];
    cipertext <<= 32;
    cipertext |= v[1];
    v_tmp[0] = v[0];
    v_tmp[1] = v[1];
    tea_decrypt(v_tmp, k);
    v[0] = v_tmp[0];
    v[1] = v_tmp[1];
    // printf("(%u, %u)\n", v[0], v[1]);
    pos = v[0];
    pre_plaintext = v[0];
    pre_plaintext <<= 32;
    pre_plaintext |= v[1];
    for (i = 1; i<length; i++){
        i_0 = i*2;
        i_1 = i_0 + 1;
        cipertext_tmp = v[i_0];
        cipertext_tmp <<= 32;
        cipertext_tmp |= v[i_1];
        encrypt_text = v[i_0];
        encrypt_text <<= 32;
        encrypt_text |= v[i_1];
        encrypt_text ^= pre_plaintext;
        v[i_0] = (uint32_t)((encrypt_text >> 32) & 0xffffffff);
        v[i_1] = (uint32_t)(encrypt_text & 0xffffffff);
        v_tmp[0] = v[i_0];
        v_tmp[1] = v[i_1];
        tea_decrypt(v_tmp, k);
        v[i_0] = v_tmp[0];
        v[i_1] = v_tmp[1];
        plaintext = v[i_0];
        plaintext <<= 32;
        plaintext |= v[i_1];
        plaintext ^= cipertext;
        v[i_0] = (uint32_t)((plaintext >> 32) & 0xffffffff);
        v[i_1] = (uint32_t)(plaintext & 0xffffffff);
        // printf("(%u, %u)\n", v[i*2], v[i*2+1]);
        pre_plaintext = plaintext ^ cipertext;
        cipertext = cipertext_tmp;
    }
    return pos;
}

void tea_str_pointer_encrypt(uint32_t* v, uint32_t* k, uint32_t length) {
    uint32_t i;
    uint64_t cipertext = 0xffffffffffffffff;
    uint64_t pre_plaintext = 0xffffffffffffffff;
    uint64_t plaintext = 0xffffffffffffffff;
    uint64_t encrypt_text = 0xffffffffffffffff;
    length = length / 2;

    pre_plaintext = v[0];
    pre_plaintext <<= 32;
    pre_plaintext |= v[1];
    tea_encrypt(v, k);
    // printf("(%u, %u)\n", v[0], v[1]);
    cipertext = v[0];
    cipertext <<= 32;
    cipertext |= v[1];
    v++;
    v++;
    for (i = 1; i<length; i++){
        plaintext = v[0];
        plaintext <<= 32;
        plaintext |= v[1];
        plaintext ^= cipertext;
        v[0] = (uint32_t)((plaintext >> 32) & 0xffffffff);;
        v[1] = (uint32_t)(plaintext & 0xffffffff);
        tea_encrypt(v, k);
        encrypt_text = v[0];
        encrypt_text <<= 32;
        encrypt_text |= v[1];
        encrypt_text ^= pre_plaintext;
        v[0] = (uint32_t)((encrypt_text >> 32) & 0x00000000ffffffff);
        v[1] = (uint32_t)(encrypt_text & 0x00000000ffffffff);
        cipertext = encrypt_text;
        pre_plaintext = plaintext;
        v++;
        v++;
    }
}

uint32_t tea_str_pointer_decrypt(uint32_t* v, uint32_t* k, uint32_t length) {
    uint32_t i;
    uint32_t pos = 0;
    uint64_t cipertext = 0xffffffffffffffff;
    uint64_t cipertext_tmp = 0xffffffffffffffff;
    uint64_t pre_plaintext = 0xffffffffffffffff;
    uint64_t plaintext = 0xffffffffffffffff;
    uint64_t encrypt_text = 0xffffffffffffffff;
    length = length / 2;

    cipertext = v[0];
    cipertext <<= 32;
    cipertext |= v[1];
    tea_decrypt(v, k);
    pos = v[0];
    pre_plaintext = v[0];
    pre_plaintext <<= 32;
    pre_plaintext |= v[1];
    v++;
    v++;
    for (i = 1; i<length; i++){
        cipertext_tmp = 0;
        cipertext_tmp |= v[0];
        cipertext_tmp <<= 32;
        cipertext_tmp |= v[1];
        encrypt_text = 0;
        encrypt_text |= v[0];
        encrypt_text <<= 32;
        encrypt_text |= v[1];
        encrypt_text ^= pre_plaintext;
        v[0] = (uint32_t)((encrypt_text >> 32) & 0xffffffff);
        v[1] = (uint32_t)(encrypt_text & 0xffffffff);
        tea_decrypt(v, k);
        plaintext = 0;
        plaintext |= v[0];
        plaintext <<= 32;
        plaintext |= v[1];
        plaintext ^= cipertext;
        v[0] = (uint32_t)((plaintext >> 32) & 0xffffffff);
        v[1] = (uint32_t)(plaintext & 0xffffffff);
        pre_plaintext = plaintext ^ cipertext;
        cipertext = cipertext_tmp;
        v++;
        v++;
    }
    return pos;
}