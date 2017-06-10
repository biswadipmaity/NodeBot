enum{
    waiting,
    right_motor,
    left_motor,
} receiverState;

int right_motor_speed, left_motor_speed;

void serialEvent() {
  while (Serial.available()) {
   // get the new byte:
   char inChar = (char)Serial.read();
   switch(receiverState)
   {
      case waiting:
        if(inChar == '+')
        {
            right_motor_direction=forward;
            receiverState=right_motor;
            right_motor_speed=0;
        }
        else if(inChar == '-')
        {
            right_motor_direction=reverse;
            receiverState=right_motor;
            right_motor_speed=0;
        }
        break;
      case right_motor:
          if(inChar >= '0' && inChar <= '9')
          {
            right_motor_speed=right_motor_speed*10 + (inChar-'0');
          }
          else if(inChar == '+')
          {
              left_motor_direction=forward;
              left_motor_speed=0;
              receiverState=left_motor;
          }
          else if(inChar == '-')
          {
              left_motor_direction=reverse;
              left_motor_speed=0;
              receiverState=left_motor;
          }
          else
          {
            receiverState=waiting;
          }
      break;
      case left_motor:
        if(inChar >= '0' && inChar <= '9')
        {
          left_motor_speed=left_motor_speed*10 + (inChar-'0');
        }
        else if(inChar == '#')
        {
            if(right_motor_direction == forward)
            {
              MR_fwd(right_motor_speed);
            }
            else
            {
              MR_rev(right_motor_speed);
            }

            if(left_motor_direction == forward)
            {
              ML_fwd(left_motor_speed);
            }
            else
            {
              ML_rev(left_motor_speed);
            }

            receiverState=waiting;
        }
        else{
          receiverState=waiting;
        }
      break;
      default:
        receiverState=waiting;
      break;
  }
}
}
