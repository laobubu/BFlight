#include "PX4Flow.h"
#include "FlyBasic.h"

#include "mavlink/common/mavlink.h"
#include "mavlink/pixhawk/mavlink.h"

typedef __packed struct {
uint64_t	time_usec	;
	float	flow_comp_m_x	;
	float	flow_comp_m_y	;
	float	ground_distance	;
	int16_t	flow_x	;
	int16_t	flow_y	;
	uint8_t	sensor_id	;
	uint8_t	quality	;
} PX4Flow_Package_Typedef;

typedef __packed struct {
	uint64_t	time_usec;
	uint32_t	integration_time_us;
	float	integrated_x;
	float	integrated_y;
	float	integrated_xgyro;
	float	integrated_ygyro;
	float	integrated_zgyro;
	uint32_t	time_delta_distance_us;
	float	distance;	
	int16_t	temperature;
	uint8_t	sensor_id;
	uint8_t	quality;
} PX4Flow_Package_RAD_Typedef;


extern UART_HandleTypeDef huart2;
#define uart huart2

PX4Flow_Typedef PX4Flow;
uint8_t byte;

void PX4Flow_Init(void)
{
	//PX4Flow is using UART
	memset(&PX4Flow, 0, sizeof(PX4Flow_Typedef));
	PX4Flow.ratio = 1.0f;
	
	HAL_UART_Receive_IT(&uart, &byte, 1);
}

static mavlink_message_t px4_msg;
static mavlink_status_t px4_status;

void PX4Flow_FeedByte(char byte)
{
	if(mavlink_parse_char(MAVLINK_COMM_0, byte, &px4_msg, &px4_status))
	{
		switch(px4_msg.msgid)
		{
			case MAVLINK_MSG_ID_HEARTBEAT:
			{
				// E.g. read GCS heartbeat and go into
				// comm lost mode if timer times out
			}
			break;
			
			case MAVLINK_MSG_ID_OPTICAL_FLOW:
			{
				PX4Flow_Package_Typedef *d1 = (PX4Flow_Package_Typedef*) px4_msg.payload64;
				if (PX4Flow.lastUpdate != 0)
				{
					float timeSpan = PX4Flow.ratio * (d1->time_usec - PX4Flow.lastUpdate) / 1000000.0f;
					PX4Flow.x += d1->flow_x * timeSpan;
					PX4Flow.y += d1->flow_y * timeSpan;
				}
				PX4Flow.lastUpdate = d1->time_usec;
			}
			break;
			
			case MAVLINK_MSG_ID_OPTICAL_FLOW_RAD:
			{
				PX4Flow_Package_RAD_Typedef *d2 = (PX4Flow_Package_RAD_Typedef*) px4_msg.payload64;
			}
			break;
		}
	}
}
