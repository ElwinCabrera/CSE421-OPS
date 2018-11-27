#ifndef __LIB_ERROR_NR_H
#define __LIB_ERROR_NR_H

/* System call numbers. */
enum {
    NO_EXIT = -16,
    UNKNOWN_FAILURE,
    PTR_FAILURE,
    HALT_FAILURE,                   /* Failed Halt the operating system. */
    EXEC_FAILURE,                   /* Start another process. */
    WAIT_FAILURE,                   /* Wait for a child process to die. */
    CREATE_FAILURE,                 /* Create a file. */
    REMOVE_FAILURE,                 /* Delete a file. */
    OPEN_FAILURE,                   /* Open a file. */
    FILESIZE_FAILURE,               /* Obtain a file's size. */
    READ_FAILURE,                   /* Read from a file. */
    WRITE_FAILURE,                  /* Write to a file. */
    SEEK_FAILURE,                   /* Change position in a file. */
    TELL_FAILURE,                   /* Report current position in a file. */
    CLOSE_FAILURE,                  /* Close a file. */
    EXIT_FAILURE,
    EXIT_SUCCESS,

  };

#endif /* lib/error-nr.h */