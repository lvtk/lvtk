
#ifndef LVTK_H_INCLUDED
#define LVTK_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
#    define LVTK_EXTERN extern "C"
#else
#    define LVTK_EXTERN
#endif

#ifdef _WIN32
// windows exports
#    if defined(LVTK_SHARED_BUILD)
#        define LVTK_API __declspec(dllexport)
#        pragma warning(disable : 4251)
#    elif defined(LVTK_SHARED)
#        define LVTK_API __declspec(dllimport)
#        pragma warning(disable : 4251)
#    endif
#    define LVTK_PLUGIN_EXPORT LVTK_EXTERN __declspec(dllexport)
#else
#    if defined(LVTK_SHARED) || defined(LVTK_SHARED_BUILD)
#        define LVTK_API __attribute__ ((visibility ("default")))
#    endif
#    define LVTK_PLUGIN_EXPORT LVTK_EXTERN __attribute__ ((visibility ("default")))
#endif

#define LVTK_EXPORT LVTK_EXTERN LVTK_API

#ifndef LVTK_API
#    define LVTK_API
#endif

// export macro: includes extern C if needed followed by visibility attribute;
#ifndef LVTK_EXPORT
#    define LVTK_EXPORT
#endif

#if __cplusplus
}
#endif

#endif
