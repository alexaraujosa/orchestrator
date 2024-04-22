/******************************************************************************
 *                             OPERATOR PROCESS                               *
 *                                                                            *
 *   The operator process is responsible for controlling and managing the     *
 * various worker processes used to parallelise the workload of the server.   *
 *   It implements an event system through the usage of anonymous pipes used  *
 * to establish a communication channel between the various worker processes  *
 * and the operator process.                                                  *
 *   Only one operator process is supposed to be used per server instance.    *
 ******************************************************************************/