
#ifndef _step42_motor_lib_
#define _step42_motor_lib_

/**
 * Enumerate supported step motor connection optoins.
 */
/*enum motor_conn_type {
    CONNECTION_4PIN
};
 */

const int CONNECTION_4PIN=1;
const int CONNECTION_STB57=2;

typedef struct motor_conn_4pin {
    int MOTOR_PIN1;
    int MOTOR_PIN2;
    int MOTOR_PIN3;
    int MOTOR_PIN4;

    /* Delay between 2 steps, microseconds */
    int step_delay;

    /* true (1): motor in microstep mode, 
     * false (0): motor in non-microstep mode */
    int microstep;
    
} motor_conn_4pin;

typedef struct motor_conn_stb57 {
    int MOTOR_DIR;
    int MOTOR_PULSE;
    int MOTOR_EN;
    
    /* Delay between 2 steps, microseconds */
    int step_delay;
    
} motor_conn_stb57;

typedef struct motor_info {
    /* Motor name */
    char* name;
    
    /* Movement distance per movement cycle, micrometre */
    int distance_per_cycle;
    
    /* Time spent for movement cycle, microseconds */
    int time_per_cycle;

    int conn_type;
    void* conn_info;
    
} motor_info;

typedef struct step_data {
    motor_info* minfo;
    int dl;
    int dt;
} step_data;

#endif

