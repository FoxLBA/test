#include <ctime>

#include "boinc_db.h"
#include "parse.h"
#include "error_numbers.h"
#include "str_util.h"

#include "sched_config.h"
#include "sched_util.h"
#include "sched_msgs.h"
#include "test_assimilator.h"

using std::vector;

#define LOCKFILE "assimilator.out"
#define PIDFILE  "assimilator.pid"
#define SLEEP_INTERVAL 1

bool update_db = true;
int sleep_interval = SLEEP_INTERVAL;
int g_argc;
char** g_argv;

int main_loop(APP& app) {
    DB_WORKUNIT wu;
    DB_RESULT canonical_result, result;
    char buf[256];
    char buf2[256];
    int retval;
    task_t task;


    while(1) {
        check_stop_daemons();

        sprintf(buf, "where appid=%d and assimilate_state=%d", app.id, ASSIMILATE_READY);

        // Заполнение полей текущего ворк юнита
        retval = wu.enumerate(buf);
        if (retval) {
            if (retval != ERR_DB_NOT_FOUND) {
                log_messages.printf(MSG_DEBUG, "DB connection lost, exiting\n");
                exit(0);
            }
        }
        // Заполнение полей текущего задания
        sscanf(wu.name, "%[^_]_%[^_]_%d_%*d_%d", task.app_name, task.name, &task.timestamp, &task.size);
        // Создание списка результатов задания
        vector<RESULT> results;
        if (strlen(task.name) > 0) {
            sprintf(buf, "INNER JOIN workunit ON result.id = workunit.canonical_resultid WHERE workunit.name like \"%%_%s_%%\" and workunit.assimilate_state=%d", task.name, ASSIMILATE_READY);
            while (!result.enumerate(buf)) {
                    results.push_back(result);
            }
        }

        // Склеивание заданий
        if ((results.size() == task.size) && (task.size != 0)) {
            log_messages.printf(MSG_NORMAL,"[%s] Assimilating task\n", task.name);
            retval = rmerge(task, results);
            if (retval) {
                log_messages.printf(MSG_CRITICAL,"[%s] Assimilation failed\n", task.name);
            } else {
                // Обновление записей в базе
                if (update_db) {
                    sprintf(buf, "assimilate_state=%d, transition_time=%d", ASSIMILATE_DONE, (int)time(0));
                    sprintf(buf2, "appid=%d and assimilate_state=%d and name like \"%%_%s_%%\"", app.id, ASSIMILATE_READY, task.name);
                    wu.update_fields_noid(buf, buf2);
                    boinc_db.commit_transaction();
                }
                log_messages.printf(MSG_NORMAL,"[%s] Task assimilated\n", task.name);

                //Очистка всех структур
                wu.clear();
                memset(&task, 0, sizeof(task));
                results.clear();
            }
        }
        sleep(SLEEP_INTERVAL);
    }
}

int main(int argc, char** argv) {
    int retval;
    DB_APP app;
    int i;
    char buf[256];

    strcpy(app.name, "");
    check_stop_daemons();
    g_argc = argc;
    g_argv = argv;
    for (i=1; i<argc; i++) {
         if (is_arg(argv[i], "sleep_interval")) {
            sleep_interval = atoi(argv[++i]);
        } else if (is_arg(argv[i], "d") || is_arg(argv[i], "debug_level")) {
            int dl = atoi(argv[++i]);
            log_messages.set_debug_level(dl);
            if (dl ==4) g_print_queries = true;
        } else if (is_arg(argv[i], "app")) {
            strcpy(app.name, argv[++i]);
        } else if (is_arg(argv[i], "dont_update_db")) {
            // This option is for testing your assimilator.  When set,
            // it ensures that the assimilator does not actually modify
            // the assimilate_state of the workunits, so you can run
            // your assimilator over and over again without affecting
            // your project.
            update_db = false;
        } else if (is_arg(argv[i], "help") || is_arg(argv[i], "h")) {
//            usage(argv);
        } else {
            log_messages.printf(MSG_CRITICAL, "Unrecognized arg: %s\n", argv[i]);
//            usage(argv);
        }
    }

    if (!strlen(app.name)) {
//        usage(argv);
    }

    retval = config.parse_file();
    if (retval) {
        log_messages.printf(MSG_CRITICAL, "Can't parse config.xml: %s\n", boincerror(retval));
        exit(1);
    }

    log_messages.printf(MSG_NORMAL, "Starting\n");

    retval = boinc_db.open(config.db_name, config.db_host, config.db_user, config.db_passwd);
    if (retval) {
        log_messages.printf(MSG_CRITICAL, "Can't open DB\n");
        exit(1);
    }
    sprintf(buf, "where name='%s'", app.name);
    retval = app.lookup(buf);
    if (retval) {
        log_messages.printf(MSG_CRITICAL, "Can't find app\n");
        exit(1);
    }
    install_stop_signal_handler();

    main_loop(app);
}
