#ifndef _GITTER_KID_DEFINE_

#ifdef __GNUC__ >= 4
#define G_KID_EXTERN __attribute__((visibility("default")))
#else
#define G_KID_EXTERN
#endif // __GNUC__

#endif //_GITTER_KID_DEFINE_