#ifdef _WIN32
#include "boinc_win.h"
#else
#include "config.h"
#include <cstdio>
#include <cctype>
#include <ctime>
#include <cstring>
#include <cstdlib>
#include <csignal>
#include <unistd.h>
#endif

#include "str_util.h"
#include "util.h"
#include "filesys.h"
#include "boinc_api.h"
#include "mfile.h"
#include "graphics2.h"

using std::string;

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

    // ��������� �������    ffmpeg -vf drawbox=400:400:250:250:red -i %s %s_copy.avi
    sprintf(buf, "mencoder -vf rectangle=400:400:250:250:red -o %s_copy.avi -oac copy -ovc lavc %s", input_path, input_path);
	//sprintf(buf, "defrag C:");
    fprintf(stderr, "command: %s\n", buf);
    system(buf);

    sprintf(oldname, "%s_copy.avi", input_path);
    sprintf(newname, "%s", output_path);

    rename(oldname, newname);

//    boinc_rename(tmp_name, output_path);

    //cleanup and exit
    boinc_fraction_done(1);
    boinc_finish(0);
}

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR Args, int WinMode) {
    LPSTR command_line;
    char* argv[100];
    int argc;

    command_line = GetCommandLine();
    argc = parse_command_line( command_line, argv );
    return main(argc, argv);
}
#endif