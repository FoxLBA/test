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
#define INFILES_COUNT 3
#define MAX_TASKS 5

using namespace std;

// globals
//
char* wu_template;
DB_APP app;
FILE *input_file;

char full_input_filename[255];
char input_dir_string[255];

int timestamp;
int current_part = 0;
int total_parts = 0;

MYSQL *conn;
MYSQL *downlevel;
MYSQL_RES *result;
MYSQL_ROW row;
MYSQL_FIELD *field;

char *db_taskID;
char *db_login;
char *db_filename;
char *db_background;
char *db_par1;
char *db_par2;

char infiles[INFILES_COUNT][255];
DB_WORKUNIT wu;

int split_input(const char *db_login, const char *db_filename) {
    char split[255];
    char buff[255];

    log_messages.printf(MSG_NORMAL, "Found new file \"%s/%s\", processing...\n", db_login, db_filename);
    // Путь до ожидающего обработки файла
    sprintf(full_input_filename, "%s/%s/holograms/%s", config.project_path("user"), db_login, db_filename); //updated
    log_messages.printf(MSG_NORMAL, "Full path: %s\n", full_input_filename);

    // Формирование имени папки для нарезок
    //
//    strncpy(input_dir_string, full_input_filename, strlen(full_input_filename)-4); //FIXME
    sprintf(buff, "%s/%s/holograms/%s", config.project_path("tmp"), db_login, db_filename);
    strncpy(input_dir_string, buff, strlen(buff)-4);  //FIXME

    log_messages.printf(MSG_NORMAL, "full_input_filename: %s\n", full_input_filename);
    log_messages.printf(MSG_NORMAL, "input_dir_string: %s\n", input_dir_string);

    // Запуск скрипта на разделение видеофайла (файл full_input_filename, складывать в input_dir_string)
    //
    sprintf(split, "%s/ffsplit.sh %s", config.project_path("bin"), full_input_filename);
    log_messages.printf(MSG_NORMAL, "==SCRIPT STARTING==: %s\n", split);
    // вызов скрипта
    return system(split)>>8;
}

int process_input(const char *input_filename, const char *db_filename) {
    log_messages.printf(MSG_NORMAL, "Processing input: %s\n", input_filename);
    char name[255];
    char newname[255];
    char oldname[255];
    char basename[255];
    char extension[255];
    int retval;

    // Чтение строки, формирование путей и имён
    //
    // Имя воркюнита
    sscanf(db_filename, "%[^.].%[^.]", basename, extension);
    sprintf(name, "%s_%s_%s_%s_%d_%d_%d.%s", app.name, db_taskID, db_login, basename, timestamp, current_part, total_parts, extension);
    config.download_path(name, newname);

    sprintf(oldname, "%s/%s", input_dir_string, input_filename);
    log_messages.printf(MSG_NORMAL, "oldname: %s\n", oldname);
    log_messages.printf(MSG_NORMAL, "newname: %s\n", newname);

    retval = boinc_copy(oldname, newname);
    if (retval) {
        log_messages.printf(MSG_CRITICAL, "Error [%d] renaming file %s to %s\n", retval, oldname, newname);
    } else {
        log_messages.printf(MSG_NORMAL, "File successfully renamed\n");
        retval = boinc_delete_file(oldname);
        if (retval)
            log_messages.printf(MSG_CRITICAL, "Error [%d] removing file %s\n", retval, oldname);
    }

    strcpy(wu.name, name);

    strcpy(infiles[0], name);
    for (int i=0; i<3; i++) {
        log_messages.printf(MSG_NORMAL, "infiles[%d]=%s\n", i, infiles[i]);
    }
    log_messages.printf(MSG_NORMAL, "============\n");

    return 0;
}

int process_background(char *filename) {
    log_messages.printf(MSG_NORMAL, "Processing background: %s\n", filename);
    char path[255];
    char outname[255];
    char basename[255];
    char extension[255];

    sscanf(filename, "%[^.].%[^.]", basename, extension);

    // Путь до бекграунда
    sprintf(full_input_filename, "%s/%s/backgrounds/%s", config.project_path("user"), db_login, filename);
    log_messages.printf(MSG_NORMAL, "Full background path: %s\n", full_input_filename);
    sprintf(outname, "%s_%s_%s_%s_%d_%d_%d.%s", app.name, db_taskID, db_login, basename, timestamp, current_part, total_parts, extension);

    config.download_path(outname, path);
    log_messages.printf(MSG_NORMAL, "Writing background to: %s\n", path);

    strcpy(infiles[1], outname);
    for (int i=0; i<3; i++) {
        log_messages.printf(MSG_NORMAL, "infiles[%d]=%s\n", i, infiles[i]);
    }
    log_messages.printf(MSG_NORMAL, "============\n");

    return boinc_copy(full_input_filename, path);
}

int process_config(const char *par1, const char *par2) {
    char in_par1[255];
    char in_par2[255];
    strcpy(in_par1, par1);
    strcpy(in_par2, par2);
    log_messages.printf(MSG_NORMAL, "Processing config: %s %s\n", in_par1, in_par2);
    FILE *configfile;
    int i=0;
    char path[255];
    char filename[255];
    // Имя конфига
    sprintf(filename, "%s_%s_%s_config_%d_%d_%d.cfg", app.name, db_taskID, db_login, timestamp, current_part, total_parts);

    // Путь до конфига
    config.download_path(filename, path);
    log_messages.printf(MSG_NORMAL, "Writing config to: %s\n", path);

    // Здесь парсятся поля, открывается файл path и в него пишется текст конфига
    configfile = fopen(path, "w");
    if (configfile==NULL) {
        printf("config open error\n");
    }

    // Парсинг полей
    while (in_par1[i]!='\0' || in_par2[i]!='\0') {
        if (in_par1[i]=='&') {
            in_par1[i]='\n';
        }
        if (in_par2[i]=='&') {
            in_par2[i]='\n';
        }
        i++;
    }

    // Запись конфига. Возможно потребуется замена path на filename
    fprintf(configfile, "Filename=%s\n[Main parameters]\n%s\n\n[Search parameters]\n%s", path, in_par1, in_par2);
    fclose(configfile);

    strcpy(infiles[2], filename);
    for (int i=0; i<3; i++) {
        log_messages.printf(MSG_NORMAL, "infiles[%d]=%s\n", i, infiles[i]);
    }
    log_messages.printf(MSG_NORMAL, "============\n");

    return 0;
}

// create one new job
//
int make_job(char *db_taskID) {
    int tid;
    tid = atoi(db_taskID);
    // Fill in the job parameters
    //
    wu.appid = app.id;
    wu.batch = tid;
    wu.rsc_fpops_est = 1e15;
    wu.rsc_fpops_bound = 1e17;
    wu.rsc_memory_bound = 1e8;
    wu.rsc_disk_bound = 1e8;
    wu.delay_bound = 86400;
    wu.min_quorum = REPLICATION_FACTOR;
    wu.target_nresults = REPLICATION_FACTOR;
    wu.max_error_results = REPLICATION_FACTOR*4;
    wu.max_total_results = REPLICATION_FACTOR*8;
    wu.max_success_results = REPLICATION_FACTOR*4;

    // Register the job with BOINC
    //
    const char* in[] = {infiles[0], infiles[1], infiles[2]};
    log_messages.printf(MSG_NORMAL, "Creating work\n\n");
    return create_work(
        wu,
        wu_template,
        "templates/result.xml",
        config.project_path("templates/result.xml"),
        in,
        INFILES_COUNT,
        config
    );
}

int st1_count() {  //FIXME
    mysql_query(conn, "select count(taskID) from task where status='1'");  //tasks->task
    result = mysql_store_result(conn);
    row = mysql_fetch_row(result);
    log_messages.printf(MSG_NORMAL, "Tasks currently running: %s\n", row[0]);
    return atoi(row[0]);
}

int cancel_wu() {
    char *c_task_id;
    char buff[255];
    log_messages.printf(MSG_NORMAL, "Trying to find task where STATUS=3 or DEL=1...\n");   //tasks->task
    mysql_query(conn, "select taskID from task where status=3 or del=1");  //tasks->task
    result = mysql_store_result(conn);
    while ((row = mysql_fetch_row(result))) {
        c_task_id = row[0];
        // Ставится соответствие taskID и filename планктона воркюнитам из таблицы workunit
        // в результате имеется начальный и конечный id
        // ВАЖНО: id ву и результатов могут не совпадать!
        sprintf(buff, "update result set server_state=5, outcome=5 where server_state=2 and batch=%s", c_task_id);
        mysql_query(downlevel, buff);
        sprintf(buff, "update workunit set error_mask=error_mask|16 where batch=%s", c_task_id);
        mysql_query(downlevel, buff);
    }
    return 0;
}

void main_loop() {
    char buff[255];
    char input_filename[255];
    int retval;
    int st1;

    check_stop_daemons();
    // Сканируем базу каждые SLEEP_INTERVAL секунд
    //
    while (1) {
        cancel_wu();
        st1 = st1_count();
        if (st1 < MAX_TASKS) {  //FIXME
            log_messages.printf(MSG_NORMAL, "Scanning database for pending files...\n");
            //запрос на все ожидающие файлы
            sprintf(buff, "select taskID, login, filename, background, par1, par2 from task inner join user on uid=id where status = '2' and del <> '1' order by taskID limit %d", MAX_TASKS-st1); //this was updated users->user, //tasks->task
            mysql_query(conn, buff);
            result = mysql_store_result(conn);
            // Подсчёт количества столбцов. Пока не используется
            //
            //num_fields = mysql_num_fields(result);
            while ((row = mysql_fetch_row(result))) {
                db_taskID =     row[0];
                db_login =      row[1];
                db_filename =   row[2];
                db_background = row[3];
                db_par1 =       row[4];
                db_par2 =       row[5];

                total_parts = split_input(db_login, db_filename);

                // Открыть папку для сканирования нарезок
                DIRREF input_dir = dir_open(input_dir_string);
                vector<string> input_vector;
                while (!(dir_scan(input_filename, input_dir, sizeof(input_filename)))) {
                    input_vector.push_back(input_filename);
                }
                sort (input_vector.begin(), input_vector.end());

                timestamp = time(0);
                current_part = 0;
                for (unsigned int i = 0; i < input_vector.size(); i++) {
                    current_part++;
                    wu.clear();
                    retval = process_input(input_vector[i].c_str(), db_filename);
                    if (retval) { log_messages.printf(MSG_CRITICAL, "Can't create process input: %d\n", retval); }
                    retval = process_background(db_background);
                    if (retval) {log_messages.printf(MSG_CRITICAL, "Can't create process background: %d\n", retval); }
                    retval = process_config(db_par1, db_par2);
                    if (retval) { log_messages.printf(MSG_CRITICAL, "Can't create process config: %d\n", retval); }
                    retval = make_job(db_taskID);
                    if (retval) {
                        log_messages.printf(MSG_CRITICAL, "Can't create job: %d\n", retval);
                        exit(1);
                    }
                    memset(infiles, 0, sizeof(infiles));
                }

                retval = boinc_rmdir(input_dir_string);
                if (retval) {
                    log_messages.printf(MSG_CRITICAL, "Can't remove directory %s: %d\n", input_dir_string, retval);
                    exit(1);
                }
                memset(input_dir_string, 0 , sizeof(input_dir_string));

                // Изменение статуса файла в БД планктон
                //
                log_messages.printf(MSG_NORMAL, "row[0] (taskID): %s\n", db_taskID);
                sprintf(buff, "UPDATE task SET status=1, startDate=NOW() WHERE taskID=%s\n", db_taskID);   //tasks->task
                log_messages.printf(MSG_NORMAL, "buff for query (status update): %s", buff);
                mysql_query(conn, buff);
            }
        }
        sleep(SLEEP_INTERVAL);
    }
}

void usage(char *name) {
    fprintf(stderr,
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
    if (app.lookup("where name='test-plankton'")) {
        log_messages.printf(MSG_CRITICAL, "can't find app\n");
        exit(1);
    }
    if (read_file_malloc(config.project_path("templates/wu.xml"), wu_template)) {
        log_messages.printf(MSG_CRITICAL, "can't read WU template\n");
        exit(1);
    }

    // инициализация подключения к БД dims
    conn = mysql_init(NULL);
    if (conn == NULL) {
        log_messages.printf(MSG_CRITICAL, "Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
        exit(1);
    }
    // подключение к БД dims
    if (mysql_real_connect(conn, "127.0.0.1", "boinc", "2011$bOiNc", "dihm1", 3313, NULL, 0) == NULL) {
        log_messages.printf(MSG_CRITICAL, "Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
        exit(1);
    }
    // инициализация подключения к БД нижнего уровня
    downlevel = mysql_init(NULL);
    if (downlevel == NULL) {
        log_messages.printf(MSG_CRITICAL, "Error %u: %s\n", mysql_errno(downlevel), mysql_error(downlevel));
        exit(1);
    }
    // подключение к БД нижнего уровня
    if (mysql_real_connect(downlevel, "localhost", "root", "password!stronk!", "boinc_test", 0, NULL, 0) == NULL) {
        log_messages.printf(MSG_CRITICAL, "Error %u: %s\n", mysql_errno(downlevel), mysql_error(downlevel));
        exit(1);
    }

    log_messages.printf(MSG_NORMAL, "Starting\n");
    main_loop();
}
