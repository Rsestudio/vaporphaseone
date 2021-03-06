/*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Copyright (c) 2019 PCB Arts GmbH - Kevin Schick
 *
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  _____    ____    ____
 *       \  /       |    \        /\            |
 *   ____/  |       |____/       /  \     __  __|__   ___
 *  |       |       |    \      /____\   /      |    |___
 *  |       \____   |____/     /      \  |      |     ___|
 *
 *         _    __   __   __     __         _    __   __
 *  \  /  /_\  |__| |  | |__|   |__| |__|  /_\  |__  |__
 *   \/  /   \ |    |__| | \    |    |  | /   \  __| |__
 *
 *
 *  PCB Arts GmbH
 *  Kurgartenstrasse 59
 *  90762 Fuerth
 *  GERMANY
 *
 *  www.pcb-arts.com
 *  mail(at)pcb-arts.com
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */


//=================================
// included dependencies
#include "ui/cli.h"
#include "ui/cli_commands.h"
#include "modules/lid.h"
#include "modules/lift.h"
#include "modules/heater.h"
#include "modules/motor.h"
#include "modules/pump.h"
#include "modules/fan.h"
#include "modules/fans.h"
#include "modules/regulator.h"
#include "modules/sequence.h"
#include "modules/access.h"
#include "modules/temp_sensors.h"
#include "utilities/ftos.h"
#include "utilities/Q7f24.h"
#include "utilities/arrays.h"

#include <stdio.h>
#include <stdlib.h>

//=================================
// definitions

//provide a short alias with predefined parameters
#define STARTS_WITH(command) cli_starts_with(buf, command, &len)

extern volatile char cli_send_buffer[];

static enum broadcaster_t bc_setting;

static void lid_cmd(const char* buf);
static void lift_cmd(const char* buf);
static void access_cmd(const char* buf);
static void heater_cmd(const char* buf);
static void regulator_cmd(const char* buf);
static void sequence_cmd(const char* buf);
static void quick_cooler_cmd(const char* buf);
static void hydro_cooler_cmd(const char* buf);
static void preset_cmd(const char* buf);

static int bc_seq_progress(volatile char* buf);

void cli_commands_execute(const char* buf) {
	int len = 0;

	if (STARTS_WITH(CMD_LID)) {
		lid_cmd(buf + len + 1);

	} else if (STARTS_WITH(CMD_LIFT)) {
		lift_cmd(buf + len + 1);

	} else if (STARTS_WITH(CMD_ACCESS)) {
		access_cmd(buf + len + 1);

	} else if (STARTS_WITH(CMD_HEATER)) {
		heater_cmd(buf + len + 1);

	} else if (STARTS_WITH(CMD_REGULATOR)) {
		regulator_cmd(buf + len + 1);

	} else if (STARTS_WITH(CMD_SEQUENCE)) {
		sequence_cmd(buf + len + 1);

	} else if (STARTS_WITH(CMD_PUMP_EN)) {
		if (atoi(buf + len))
			pump_enable();
		else
			pump_disable();

	} else if (STARTS_WITH(CMD_QUICK_COOLER)) {
		quick_cooler_cmd(buf + len + 1);

	} else if (STARTS_WITH(CMD_HYDRO_COOLER)) {
		hydro_cooler_cmd(buf + len + 1);

	} else if (STARTS_WITH(CMD_PRESET)) {
		preset_cmd(buf + len + 1);

	} else if (STARTS_WITH(CMD_BC_OFF)) {
		bc_setting = BC_OFF;

	} else if (STARTS_WITH(CMD_BC_TEMPS)) {
		bc_setting = BC_TEMP;

	} else if (STARTS_WITH("debug")) {
		bc_setting = BC_DEBUG;
	}
}

static void lid_cmd(const char* buf) {
	int len = 0;

	if (STARTS_WITH(CMD_SUB_UP)) {
		axis_move(&lid_axis, FWD);
	} else if (STARTS_WITH(CMD_SUB_DOWN)) {
		axis_move(&lid_axis, BWD);
	} else if (STARTS_WITH(CMD_SUB_STOP)) {
		motor_stop(lid_axis.motor);
	} else if (STARTS_WITH(CMD_SUB_TO)) {
		axis_move_to(&lid_axis, atoi(buf + len));
	} else if (STARTS_WITH(CMD_SUB_MIN)) {
		lid_axis.pos_min = atoi(buf + len);
	} else if (STARTS_WITH(CMD_SUB_MAX)) {
		lid_axis.pos_max = atoi(buf + len);
	} else if (STARTS_WITH(CMD_SUB_POS)) {
		len = sprintf((char*)cli_send_buffer, "lid pos: %i",
				(int)lid_axis.encoder->position);
		len = cli_add_CRLF(cli_send_buffer, len);
		cli_send_single((char*)cli_send_buffer, len);
	} else if (STARTS_WITH(CMD_SUB_BC_POS)) {
		bc_setting = BC_LID_POS;
	} else if (STARTS_WITH(CMD_SUB_CAL)) {
		axis_cal(&lid_axis, BWD);
	} else if (STARTS_WITH(CMD_SUB_SET)) {
		lid_axis.encoder->position = 0;
	} else if (STARTS_WITH(CMD_SUB_SPEED)) {
		motor_set_speed(lid_axis.motor, atoi(buf + len));
	}
}

static void lift_cmd(const char* buf) {
	int len = 0;

	if (STARTS_WITH(CMD_SUB_UP)) {
		axis_move(&lift_axis, FWD);
	} else if (STARTS_WITH(CMD_SUB_DOWN)) {
		axis_move(&lift_axis, BWD);
	} else if (STARTS_WITH(CMD_SUB_STOP)) {
		motor_stop(lift_axis.motor);
	} else if (STARTS_WITH(CMD_SUB_TO)) {
		axis_move_to(&lift_axis, atoi(buf + len));
	} else if (STARTS_WITH(CMD_SUB_MIN)) {
		lift_axis.pos_min = atoi(buf + len);
	} else if (STARTS_WITH(CMD_SUB_MAX)) {
		lift_axis.pos_max = atoi(buf + len);
	} else if (STARTS_WITH(CMD_SUB_POS)) {
		len = sprintf((char*)cli_send_buffer, "lift pos: %i",
				(int)lift_axis.encoder->position);
		len = cli_add_CRLF(cli_send_buffer, len);
		cli_send_single((char*)cli_send_buffer, len);
	} else if (STARTS_WITH(CMD_SUB_BC_POS)) {
		bc_setting = BC_LIFT_POS;
	} else if (STARTS_WITH(CMD_SUB_CAL)) {
		axis_cal(&lift_axis, FWD);
	} else if (STARTS_WITH(CMD_SUB_SET)) {
		lift_axis.encoder->position = 0;
	} else if (STARTS_WITH(CMD_SUB_SPEED)) {
		motor_set_speed(lift_axis.motor, atoi(buf + len));
	}
}

static void access_cmd(const char* buf) {
	int len = 0;

	if (STARTS_WITH(CMD_SUB_OPEN)) {
		access_open();
	} else if (STARTS_WITH(CMD_SUB_CLOSE)) {
		access_close();
	} else if (STARTS_WITH(CMD_SUB_STOP)) {
		access_stop();
	}
}

static void heater_cmd(const char* buf) {
	int len = 0;

	if (STARTS_WITH(CMD_SUB_REG)) {
		if (atoi(buf + len))
			heater_regulation_enable();
		else
			heater_regulation_disable();

	} else if (STARTS_WITH(CMD_SUB_EN)) {
		if (atoi(buf + len))
			heater_enable();
		else
			heater_disable();
	} else if (STARTS_WITH(CMD_SUB_TEMP)) {
		heater_set_temperature(atoi(buf + len));
	} else if (STARTS_WITH(CMD_SUB_HYSTERESIS)) {
		heater_set_hysteresis(atoi(buf + len));
	}
}

static void regulator_cmd(const char* buf) {
	int len = 0;

	if (STARTS_WITH(CMD_SUB_EN)) {
		if (atoi(buf + len))
			regulator_enable();
		else
			regulator_disable();
	} else if (STARTS_WITH(CMD_SUB_TEMP)) {
		regulator_set_temp(atoi(buf + len));
	} else if (STARTS_WITH(CMD_SUB_KP)) {
		Q7f24 val = atol(buf + len);
		regulator_pid.Kp = Q7f24TOFLT(val);
		pid_reset(&regulator_pid);
	} else if (STARTS_WITH(CMD_SUB_KI)) {
		Q7f24 val = atol(buf + len);
		regulator_pid.Ki = Q7f24TOFLT(val);
		pid_reset(&regulator_pid);
	} else if (STARTS_WITH(CMD_SUB_KD)) {
		Q7f24 val = atol(buf + len);
		regulator_pid.Kd = Q7f24TOFLT(val);
		pid_reset(&regulator_pid);
	}
}

static void sequence_cmd(const char* buf) {
	int len = 0;

	if (STARTS_WITH(CMD_SUB_START)) {
		seq_start();
	} else if (STARTS_WITH(CMD_SUB_STOP)) {
		seq_stop();
	} else if (STARTS_WITH(CMD_SUB_ABORT)) {
		seq_abort();
	} if (STARTS_WITH(CMD_SUB_CONTINUE)) {
		seq_continue();
	} if (STARTS_WITH(CMD_SUB_PROGRESS)) {
		len = sprintf((char*)cli_send_buffer, "sequence progress: %i%%",
				seq_progress());
		len = cli_add_CRLF(cli_send_buffer, len);
		cli_send_single((char*)cli_send_buffer, len);
	} else if (STARTS_WITH(CMD_SUB_SET)) {
		char* end;
		int32_t index = strtol (buf + len, &end, 10);
		int32_t temperature = strtol (end, &end, 10);
		int32_t duration = strtol (end, &end, 10);
		sequence.steps[index][SEQ_TEMP] = temperature;
		sequence.steps[index][SEQ_DUR] = duration;
	} else if (STARTS_WITH(CMD_SUB_BC_PROGRESS)) {
		bc_setting = BC_SEQ_PROGRESS;
	}
}

static void quick_cooler_cmd(const char* buf) {
	int len = 0;

	if (STARTS_WITH(CMD_SUB_MAN)) {
		fan_mode(&quick_cooler, FAN_MODE_MANUAL);
		fan_set_level(&quick_cooler, atoi(buf + len));

	} else if (STARTS_WITH(CMD_SUB_AUTO)) {
		fan_mode(&quick_cooler, FAN_MODE_2POS);
	}
}

static void hydro_cooler_cmd(const char* buf) {
	int len = 0;

	if (STARTS_WITH(CMD_SUB_MAN)) {
		fan_mode(&hydro_cooler, FAN_MODE_MANUAL);
		fan_set_level(&hydro_cooler, atoi(buf + len));

	} else if (STARTS_WITH(CMD_SUB_AUTO)) {
		fan_mode(&hydro_cooler, FAN_MODE_PID);
	}
}

static void preset_cmd(const char* buf) {
	switch (atoi(buf)) {
	default:
		break;

	case 1:
		//set heater hysteresis to 5°C
		heater_set_hysteresis(2.0f);

		//set heater to 100°C
		//heater_set_temperature(235.0f);

		//enable heater temperature regulation
		//heater_regulation_enable();

		regulator_pid.Kp = 0.5f;
		regulator_pid.Ki = 0.01f;
		regulator_pid.Kd = 4.0f;

		//regulator_set_temp(150);
		//regulator_enable();

		//close lid
		axis_move_to(&lid_axis, 0);

		//disable regulation and raise lift
		regulator_disable();
		axis_move_to(&lift_axis, 0);

		//enable cooling pump
		pump_enable();

		//enable cooling fans
		hydro_cooler.mode = FAN_MODE_MANUAL;
		fan_set_level(&hydro_cooler, 60);

		//disable quick cool fans
		quick_cooler.mode = FAN_MODE_MANUAL;
		fan_set_level(&quick_cooler, 0);
		break;

	case 2:
		seq_start();
		break;
	}
}

int cli_commands_broadcast(volatile char* buf) {
	int len;

	switch (bc_setting) {
	default:
	case BC_OFF:
		len = 0;
		cli_line_pos = 0;
		break;

	case BC_TEMP:
		//check limits
		if (cli_line_pos >= temp_sensors_size)
			cli_line_pos = 0;

		len = sprintf((char*)buf, "t%u: %s", cli_line_pos,
				ftos(temp_sensors[cli_line_pos]->temperature, 2) );

		//check if all lines were sent
		if (++cli_line_pos == temp_sensors_size)
			cli_line_pos = 0;
		break;

	case BC_LID_POS:;
		len = sprintf((char*)buf, "lid pos: %i",
				(int)lid_axis.encoder->position);
		break;

	case BC_LIFT_POS:
		len = sprintf((char*)buf, "lift pos: %i",
				(int)lift_axis.encoder->position);
		break;

	case BC_SEQ_PROGRESS:;
		len = bc_seq_progress(buf);
		break;

	case BC_DEBUG:
		len = sprintf((char*)buf, "%s",
				ftos(temp_sensors[2]->temperature, 2) );
		break;
	}

	return len;
}

static int bc_seq_progress(volatile char* buf) {
	int len;

	switch (cli_line_pos) {
	case 0:
		len = sprintf((char*)buf, "seq;");
		break;

	case 1:
		len = sprintf((char*)buf, "%s;", seq_get_state_str());
		break;

	case 2:
		len = sprintf((char*)buf, "%i%%;", seq_progress());
		break;

	default:
		len = sprintf((char*)buf, "%s;",
				ftos(temp_sensors[cli_line_pos - 3]->
						temperature, 2) );
		break;
	}

	if (++cli_line_pos >= 3 + temp_sensors_size) {
		cli_line_pos = 0;
	} else {
		cli_append_CRLF = 0;
	}

	return len;
}
