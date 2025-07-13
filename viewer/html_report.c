#include <stdio.h>
#include <string.h>
#include <time.h>

void write_suggestions(FILE *html, const char *asan_type);

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
        "<!DOCTYPE html><html><head><meta charset='UTF-8'>"
        "<title>Crash Report #%d</title>"
        "<style>"
        "body { font-family: 'Segoe UI', Tahoma, sans-serif; background-color: #f9f9f9; color: #333; margin: 40px; }"
        "h1, h2 { color: #004080; border-bottom: 1px solid #ccc; padding-bottom: 5px; }"
        "pre { background-color: #f0f0f0; padding: 10px; border-radius: 5px; overflow-x: auto; }"
        ".label { font-weight: bold; display: inline-block; width: 180px; }"
        ".bug { color: red; font-weight: bold; }"
        "div { margin-bottom: 10px; }"
        "</style>"
        "<script src='https://cdn.jsdelivr.net/npm/chart.js'></script>"
        "</head><body>"
        "<h1>Crash Report #%d</h1>",
        crash_id, crash_id
    );

    time_t now = time(NULL);
    struct tm *tinfo = localtime(&now);
    char time_str[64];
    strftime(time_str, sizeof(time_str), "%A %d %B %Y %I:%M:%S %p", tinfo);
    fprintf(html,
        "<div><span class='label'>Crash ID:</span> %d</div>"
        "<div><span class='label'>Target:</span> %s</div>"
        "<div><span class='label'>Timestamp:</span> %s</div>"
        "<div><span class='label'>Mutation Strategy:</span> %s</div>"
        "<div><span class='label'>Exit Signal:</span> %d</div>"
        "<div><span class='label'>Bug Type:</span> <span class='bug'>%s</span></div>",
        crash_id,
        target_path,
        time_str,
        strat,
        signal,
        asan_type && strlen(asan_type) > 0 ? asan_type : "Unknown"
    );

    fprintf(html, "<h2>Input</h2><details open><summary>Show Input</summary><pre>%s</pre></details>", input);


    FILE *err = fopen(stderr, "r");
    if (err) {
        fprintf(html, "<h2>ASan Output</h2>");
        fprintf(html, "<details><summary>Show ASan Output</summary><pre>");
        
        char line[1024];
        while (fgets(line, sizeof(line), err)) {
            fputs(line, html);
        }

        fprintf(html, "</pre></details>");
        fclose(err);
    } else {
        fprintf(html, "<p>No stderr output captured.</p>");
    }

    fprintf(html,
        "<h2>Vulnerability Profile</h2>"
        "<div style='max-width: 490px; margin: 0 auto;'>"
        "<canvas id='radarChart'></canvas>"
        "</div>"
    );

    // values for the radar chart
    int exploit = 0, reproduce = 0, impact = 0, fix = 0, freq = 0, detect = 0;

    // Classify based on asan_type
    if (strcmp(asan_type, "stack-buffer-overflow") == 0) {
        exploit = 3; reproduce = 5; impact = 4; fix = 2; freq = 4; detect = 2;
    } else if (strcmp(asan_type, "heap-buffer-overflow") == 0) {
        exploit = 4; reproduce = 5; impact = 4; fix = 3; freq = 4; detect = 3;
    } else if (strcmp(asan_type, "use-after-free") == 0) {
        exploit = 5; reproduce = 5; impact = 4; fix = 3; freq = 5; detect = 4;
    } else if (strcmp(asan_type, "format-strings") == 0) {
        exploit = 5; reproduce = 5; impact = 5; fix = 3; freq = 5; detect = 1;
    }

    fprintf(html,
        "<script>"
        "const ctx = document.getElementById('radarChart').getContext('2d');"
        "new Chart(ctx, {"
        "type: 'radar',"
        "data: {"
        "labels: ['Exploitability', 'Reproducibility', 'Impact', 'Fix Complexity', 'Frequency', 'Detection Difficulty'],"
        "datasets: [{"
        "label: 'Crash Severity',"
        "data: [%d, %d, %d, %d, %d, %d],"
        "backgroundColor: 'rgba(255, 99, 132, 0.2)',"
        "borderColor: 'rgba(255, 99, 132, 1)',"
        "borderWidth: 2"
        "}]},"
        "options: {"
        "scales: { r: { suggestedMin: 0, suggestedMax: 5 } }"
        "}});"
        "</script>",
        exploit, reproduce, impact, fix, freq, detect
    );
    write_suggestions(html, asan_type);
    fprintf(html, "</pre></body></html>");
    fclose(html);
}

void write_suggestions(FILE *html, const char *asan_type) {
    fprintf(html, "<h2>Suggested Fix</h2><pre>");
    if (strcmp(asan_type, "stack-buffer-overflow") == 0) {
        fprintf(html,
            "<details><summary>Stack Buffer Overflow</summary><pre>"
            "<b>Description:</b> This occurs when a program writes more data to a stack-allocated buffer than it can hold.\n\n"
            "<b>Why it happens:</b>\n"
            "- Usage of unsafe functions like <code>strcpy</code>, <code>sprintf</code>, etc.\n"
            "- Lack of bounds checks before writing to stack arrays.\n\n"
            "<b>Fix:</b>\n"
            "- Replace unsafe functions with safe versions (e.g., <code>strncpy</code>, <code>snprintf</code>).\n"
            "- Always enforce size checks before writing.\n"
            "</pre></details>"
        );
    } else if (strcmp(asan_type, "heap-buffer-overflow") == 0) {
        fprintf(html,
            "<details><summary>Heap Buffer Overflow</summary><pre>"
            "<b>Description:</b> This occurs when you write beyond the allocated heap memory bounds.\n\n"
            "<b>Why it happens:</b>\n"
            "- Allocating less memory than needed.\n"
            "- Off-by-one or incorrect loop bounds.\n\n"
            "<b>Fix:</b>\n"
            "- Check allocations match the maximum expected input size.\n"
            "- Use safer functions and add manual null termination if needed.\n"
            "</pre></details>"
        );
    } else if (strcmp(asan_type, "use-after-free") == 0) {
        fprintf(html,
            "<details><summary>Use-After-Free</summary><pre>"
            "<b>Description:</b> Occurs when memory is accessed after it's been freed.\n\n"
            "<b>Why it happens:</b>\n"
            "- Dereferencing or printing a pointer after <code>free()</code>.\n"
            "- Forgetting to nullify freed pointers.\n\n"
            "<b>Fix:</b>\n"
            "- Set freed pointers to NULL immediately.\n"
            "- Avoid accessing memory after free.\n"
            "</pre></details>"
        );
    } else if (strcmp(asan_type, "format-strings") == 0) {
        fprintf(html,
            "<details><summary>Format String Vulnerability</summary><pre>"
            "<b>Description:</b> Format string vulnerabilities occur when user-controlled input is passed directly to functions like <code>printf</code> without a format specifier.\n\n"
            "<b>Impact:</b>\n"
            "- Can allow memory leaks or even arbitrary writes using specifiers like <code>%%n</code>.\n"
            "- In severe cases, attackers may hijack execution flow or leak secrets.\n\n"
            "<b>Fix:</b>\n"
            "- Never pass user input directly as the format string.\n"
            "- Always use a fixed format like <code>printf(\"%%s\", input)</code>.\n"
            "- Use format-safe logging or sanitization libraries.\n"
            "</pre></details>"
        );
    } else {
        fprintf(html,
            "<details><summary>Unknown or Unclassified Bug</summary><pre>"
            "<b>Description:</b> Unable to determine the exact memory bug.\n\n"
            "<b>Fix:</b>\n"
            "- Review ASan output carefully.\n"
            "- Check recent changes in memory allocation and pointer logic.\n"
            "- Use <code>valgrind</code> or <code>gdb</code> to inspect the flow.\n"
            "</pre></details>"
        );
    }
}