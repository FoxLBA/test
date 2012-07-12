#include "str_util.h"
#include "util.h"
#include "filesys.h"
#include "boinc_api.h"

#include "plankton.h"

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
    char inbg_path[512],    outbg_path[512];
    char config_path[512],  log_path[512];

    retval = boinc_init();
    if (retval) {
        fprintf(stderr, "%s boinc_init returned %d\n", boinc_msg_prefix(buf, sizeof(buf)), retval);
        exit(retval);
    }

    // resolve file names
    boinc_resolve_filename(INPUT_FILENAME,      input_path,     sizeof(input_path));
    boinc_resolve_filename(OUTPUT_FILENAME,     output_path,    sizeof(output_path));
    boinc_resolve_filename(INPUT_BACKGROUND,    inbg_path,      sizeof(inbg_path));
    boinc_resolve_filename(OUTPUT_BACKGROUND,   outbg_path,     sizeof(outbg_path));
    boinc_resolve_filename(CONFIG_FILENAME,     config_path,    sizeof(config_path));
    boinc_resolve_filename(LOG_FILENAME,        log_path,       sizeof(log_path));

    // assume we did something with those files
    boinc_copy(inbg_path, outbg_path);
    boinc_copy(config_path, log_path);
/*
    // decrypt
    decrypt_file(input_path);
*/
    // apply filter
    sprintf(buf, "mencoder -vf rectangle=400:400:250:250:red -o %s -oac copy -ovc lavc %s", output_path, input_path);
    fprintf(stderr, "command: %s\n", buf);
    system(buf);
/*
    //encrypt
    encrypt_file(output_path);
*/
    //cleanup and exit
    boinc_fraction_done(1);
    boinc_finish(0);
}
