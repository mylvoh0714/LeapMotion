/******************************************************************************\
* Copyright (C) 2012-2017 Leap Motion, Inc. All rights reserved.               *
* Leap Motion proprietary and confidential. Not for distribution.              *
* Use subject to the terms of the Leap Motion SDK Agreement available at       *
* https://developer.leapmotion.com/sdk_agreement, or another agreement         *
* between Leap Motion and you, your company or other organization.             *
\******************************************************************************/

#undef __cplusplus

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include "LeapC.h"
#include "ExampleConnection.h"


float getDistance(LEAP_VECTOR v1, LEAP_VECTOR v2)
{
	float ret = ( v1.x - v2.x )*( v1.x - v2.x ) +
		( v1.y - v2.y )*( v1.y - v2.y ) +
		( v1.z - v2.z )*( v1.z - v2.z );
	return (float)sqrt(ret);
}

int64_t lastFrameID = 0; //The last frame received

bool isOkay = false;
int isNumber = 0;

void checkOkay(LEAP_HAND* hand)
{
	if ( getDistance(hand->thumb.distal.prev_joint, hand->index.distal.next_joint) < 30 && hand->pinch_strength == 1 )
		isOkay = true;
}

void checkNumber(LEAP_HAND* hand)
{
	for ( int i = 0; i < 5; i++ )
	{
		isNumber += hand->digits[i].is_extended;
	}
}

void onOkay()
{
	printf("Okay event happens\n");
	isOkay = false;
}

void onNumbering()
{
	printf("Your gesture is pointing number %d!!\n", isNumber);
	isNumber = 0;
}

void EventHandler(LEAP_HAND* hand)
{
	checkOkay(hand);
	checkNumber(hand); //todo
	//todo

	if ( isOkay ) {
		onOkay();
		return;
	}
	else if ( isNumber ) {
		onNumbering();
		return;
	}
	//todo
	//todo
}

int main(int argc, char** argv) {
  OpenConnection();
  while(!IsConnected)
    millisleep(100); //wait a bit to let the connection complete

  printf("Connected.");
  LEAP_DEVICE_INFO* deviceProps = GetDeviceProperties();
  if(deviceProps)
    printf("Using device %s.\n", deviceProps->serial);

  for(;;){
	//getchar();
    LEAP_TRACKING_EVENT *frame = GetFrame();
	printf("-------------------------------------\n");

    if(frame && (frame->tracking_frame_id > lastFrameID)){
      lastFrameID = frame->tracking_frame_id;
      //printf("Frame %lli with %i hands.\n", (long long int)frame->tracking_frame_id, frame->nHands);
      for(uint32_t h = 0; h < frame->nHands; h++){
        LEAP_HAND* hand = &frame->pHands[h];

		EventHandler(hand);
		/*printf("hand id : %i\n", hand->id);
		printf("confidence : %f\n", hand->confidence);
		printf("visible time : %f seconds\n", (float)hand->visible_time/1000000);
		printf("pinch_distance : %f mm\n", hand->pinch_distance);
		printf("pinch_strength : %f\n", hand->pinch_strength);
		printf("grab_angle : %f\n", hand->grab_angle);
		printf("grab_strength : %f\n", hand->grab_strength);*/
		
		/*printf("thumb distal next position (%f,%f,%f)\n",
			   hand->digits[0].distal.next_joint.x,
			   hand->digits[0].distal.next_joint.y,
			   hand->digits[0].distal.next_joint.z);
		printf("index distal next position (%f,%f,%f)\n",
			   hand->digits[1].distal.next_joint.x,
			   hand->digits[1].distal.next_joint.y,
			   hand->digits[1].distal.next_joint.z);*/
        
		/*printf("    Hand id %i is a %s hand with position (%f, %f, %f).\n",
                    hand->id,
                    (hand->type == eLeapHandType_Left ? "left" : "right"),
                    hand->palm.position.x,
                    hand->palm.position.y,
                    hand->palm.position.z);*/
      }
    }
  } //ctrl-c to exit
  return 0;
}
//End-of-Sample
