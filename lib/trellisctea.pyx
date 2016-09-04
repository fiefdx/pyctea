# -*- coding: utf-8 -*-
'''
Created on 2014-06-07
@summary:  tea
@author: fiefdx
''' 
import numpy as np
cimport numpy as np
from libc.stdint cimport uint32_t

cimport ctealink

def tea_c_encrypt(np.ndarray[np.uint32_t, ndim=1] v, np.ndarray[np.uint32_t, ndim=1] k):
    ctealink.tea_encrypt (<uint32_t*> v.data, <uint32_t*> k.data)

def tea_c_decrypt(np.ndarray[np.uint32_t, ndim=1] v, np.ndarray[np.uint32_t, ndim=1] k):
    ctealink.tea_decrypt (<uint32_t*> v.data, <uint32_t*> k.data)

def tea_c_str_encrypt(np.ndarray[np.uint32_t, ndim=1] v, np.ndarray[np.uint32_t, ndim=1] k, length):
    ctealink.tea_str_encrypt(<uint32_t*> v.data, <uint32_t*> k.data, <uint32_t> length)

def tea_c_str_decrypt(np.ndarray[np.uint32_t, ndim=1] v, np.ndarray[np.uint32_t, ndim=1] k, length):
    pos = ctealink.tea_str_pointer_decrypt(<uint32_t*> v.data, <uint32_t*> k.data, <uint32_t> length)
    return pos

def tea_c_str_pointer_encrypt(np.ndarray[np.uint32_t, ndim=1] v, np.ndarray[np.uint32_t, ndim=1] k, length):
    ctealink.tea_str_encrypt(<uint32_t*> v.data, <uint32_t*> k.data, <uint32_t> length)

def tea_c_str_pointer_decrypt(np.ndarray[np.uint32_t, ndim=1] v, np.ndarray[np.uint32_t, ndim=1] k, length):
    pos = ctealink.tea_str_pointer_decrypt(<uint32_t*> v.data, <uint32_t*> k.data, <uint32_t> length)
    return pos