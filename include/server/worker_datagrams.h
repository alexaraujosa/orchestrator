#ifndef SERVER_WORKER_DATAGRAMS_H
#define SERVER_WORKER_DATAGRAMS_H

#include <stdint.h>
#include "common/datagram/execute.h"

enum WorkerDatagramMode {
    WORKER_DATAGRAM_MODE_NONE,
    WORKER_DATAGRAM_MODE_STATUS,
    WORKER_DATAGRAM_MODE_EXECUTE,
    WORKER_DATAGRAM_MODE_SHUTDOWN,
};

typedef struct worker_datagram_header {
    uint8_t mode; // The mode for this datagram. See WorkerDatagramMode.
    uint8_t type; // The type for this datagram. See DatagramType.
    int task_id;  // The id of the current task.
} WORKER_DATAGRAM_HEADER, *WorkerDatagramHeader;

/**
 * @brief Generic datagram pointer type, compatible with every datagram.
 */
typedef struct worker_datagram {
    WORKER_DATAGRAM_HEADER header; // Header for this datagram. Mode must be set to WORKER_DATAGRAM_MODE_STATUS.
} *WorkerDatagram;

typedef struct worker_status_datagram {
    WORKER_DATAGRAM_HEADER header; // Header for this datagram. Mode must be set to WORKER_DATAGRAM_MODE_STATUS.
} WORKER_STATUS_DATAGRAM, *WorkerStatusDatagram;

typedef struct worker_execute_datagram {
    WORKER_DATAGRAM_HEADER header; // Header for this datagram. Mode must be set to WORKER_DATAGRAM_MODE_EXECUTE.
    char data[EXECUTE_REQUEST_DATAGRAM_PAYLOAD_LEN + 1]; // The task to be executed.
} WORKER_EXECUTE_DATAGRAM, *WorkerExecuteDatagram;

typedef struct worker_shutdown_datagram {
    WORKER_DATAGRAM_HEADER header; // Header for this datagram. Mode must be set to WORKER_DATAGRAM_MODE_SHUTDOWN.
} WORKER_SHUTDOWN_DATAGRAM, *WorkerShutdownDatagram;

/**
 * @brief Creates a new empty Worker Datagram Header.
 */
WORKER_DATAGRAM_HEADER create_worker_datagram_header();

/**
 * @brief Reads a Worker Datagram Header from a file descriptor, or NULL if it fails.
 * @param fd The file descriptor to read.
 */
WORKER_DATAGRAM_HEADER read_worker_datagram_header(int fd);

/**
 * @brief Creates a new empty Worker Status Datagram.
 */
WorkerStatusDatagram create_worker_status_datagram();

/**
 * @brief Reads a Worker Status Datagram from a file descriptor, or NULL if it fails. This version should be called after
 * reading the header of a datagram, for distinguishing the datagram mode.
 * 
 * @param fd     The file descriptor to read.
 * @param header The already read header.
 */
WorkerStatusDatagram read_partial_worker_status_datagram(int fd, WORKER_DATAGRAM_HEADER header);

WorkerExecuteDatagram create_worker_execute_datagram();

/**
 * @brief Reads a Worker Execute Datagram from a file descriptor, or NULL if it fails. This version should be called after
 * reading the header of a datagram, for distinguishing the datagram mode.
 * 
 * @param fd     The file descriptor to read.
 * @param header The already read header.
 */
WorkerExecuteDatagram read_partial_worker_execute_datagram(int fd, WORKER_DATAGRAM_HEADER header);

WorkerShutdownDatagram create_worker_shutdown_datagram();

/**
 * @brief Reads a Worker Shutdown Datagram from a file descriptor, or NULL if it fails. This version should be called after
 * reading the header of a datagram, for distinguishing the datagram type.
 * 
 * @param fd     The file descriptor to read.
 * @param header The already read header.
 */
WorkerShutdownDatagram read_partial_worker_shutdown_datagram(int fd, WORKER_DATAGRAM_HEADER header);


#endif