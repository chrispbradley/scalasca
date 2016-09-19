/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2016                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2009-2014                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#ifndef SCOUT_PATTERNS_GEN_H
#define SCOUT_PATTERNS_GEN_H


namespace scout
{

//--- Constants -------------------------------------------------------------

const long PAT_NONE                           =  -1;
const long PAT_TIME                           =   0;
const long PAT_VISITS                         =   1;
const long PAT_EXECUTION                      =   2;
const long PAT_OVERHEAD                       =   3;
const long PAT_COMP                           =   4;
const long PAT_ASYNC                          =   5;
const long PAT_MPI                            =   6;
const long PAT_MPI_MANAGEMENT                 =   7;
const long PAT_MPI_INIT_EXIT                  =   8;
const long PAT_MPI_INIT_COMPLETION            =   9;
const long PAT_MPI_FINALIZE_WAIT              =  10;
const long PAT_MPI_MGMT_COMM                  =  11;
const long PAT_MPI_MGMT_FILE                  =  12;
const long PAT_MPI_MGMT_WIN                   =  13;
const long PAT_MPI_SYNCHRONIZATION            =  14;
const long PAT_MPI_SYNC_COLLECTIVE            =  15;
const long PAT_MPI_BARRIER_WAIT               =  16;
const long PAT_MPI_BARRIER_COMPLETION         =  17;
const long PAT_MPI_COMMUNICATION              =  18;
const long PAT_MPI_POINT2POINT                =  19;
const long PAT_MPI_LATESENDER                 =  20;
const long PAT_MPI_LATESENDER_WO              =  21;
const long PAT_MPI_LSWO_DIFFERENT             =  22;
const long PAT_MPI_LSWO_SAME                  =  23;
const long PAT_MPI_LATERECEIVER               =  24;
const long PAT_MPI_COLLECTIVE                 =  25;
const long PAT_MPI_EARLYREDUCE                =  26;
const long PAT_MPI_EARLYSCAN                  =  27;
const long PAT_MPI_LATEBROADCAST              =  28;
const long PAT_MPI_WAIT_NXN                   =  29;
const long PAT_MPI_NXN_COMPLETION             =  30;
const long PAT_MPI_IO                         =  31;
const long PAT_MPI_IO_INDIVIDUAL              =  32;
const long PAT_MPI_IO_COLLECTIVE              =  33;
const long PAT_OMP_IDLE_THREADS               =  34;
const long PAT_OMP_LIMITED_PARALLELISM        =  35;
const long PAT_OMP_TIME                       =  36;
const long PAT_OMP_MANAGEMENT                 =  37;
const long PAT_OMP_FORK                       =  38;
const long PAT_OMP_SYNCHRONIZATION            =  39;
const long PAT_OMP_BARRIER                    =  40;
const long PAT_OMP_EBARRIER                   =  41;
const long PAT_OMP_EBARRIER_WAIT              =  42;
const long PAT_OMP_IBARRIER                   =  43;
const long PAT_OMP_IBARRIER_WAIT              =  44;
const long PAT_OMP_CRITICAL                   =  45;
const long PAT_OMP_LOCK_CONTENTION_CRITICAL   =  46;
const long PAT_OMP_LOCK_API                   =  47;
const long PAT_OMP_LOCK_CONTENTION_API        =  48;
const long PAT_OMP_ORDERED                    =  49;
const long PAT_OMP_TASKWAIT                   =  50;
const long PAT_OMP_FLUSH                      =  51;
const long PAT_PTHREAD_TIME                   =  52;
const long PAT_PTHREAD_MANAGEMENT             =  53;
const long PAT_PTHREAD_SYNCHRONIZATION        =  54;
const long PAT_PTHREAD_LOCK_API               =  55;
const long PAT_PTHREAD_LOCK_CONTENTION_MUTEX_LOCK =  56;
const long PAT_PTHREAD_CONDITIONAL            =  57;
const long PAT_PTHREAD_LOCK_CONTENTION_CONDITIONAL =  58;
const long PAT_THREAD_LOCK_CONTENTION         =  59;
const long PAT_SYNCS                          =  60;
const long PAT_SYNCS_P2P                      =  61;
const long PAT_SYNCS_SEND                     =  62;
const long PAT_SYNCS_RECV                     =  63;
const long PAT_SYNCS_COLL                     =  64;
const long PAT_SYNCS_RMA                      =  65;
const long PAT_SYNCS_RMA_ACTIVE               =  66;
const long PAT_SYNCS_RMA_PASSIVE              =  67;
const long PAT_COMMS                          =  68;
const long PAT_COMMS_P2P                      =  69;
const long PAT_COMMS_SEND                     =  70;
const long PAT_COMMS_RECV                     =  71;
const long PAT_COMMS_COLL                     =  72;
const long PAT_COMMS_CXCH                     =  73;
const long PAT_COMMS_CSRC                     =  74;
const long PAT_COMMS_CDST                     =  75;
const long PAT_COMMS_RMA                      =  76;
const long PAT_COMMS_RMA_PUTS                 =  77;
const long PAT_COMMS_RMA_GETS                 =  78;
const long PAT_COMMS_RMA_ATOMICS              =  79;
const long PAT_BYTES                          =  80;
const long PAT_BYTES_P2P                      =  81;
const long PAT_BYTES_SENT                     =  82;
const long PAT_BYTES_RCVD                     =  83;
const long PAT_BYTES_COLL                     =  84;
const long PAT_BYTES_COUT                     =  85;
const long PAT_BYTES_CIN                      =  86;
const long PAT_BYTES_RMA                      =  87;
const long PAT_BYTES_PUT                      =  88;
const long PAT_BYTES_GET                      =  89;
const long PAT_MPI_SLR_COUNT                  =  90;
const long PAT_MPI_SLS_COUNT                  =  91;
const long PAT_MPI_SLSWO_COUNT                =  92;
const long PAT_MPI_CLR_COUNT                  =  93;
const long PAT_MPI_CLS_COUNT                  =  94;
const long PAT_MPI_CLSWO_COUNT                =  95;
const long PAT_MPI_FILE_OPS                   =  96;
const long PAT_MPI_FILE_IOPS                  =  97;
const long PAT_MPI_FILE_IROPS                 =  98;
const long PAT_MPI_FILE_IWOPS                 =  99;
const long PAT_MPI_FILE_COPS                  = 100;
const long PAT_MPI_FILE_CROPS                 = 101;
const long PAT_MPI_FILE_CWOPS                 = 102;
const long PAT_MPI_RMA_WAIT_AT_CREATE         = 103;
const long PAT_MPI_RMA_WAIT_AT_FREE           = 104;
const long PAT_MPI_RMA_SYNCHRONIZATION        = 105;
const long PAT_MPI_RMA_SYNC_ACTIVE            = 106;
const long PAT_MPI_RMA_SYNC_LATE_POST         = 107;
const long PAT_MPI_RMA_EARLY_WAIT             = 108;
const long PAT_MPI_RMA_LATE_COMPLETE          = 109;
const long PAT_MPI_RMA_WAIT_AT_FENCE          = 110;
const long PAT_MPI_RMA_EARLY_FENCE            = 111;
const long PAT_MPI_RMA_SYNC_PASSIVE           = 112;
const long PAT_MPI_RMA_SYNC_LOCK_COMPETITION  = 113;
const long PAT_MPI_RMA_SYNC_WAIT_FOR_PROGRESS = 114;
const long PAT_MPI_RMA_COMMUNICATION          = 115;
const long PAT_MPI_RMA_COMM_LATE_POST         = 116;
const long PAT_MPI_RMA_COMM_LOCK_COMPETITION  = 117;
const long PAT_MPI_RMA_COMM_WAIT_FOR_PROGRESS = 118;
const long PAT_MPI_RMA_PAIRSYNC_COUNT         = 119;
const long PAT_MPI_RMA_PAIRSYNC_UNNEEDED_COUNT = 120;
const long PAT_IMBALANCE                      = 121;
const long PAT_IMBALANCE_ABOVE                = 122;
const long PAT_IMBALANCE_ABOVE_SINGLE         = 123;
const long PAT_IMBALANCE_BELOW                = 124;
const long PAT_IMBALANCE_BELOW_BYPASS         = 125;
const long PAT_IMBALANCE_BELOW_SINGULARITY    = 126;
const long PAT_STATISTICS                     = 127;
const long PAT_CRITICAL_PATH                  = 128;
const long PAT_CRITICAL_PATH_IMBALANCE        = 129;
const long PAT_PERFORMANCE_IMPACT             = 130;
const long PAT_PERFORMANCE_IMPACT_CRITICALPATH = 131;
const long PAT_CRITICAL_PATH_ACTIVITIES       = 132;
const long PAT_CRITICAL_IMBALANCE_IMPACT      = 133;
const long PAT_INTRA_PARTITION_IMBALANCE      = 134;
const long PAT_INTER_PARTITION_IMBALANCE      = 135;
const long PAT_NON_CRITICAL_PATH_ACTIVITIES   = 136;
const long PAT_DELAY                          = 137;
const long PAT_DELAY_MPI                      = 138;
const long PAT_DELAY_P2P                      = 139;
const long PAT_DELAY_LATESENDER_AGGREGATE     = 140;
const long PAT_DELAY_LATESENDER               = 141;
const long PAT_DELAY_LATESENDER_LONGTERM      = 142;
const long PAT_DELAY_LATERECEIVER_AGGREGATE   = 143;
const long PAT_DELAY_LATERECEIVER             = 144;
const long PAT_DELAY_LATERECEIVER_LONGTERM    = 145;
const long PAT_DELAY_COLLECTIVE               = 146;
const long PAT_DELAY_BARRIER_AGGREGATE        = 147;
const long PAT_DELAY_BARRIER                  = 148;
const long PAT_DELAY_BARRIER_LONGTERM         = 149;
const long PAT_DELAY_N2N_AGGREGATE            = 150;
const long PAT_DELAY_N2N                      = 151;
const long PAT_DELAY_N2N_LONGTERM             = 152;
const long PAT_DELAY_12N_AGGREGATE            = 153;
const long PAT_DELAY_12N                      = 154;
const long PAT_DELAY_12N_LONGTERM             = 155;
const long PAT_DELAY_OMP                      = 156;
const long PAT_DELAY_OMPBARRIER_AGGREGATE     = 157;
const long PAT_DELAY_OMPBARRIER               = 158;
const long PAT_DELAY_OMPBARRIER_LONGTERM      = 159;
const long PAT_DELAY_OMPIDLE_AGGREGATE        = 160;
const long PAT_DELAY_OMPIDLE                  = 161;
const long PAT_DELAY_OMPIDLE_LONGTERM         = 162;
const long PAT_WAITSTATES_DIRECT_VS_INDIRECT  = 163;
const long PAT_MPI_WAIT_DIRECT                = 164;
const long PAT_MPI_WAIT_DIRECT_LATESENDER     = 165;
const long PAT_MPI_WAIT_DIRECT_LATERECEIVER   = 166;
const long PAT_MPI_WAIT_INDIRECT              = 167;
const long PAT_MPI_WAIT_INDIRECT_LATESENDER   = 168;
const long PAT_MPI_WAIT_INDIRECT_LATERECEIVER = 169;
const long PAT_WAITSTATES_PROPAGATING_VS_TERMINAL = 170;
const long PAT_MPI_WAIT_PROPAGATING           = 171;
const long PAT_MPI_WAIT_PROPAGATING_LS        = 172;
const long PAT_MPI_WAIT_PROPAGATING_LR        = 173;
const long PAT_MPI_WAIT_TERMINAL              = 174;
const long PAT_MPI_WAIT_TERMINAL_LS           = 175;
const long PAT_MPI_WAIT_TERMINAL_LR           = 176;


//--- Forward declarations --------------------------------------------------

class AnalyzeTask;


//--- Function prototypes ---------------------------------------------------

void create_patterns(AnalyzeTask* analyzer);


}   /* namespace scout */


#endif   /* !SCOUT_PATTERNS_GEN_H */
