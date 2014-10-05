#ifndef H_FLOAT
     #define H_FLOAT

#ifdef __cplusplus
extern "C"{
#endif 

void fmtDouble(double val, char precision, char *buf, unsigned bufLen);
unsigned fmtUnsigned(unsigned long val, char *buf, unsigned bufLen, char width);

#ifdef __cplusplus
}
#endif

#endif
