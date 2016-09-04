# -*- coding: utf-8 -*-
'''
Created on 2014-07-16
@summary: Just a python module with c and cython for encrypt and decrypt with 
          interleaving and random padding random number tea.
@author: fiefdx
'''

import os
import sys
import struct
import logging
import time
import binascii
import array

from random import seed
from random import randint
import ctea
import numpy as np

LOG = logging.getLogger(__name__)

op_64 = 0xffffffffffffffff

def str_c_encrypt(v, k):
    '''
    v is unicode or string
    k is md5 unicode
    return string
    '''
    v = v.encode("utf-8") if isinstance(v, unicode) else v
    k = str(k)
    # ascii str to bin str
    k = binascii.unhexlify(k)
    # LOG.info(">>>>>>>>>>>>>>k: %s",type(k))
    result = ""
    cipertext = op_64
    pre_plaintext = op_64
    end_char = "\0"
    fill_n_or = 0xf8
    v_length = len(v)
    fill_n = (8 - (v_length + 2))%8 + 2
    fill_s = ""
    for i in xrange(fill_n):
        fill_s = fill_s + chr(randint(0, 0xff))
    v = (chr((fill_n - 2) | fill_n_or) + fill_s + v + end_char * 7)
    k0, k1, k2, k3 = struct.unpack(">LLLL", k)
    k_c = np.ascontiguousarray([k0, k1, k2, k3], dtype = np.uint32)
    for i in xrange(0, len(v), 8):
        if i == 0:
            v0, v1 = struct.unpack(">LL", v[i:i + 8])
            v_c = np.ascontiguousarray([v0, v1], dtype = np.uint32)
            ctea.tea_c_encrypt(v_c, k_c)
            encrypt_text = struct.pack('>LL', v_c[0], v_c[1])
            result += encrypt_text
            cipertext = struct.unpack(">Q", encrypt_text)[0]
            pre_plaintext = struct.unpack(">Q", v[i:i + 8])[0]
        else:
            plaintext = struct.unpack(">Q", v[i:i + 8])[0] ^ cipertext
            v0, v1 = struct.unpack(">LL", struct.pack(">Q", plaintext))
            v_c = np.ascontiguousarray([v0, v1], dtype = np.uint32)
            ctea.tea_c_encrypt(v_c, k_c)
            encrypt_text = struct.pack('>LL', v_c[0], v_c[1])
            encrypt_text = struct.pack(">Q", struct.unpack(">Q", encrypt_text)[0] ^ pre_plaintext)
            result += encrypt_text
            cipertext = struct.unpack(">Q", encrypt_text)[0]
            pre_plaintext = plaintext
    # bin to ascii return string
    return binascii.hexlify(result)

def str_c_decrypt(v, k):
    '''
    v is unicode or string
    k is md5 unicode
    return string
    '''
    v = v.encode("utf-8") if isinstance(v, unicode) else v
    k = str(k)
    # ascii to bin
    v = binascii.unhexlify(v)
    k = binascii.unhexlify(k)
    result = ""
    cipertext = op_64
    pre_plaintext = op_64
    pos = 0
    k0, k1, k2, k3 = struct.unpack(">LLLL", k)
    k_c = np.ascontiguousarray([k0, k1, k2, k3], dtype = np.uint32)
    for i in xrange(0, len(v), 8):
        if i == 0:
            cipertext = struct.unpack(">Q", v[i:i + 8])[0]
            v0, v1 = struct.unpack(">LL", v[i:i + 8])
            v_c = np.ascontiguousarray([v0, v1], dtype = np.uint32)
            ctea.tea_c_decrypt(v_c, k_c)
            plaintext = struct.pack('>LL', v_c[0], v_c[1])
            pos = (ord(plaintext[0]) & 0x07) + 2
            result += plaintext
            pre_plaintext = struct.unpack(">Q", plaintext)[0]
        else:
            encrypt_text = struct.pack(">Q", struct.unpack(">Q", v[i:i + 8])[0] ^ pre_plaintext)
            v0, v1 = struct.unpack(">LL", encrypt_text)
            v_c = np.ascontiguousarray([v0, v1], dtype = np.uint32)
            ctea.tea_c_decrypt(v_c, k_c)
            plaintext = struct.pack('>LL', v_c[0], v_c[1])
            plaintext = struct.unpack(">Q", plaintext)[0] ^ cipertext
            result += struct.pack(">Q", plaintext)
            pre_plaintext = plaintext ^ cipertext
            cipertext = struct.unpack(">Q", v[i:i + 8])[0]

    # if result[-7:] != "\0" * 7: return None
    if result[-7:] != "\0" * 7: return ""
    return result[pos + 1: -7]

def tea_str_c_encrypt(v, k):
    '''
    v is unicode or string
    k is md5 unicode
    return string
    '''
    v = v.encode("utf-8") if isinstance(v, unicode) else v
    k = str(k)
    # ascii str to bin str
    k = binascii.unhexlify(k)
    # LOG.info(">>>>>>>>>>>>>>k: %s",type(k))
    result = ""
    end_char = "\0"
    fill_n_or = 0xf8
    v_length = len(v)
    fill_n = (8 - (v_length + 2))%8 + 2
    fill_s = ""
    for i in xrange(fill_n):
        fill_s = fill_s + chr(randint(0, 0xff))
        # fill_s = fill_s + chr(0x02)
    v = (chr((fill_n - 2) | fill_n_or) + fill_s + v + end_char * 7)
    k0, k1, k2, k3 = struct.unpack(">LLLL", k)
    k_c = np.ascontiguousarray([k0, k1, k2, k3], dtype = np.uint32)
    v_new_length = len(v) # after add '\0'
    v_pack_str = ">" + "LL"*(v_new_length/8)
    tube_v = struct.unpack(v_pack_str, v)
    v_c = np.ascontiguousarray(tube_v, dtype = np.uint32)
    ctea.tea_c_str_encrypt(v_c, k_c, v_new_length/4)
    # ctea.tea_c_str_pointer_encrypt(v_c, k_c, v_new_length/4)
    for n,i in enumerate(v_c):
        result += struct.pack(">L", i)
    return binascii.hexlify(result)

def tea_str_c_decrypt(v, k):
    '''
    v is unicode or string
    k is md5 unicode
    return string
    '''
    v = v.encode("utf-8") if isinstance(v, unicode) else v
    k = str(k)
    # ascii to bin str
    v = binascii.unhexlify(v)
    k = binascii.unhexlify(k)
    result = ""
    v_length = len(v)
    pos = 0
    k0, k1, k2, k3 = struct.unpack(">LLLL", k)
    k_c = np.ascontiguousarray([k0, k1, k2, k3], dtype = np.uint32)
    v_pack_str = ">" + "LL"*(v_length/8)
    tube_v = struct.unpack(v_pack_str, v)
    v_c = np.ascontiguousarray(tube_v, dtype = np.uint32)
    pos = ctea.tea_c_str_decrypt(v_c, k_c, v_length/4)
    # pos = ctea.tea_c_str_pointer_decrypt(v_c, k_c, v_length/4)
    plaintext = struct.pack('>L', pos)
    pos = (ord(plaintext[0]) & 0x07) + 2
    # LOG.info("pos: %s", pos)
    for n,i in enumerate(v_c):
        result += struct.pack(">L", i)
    # if result[-7:] != "\0" * 7: return None
    if result[-7:] != "\0" * 7: return ""
    return result[pos + 1: -7]