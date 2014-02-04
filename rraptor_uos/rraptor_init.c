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

step_data sdataX;
step_data sdataY;
step_data sdataZ;

task_t *taskMain;
task_t *taskX;
task_t *taskY;
task_t *taskZ;
task_t *taskExt1;


timer_t timer;

motor_conn_4pin mconn_4pin_x = {1<<10,1<<5,1<<6,1<<7,0};
motor_conn_4pin mconn_4pin_y = {1<<0,1<<1,1<<2,1<<3,0};
motor_conn_4pin mconn_4pin_z = {1<<10,1<<5,1<<6,1<<7,0};
motor_conn_4pin mconn_4pin_ext1 = {1<<10,1<<5,1<<6,1<<7,0};

motor_conn_stb57 mconn_stb57_x;
motor_conn_stb57 mconn_stb57_y;
motor_conn_stb57 mconn_stb57_z;
motor_conn_stb57 mconn_stb57_ext1;

motor_info motor_info_x;
motor_info motor_info_y;
motor_info motor_info_z;
motor_info motor_info_ext1;



/**
 * Program main cycle.
 */
void rraptor_main(void);


void init_motors() {
    // Connection info
    mconn_stb57_x.MOTOR_DIR = 1<<3;
    mconn_stb57_x.MOTOR_PULSE = 1<<4;
    mconn_stb57_x.MOTOR_EN = 0;
    mconn_stb57_x.step_delay = 250;

    mconn_stb57_y.MOTOR_DIR = 1<<1;
    mconn_stb57_y.MOTOR_PULSE = 1<<2;
    mconn_stb57_y.MOTOR_EN = 0;
    mconn_stb57_y.step_delay = 250;

    mconn_stb57_z.MOTOR_DIR = 1<<10;
    mconn_stb57_z.MOTOR_PULSE = 1<<5;
    mconn_stb57_z.MOTOR_EN = 0;
    mconn_stb57_z.step_delay = 250;

    mconn_stb57_ext1.MOTOR_DIR = 0;
    mconn_stb57_ext1.MOTOR_PULSE = 0;
    mconn_stb57_ext1.MOTOR_EN = 0;
    mconn_stb57_ext1.step_delay = 250;

    // Motor info
    motor_info_x.name = "X";
    motor_info_x.distance_per_cycle = 15;
    // with step_delay=250 1 cycle=1 mls
    motor_info_x.time_per_cycle = mconn_stb57_x.step_delay * 4;
    motor_info_x.conn_type = CONNECTION_STB57;
    motor_info_x.conn_info = &mconn_stb57_x;
    
    motor_info_y.name = "Y";
    motor_info_y.distance_per_cycle = 15;
    motor_info_y.time_per_cycle = mconn_stb57_y.step_delay * 4;
    motor_info_y.conn_type = CONNECTION_STB57;
    motor_info_y.conn_info = &mconn_stb57_y;
    
    motor_info_z.name = "Z";
    motor_info_z.distance_per_cycle = 15;
    motor_info_z.time_per_cycle = mconn_stb57_z.step_delay * 4;
    motor_info_z.conn_type = CONNECTION_STB57;
    motor_info_z.conn_info = &mconn_stb57_z;
    
    motor_info_ext1.name = "Ext1";
    motor_info_ext1.distance_per_cycle = 15;
    motor_info_ext1.time_per_cycle = mconn_stb57_ext1.step_delay * 4;
    motor_info_ext1.conn_type = CONNECTION_STB57;
    motor_info_ext1.conn_info = &mconn_stb57_ext1;

    // Init motors pinout
    TRISDCLR=mconn_stb57_x.MOTOR_DIR;
    TRISDCLR=mconn_stb57_x.MOTOR_PULSE;
    
    TRISDCLR=mconn_stb57_y.MOTOR_DIR;
    TRISDCLR=mconn_stb57_y.MOTOR_PULSE;
    
    TRISDCLR=mconn_stb57_z.MOTOR_DIR;
    TRISDCLR=mconn_stb57_z.MOTOR_PULSE;

    /*TRISDCLR=mconn_4pin_x.MOTOR_PIN1;

    TRISDCLR=mconn_4pin_x.MOTOR_PIN2;

    TRISDCLR=mconn_4pin_x.MOTOR_PIN3;

    TRISDCLR=mconn_4pin_x.MOTOR_PIN4;

    

    TRISDCLR=mconn_4pin_y.MOTOR_PIN1;
    TRISDCLR=mconn_4pin_y.MOTOR_PIN2;

    TRISDCLR=mconn_4pin_y.MOTOR_PIN3;

    TRISDCLR=mconn_4pin_y.MOTOR_PIN4;*/
}

void test1(motor_conn_stb57* mconn) {
    LATDCLR=mconn->MOTOR_DIR;
    //LATDSET=mconn->MOTOR_DIR;
    
    while (1) {
        LATDSET=mconn->MOTOR_PULSE;
        udelay(mconn->step_delay);
        LATDCLR=mconn->MOTOR_PULSE;
        udelay(mconn->step_delay);
        LATDSET=mconn->MOTOR_PULSE;
        udelay(mconn->step_delay);
        LATDCLR=mconn->MOTOR_PULSE;
        udelay(mconn->step_delay);
        
        //timer_delay(&timer, cdelay - mconn->step_delay * 4);
        //timer_delay(&timer, cdelay - mconn->step_delay * 4);
    }
}


void uos_init (void) {    
    //debug
    //TRISGCLR=1<<6;

    // motors
    init_motors();

    // uos
    timer_init (&timer, KHZ, 1);

    // start main task
    taskMain = task_create(rraptor_main, 0, "Main", 2, stackMain, sizeof(stackMain));
    //rraptor_main();
    //test1(&mconn_stb57_y);
}

void rraptor_main(void) {
    //move_head(50000, 20000, -20000, 10000);
    move_head(-20000, 40000, -20000, 10000000);
    //move_head(0, 50000, 0, 10000);
    //move_head(50000, 0, 0, 10000);
    //move_head(50000, 100000, 0, 10000);
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
        // step motor with 4-pin l294d driver
        step_motor_4pin(sdata->minfo, (motor_conn_4pin*)sdata->minfo->conn_info, sdata->dl, sdata->dt);
    } else if(sdata->minfo->conn_type == CONNECTION_STB57) {  
        // step motor with 3-pin stb57 driver
        step_motor_stb57(sdata->minfo, (motor_conn_stb57*)sdata->minfo->conn_info, sdata->dl, sdata->dt);        
    }
    task_exit(sdata->minfo->name);
}

task_t* move_dim(motor_info* minfo, unsigned int dl, unsigned int dt, step_data* sdata, array_t* stack, int stacksz) {
    
    sdata->minfo = minfo;
    sdata->dl = dl;
    sdata->dt = dt;
	
    return task_create(step_motor, sdata, minfo->name, 1, stack, stacksz);
}

void error(char* msg) {
    //printf("Error: %s\n", msg);
    //LATGCLR=1<<6; // light off
    //LATGSET=1<<6; // light on
}

void move_head(unsigned int dx, unsigned int dy, unsigned int dz, unsigned int dt) {
    // with current precision max dl (dx,dy,dy) ~ 1300000um (1m 30cm for unsigned int),
    // max dt ~ 250000000us (4min for unsigned int)
    
	//printf("move(%d, %d, %d, %d)\n", dx, dy, dz, dt);

	// validate values - calculate maximum speed for 3 motors:
	//int max_speed_x = motor_info_x.distance_per_cycle / 
	//    (motor_info_x.time_per_cycle + motor_info_y.time_per_cycle + motor_info_z.time_per_cycle);
	//int max_speed_y = motor_info_y.distance_per_cycle / 
	//    (motor_info_x.time_per_cycle + motor_info_y.time_per_cycle + motor_info_z.time_per_cycle);
	//int max_speed_z = motor_info_z.distance_per_cycle / 
	//    (motor_info_x.time_per_cycle + motor_info_y.time_per_cycle + motor_info_z.time_per_cycle);

    // wait for old movement tasks to complete
	task_wait(taskX);
	task_wait(taskY);
	task_wait(taskZ);

    // start new movement tasks
	taskX = move_dim(&motor_info_x, dx, dt, &sdataX, stackX, sizeof(stackX));
	taskY = move_dim(&motor_info_y, dy, dt, &sdataY, stackY, sizeof(stackY));
	taskZ = move_dim(&motor_info_z, dz, dt, &sdataZ, stackZ, sizeof(stackZ));
}

void step_motor_stb57 (motor_info* minfo, motor_conn_stb57* mconn, unsigned int dl, unsigned int dt) {
    // calculate number of steps
    int step = dl / minfo->distance_per_cycle;
    step = step >= 0 ? step : -step;
    
    // calculate cycle delay
    unsigned int cdelay = dt / step;

    // calculate timer delay - in millis
    // time_per_cycle = step_delay * 4
    int tdelay = (cdelay - minfo->time_per_cycle) / 1000;
    if(tdelay <= 0) {
//        printf("Error: timer delay < 0 [motor=%s]", minfo->name);
        error("Error: timer delay < 0");
        return;
    }
      
    // set direction
    if(dl > 0) {
        LATDSET = mconn->MOTOR_DIR;
    } else {
        LATDCLR = mconn->MOTOR_DIR;
    }
    
    int i=0;
    while (i < step) {        
        LATDSET=mconn->MOTOR_PULSE;
        udelay(mconn->step_delay);
        LATDCLR=mconn->MOTOR_PULSE;
        udelay(mconn->step_delay);
        LATDSET=mconn->MOTOR_PULSE;
        udelay(mconn->step_delay);
        LATDCLR=mconn->MOTOR_PULSE;
        udelay(mconn->step_delay);
        
        i++;

        // timer delay - let other motors to work a bit too
        timer_delay(&timer, tdelay);
        //timer_delay(&timer, 2);
    }
}

void step_motor_4pin (motor_info* minfo, motor_conn_4pin* mconn, unsigned int dl, unsigned int dt) {
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

