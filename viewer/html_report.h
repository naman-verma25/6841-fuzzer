#ifndef HTML_REPORT_H
#define HTML_REPORT_H

void generate_html(int crash_id, const char *target_path, const char *input, 
const char *strat, int signal, const char *asan_type, const char *stderr);

#endif
