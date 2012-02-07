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

#ifndef BRIDGE_UTILS_H
#define BRIDGE_UTILS_H

template <class T> class TArray {
public:
  static TArray<T> *New(unsigned int count=32, unsigned int increment=32);
  T get(unsigned int idx);
  T *addr(unsigned int idx);
  int put(unsigned int idx, T value);
  int setLength(unsigned int count);
  inline int getLength() {return count;}
  ~TArray();
  
protected:
  TArray(unsigned int increment);
  int alloc(unsigned int count);
  T *arr;
  unsigned int count;
  unsigned int increment;
};

#endif
