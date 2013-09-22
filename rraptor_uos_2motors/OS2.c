#include <runtime/lib.h>
#include <kernel/uos.h>
#include <uart/uart.h>
#include "step42_motor_lib.h"
#include <timer/timer.h>


ARRAY (stack1, 1000);
ARRAY (stack2,1000);

task_t *taskX;
task_t *taskY;


timer_t timer;

step_data datx={500,1,0,1<<0,1<<1,1<<2,1<<3,0};
step_data daty={500,10,0,1<<10,1<<5,1<<6,1<<7,0};

void step_motor (step_data*);

void step_motor42_x (void *arg)
{
   step_motor(&datx);
   task_exit("MotorX finish");
}

void step_motor42_y (void *arg)
{
    step_motor(&data);
    task_exit("MotorY finish");
}



void uos_init (void)
{
    TRISDCLR=datx.MOTOR_PIN1;
    TRISDCLR=datx.MOTOR_PIN2;
    TRISDCLR=datx.MOTOR_PIN3;
    TRISDCLR=datx.MOTOR_PIN4;
    
    TRISDCLR=daty.MOTOR_PIN1;
    TRISDCLR=daty.MOTOR_PIN2;
    TRISDCLR=daty.MOTOR_PIN3;
    TRISDCLR=daty.MOTOR_PIN4;
    
    timer_init (&timer, KHZ, 1);
    
    taskX=task_create(step_motor42_x,0, "MotorX", 1,stack1, sizeof(stack1));
    taskY=task_create(step_motor42_y,0, "MotorY", 1,stack2, sizeof(stack2));

    
}

void step_motor (step_data* data)
{

int i=0;
    
if ((*data).microstep==0)
{
if ((*data).dir==0)
{
while (i<(*data).step)
{
    timer_delay(&timer,10);
    
                LATDSET=(*data).MOTOR_PIN1;
                LATDCLR=(*data).MOTOR_PIN2;
                LATDCLR=(*data).MOTOR_PIN3;
                LATDCLR=(*data).MOTOR_PIN4;
                mdelay((*data).time);
                
                LATDCLR=(*data).MOTOR_PIN1;
                LATDSET=(*data).MOTOR_PIN2;
                LATDCLR=(*data).MOTOR_PIN3;
                LATDCLR=(*data).MOTOR_PIN4;
                mdelay((*data).time);
                
                LATDCLR=(*data).MOTOR_PIN1;
                LATDCLR=(*data).MOTOR_PIN2;
                LATDSET=(*data).MOTOR_PIN3;
                LATDCLR=(*data).MOTOR_PIN4;
                mdelay((*data).time);
                
                LATDCLR=(*data).MOTOR_PIN1;
                LATDCLR=(*data).MOTOR_PIN2;
                LATDCLR=(*data).MOTOR_PIN3;
                LATDSET=(*data).MOTOR_PIN4;
                mdelay((*data).time);
                
                i=i+1;
    
}
            
            LATDCLR=(*data).MOTOR_PIN1;
            LATDCLR=(*data).MOTOR_PIN2;
            LATDCLR=(*data).MOTOR_PIN3;
            LATDCLR=(*data).MOTOR_PIN4;
            
}
        
if ((*data).dir==1)
{
while (i<(*data).step)
{
    timer_delay(&timer,10);
    
                LATDSET=(*data).MOTOR_PIN4;
                LATDCLR=(*data).MOTOR_PIN3;
                LATDCLR=(*data).MOTOR_PIN2;
                LATDCLR=(*data).MOTOR_PIN1;
                mdelay((*data).time);
                
                LATDCLR=(*data).MOTOR_PIN4;
                LATDSET=(*data).MOTOR_PIN3;
                LATDCLR=(*data).MOTOR_PIN2;
                LATDCLR=(*data).MOTOR_PIN1;
                mdelay((*data).time);
                
                LATDCLR=(*data).MOTOR_PIN4;
                LATDCLR=(*data).MOTOR_PIN3;
                LATDSET=(*data).MOTOR_PIN2;
                LATDCLR=(*data).MOTOR_PIN1;
                mdelay((*data).time);
                
                LATDCLR=(*data).MOTOR_PIN4;
                LATDCLR=(*data).MOTOR_PIN3;
                LATDCLR=(*data).MOTOR_PIN2;
                LATDSET=(*data).MOTOR_PIN1;
                mdelay((*data).time);
                
                i=i+1;
    
    
}
            
            LATDCLR=(*data).MOTOR_PIN4;
            LATDCLR=(*data).MOTOR_PIN3;
            LATDCLR=(*data).MOTOR_PIN2;
            LATDCLR=(*data).MOTOR_PIN1;
}
}
    
if ((*data).microstep==1)
{
if ((*data).dir==0)
{
while (i<(*data).step)
{
    timer_delay(&timer,10);
                
                LATDSET=(*data).MOTOR_PIN1;
                LATDCLR=(*data).MOTOR_PIN2;
                LATDCLR=(*data).MOTOR_PIN3;
                LATDCLR=(*data).MOTOR_PIN4;
                mdelay((*data).time);
                
                LATDSET=(*data).MOTOR_PIN1;
                LATDSET=(*data).MOTOR_PIN2;
                LATDCLR=(*data).MOTOR_PIN3;
                LATDCLR=(*data).MOTOR_PIN4;
                mdelay((*data).time);
                
                LATDCLR=(*data).MOTOR_PIN1;
                LATDSET=(*data).MOTOR_PIN2;
                LATDCLR=(*data).MOTOR_PIN3;
                LATDCLR=(*data).MOTOR_PIN4;
                mdelay((*data).time);
                
                LATDCLR=(*data).MOTOR_PIN1;
                LATDSET=(*data).MOTOR_PIN2;
                LATDSET=(*data).MOTOR_PIN3;
                LATDCLR=(*data).MOTOR_PIN4;
                mdelay((*data).time);
                
                LATDCLR=(*data).MOTOR_PIN1;
                LATDCLR=(*data).MOTOR_PIN2;
                LATDSET=(*data).MOTOR_PIN3;
                LATDCLR=(*data).MOTOR_PIN4;
                mdelay((*data).time);
                
                LATDCLR=(*data).MOTOR_PIN1;
                LATDCLR=(*data).MOTOR_PIN2;
                LATDSET=(*data).MOTOR_PIN3;
                LATDSET=(*data).MOTOR_PIN4;
                mdelay((*data).time);
                
                LATDCLR=(*data).MOTOR_PIN1;
                LATDCLR=(*data).MOTOR_PIN2;
                LATDCLR=(*data).MOTOR_PIN3;
                LATDSET=(*data).MOTOR_PIN4;
                mdelay((*data).time);
                
                LATDSET=(*data).MOTOR_PIN1;
                LATDCLR=(*data).MOTOR_PIN2;
                LATDCLR=(*data).MOTOR_PIN3;
                LATDSET=(*data).MOTOR_PIN4;
                mdelay((*data).time);
                
                i=i+1;
    
    

}
            LATDCLR=(*data).MOTOR_PIN4;
            LATDCLR=(*data).MOTOR_PIN3;
            LATDCLR=(*data).MOTOR_PIN2;
            LATDCLR=(*data).MOTOR_PIN1;
}
        
if ((*data).dir==1)
{
while(i<(*data).step)
{
    timer_delay(&timer,10);
                
                LATDSET=(*data).MOTOR_PIN4;
                LATDCLR=(*data).MOTOR_PIN3;
                LATDCLR=(*data).MOTOR_PIN2;
                LATDCLR=(*data).MOTOR_PIN1;
                mdelay((*data).time);
                
                LATDSET=(*data).MOTOR_PIN4;
                LATDSET=(*data).MOTOR_PIN3;
                LATDCLR=(*data).MOTOR_PIN2;
                LATDCLR=(*data).MOTOR_PIN1;
                mdelay((*data).time);
                
                LATDCLR=(*data).MOTOR_PIN4;
                LATDSET=(*data).MOTOR_PIN3;
                LATDCLR=(*data).MOTOR_PIN2;
                LATDCLR=(*data).MOTOR_PIN1;
                mdelay((*data).time);
                
                LATDCLR=(*data).MOTOR_PIN4;
                LATDSET=(*data).MOTOR_PIN3;
                LATDSET=(*data).MOTOR_PIN2;
                LATDCLR=(*data).MOTOR_PIN1;
                mdelay((*data).time);
                
                LATDCLR=(*data).MOTOR_PIN4;
                LATDCLR=(*data).MOTOR_PIN3;
                LATDSET=(*data).MOTOR_PIN2;
                LATDCLR=(*data).MOTOR_PIN1;
                mdelay((*data).time);
                
                LATDCLR=(*data).MOTOR_PIN4;
                LATDCLR=(*data).MOTOR_PIN3;
                LATDSET=(*data).MOTOR_PIN2;
                LATDSET=(*data).MOTOR_PIN1;
                mdelay((*data).time);
                
                LATDCLR=(*data).MOTOR_PIN4;
                LATDCLR=(*data).MOTOR_PIN3;
                LATDCLR=(*data).MOTOR_PIN2;
                LATDSET=(*data).MOTOR_PIN1;
                mdelay((*data).time);
                
                LATDSET=(*data).MOTOR_PIN4;
                LATDCLR=(*data).MOTOR_PIN3;
                LATDCLR=(*data).MOTOR_PIN2;
                LATDSET=(*data).MOTOR_PIN1;
                mdelay((*data).time);
                
                i=i+1;
    
    
                
}
            LATDCLR=(*data).MOTOR_PIN4;
            LATDCLR=(*data).MOTOR_PIN3;
            LATDCLR=(*data).MOTOR_PIN2;
            LATDCLR=(*data).MOTOR_PIN1;
}
}


}