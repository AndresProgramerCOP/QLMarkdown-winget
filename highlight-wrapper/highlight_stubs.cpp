#include "wrapper_highlight.h"
#include <cstdlib>
#include <cstring>

#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT __attribute__((visibility("default")))
#endif

#ifdef __cplusplus
extern "C" {
#endif

EXPORT char *get_highlight_version(void) { return strdup("0.0.0"); }
EXPORT char *get_highlight_website(void) { return strdup(""); }
EXPORT char *get_highlight_email(void) { return strdup(""); }
EXPORT const char *get_lua_info(void) { return ""; }
EXPORT void highlight_init(const char *search_dir) {}
EXPORT int highlight_init_generator(void) { return 0; }
EXPORT void highlight_release_generator(void) {}
EXPORT const char *highlight_get_current_theme(void) { return ""; }
EXPORT int highlight_set_current_theme(const char *theme) { return 0; }
EXPORT char *highlight_get_current_font(void) { return strdup(""); }
EXPORT void highlight_set_current_font(const char *font, const char *font_size) {}
EXPORT char *highlight_get_current_font_size(void) { return strdup(""); }
EXPORT void highlight_set_current_font_size(const char *font_size) {}
EXPORT int highlight_get_print_line_numbers(void) { return 0; }
EXPORT void highlight_set_print_line_numbers(int state) {}
EXPORT void highlight_set_formatting_mode(int wrap_at_characters, int tab_replace_spaces) {}
EXPORT void highlight_format_string(const char *code, const char *language, void *context, ResultCallback callback, int export_fragment) {}
EXPORT char *highlight_format_string2(const char *code, const char *language, int *exit_code, int export_fragment) {
    if (exit_code) *exit_code = 0;
    return strdup(code); // Return code raw
}
EXPORT void highlight_format_style(void *context, ResultCallback callback, const char *background) {}
EXPORT char *highlight_format_style2(int *exit_code, const char *background) {
    if (exit_code) *exit_code = 0;
    return strdup("");
}
EXPORT int highlight_list_themes( void *context, ResultThemeListCallback callback) { return 0; }
EXPORT int highlight_list_themes2(HThemeInfo ***theme_list, int *count, ReleaseThemeInfoList *release) { return 0; }
EXPORT int highlight_get_theme( const char *theme, void *context, ResultThemeCallback callback) { return 0; }
EXPORT HTheme *highlight_get_theme2(const char *theme, int *exit_code, ReleaseTheme *release) { return nullptr; }

#ifdef __cplusplus
}
#endif
