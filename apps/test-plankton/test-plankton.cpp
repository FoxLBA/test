#include "str_util.h"
#include "util.h"
#include "filesys.h"
#include "boinc_api.h"

#include <tomcrypt.h>

#define INPUT_FILENAME "in"
#define OUTPUT_FILENAME "out"
#define INPUT_BACKGROUND "in_back"
#define OUTPUT_BACKGROUND "out_back"
#define CONFIG_FILENAME "config"
#define LOG_FILENAME "log"

int decrypt(char *infile) {
    unsigned char key[16]="qwertyuiopasdfg", IV[16]="1234567890ABCDE", buffer[512];
    symmetric_CTR ctr;
    int err, numbytes;
    int retval;

    FILE* input = fopen(infile, "rb");
    if (input == NULL) {fprintf(stderr, "error on input file\n"); exit (1);}
    FILE* output = fopen("cipher.bin", "wb");
    if (output == NULL) {fprintf(stderr, "error on output file\n"); exit (1);}

    if (register_cipher (&twofish_desc) == -1) {
        fprintf(stderr, "Error registering cipher\n");
        return -1;
    }

    /* start up CTR mode */
    if ((err = ctr_start (find_cipher("twofish"), /* index of desired cipher */
                          IV,                     /* the initial vector */
                          key,                    /* the secret key */
                          16,                     /* length of secret key (16 bytes) */
                          0,                      /* 0 == default # of rounds */
                    CTR_COUNTER_LITTLE_ENDIAN,    /* Little endian counter */
                          &ctr)                   /* where to store the CTR state */
      ) != CRYPT_OK)
    {
      printf("ctr_start error: %s\n", error_to_string (err));
      return -1;
    }

    // Decrypt
    if ((err = ctr_setiv(IV,    /* the initial IV we gave to ctr_start */
                       16,      /* the IV is 16 bytes long */
                       &ctr)    /* the ctr state we wish to modify */
    )!= CRYPT_OK) {
        fprintf(stderr, "ctr_setiv error: %s\n", error_to_string (err));
        return -1;
    }

    while (numbytes = fread(buffer, 1, sizeof(buffer), input)) {
        if ((err = ctr_decrypt (buffer, buffer, sizeof(buffer), &ctr)) != CRYPT_OK) {
            fprintf(stderr, "ctr_decrypt error: %s\n", error_to_string (err));
            return -1;
        }
        fwrite(buffer, 1, numbytes, output);
    }

    /* terminate the stream */
    if ((err = ctr_done (&ctr)) != CRYPT_OK) {
        fprintf(stderr, "ctr_done error: %s\n", error_to_string (err));
        return -1;
    }
    // rename
    retval = boinc_rename("cipher.bin", infile);
    if (retval) {
        fprintf(stderr, "Error [%d] renaming file cipher.bin to %s\n", retval, infile);
    }
    /* clear up and return */
    fclose(input);
    fclose(output);
    zeromem(key, sizeof (key));
    zeromem(&ctr, sizeof (ctr));

    return 0;
}

int main(int argc, char **argv) {
    int retval;
    char buf[65535];
    char input_path[512],   output_path[512];
    char inbg_path[512],    outbg_path[512];
    char config_path[512],  log_path[512];

    retval = boinc_init();
    if (retval) {
        fprintf(stderr, "%s boinc_init returned %d\n", boinc_msg_prefix(buf), retval);
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

    // decrypt
    decrypt(input_path);

    // apply filter
    sprintf(buf, "mencoder -vf rectangle=400:400:250:250:red -o %s -oac copy -ovc lavc %s", output_path, input_path);
    fprintf(stderr, "command: %s\n", buf);
    system(buf);

    //cleanup and exit
    boinc_fraction_done(1);
    boinc_finish(0);
}
