#define BETA 1
#define VERSION_NUMBER "4.3.1"
#define MININ 0

#ifdef RELEASE
#if BETA
#define VERSION crypt_str(VERSION_NUMBER " beta")
#else
#define VERSION crypt_str(VERSION_NUMBER)
#endif
#else
#define VERSION crypt_str(VERSION_NUMBER " alpha")
#endif