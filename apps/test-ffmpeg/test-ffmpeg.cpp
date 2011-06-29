#include "str_util.h"
#include "util.h"
#include "filesys.h"
#include "boinc_api.h"

#include <unistd.h>
#include <cstdlib>
#include <string>
#include <cstring>

#define INPUT_FILENAME "in"
#define OUTPUT_FILENAME "out"


int main(int argc, char **argv) {
    int retval;
    char input_path[512], output_path[512], tmp_name[512]="tmp.avi", buf[65535];
    char oldname[65535];
    char newname[65535];

    retval = boinc_init();
    if (retval) {
        fprintf(stderr, "%s boinc_init returned %d\n",
            boinc_msg_prefix(buf), retval);
        exit(retval);
    }

    // resolve the input file name
    boinc_resolve_filename(INPUT_FILENAME, input_path, sizeof(input_path));

    // resolve the output file name
    boinc_resolve_filename(OUTPUT_FILENAME, output_path, sizeof(output_path));

    // copy
//    sprintf(buf, "cp %s %s_copy.avi", input_path, input_path);
//    system(buf);

    // наложение фильтра    ffmpeg -vf drawbox=400:400:250:250:red -i %s %s_copy.avi
    sprintf(buf, "mencoder -vf rectangle=400:400:250:250:red -o %s_copy.avi -oac copy -ovc lavc %s", input_path, input_path);
    fprintf(stderr, "command: %s\n", buf);
    system(buf);

    sprintf(oldname, "%s_copy.avi", input_path);
    sprintf(newname, "%s", output_path);

    rename(oldname, newname);

//    sprintf(buf, "cp copy_%s %s", input_path, output_path);

//    boinc_rename(tmp_name, output_path);

    //copy in =>> out
//    sprintf(buf, "cp %s %s", input_path, output_path);
//    printf("copy: %s\n", buf);
//    system(buf);

    //cleanup and exit
    boinc_fraction_done(1);
    boinc_finish(0);
}
