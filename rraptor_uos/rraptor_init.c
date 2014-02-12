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
task_t* taskX;
task_t* taskY;
task_t* taskZ;
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
void rraptor_main(void*);

void error(char* msg) {
    debug_printf("Error: %s\n", msg);
    //LATGCLR=1<<6; // light off
    //LATGSET=1<<6; // light on
}

void warn(char* msg) {
    debug_printf("Warning: %s\n", msg);
    //LATGCLR=1<<6; // light off
    //LATGSET=1<<6; // light on
}

void info(char* msg) {
    debug_printf("Info: %s\n", msg);
    //LATGCLR=1<<6; // light off
    //LATGSET=1<<6; // light on
}

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

void test_motor(motor_conn_stb57* mconn) {
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

int blink = 0;
void test_blink() {
    // blink led on PORTG#8 (ChipKIT#pin13)
    while(1) {
        LATGCLR=1<<6; // light off
        mdelay(1000);
        LATGSET=1<<6; // light on
        mdelay(1000);

        debug_printf("blink %d\n", blink);
        blink++;
    }
}


void uos_init (void) {
    info("Starting system...");    
    //debug
    TRISGCLR=1<<6;
    //LATGCLR=1<<6; // light off
    //LATGSET=1<<6; // light on

    info("Setup pinout and timers");
    // motors
    init_motors();

    // uos
    timer_init (&timer, KHZ, 1);

    // start main task
    info("Start main task");
    taskMain = task_create(rraptor_main, 0, "Main", 2, stackMain, sizeof(stackMain));
    //rraptor_main();
/*
while(1) {
    debug_printf(" timer delay=%dms\n", 0);
    mdelay(10);
    }*/
    //test_motor(&mconn_stb57_y);
    //test_blink();
}

void rraptor_main(void* arg) {
    move_head(50000, 20000, -20000, 10000);
    move_head(-20000, 40000, -20000, 10000);
    move_head(20000, 50000, 30000, 10000);
    move_head(50000, 50000, 20000, 10000);
    move_head(50000, 100000, 10000, 10000);

    info("Drawing finished\n");
    test_blink();
    /*while(1) {
        info("ping");
        mdelay(2000);
    }*/
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
    
    debug_printf("Info: Step motor name=%s", sdata->minfo->name);

    if(sdata->minfo->conn_type == CONNECTION_4PIN) {
        // step motor with 4-pin l294d driver
        debug_printf(", driver=%s\n", "l294d");
        step_motor_4pin(sdata->minfo, (motor_conn_4pin*)sdata->minfo->conn_info, sdata->dl, sdata->dt);
    } else if(sdata->minfo->conn_type == CONNECTION_STB57) {  
        // step motor with 3-pin stb57 driver
        debug_printf(", driver=%s\n", "stb57");
        step_motor_stb57(sdata->minfo, (motor_conn_stb57*)sdata->minfo->conn_info, sdata->dl, sdata->dt);        
    } else {
        warn("Unknown motor driver\n");
    }
    debug_printf("Info: Exit step motor task, name=%s\n", sdata->minfo->name);
    task_exit(sdata->minfo->name);
}

task_t* move_dim(motor_info* minfo, int dl, unsigned int dt, step_data* sdata, array_t* stack, int stacksz) {
    
    sdata->minfo = minfo;
    sdata->dl = dl;
    sdata->dt = dt;
	
    return task_create(step_motor, sdata, minfo->name, 1, stack, stacksz);
}


void move_head(int dx, int dy, int dz, unsigned int dt) {
    debug_printf("Info: Move head: dx=%d, dy=%d, dz=%d, dt=%u\n", dx, dy, dz, dt);
    // printf("move(%d, %d, %d, %d)\n", dx, dy, dz, dt);

    // for 3 motors with dpc=15um, tpc=250*4us=1ms max speed is 5mm/sec
    
    // with current precision:
    // max dl (dx,dy,dy) ~ 650000um (650mm=65cm for signed int);
    // max dt ~ 260000000us (260sec~4min 20 sec for unsigned int);
    // max distance for min time:
    //     ~ 650mm/130sec (ok for signed int)

	// validate values - check maximum speed for 3 motors:
	// dx/dt <= dpc_x/(tpc_x+tpc_y+tpc_z)
	// dy/dt <= dpc_y/(tpc_x+tpc_y+tpc_z)
	// dz/dt <= dpc_z/(tpc_x+tpc_y+tpc_z)

    int reset_time = FALSE;
	
	int min_tpc = 
	    motor_info_x.time_per_cycle + 
	    motor_info_y.time_per_cycle + 
	    motor_info_z.time_per_cycle;

	if(dx * min_tpc > motor_info_x.distance_per_cycle * dt) {
	    reset_time = TRUE;
	    warn("Speed is too high for dx, reset dt to min possible value");
	}
	if(dy * min_tpc > motor_info_y.distance_per_cycle * dt) {
	    reset_time = TRUE;
	    warn("Speed is too high for dy, reset dt to min possible value");
	}
	if(dz * min_tpc > motor_info_z.distance_per_cycle * dt) {
	    reset_time = TRUE;
	    warn("Speed is too high for dz, reset dt to min possible value");
	}

    // reset time to minimum possible value (for max speed) - maximum of minimums
	if(dt == 0 || reset_time) {
	    int min_dt_x = abs(dx) * min_tpc / motor_info_x.distance_per_cycle;
	    int min_dt_y = abs(dy) * min_tpc / motor_info_y.distance_per_cycle;
	    int min_dt_z = abs(dz) * min_tpc / motor_info_z.distance_per_cycle;

	    int max_min1 = min_dt_x > min_dt_y ? min_dt_x : min_dt_y;
	    dt = max_min1 > min_dt_z ? max_min1 : min_dt_z;

	    debug_printf("Warning: Reset dt=%d\n", dt);
	}
	
    // start new movement tasks
    info("Moving motors...");
	taskX = move_dim(&motor_info_x, dx, dt, &sdataX, stackX, sizeof(stackX));
	taskY = move_dim(&motor_info_y, dy, dt, &sdataY, stackY, sizeof(stackY));
	taskZ = move_dim(&motor_info_z, dz, dt, &sdataZ, stackZ, sizeof(stackZ));

	// wait for movement tasks to complete
    task_wait(taskX);
    task_wait(taskY);
	task_wait(taskZ);	
	info("Moving motors done");
}

void step_motor_stb57 (motor_info* minfo, motor_conn_stb57* mconn, int dl, unsigned int dt) {
    debug_printf("Info: Stepping motor [name=%s, driver=%s, dl=%d, dt=%d]: ", minfo->name, "stb57", dl, dt);

    if(dl == 0) {
        debug_printf("skip\n");
        return;
    }

    // calculate number of steps
    int step = abs(dl) / minfo->distance_per_cycle;
    debug_printf("step count=%d, ", step);
    
    // calculate cycle delay
    unsigned int cdelay = dt / step;
    debug_printf("cycle delay=%dus, ", cdelay);

    // calculate timer delay - in millis
    // time_per_cycle = step_delay * 4
    int tdelay = (cdelay - minfo->time_per_cycle) / 1000;
    debug_printf(" timer delay=%dms\n", tdelay);
    if(tdelay <= 0) {
        debug_printf("Error: timer delay <= 0 [motor=%s]\n", minfo->name);
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
        // timer delay - let other motors to work a bit too
        timer_delay(&timer, tdelay);
        //timer_delay(&timer, 2);
                
        LATDSET=mconn->MOTOR_PULSE;
        udelay(mconn->step_delay);
        LATDCLR=mconn->MOTOR_PULSE;
        udelay(mconn->step_delay);
        LATDSET=mconn->MOTOR_PULSE;
        udelay(mconn->step_delay);
        LATDCLR=mconn->MOTOR_PULSE;
        udelay(mconn->step_delay);
        
        i++;
    }
}

void step_motor_4pin (motor_info* minfo, motor_conn_4pin* mconn, int dl, unsigned int dt) {
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

