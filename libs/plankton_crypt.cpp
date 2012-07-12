#include "plankton.h"

int encrypt_file(const char *infile) {
    int retval;
    unsigned char key[16]="qwertyuiopasdfg", IV[16]="1234567890ABCDE", buffer[512];
    symmetric_CTR ctr;
    int err, numbytes;

    // Открыть входной файл
    FILE *input=fopen(infile, "rb");
    log_messages.printf(MSG_NORMAL, "Encrypting file: %s\n", infile);
    if (input == NULL) {log_messages.printf(MSG_CRITICAL, "Error on input file\n"); exit (1);}
    // Открыть шифр
    FILE* cipher = fopen("cipher.bin", "w+b");  //FIXME
    if (cipher == NULL) {log_messages.printf(MSG_CRITICAL, "Error on cipher file\n"); exit (1);}

    /* register twofish first */
    if (register_cipher(&twofish_desc) == -1) {
        log_messages.printf(MSG_CRITICAL, "Error registering cipher\n");
        return -1;
    }

    if ((err = ctr_start(find_cipher("twofish"), /* index of desired cipher */
                      IV,                        /* the initial vector */
                      key,                       /* the secret key */
                      16,                        /* length of secret key (16 bytes) */
                      0,                         /* 0 == default # of rounds */
                CTR_COUNTER_LITTLE_ENDIAN,       /* Little endian counter */
                      &ctr)                      /* where to store the CTR state */
    ) != CRYPT_OK) {
        log_messages.printf(MSG_CRITICAL, "ctr_start error: %s\n", error_to_string (err));
        return -1;
    }
    // Encrypt
    while ((numbytes = fread(buffer, 1, sizeof(buffer), input)) != 0) {
        if ((err = ctr_encrypt(buffer, buffer, sizeof(buffer), &ctr)) != CRYPT_OK) {
            log_messages.printf(MSG_CRITICAL, "ctr_encrypt error: %s\n", error_to_string (err));
            return -1;
        }
        fwrite(buffer, 1, numbytes, cipher);
    }
    /* terminate the stream */
    if ((err = ctr_done(&ctr)) != CRYPT_OK) {
        log_messages.printf(MSG_CRITICAL, "ctr_done error: %s\n", error_to_string(err));
        return -1;
    }
    // rename
    retval = boinc_rename("cipher.bin", infile);
    if (retval) {
        log_messages.printf(MSG_CRITICAL, "Error [%d] renaming file cipher.bin to %s\n", retval, infile);
    }
    /* clear up and return */
    fclose(input);
    fclose(cipher);
    zeromem(key, sizeof (key));
    zeromem(&ctr, sizeof (ctr));
    return 0;
}

int decrypt_file(const char *infile) {
    unsigned char key[16]="qwertyuiopasdfg", IV[16]="1234567890ABCDE", buffer[512];
    symmetric_CTR ctr;
    int err, numbytes;
    int retval;

    FILE* input = fopen(infile, "rb");
    log_messages.printf(MSG_NORMAL, "Decrypting file: %s\n", infile);
    if (input == NULL) {log_messages.printf(MSG_CRITICAL, "Error on input file\n"); exit (1);}
    FILE* output = fopen("cipher.bin", "wb");
    if (output == NULL) {log_messages.printf(MSG_CRITICAL, "Error on output file\n"); exit (1);}

    if (register_cipher(&twofish_desc) == -1) {
        log_messages.printf(MSG_CRITICAL, "Error registering cipher\n");
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
      log_messages.printf(MSG_CRITICAL, "ctr_start error: %s\n", error_to_string(err));
      return -1;
    }

    // Decrypt
    if ((err = ctr_setiv(IV,    /* the initial IV we gave to ctr_start */
                       16,      /* the IV is 16 bytes long */
                       &ctr)    /* the ctr state we wish to modify */
    )!= CRYPT_OK) {
        log_messages.printf(MSG_CRITICAL, "ctr_setiv error: %s\n", error_to_string(err));
        return -1;
    }

    while ((numbytes = fread(buffer, 1, sizeof(buffer), input)) != 0) {
        if ((err = ctr_decrypt(buffer, buffer, sizeof(buffer), &ctr)) != CRYPT_OK) {
            log_messages.printf(MSG_CRITICAL, "ctr_decrypt error: %s\n", error_to_string(err));
            return -1;
        }
        fwrite(buffer, 1, numbytes, output);
    }

    /* terminate the stream */
    if ((err = ctr_done(&ctr)) != CRYPT_OK) {
        log_messages.printf(MSG_CRITICAL, "ctr_done error: %s\n", error_to_string(err));
        return -1;
    }
    // rename
    retval = boinc_rename("cipher.bin", infile);
    if (retval) {
        log_messages.printf(MSG_CRITICAL, "Error [%d] renaming file cipher.bin to %s\n", retval, infile);
    }
    /* clear up and return */
    fclose(input);
    fclose(output);
    zeromem(key, sizeof (key));
    zeromem(&ctr, sizeof (ctr));

    return 0;
}
