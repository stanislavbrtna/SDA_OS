/*
Copyright (c) 2023 Stanislav Brtna

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef SDA_CRYPTO_STRINGS_H
#define SDA_CRYPTO_STRINGS_H

#include "sda_util.h"

// Encrypts given string using the integrated crypto functions,
// Len is length of the dest buffer.
// Encrypted string is stored as sting of hex values terminated by zero.
uint8_t sda_encrypt_string(uint8_t * source, uint8_t * dest, uint32_t len);

// Decrypts given string using the integrated crypto functions,
// len is length of the dest buffer
uint8_t sda_decrypt_string(uint8_t * source, uint8_t * dest, uint32_t len);

#endif