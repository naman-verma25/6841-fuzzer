#include <stdio.h>
#include <string.h>
#include <time.h>

void generate_html(int crash_id, const char *target_path, const char *input, const char *strat, int signal,
const char *asan_type, const char *stderr) {
    
    char htmlfile[256];
    snprintf(htmlfile, sizeof(htmlfile), "crashes/crash_report_%d.html", crash_id);
    FILE *html = fopen(htmlfile, "w");
    
    if (!html) {
        perror("Failed to write HTML report");
        return;
    }

    fprintf(html,
        "<html><head><title>Crash Report #%d</title>"
        "<style>"
        "body { font-family: monospace; background: #1e1e1e; color: #ddd; padding: 2em; }"
        ".meta { background: #2e2e2e; padding: 1em; border-left: 5px solid #555; margin-bottom: 2em; }"
        ".label { color: #999; font-weight: bold; }"
        ".bug { color: #ff6666; font-weight: bold; }"
        "pre { background: #2e2e2e; padding: 1em; overflow-x: auto; }"
        "</style></head><body>"
        "<h1>Crash Report #%d</h1>"
        "<div class='meta'><div><span class='label'>Target:</span> %s</div>"
        "<div><span class='label'>Timestamp:</span> %ld</div>"
        "<div><span class='label'>Strategy:</span> %s</div>"
        "<div><span class='label'>Input Length:</span> %lu</div>"
        "<div><span class='label'>Exit Signal:</span> %d</div>"
        "<div><span class='label'>Bug Type:</span> <span class='bug'>%s</span></div></div>",
        crash_id, crash_id, target_path, time(NULL), strat, signal, strlen(asan_type) > 0 ? asan_type : "Unknown"
    );

    fprintf(html, "<h2>Input</h2><pre>%s</pre>", input);

    FILE *err = fopen(stderr, "r");
    if (err) {
        fprintf(html, "<h2>ASan Output</h2><pre>");
        char line[1024];
        while (fgets(line, sizeof(line), err)) {
            fputs(line, html);
        }
        fprintf(html, "</pre>");
        fclose(err);
    } else {
        fprintf(html, "<p>No stderr output captured.</p>");
    }

    fprintf(html, "<h2>Suggested Fix</h2><pre>");
    if (strcmp(asan_type, "stack-buffer-overflow") == 0) {
        fprintf(html, "Avoid strcpy/sprintf. Use strncpy/snprintf with bounds checks.");
    } else if (strcmp(asan_type, "heap-buffer-overflow") == 0) {
        fprintf(html, "Ensure sufficient heap allocation. Check buffer limits.");
    } else if (strcmp(asan_type, "use-after-free") == 0) {
        fprintf(html, "Do not use freed memory. Set pointer to NULL after free().");
    } else if (strcmp(asan_type, "double-free") == 0) {
        fprintf(html, "Ensure memory is only freed once. Nullify pointer after free().");
    } else {
        fprintf(html, "Review crash trace and fix memory misuse.");
    }

    fprintf(html, "</pre></body></html>");
    fclose(html);
}