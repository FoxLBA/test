#include <unistd.h>
#include <cstdlib>
#include <string>
#include <cstring>

#include "boinc_db.h"
#include "error_numbers.h"
#include "backend_lib.h"
#include "parse.h"
#include "util.h"
#include "filesys.h"

#include "sched_config.h"
#include "sched_util.h"
#include "sched_msgs.h"
#include "str_util.h"

#define REPLICATION_FACTOR 1
#define SLEEP_INTERVAL 10

// globals
//
char* wu_template;
DB_APP app;
FILE *input_file;
DIRREF input_dir;
char input_filename[255];
char full_input_filename[255];

int timestamp;
int current_line = 1;
int total_lines = 0;
int start_iteration;


// create one new job
//
int make_job() {
    DB_WORKUNIT wu;
    char name[256], path[256];
    const char* infiles[1];

    // Чтение строки, формирование путей и имён
    //
    sprintf(name, "%s_%s_%d_%d_%d", app.name, input_filename, timestamp, current_line, total_lines);
    config.download_path(name, path);

    // Запись строки в ворк юнит
    //
    FILE *f = fopen(path, "w");
    if (!f) {
        log_messages.printf(MSG_CRITICAL, "Can't open workunit file\n");
        exit(2);
    }
    fprintf(f, "%d", start_iteration);
    fclose(f);

    // Fill in the job parameters
    //
    wu.clear();
    wu.appid = app.id;
    strcpy(wu.name, name);
    wu.rsc_fpops_est = 1e12;
    wu.rsc_fpops_bound = 1e14;
    wu.rsc_memory_bound = 1e8;
    wu.rsc_disk_bound = 1e8;
    wu.delay_bound = 86400;
    wu.min_quorum = REPLICATION_FACTOR;
    wu.target_nresults = REPLICATION_FACTOR;
    wu.max_error_results = REPLICATION_FACTOR*4;
    wu.max_total_results = REPLICATION_FACTOR*8;
    wu.max_success_results = REPLICATION_FACTOR*4;
    infiles[0] = name;

    // Register the job with BOINC
    //
    return create_work(
        wu,
        wu_template,
        "templates/test_result",
        config.project_path("templates/test_result"),
        infiles,
        1,
        config
    );
}

void main_loop() {
    int retval;
    check_stop_daemons();

    // Сканируем папку каждые 30 секунд
    //
    while (1) {
        log_messages.printf(MSG_NORMAL, "Scanning folder for input files\n");
        input_dir = dir_open(config.project_path("incoming"));
        // Цикл по всем файлам в папке
        //
        while (!(dir_scan(input_filename, input_dir, sizeof(input_filename)))) {
            log_messages.printf(MSG_NORMAL, "Found new file \"%s\", processing...\n", input_filename);

            // Открыть и проверить входной файл
            //
            sprintf(full_input_filename, "%s/%s", config.project_path("incoming"), input_filename);
            input_file = fopen(full_input_filename, "r");
            if (!input_file) {
                log_messages.printf(MSG_CRITICAL, "Input file not found\n");
                exit(1);
            }

            // Очиста переменных группы заданий
            //
            total_lines = 0;
            current_line = 0;
            timestamp = time(0);

            // Посчитать число строк с числами
            //
            while (fscanf (input_file,"%d",&start_iteration) > 0) {
                total_lines++;
            }
            rewind(input_file);

            // Пока файл не кончился - делать задания
            //
            while (fscanf(input_file,"%d",&start_iteration) > 0) {
                current_line++;
                retval = make_job();
            }

            // Закрываем и удаляем входной файл
            //
            log_messages.printf(MSG_NORMAL, "Processing complete, deleting file \"%s\"\n", input_filename);
            fclose(input_file);
            boinc_delete_file(full_input_filename);
        }
        dir_close(input_dir);
        sleep(SLEEP_INTERVAL);
    }
}

void usage(char *name) {
    fprintf(stderr, "This is an example BOINC work generator.\n"
        "This work generator has the following properties\n"
        "(you may need to change some or all of these):\n"
        "- Runs as a daemon, and creates an unbounded supply of work.\n"
        "  It attempts to maintain a \"cushion\" of 100 unsent job instances.\n"
        "  (your app may not work this way; e.g. you might create work in batches)\n"
        "- Creates work for the application \"uppercase\".\n"
        "- Creates a new input file for each job;\n"
        "  the file (and the workunit names) contain a timestamp\n"
        "  and sequence number, so that they're unique.\n\n"
        "Usage: %s [OPTION]...\n\n"
        "Options:\n"
        "  [ -d X ]                    Sets debug level to X.\n"
        "  [ -h | --help ]             Shows this help text.\n",
        name
    );
}

int main(int argc, char** argv) {
    int i, retval;

    for (i=1; i<argc; i++) {
        if (is_arg(argv[i], "d")) {
            if (!argv[++i]) {
                log_messages.printf(MSG_CRITICAL, "%s requires an argument\n\n", argv[--i]);
                usage(argv[0]);
                exit(1);
            }
            int dl = atoi(argv[i]);
            log_messages.set_debug_level(dl);
            if (dl == 4) g_print_queries = true;
        } else if (is_arg(argv[i], "h") || is_arg(argv[i], "help")) {
            usage(argv[0]);
            exit(0);
        } else {
            log_messages.printf(MSG_CRITICAL, "unknown command line argument: %s\n\n", argv[i]);
            usage(argv[0]);
            exit(1);
        }
    }

    retval = config.parse_file();
    if (retval) {
        log_messages.printf(MSG_CRITICAL,
            "Can't parse config.xml: %s\n", boincerror(retval)
        );
        exit(1);
    }

    retval = boinc_db.open(
        config.db_name, config.db_host, config.db_user, config.db_passwd
    );
    if (retval) {
        log_messages.printf(MSG_CRITICAL, "can't open db\n");
        exit(1);
    }
    if (app.lookup("where name='test-text'")) {
        log_messages.printf(MSG_CRITICAL, "can't find app\n");
        exit(1);
    }
    if (read_file_malloc(config.project_path("templates/test_wu"), wu_template)) {
        log_messages.printf(MSG_CRITICAL, "can't read WU template\n");
        exit(1);
    }

    log_messages.printf(MSG_NORMAL, "Starting\n");

    main_loop();
}
