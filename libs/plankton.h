#include <tomcrypt.h>

#include "error_numbers.h"
#include "parse.h"
#include "util.h"
#include "filesys.h"

#include "sched_msgs.h"
#include "str_util.h"

const char* source_path="/mnt/boinc/source";
const char* result_path="/mnt/boinc/results";

extern int encrypt_file(const char *infile);
extern int decrypt_file(const char *infile);
