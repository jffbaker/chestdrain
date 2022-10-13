#include <xc.h>
#include <stdlib.h>
#include "globals.h"
#include "options.h"
#include "measure.h"
#include "util.h"
#include "adi.h"

void measure(){
    unsigned char col;
    
    adi_start();
    adi_clear_int();
    adi_wait_for_int();
    adi_clear_int();
    adi_read_conversions();
    
    
    for(col=0;col<3;col++){
    //setup to measure 0-11
    
    
    //setup to measure 12    
        
    }
    
    
    
    
    
}