
#ifndef _step42_motor_lib_
#define _step42_motor_lib_

#define TRUE 1
#define FALSE 0

/**
 * Enumerate supported step motor connection optoins.
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


/**
 * Step motor, connected with 4 pins with L293D driver.
 */
void step_motor_4pin (motor_info* minfo, motor_conn_4pin* mconn, unsigned int dl, unsigned int dt);

/**
 * Step motor, connected with 3 pins.
 */
void step_motor_stb57 (motor_info* minfo, motor_conn_stb57* mconn, unsigned int dl, unsigned int dt);

/**
 * Step motor.
 */
void step_motor(void* arg);

/** 
 * Move dimension defined by motor to destination 
 * for the provided period of time.
 * dl (delta length) - shift to destination point, micrometre.
 * dt (delta time) - time for movement, microseconds.
 */
task_t* move_dim(motor_info* minfo, unsigned int dl, unsigned int dt, step_data* sdata, array_t* stack, int stacksz);

/**
 * Move head from its current position to destination point 
 * for the provided period of time.
 * dx, dy, dz - shift to destination point, micrometre.
 * dt - time for moving, microseconds (autoset to min possible value for max speed if dt=0).
 */
void move_head(unsigned int dx, unsigned int dy, unsigned int dz, unsigned int dt);

#endif

