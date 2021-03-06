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

#include <my_global.h>
#include <mysql.h>


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
int current_part = 1;
int total_parts = 0;
int start_iteration;

MYSQL *conn;
MYSQL_RES *result;
MYSQL_ROW row;
MYSQL_FIELD *field;
int num_fields;
int i;
char buff[255];//10
char * pch;
char input_dir_string[255];
char split[255];
// create one new job
//
int make_job() {
    DB_WORKUNIT wu;
    char name[256], path[256];
    const char* infiles[1];
    int rslt;
    char newname[255];
    char oldname[255];
    char basename[255];
    char extension[255];

    // Чтение строки, формирование путей и имён
    //
    // Имя воркюнита
    sscanf(row[2], "%[^.].%[^.]", basename, extension);
    sprintf(name, "%s_%s_%s_%s_%d_%d_%d.%s", app.name, row[0], row[1], basename, timestamp, current_part, total_parts, extension);
    config.download_path(name, path);
    sprintf(newname, "%s", path);
    log_messages.printf(MSG_NORMAL, "newname: %s\n", newname);

    log_messages.printf(MSG_NORMAL, "oldname input_dir_string: %s\n", input_dir_string);
    log_messages.printf(MSG_NORMAL, "oldname input_filename: %s\n", input_filename);

    sprintf(oldname, "%s/%s", input_dir_string, input_filename);
    log_messages.printf(MSG_NORMAL, "oldname: %s\n", oldname);
    rslt = rename(oldname, newname);
    if ( rslt == 0 )
        puts ( "File successfully renamed" );
    else
        perror( "Error renaming file" );

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
    // Сканируем базу каждые 30 секунд
    //
    while (1) {
        log_messages.printf(MSG_NORMAL, "Scanning database for pending files...\n");
        //запрос на все ожидающие файлы
        mysql_query(conn, "select taskID, login, filename, par1, par2 from tasks inner join users on uid=id where status = '2'");
        result = mysql_store_result(conn);
        // Подсчёт количества столбцов. Пока не используется
        //
//        num_fields = mysql_num_fields(result);
        while ((row = mysql_fetch_row(result))) {
            log_messages.printf(MSG_NORMAL, "Found new file \"%s/%s\", processing...\n", row[1], row[2]);
            // Путь до ожидающего обработки файла
            sprintf(full_input_filename, "%s/%s/%s", config.project_path("dir"), row[1], row[2]);
            log_messages.printf(MSG_NORMAL, "Full path: %s\n", full_input_filename);
            // Формирование имени папки для нарезок
            //
//            pch = strpbrk(full_input_filename, ".a");  // ДО ПЕРВОЙ ТОЧКИ
//            while (pch != NULL) {
                strncpy(input_dir_string, full_input_filename, strlen(full_input_filename)-4);
//                pch = strpbrk(pch+1, ".a");
//            }
            log_messages.printf(MSG_NORMAL, "full_input_filename 13: %s\n", full_input_filename);
            log_messages.printf(MSG_NORMAL, "input_dir_string 13: %s\n", input_dir_string);

            total_parts=0;
            sprintf(split, "%s/ffsplit.sh %s", config.project_path("bin"), full_input_filename);
            log_messages.printf(MSG_NORMAL, "==SCRIPT STARTING==: %s\n", split);
            total_parts = system(split)>>8;
            // Запуск скрипта на разделение видеофайла (файл full_input_filename, складывать в input_dir_string)
            //
            // вызов скрипта


            current_part=0;
            timestamp = time(0);
            // Посчитать сколько всего частей
            //
            //while (fscanf (input_file,"%d",&start_iteration) > 0) {
            //    total_lines++;
            //}

            // Открыть папку для сканирования нарезок
            input_dir = dir_open(input_dir_string);

            while (!(dir_scan(input_filename, input_dir, sizeof(input_filename)))) {
                current_part++;
                retval = make_job();
            }

            // Изменение статуса файла в БД планктон
            //
            log_messages.printf(MSG_NORMAL, "row[0] (taskID): %s\n", row[0]);
            sprintf(buff, "UPDATE tasks SET status=1 WHERE taskID=%s\n", row[0]);
            log_messages.printf(MSG_NORMAL, "buff for query (status update): %s", buff);
            mysql_query(conn, buff);

            // Удаление исходного файла будет происходить только после изменения в БД
            //
            //log_messages.printf(MSG_NORMAL, "Processing complete, deleting file \"%s\"\n", input_filename);
            //boinc_delete_file(full_input_filename);
        }
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
    if (app.lookup("where name='test-ffmpeg'")) {
        log_messages.printf(MSG_CRITICAL, "can't find app\n");
        exit(1);
    }
    if (read_file_malloc(config.project_path("templates/test_wu"), wu_template)) {
        log_messages.printf(MSG_CRITICAL, "can't read WU template\n");
        exit(1);
    }

    //инициализация подключения к базе данных
    conn = mysql_init(NULL);
    if (conn == NULL) {
        printf("Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
        exit(1);
    }
    //подключение к БД
    if (mysql_real_connect(conn, "localhost", "root", "password!stronk!", "plankton", 0, NULL, 0) == NULL) {
        printf("Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
        exit(1);
    }

    log_messages.printf(MSG_NORMAL, "Starting\n");
    main_loop();
}
