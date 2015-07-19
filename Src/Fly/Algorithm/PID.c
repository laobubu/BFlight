/****************************************************************************
 *
 *   Copyright (C) 2008-2013 PX4 Development Team. All rights reserved.
 *   Author: Laurens Mackay <mackayl@student.ethz.ch>
 *           Tobias Naegeli <naegelit@student.ethz.ch>
 *           Martin Rutschmann <rutmarti@student.ethz.ch>
 *           Anton Babushkin <anton.babushkin@me.com>
 *           Julian Oes <joes@student.ethz.ch>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name PX4 nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/**
 * @file pid.c
 *
 * Implementation of generic PID controller.
 *
 * @author Laurens Mackay <mackayl@student.ethz.ch>
 * @author Tobias Naegeli <naegelit@student.ethz.ch>
 * @author Martin Rutschmann <rutmarti@student.ethz.ch>
 * @author Anton Babushkin <anton.babushkin@me.com>
 * @author Julian Oes <joes@student.ethz.ch>
 */

#include "PID.h"
#include "AlgorithmBasic.h"

#define SIGMA 0.000001f

void PID_Init(PID_Typedef * pid, pid_mode_t mode, float dt_min) {
	pid->mode = mode;
	pid->dt_min = dt_min;
	pid->kp = 0.0f;
	pid->ki = 0.0f;
	pid->kd = 0.0f;
	pid->kdd = 0.0f;
	pid->integral = 0.0f;
	pid->integral_limit = 0.0f;
	pid->integral_max_error = 0.0f;
	pid->output_limit = 0.0f;
	pid->error_previous = 0.0f;
	pid->d_previous = 0.0f;
	pid->last_output = 0.0f;
}

float PID_Postion_Cal(PID_Typedef *pid, float sp, float val, float val_dot, float dt)
{
	if (!isfinite(sp) || !isfinite(val) || !isfinite(val_dot) || !isfinite(dt)) {
		return pid->last_output;
	}

	float i, d,dd;

	/* current error value */
	float error = sp - val;

	/* current error derivative */
	if (pid->mode == PID_MODE_DERIVATIV_CALC) {
		d = (error - pid->error_previous) / fmaxf(dt, pid->dt_min);
		pid->error_previous = error;

	} else if (pid->mode == PID_MODE_DERIVATIV_CALC_NO_SP) {
		d = (-val - pid->error_previous) / fmaxf(dt, pid->dt_min);
		pid->error_previous = -val;

	} else if (pid->mode == PID_MODE_DERIVATIV_SET) {
		d = -val_dot;

	} else {
		d = 0.0f;
	}

	if (!isfinite(d)) {
		d = 0.0f;
	}
	
	dd = d - pid->d_previous;
	pid->d_previous = d;

	/* calculate PD output */
	float output = (error * pid->kp) + (d * pid->kd);
	
	output += dd * pid->kdd;

	if ((pid->ki > SIGMA)) {
		// Calculate the error integral and check for saturation
		
		i = pid->integral;
		
		if (pid->integral_max_error < SIGMA || fabsf(error) < pid->integral_max_error) {
			i += (error * dt);

			/* check for saturation */
			if (isfinite(i)) {
				if ((pid->output_limit < SIGMA || (fabsf(output + (i * pid->ki)) <= pid->output_limit)) &&
					fabsf(i) <= pid->integral_limit) {
					/* not saturated, use new integral value */
					pid->integral = i;
				}else {
				pid->integral = 0;   //IÏ´°×°×
				}
			}
		}

		/* add I component to output */
		output += pid->integral * pid->ki;
	}

	/* limit output */
	if (isfinite(output)) {
		if (pid->output_limit > SIGMA) {
			if (output > pid->output_limit) {
				output = pid->output_limit;

			} else if (output < -pid->output_limit) {
				output = -pid->output_limit;
			}
		}

		pid->last_output = output;
	}

	return pid->last_output;
}

void pid_Reset_Integral(PID_Typedef *pid)
{
	pid->integral = 0.0f;
}
