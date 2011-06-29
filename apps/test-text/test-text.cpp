#include "str_util.h"
#include "util.h"
#include "filesys.h"
#include "boinc_api.h"
#include "mfile.h"
#include <time.h>

using std::string;

#define INPUT_FILENAME "in"
#define OUTPUT_FILENAME "out"
#define CHECKPOINT_FILE "test_checkpoint"
#define CHECKPOINT_INTERVAL 1.
#define TOTAL_WORK_TIME 100.

int do_checkpoint(int iterations) {
    int retval;
    string resolved_name;

    FILE* f = fopen("temp", "w");
    if (!f) return 1;
    fprintf(f, "%d", iterations);
    fclose(f);

    boinc_resolve_filename_s(CHECKPOINT_FILE, resolved_name);
    retval = boinc_rename("temp", resolved_name.c_str());
    if (retval) return retval;

    return 0;
}

int main(int argc, char **argv) {
    int iterations = 0;
    int checkpoint_iterations = 0;
    char result[512];
    int retval;
    double fract_done = 0;
    char input_path[512], output_path[512], chkpt_path[512], buf[256];
    FILE* infile;
    FILE* checkpoint_file;
    MFILE out;

    retval = boinc_init();
    if (retval) {
        fprintf(stderr, "%s boinc_init returned %d\n",
            boinc_msg_prefix(buf), retval);
        exit(retval);
    }

    // open the input file (resolve logical name first)
    //
    boinc_resolve_filename(INPUT_FILENAME, input_path, sizeof(input_path));
    infile = boinc_fopen(input_path, "r");
    if (!infile) {
        fprintf(stderr,
            "%s Couldn't find input file, resolved name %s.\n",
            boinc_msg_prefix(buf), input_path);
        exit(-1);
    } else {
        fscanf(infile, "%d", &iterations);
        sprintf(result, "%d", iterations);
        fclose(infile);
        if (iterations > TOTAL_WORK_TIME) {
            printf("Input file seems to be corrupt: start iteration is bigger than last\n");
            exit(-2);
        }
    }

    // See if there's a valid checkpoint file.
    // If so seek input file and truncate output file
    //
    boinc_resolve_filename(CHECKPOINT_FILE, chkpt_path, sizeof(chkpt_path));
    checkpoint_file = boinc_fopen(chkpt_path, "r");
    if (checkpoint_file) {
        fscanf(checkpoint_file, "%d", &checkpoint_iterations);
        fclose(checkpoint_file);

        // Догоняем итерации до чекпоинта, восстанавливаем строку
        //
        for (iterations++; iterations <= checkpoint_iterations; iterations++) {
            sprintf(result, "%s,%d", result, iterations);
        }

        iterations = checkpoint_iterations;
        if (iterations > TOTAL_WORK_TIME) {
            printf("Checkpoint file seems to be corrupt: checkpoint iteration is bigger than last\n");
            exit(-2);
        }
    }

    fract_done = iterations / TOTAL_WORK_TIME;

    // main loop
    //
    time_t current, previous;
    double diff;
    time (&previous);

    while (fract_done < 1.) {
        boinc_sleep(1.); //set iteration time
        time (&current); //time_2
        diff = difftime(current, previous);
        if (diff >= CHECKPOINT_INTERVAL) { //set time interval
            printf("%d seconds are gone; fraction done=%f\n", iterations, fract_done);
            time (&previous);
            if (boinc_time_to_checkpoint()) {
                retval = do_checkpoint(iterations);
                if (retval) {
                    fprintf(stderr, "%s APP: test checkpoint failed %d\n",
                        boinc_msg_prefix(buf), retval
                    );
                    exit(retval);
                }
                boinc_checkpoint_completed();
            }
        }
        iterations++;
        sprintf(result, "%s,%d", result, iterations);
        fract_done = iterations / TOTAL_WORK_TIME;
        if (fract_done > 1) {
            fract_done = 1;
        }
        boinc_fraction_done(fract_done);
    }

    //write output file
    boinc_resolve_filename(OUTPUT_FILENAME, output_path, sizeof(output_path));
    retval = out.open(output_path, "wb");
    if (retval) {
        fprintf(stderr, "%s APP: test output open failed:\n",
            boinc_msg_prefix(buf));
        fprintf(stderr, "%s resolved name %s, retval %d\n",
            boinc_msg_prefix(buf), output_path, retval);
        perror("open");
        exit(1);
    }
    out.printf("%s", result);
    retval = out.flush();
    if (retval) {
        fprintf(stderr, "%s APP: test flush failed %d\n",
            boinc_msg_prefix(buf), retval
        );
        exit(1);
    }
    out.close();

    //cleanup and exit
    remove(CHECKPOINT_FILE);
    boinc_fraction_done(1);
    boinc_finish(0);
}
