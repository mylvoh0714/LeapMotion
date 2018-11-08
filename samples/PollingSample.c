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

int64_t lastFrameID = 0; //The last frame received

bool isOkay = false;
bool isStop = false;
int isNumber = 0;

void print(LEAP_VECTOR v)
{
	printf("(%f,%f,%f)\n", v.x, v.y, v.z);
}

LEAP_VECTOR getDifference(LEAP_VECTOR v1, LEAP_VECTOR v2)
{
	LEAP_VECTOR ret;
	for ( int i = 0; i < 3; i++ )
	{
		ret.v[i] = v1.v[i] - v2.v[i];
	}
	ret.x = v1.x - v2.x;
	ret.y = v1.y - v2.y;
	ret.z = v1.z - v2.z;
	return ret;
}

float getNorm(LEAP_VECTOR v)
{
	float ret = v.x*v.x + v.y*v.y + v.z*v.z;
	return (float)sqrt(ret);
}

float getDistance(LEAP_VECTOR v1, LEAP_VECTOR v2)
{
	LEAP_VECTOR temp = getDifference(v1, v2);
	return getNorm(temp);
	/*float ret = ( v1.x - v2.x )*( v1.x - v2.x ) +
		( v1.y - v2.y )*( v1.y - v2.y ) +
		( v1.z - v2.z )*( v1.z - v2.z );
	return (float)sqrt(ret);*/
}

float innerProduct(LEAP_VECTOR v1, LEAP_VECTOR v2)
{
	return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}

// get cosine similiary between 2 vector.
float getCos(LEAP_VECTOR v1, LEAP_VECTOR v2)
{
	float dot = innerProduct(v1,v2);
	if ( dot > 0 )
		return dot / ( getNorm(v1)*getNorm(v2) );
	else
		return -dot / ( getNorm(v1)*getNorm(v2) );
}

void checkOkay(LEAP_HAND* hand)
{
	isOkay = false;
	if ( getDistance(hand->thumb.distal.prev_joint, hand->index.distal.next_joint) < 30 && hand->pinch_strength == 1 )
		isOkay = true;
}

void checkStop(LEAP_HAND* hand)
{
	isStop = false;

	// assert 2~5th finger extended.
	for ( int i = 1; i < 5; i++ )
	{
		if ( hand->digits[i].is_extended == false )
			return;
	}

	LEAP_VECTOR middleVector;
	for ( int i = 0; i < 3; i++ )
	{
		middleVector.v[i] = hand->middle.bones[3].next_joint.v[i] - hand->middle.bones[0].prev_joint.v[i];
	}
	middleVector.x = middleVector.v[0];
	middleVector.y = middleVector.v[1];
	middleVector.z = middleVector.v[2];

	if ( getCos(middleVector, hand->palm.normal) > 0.3 )
		return;

	LEAP_VECTOR j;
	j.x = 0; j.y = 1; j.z = 0;
	j.v[0] = 0; j.v[1] = 1; j.v[2] = 0;
	if ( getCos(hand->palm.direction, j) < 0.8 )
		return;

	isStop = true;
}

void checkNumber(LEAP_HAND* hand)
{
	isNumber = 0;
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

void onStop()
{
	printf("Stop event happens\n");
	isStop = false;
}

void onNumbering()
{
	printf("Your gesture is pointing number %d!!\n", isNumber);
	isNumber = 0;
}

void EventHandler(LEAP_HAND* hand)
{
	checkOkay(hand);
	checkStop(hand);
	checkNumber(hand); //todo
	//todo

	if ( isOkay ) {
		onOkay();
		isOkay = false;
		return;
	}
	else if ( isStop ) {
		onStop();
		isStop = false;
		return;
	}
	else if ( isNumber ) {
		onNumbering();
		isNumber = 0;
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
	millisleep(200);
	//getchar();
    LEAP_TRACKING_EVENT *frame = GetFrame();
	printf("-------------------------------------\n");

    if(frame && (frame->tracking_frame_id > lastFrameID)){
      lastFrameID = frame->tracking_frame_id;
      //printf("Frame %lli with %i hands.\n", (long long int)frame->tracking_frame_id, frame->nHands);
      for(uint32_t h = 0; h < frame->nHands; h++){
        LEAP_HAND* hand = &frame->pHands[h];
		
		LEAP_PALM* palm = &hand->palm;
		
		float x = hand->digits[1].distal.next_joint.x - hand->digits[1].distal.prev_joint.x;
		float y = hand->digits[1].distal.next_joint.y - hand->digits[1].distal.prev_joint.y;
		float z = hand->digits[1].distal.next_joint.z - hand->digits[1].distal.prev_joint.z;
		LEAP_VECTOR d1;
		d1.x = x; d1.y = y; d1.z = z;

		float x2 = hand->digits[2].distal.next_joint.x - hand->digits[2].distal.prev_joint.x;
		float y2 = hand->digits[2].distal.next_joint.y - hand->digits[2].distal.prev_joint.y;
		float z2 = hand->digits[2].distal.next_joint.z - hand->digits[2].distal.prev_joint.z;
		LEAP_VECTOR d2;
		d2.x = x2; d2.y = y2; d2.z = z2;
		
		d2.x = 0;
		d2.y = 1;
		d2.z = 0;

		//printf("%f\n", getCos(palm->direction, d2));
		//LEAP_VECTOR v = palm->velocity;
		//print(v);
		//printf("%f\n", getNorm(v));


		//LEAP_VECTOR n = palm->normal;
		//printf("cos(theta) : %lf\n", getCos(n,d1));

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
