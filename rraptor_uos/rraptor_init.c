#include <runtime/lib.h>
#include <kernel/uos.h>
#include <uart/uart.h>
#include "step42_motor_lib.h"
#include <timer/timer.h>


ARRAY (stackMain, 1000);
ARRAY (stackX, 1000);
ARRAY (stackY,1000);
ARRAY (stackZ, 1000);
ARRAY (stackEXT1,1000);

task_t *taskMain;
task_t *taskX;
task_t *taskY;
task_t *taskZ;
task_t *taskExt1;


timer_t timer;

motor_conn_4pin mconn_4pin_x = {1<<0,1<<1,1<<2,1<<3,0};
motor_conn_4pin mconn_4pin_y = {1<<10,1<<5,1<<6,1<<7,0};
motor_conn_4pin mconn_4pin_z = {1<<10,1<<5,1<<6,1<<7,0};
motor_conn_4pin mconn_4pin_ext1 = {1<<10,1<<5,1<<6,1<<7,0};

motor_conn_3pin mconn_3pin_x = {1<<1, 1<<2, 0, 1};
motor_conn_3pin mconn_3pin_y = {1<<3, 1<<4, 0, 1};
motor_conn_3pin mconn_3pin_z = {1<<10, 1<<5, 1<<6, 1};
motor_conn_3pin mconn_3pin_ext1 = {1<<10, 1<<5, 1<<6, 1};

motor_info motor_info_x;// = {"X", 15, 2, CONNECTION_3PIN, &mconn_3pin_z};
motor_info motor_info_y;// = {"Y", 15, 2, CONNECTION_3PIN, &mconn_3pin_z};
motor_info motor_info_z;// = {"Z", 15, 2, CONNECTION_3PIN, &mconn_3pin_z};
motor_info motor_info_ext1;// = {"Ext1", 15, 2, CONNECTION_3PIN, &mconn_3pin_ext1};

/**
 * Program main cycle.
 */
void rraptor_main(void);

/**
 * Step motor, connected with 4 pins with L293D driver.
 */
void step_motor_4pin (motor_info* minfo, motor_conn_4pin* mconn, int dl, int dt);

/**
 * Step motor, connected with 3 pins.
 */
void step_motor_3pin (motor_info* minfo, motor_conn_3pin* mconn, int dl, int dt);

/**
 * Step motor.
 */
void step_motor(void* arg);

/** 
 * Move dimension defined by motor to destination 
 * for the provided period of time.
 * dl (delta length) - shift to destination point, micrometre.
 * dt (delta time) - time for movement, millis,
 */
void move_dim(motor_info* minfo, int dl, int dt, array_t* stack, int stacksz);

/**
 * Move head from its current position to destination point 
 * for the provided period of time.
 * dx, dy, dz - shift to destination point, micrometre.
 * dt - time for moving, millis.
 */
void move_head(int dx, int dy, int dz, int dt);


void init_motors_3pin() {
    motor_info_x.name = "X";
    motor_info_x.distance_per_cycle = 15;
    motor_info_x.time_per_cycle = 2;
    motor_info_x.conn_type = CONNECTION_3PIN;
    motor_info_x.conn_info = &mconn_3pin_x;
    
    motor_info_y.name = "Y";
    motor_info_y.distance_per_cycle = 15;
    motor_info_y.time_per_cycle = 2;
    motor_info_y.conn_type = CONNECTION_3PIN;
    motor_info_y.conn_info = &mconn_3pin_y;
    
    motor_info_z.name = "Z";
    motor_info_z.distance_per_cycle = 15;
    motor_info_z.time_per_cycle = 2;
    motor_info_z.conn_type = CONNECTION_3PIN;
    motor_info_z.conn_info = &mconn_3pin_z;
    
    motor_info_ext1.name = "Ext1";
    motor_info_ext1.distance_per_cycle = 15;
    motor_info_ext1.time_per_cycle = 2;
    motor_info_ext1.conn_type = CONNECTION_3PIN;
    motor_info_ext1.conn_info = &mconn_3pin_ext1;
    
    TRISDSET=mconn_3pin_x.MOTOR_DIR;
    TRISDCLR=mconn_3pin_x.MOTOR_PULSE;
    
    TRISDSET=mconn_3pin_y.MOTOR_DIR;
    TRISDCLR=mconn_3pin_y.MOTOR_PULSE;
    
}

void init_motors_4pin() {
    TRISDCLR=mconn_4pin_x.MOTOR_PIN1;
    TRISDCLR=mconn_4pin_x.MOTOR_PIN2;

    TRISDCLR=mconn_4pin_x.MOTOR_PIN3;
    TRISDCLR=mconn_4pin_x.MOTOR_PIN4;
    

    TRISDCLR=mconn_4pin_y.MOTOR_PIN1;
    TRISDCLR=mconn_4pin_y.MOTOR_PIN2;

    TRISDCLR=mconn_4pin_y.MOTOR_PIN3;
    TRISDCLR=mconn_4pin_y.MOTOR_PIN4;
}

void test1(motor_conn_3pin* mconn) {
    while (1) {
        LATDSET=mconn->MOTOR_PULSE;
        mdelay(mconn->step_delay);
        LATDCLR=mconn->MOTOR_PULSE;
        mdelay(mconn->step_delay);
        LATDSET=mconn->MOTOR_PULSE;
        mdelay(mconn->step_delay);
        LATDCLR=mconn->MOTOR_PULSE;
        mdelay(mconn->step_delay);
        
        //timer_delay(&timer, cdelay - mconn->step_delay * 4);
    }
}


void uos_init (void) {
    init_motors_3pin();

    // uos
    timer_init (&timer, KHZ, 1);

    //taskMain = task_create(rraptor_main,0, "Main", 2, stackMain, sizeof(stackMain));
    //rraptor_main();
    test1(&mconn_3pin_y);
}

void rraptor_main(void) {
    move_head(50000, 50000, 100000, 10000);
    move_head(0, 50000, 0, 10000);
    move_head(50000, 0, 0, 10000);
    move_head(50000, 100000, 0, 10000);
}




/*void handle_command_stream(char* file_path) {*/
/*	f=fopen(file_path, "r");*/

/*	int i;*/
/*	for(i=0; i<5; i++)*/
/*		if(file_processing(f))*/
/*			return 0;*/
/*}*/

void step_motor(void* arg) {
    step_data* sdata = (step_data*) arg;
    if(sdata->minfo->conn_type == CONNECTION_4PIN) {
        step_motor_4pin(sdata->minfo, (motor_conn_4pin*)sdata->minfo->conn_info, sdata->dl, sdata->dt);

        task_exit(sdata->minfo->name);
    } else if(sdata->minfo->conn_type == CONNECTION_3PIN) {
        step_motor_3pin(sdata->minfo, (motor_conn_3pin*)sdata->minfo->conn_info, sdata->dl, sdata->dt);
        
        task_exit(sdata->minfo->name);
    }
}

void move_dim(motor_info* minfo, int dl, int dt, array_t* stack, int stacksz) {
    step_data sdata;
    sdata.minfo = minfo;
    sdata.dl = dl;
    sdata.dt = dt;
    
	task_create(step_motor, &sdata, minfo->name, 1, stack, stacksz);
}

void move_head(int dx, int dy, int dz, int dt) {
	//printf("move(%d, %d, %d, %d)\n", dx, dy, dz, dt);

	move_dim(&motor_info_x, dx, dt, stackX, sizeof(stackX));
	move_dim(&motor_info_y, dy, dt, stackY, sizeof(stackY));
	move_dim(&motor_info_z, dz, dt, stackZ, sizeof(stackZ));
}

void step_motor_3pin (motor_info* minfo, motor_conn_3pin* mconn, int dl, int dt) {
    if(dl == 0)
        return;
    // calculate number of steps
    int step = dl / minfo->distance_per_cycle;
    
    // calculate cycle delay
    int cdelay = dt / minfo->time_per_cycle;
    
    // set direction
    if(dl > 0) {
        LATDSET = mconn->MOTOR_DIR;
    } else {
        LATDCLR = mconn->MOTOR_DIR;
    }
    
    int i=0;
    while (i < step) {
        timer_delay(&timer, cdelay - mconn->step_delay * 4);
    
        LATDSET=mconn->MOTOR_PULSE;
        mdelay(mconn->step_delay);
        LATDCLR=mconn->MOTOR_PULSE;
        mdelay(mconn->step_delay);
        LATDSET=mconn->MOTOR_PULSE;
        mdelay(mconn->step_delay);
        LATDCLR=mconn->MOTOR_PULSE;
        mdelay(mconn->step_delay);
    }
}

void step_motor_4pin (motor_info* minfo, motor_conn_4pin* mconn, int dl, int dt) {
    // calculate number of steps
    int step = dl / minfo->distance_per_cycle;

    // calculate cycle delay
    int cdelay = dt / minfo->time_per_cycle;

    int i=0;
    
    if (mconn->microstep == 0) {
        if (dl > 0) {
            while (i < step) {
                // 4 steps in full cycle
                timer_delay(&timer, cdelay - mconn->step_delay * 4);
    
                LATDSET=mconn->MOTOR_PIN1;
                LATDCLR=mconn->MOTOR_PIN2;
                LATDCLR=mconn->MOTOR_PIN3;
                LATDCLR=mconn->MOTOR_PIN4;
                mdelay(mconn->step_delay);
                
                LATDCLR=mconn->MOTOR_PIN1;
                LATDSET=mconn->MOTOR_PIN2;
                LATDCLR=mconn->MOTOR_PIN3;
                LATDCLR=mconn->MOTOR_PIN4;
                mdelay(mconn->step_delay);
                
                LATDCLR=mconn->MOTOR_PIN1;
                LATDCLR=mconn->MOTOR_PIN2;
                LATDSET=mconn->MOTOR_PIN3;
                LATDCLR=mconn->MOTOR_PIN4;
                mdelay(mconn->step_delay);
                
                LATDCLR=mconn->MOTOR_PIN1;
                LATDCLR=mconn->MOTOR_PIN2;
                LATDCLR=mconn->MOTOR_PIN3;
                LATDSET=mconn->MOTOR_PIN4;
                mdelay(mconn->step_delay);
                
                i++;
            }
            
            LATDCLR=mconn->MOTOR_PIN1;
            LATDCLR=mconn->MOTOR_PIN2;
            LATDCLR=mconn->MOTOR_PIN3;
            LATDCLR=mconn->MOTOR_PIN4;
            
        } else if (dl < 0) {
            while (i < step) {
                // 4 steps in full cycle
                timer_delay(&timer, cdelay - mconn->step_delay * 4);
    
                LATDSET=mconn->MOTOR_PIN4;
                LATDCLR=mconn->MOTOR_PIN3;
                LATDCLR=mconn->MOTOR_PIN2;
                LATDCLR=mconn->MOTOR_PIN1;
                mdelay(mconn->step_delay);
                
                LATDCLR=mconn->MOTOR_PIN4;
                LATDSET=mconn->MOTOR_PIN3;
                LATDCLR=mconn->MOTOR_PIN2;
                LATDCLR=mconn->MOTOR_PIN1;
                mdelay(mconn->step_delay);
                
                LATDCLR=mconn->MOTOR_PIN4;
                LATDCLR=mconn->MOTOR_PIN3;
                LATDSET=mconn->MOTOR_PIN2;
                LATDCLR=mconn->MOTOR_PIN1;
                mdelay(mconn->step_delay);
                
                LATDCLR=mconn->MOTOR_PIN4;
                LATDCLR=mconn->MOTOR_PIN3;
                LATDCLR=mconn->MOTOR_PIN2;
                LATDSET=mconn->MOTOR_PIN1;
                mdelay(mconn->step_delay);
                
                i++;
            }
            
            LATDCLR=mconn->MOTOR_PIN4;
            LATDCLR=mconn->MOTOR_PIN3;
            LATDCLR=mconn->MOTOR_PIN2;
            LATDCLR=mconn->MOTOR_PIN1;
        }
        
    } else if (mconn->microstep == 1) {
        if (dl > 0) {
            while (i < step) {
                // 8 steps in full cycle
                timer_delay(&timer, cdelay - mconn->step_delay * 8);
                
                LATDSET=mconn->MOTOR_PIN1;
                LATDCLR=mconn->MOTOR_PIN2;
                LATDCLR=mconn->MOTOR_PIN3;
                LATDCLR=mconn->MOTOR_PIN4;
                mdelay(mconn->step_delay);
                
                LATDSET=mconn->MOTOR_PIN1;
                LATDSET=mconn->MOTOR_PIN2;
                LATDCLR=mconn->MOTOR_PIN3;
                LATDCLR=mconn->MOTOR_PIN4;
                mdelay(mconn->step_delay);
                
                LATDCLR=mconn->MOTOR_PIN1;
                LATDSET=mconn->MOTOR_PIN2;
                LATDCLR=mconn->MOTOR_PIN3;
                LATDCLR=mconn->MOTOR_PIN4;
                mdelay(mconn->step_delay);
                
                LATDCLR=mconn->MOTOR_PIN1;
                LATDSET=mconn->MOTOR_PIN2;
                LATDSET=mconn->MOTOR_PIN3;
                LATDCLR=mconn->MOTOR_PIN4;
                mdelay(mconn->step_delay);
                
                LATDCLR=mconn->MOTOR_PIN1;
                LATDCLR=mconn->MOTOR_PIN2;
                LATDSET=mconn->MOTOR_PIN3;
                LATDCLR=mconn->MOTOR_PIN4;
                mdelay(mconn->step_delay);
                
                LATDCLR=mconn->MOTOR_PIN1;
                LATDCLR=mconn->MOTOR_PIN2;
                LATDSET=mconn->MOTOR_PIN3;
                LATDSET=mconn->MOTOR_PIN4;
                mdelay(mconn->step_delay);
                
                LATDCLR=mconn->MOTOR_PIN1;
                LATDCLR=mconn->MOTOR_PIN2;
                LATDCLR=mconn->MOTOR_PIN3;
                LATDSET=mconn->MOTOR_PIN4;
                mdelay(mconn->step_delay);
                
                LATDSET=mconn->MOTOR_PIN1;
                LATDCLR=mconn->MOTOR_PIN2;
                LATDCLR=mconn->MOTOR_PIN3;
                LATDSET=mconn->MOTOR_PIN4;
                mdelay(mconn->step_delay);
                
                i++;
            }
            
            LATDCLR=mconn->MOTOR_PIN4;
            LATDCLR=mconn->MOTOR_PIN3;
            LATDCLR=mconn->MOTOR_PIN2;
            LATDCLR=mconn->MOTOR_PIN1;
            
        } else if (dl < 0) {
            while(i<step) {
                // 8 steps in full cycle
                timer_delay(&timer, cdelay - mconn->step_delay * 8);
                
                LATDSET=mconn->MOTOR_PIN4;
                LATDCLR=mconn->MOTOR_PIN3;
                LATDCLR=mconn->MOTOR_PIN2;
                LATDCLR=mconn->MOTOR_PIN1;
                mdelay(mconn->step_delay);
                
                LATDSET=mconn->MOTOR_PIN4;
                LATDSET=mconn->MOTOR_PIN3;
                LATDCLR=mconn->MOTOR_PIN2;
                LATDCLR=mconn->MOTOR_PIN1;
                mdelay(mconn->step_delay);
                
                LATDCLR=mconn->MOTOR_PIN4;
                LATDSET=mconn->MOTOR_PIN3;
                LATDCLR=mconn->MOTOR_PIN2;
                LATDCLR=mconn->MOTOR_PIN1;
                mdelay(mconn->step_delay);
                
                LATDCLR=mconn->MOTOR_PIN4;
                LATDSET=mconn->MOTOR_PIN3;
                LATDSET=mconn->MOTOR_PIN2;
                LATDCLR=mconn->MOTOR_PIN1;
                mdelay(mconn->step_delay);
                
                LATDCLR=mconn->MOTOR_PIN4;
                LATDCLR=mconn->MOTOR_PIN3;
                LATDSET=mconn->MOTOR_PIN2;
                LATDCLR=mconn->MOTOR_PIN1;
                mdelay(mconn->step_delay);
                
                LATDCLR=mconn->MOTOR_PIN4;
                LATDCLR=mconn->MOTOR_PIN3;
                LATDSET=mconn->MOTOR_PIN2;
                LATDSET=mconn->MOTOR_PIN1;
                mdelay(mconn->step_delay);
                
                LATDCLR=mconn->MOTOR_PIN4;
                LATDCLR=mconn->MOTOR_PIN3;
                LATDCLR=mconn->MOTOR_PIN2;
                LATDSET=mconn->MOTOR_PIN1;
                mdelay(mconn->step_delay);
                
                LATDSET=mconn->MOTOR_PIN4;
                LATDCLR=mconn->MOTOR_PIN3;
                LATDCLR=mconn->MOTOR_PIN2;
                LATDSET=mconn->MOTOR_PIN1;
                mdelay(mconn->step_delay);
                
                i++;
            }
            LATDCLR=mconn->MOTOR_PIN4;
            LATDCLR=mconn->MOTOR_PIN3;
            LATDCLR=mconn->MOTOR_PIN2;
            LATDCLR=mconn->MOTOR_PIN1;
        }
    }
}

