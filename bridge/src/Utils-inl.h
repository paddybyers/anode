/*
 * Copyright 2011-2012 Paddy Byers
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 */

#ifndef BRIDGE_UTILSINL_H
#define BRIDGE_UTILSINL_H

#include "Utils.h"
#include "defines.h"
#include <string.h>

template <class T> TArray<T> *TArray<T>::New(unsigned int count, unsigned int increment) {
  TArray<T> *ob = new TArray<T>(increment);
  if(ob) {
    if(ob->alloc(count) == ErrorMem) {
      delete ob;
      ob = 0;
    }
  }
  return ob;
}

template <class T> TArray<T>::TArray(unsigned int increment) {
  arr = 0;
  count = 0;
  this->increment = increment;
}

template <class T> T TArray<T>::get(unsigned int idx) {
  return arr[idx];
}

template <class T> T *TArray<T>::addr(unsigned int idx) {
  return &arr[idx];
}

template <class T> int TArray<T>::put(unsigned int idx, T value) {
  if(idx >= count) {
    int newCount = (idx + increment) & -increment;
    int result = alloc(newCount);
    if(result != OK) return result;
  }
  arr[idx] = value;
  return OK;
}

template <class T> int TArray<T>::setLength(unsigned int count) {
  int result = OK;
  if(count >= this->count)
    result = alloc(count);
  if(result == OK)
    this->count = count;
  return result;
}

template <class T> int TArray<T>::alloc(unsigned int count) {
  if(count > 0) {
    T *arr = new T[count];
    if(!arr) return ErrorMem;
    if(this->arr)
      memcpy(arr, this->arr, this->count * sizeof(T));
    memset(&arr[this->count], 0, (count - this->count) * sizeof(T));
    this->count = count;
    this->arr = arr;
  }
  return OK;
}

template <class T> TArray<T>::~TArray() {
  delete[] arr;
}

#endif
