#include "str_util.h"
#include "util.h"
#include "filesys.h"
#include "boinc_api.h"

#define INPUT_FILENAME "in"
#define OUTPUT_FILENAME "out"
#define INPUT_BACKGROUND "in_back"
#define OUTPUT_BACKGROUND "out_back"
#define CONFIG_FILENAME "config"
#define LOG_FILENAME "log"


int main(int argc, char **argv) {
    int retval;
    char buf[65535];
    char input_path[512],   output_path[512];
    char inputbg_path[512], outputbg_path[512];
    char config_path[512],  log_path[512];

    retval = boinc_init();
    if (retval) {
        fprintf(stderr, "%s boinc_init returned %d\n",
            boinc_msg_prefix(buf), retval);
        exit(retval);
    }

    // resolve file names
    boinc_resolve_filename(INPUT_FILENAME,      input_path,     sizeof(input_path));
    boinc_resolve_filename(OUTPUT_FILENAME,     output_path,    sizeof(output_path));
    boinc_resolve_filename(INPUT_BACKGROUND,    inputbg_path,   sizeof(inputbg_path));
    boinc_resolve_filename(OUTPUT_BACKGROUND,   outputbg_path,  sizeof(outputbg_path));
    boinc_resolve_filename(CONFIG_FILENAME,     config_path,    sizeof(config_path));
    boinc_resolve_filename(LOG_FILENAME,        log_path,       sizeof(log_path));

    // наложение фильтра    ffmpeg -vf drawbox=400:400:250:250:red -i %s %s_copy.avi
    sprintf(buf, "mencoder -vf rectangle=400:400:250:250:red -o %s -oac copy -ovc lavc %s", output_path, input_path);
    fprintf(stderr, "command: %s\n", buf);
    system(buf);

    //cleanup and exit
    boinc_fraction_done(1);
    boinc_finish(0);
}
