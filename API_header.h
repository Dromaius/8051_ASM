#ifdef __WIN32

#define API_HIDE 
#define API_SHOW __attribute__ ((dllexport))

#define API_HIDE_var 
#define API_SHOW_var __attribute__ ((dllexport))

#else

#define API_HIDE __attribute__ ((visibility ("hidden")))
#define API_SHOW __attribute__ ((visibility ("default")))

#define API_HIDE_var
#define API_SHOW_var

#endif