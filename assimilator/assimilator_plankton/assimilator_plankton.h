#include <stdio.h>
#include <cstring>
#include <vector>
#include "boinc_db.h"
#include <cstdlib>

struct task_t {
    char app_name[255];
    unsigned int id;
    char login[255];
    char name[255];
    char extension[255];
    unsigned int timestamp;
    unsigned int size;
};

extern int handle_result(task_t task, vector<RESULT> results);
