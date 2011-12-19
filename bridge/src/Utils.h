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
