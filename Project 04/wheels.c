
void RunMotors(void){
    
  if (stopwatch_seconds==6 && cycle_count==0){
    P6OUT &= ~R_FORWARD;
    P6OUT &= ~L_FORWARD;
  }
  else if (stopwatch_seconds==1 && cycle_count==0){
    P6OUT |= R_FORWARD;
    P6OUT |= L_FORWARD;
  }
    
    
}