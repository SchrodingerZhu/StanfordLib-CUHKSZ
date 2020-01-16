#if defined(__GNUC__) || defined(__clang__)
#define unreachable()  __builtin_unreachable()
#define unlikely(x)     __builtin_expect((x),0)
#define likely(x)       __builtin_expect((x),1)
#else
#define unlikely(x)     (x)
#define likely(x)       (x)
#define unreachable()
#endif
