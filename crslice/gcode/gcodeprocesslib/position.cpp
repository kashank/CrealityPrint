////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Gcode Processor Library
//
// Tools for parsing gcode and calculating printer state from parsed gcode commands.
//
// Copyright(C) 2021 - Brad Hochgesang
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// This program is free software : you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
// GNU Affero General Public License for more details.
//
//
// You can contact the author at the following email address: 
// FormerLurker@pm.me
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "position.h"
#include <iostream>
#include <iomanip>
#include <sstream>
std::string position::to_string(bool rewrite, bool verbose, std::string additional_comment)
{
	if (verbose)
	{
		std::stringstream stream;
		stream << std::fixed << std::setprecision(5);
		stream << "," << get_current_extruder().e << "," << get_current_extruder().get_offset_e() << "," << get_current_extruder().e_relative;
		command.comment.append(stream.str());
	}
	command.comment.append(additional_comment);

	if (rewrite)
	{
		return command.rewrite_gcode_string();
	}
	
	return command.to_string();
}

void position::set_xyz_axis_mode(const std::string& xyz_axis_default_mode)
{
	if (xyz_axis_default_mode == "relative" || xyz_axis_default_mode == "force-relative")
	{
		is_relative = true;
		is_relative_null = false;
	}
	else if (xyz_axis_default_mode == "absolute" || xyz_axis_default_mode == "force-absolute")
	{
		is_relative = false;
		is_relative_null = false;
	}

	
}

void position::set_e_axis_mode(const std::string& e_axis_default_mode)
{
	if (e_axis_default_mode == "relative" || e_axis_default_mode == "force-relative")
	{
		is_extruder_relative = true;
		is_extruder_relative_null = false;
	}
	else if (e_axis_default_mode == "absolute" || e_axis_default_mode == "force-absolute")
	{
		is_extruder_relative = false;
		is_extruder_relative_null = false;
	}

	
}

void position::set_units_default(const std::string&	units_default)
{
	if (units_default == "inches")
	{
		is_metric = false;
		is_metric_null = false;
	}
	else if (units_default == "millimeters")
	{
		is_metric = true;
		is_metric_null = false;
	}
}

bool position::can_take_snapshot()
{
	return (
		!is_relative_null && 
		!is_extruder_relative_null &&
		has_definite_position &&
		is_printer_primed &&
		!is_metric_null
	);
}

position::position()
{
	has_been_deleted = false;
	is_empty = true;
	feature_type_tag = 0;
	f = 0;
	f_null = true;
	x = 0;
	x_null = true;
	x_offset = 0;
	x_firmware_offset = 0;
	x_homed = false;
	y = 0;
	y_null = true;
	y_offset = 0;
	y_firmware_offset = 0;
	y_homed = false;
	z = 0;
	z_null = true;
	z_offset = 0;
	z_firmware_offset = 0;
	z_homed = false;
	is_relative = false;
	is_relative_null = true;
	is_extruder_relative = false;
	is_extruder_relative_null = true;
	is_metric = true;
	is_metric_null = true;
	last_extrusion_height = 0;
	last_extrusion_height_null = true;
	layer = 0;
	height = 0;
	height_increment = 0;
	height_increment_change_count = 0;
	is_printer_primed = false;
	has_definite_position = false;
	z_relative = 0;
	is_in_position = false;
	in_path_position = false;
	is_zhop = false;
	is_layer_change = false;
	is_height_change = false;
	is_height_increment_change = false;
	is_xy_travel = false;
	is_xyz_travel = false;
	has_xy_position_changed = false;
	has_position_changed = false;
	has_received_home_command = false;
	file_line_number = -1;
	gcode_number = -1;
	file_position = -1;
	gcode_ignored = true;
	is_in_bounds = true;
	current_tool = -1;
	p_extruders = NULL;
	num_extruders = 0;
	set_num_extruders(num_extruders);
}

position::position(int extruder_count)
{ 
	has_been_deleted = false;
	is_empty = true;
	feature_type_tag = 0;
	f = 0;
	f_null = true;
	x = 0;
	x_null = true;
	x_offset = 0;
	x_firmware_offset = 0;
	x_homed = false;
	y = 0;
	y_null = true;
	y_offset = 0;
	y_firmware_offset = 0;
	y_homed = false;
	z = 0;
	z_null = true;
	z_offset = 0;
	z_firmware_offset = 0;
	z_homed = false;
	is_relative = false;
	is_relative_null = true;
	is_extruder_relative = false;
	is_extruder_relative_null = true;
	is_metric = true;
	is_metric_null = true;
	last_extrusion_height = 0;
	last_extrusion_height_null = true;
	layer = 0;
	height = 0;
	height_increment = 0;
	height_increment_change_count = 0;
	is_printer_primed = false;
	has_definite_position = false;
	z_relative = 0;
	is_in_position = false;
	in_path_position = false;
	is_zhop = false;
	is_layer_change = false;
	is_height_change = false;
	is_height_increment_change = false;
	is_xy_travel = false;
	is_xyz_travel = false;
	has_xy_position_changed = false;
	has_position_changed = false;
	has_received_home_command = false;
	file_line_number = -1;
	gcode_number = -1;
	file_position = -1;
	gcode_ignored = true;
	is_in_bounds = true;
	current_tool = 0;
	p_extruders = NULL;
	set_num_extruders(extruder_count);
	
}

position::position(const position &pos)
{
	has_been_deleted = false;
	is_empty = pos.is_empty;
	feature_type_tag = pos.feature_type_tag;
	f = pos.f;
	f_null = pos.f_null;
	x = pos.x;
	x_null = pos.x_null;
	x_offset = pos.x_offset;
	x_firmware_offset = pos.x_firmware_offset;
	x_homed = pos.x_homed;
	y = pos.y;
	y_null = pos.y_null;
	y_offset = pos.y_offset;
	y_firmware_offset = pos.y_firmware_offset;
	y_homed = pos.y_homed;
	z = pos.z;
	z_null = pos.z_null;
	z_offset = pos.z_offset;
	z_firmware_offset = pos.z_firmware_offset;
	z_homed = pos.z_homed;
	is_relative = pos.is_relative;
	is_relative_null = pos.is_relative_null;
	is_extruder_relative = pos.is_extruder_relative;
	is_extruder_relative_null = pos.is_extruder_relative_null;
	is_metric = pos.is_metric;
	is_metric_null = pos.is_metric_null;
	last_extrusion_height = pos.last_extrusion_height;
	last_extrusion_height_null = pos.last_extrusion_height_null;
	layer = pos.layer;
	height = pos.height;
	height_increment = pos.height_increment;
	height_increment_change_count = pos.height_increment_change_count;
	is_printer_primed = pos.is_printer_primed;
	has_definite_position = pos.has_definite_position;
	z_relative = pos.z_relative;
	is_in_position = pos.is_in_position;
	in_path_position = pos.in_path_position;
	is_zhop = pos.is_zhop;
	is_layer_change = pos.is_layer_change;
	is_height_change = pos.is_height_change;
	is_height_increment_change = pos.is_height_increment_change;
	is_xy_travel = pos.is_xy_travel;
	is_xyz_travel = pos.is_xyz_travel;
	has_xy_position_changed = pos.has_xy_position_changed;
	has_position_changed = pos.has_position_changed;
	has_received_home_command = pos.has_received_home_command;
	file_line_number = pos.file_line_number;
	gcode_number = pos.gcode_number;
	file_position = pos.file_position;
	gcode_ignored = pos.gcode_ignored;
	is_in_bounds = pos.is_in_bounds;
	current_tool = pos.current_tool;
	p_extruders = NULL;
	command = pos.command;
	num_extruders = 0;
	set_num_extruders(pos.num_extruders);
	for(int index=0; index < pos.num_extruders; index++)
	{
		p_extruders[index] = pos.p_extruders[index];
	}
}

position::~position()
{
	if (has_been_deleted)
		return;
	has_been_deleted = true;
	delete_extruders();
}

position& position::operator=(const position& pos) {
	is_empty = pos.is_empty;
	feature_type_tag = pos.feature_type_tag;
	f = pos.f;
	f_null = pos.f_null;
	x = pos.x;
	x_null = pos.x_null;
	x_offset = pos.x_offset;
	x_firmware_offset = pos.x_firmware_offset;
	x_homed = pos.x_homed;
	y = pos.y;
	y_null = pos.y_null;
	y_offset = pos.y_offset;
	y_firmware_offset = pos.y_firmware_offset;
	y_homed = pos.y_homed;
	z = pos.z;
	z_null = pos.z_null;
	z_offset = pos.z_offset;
	z_firmware_offset = pos.z_firmware_offset;
	z_homed = pos.z_homed;
	is_relative = pos.is_relative;
	is_relative_null = pos.is_relative_null;
	is_extruder_relative = pos.is_extruder_relative;
	is_extruder_relative_null = pos.is_extruder_relative_null;
	is_metric = pos.is_metric;
	is_metric_null = pos.is_metric_null;
	last_extrusion_height = pos.last_extrusion_height;
	last_extrusion_height_null = pos.last_extrusion_height_null;
	layer = pos.layer;
	height = pos.height;
	height_increment = pos.height_increment;
	height_increment_change_count = pos.height_increment_change_count;
	is_printer_primed = pos.is_printer_primed;
	has_definite_position = pos.has_definite_position;
	z_relative = pos.z_relative;
	is_in_position = pos.is_in_position;
	in_path_position = pos.in_path_position;
	is_zhop = pos.is_zhop;
	is_layer_change = pos.is_layer_change;
	is_height_change = pos.is_height_change;
	is_height_increment_change = pos.is_height_increment_change;
	is_xy_travel = pos.is_xy_travel;
	is_xyz_travel = pos.is_xyz_travel;
	has_xy_position_changed = pos.has_xy_position_changed;
	has_position_changed = pos.has_position_changed;
	has_received_home_command = pos.has_received_home_command;
	file_line_number = pos.file_line_number;
	file_position = pos.file_position;
	gcode_number = pos.gcode_number;
	gcode_ignored = pos.gcode_ignored;
	is_in_bounds = pos.is_in_bounds;
	current_tool = pos.current_tool;
	command = pos.command;
	if (pos.num_extruders != num_extruders)
	{
		set_num_extruders(pos.num_extruders);
	}
	
	
	for (int index = 0; index < pos.num_extruders; index++)
	{
		p_extruders[index] = pos.p_extruders[index];
	}
	return *this;
}

bool position::is_travel()
{
	return is_xyz_travel || is_xy_travel;
}
void position::set_num_extruders(int num_extruders_)
{
	if (num_extruders_ == num_extruders)
		return;
	delete_extruders();
	num_extruders = num_extruders_;
	if (num_extruders_ > 0)
	{
		p_extruders = new extruder[num_extruders_];
	}
	else
	{
		throw std::exception();
	}
}

void position::delete_extruders()
{
	if (p_extruders != NULL)
	{
		delete[] p_extruders;
		p_extruders = NULL;
	}
}

double position::get_gcode_x() const
{
	return x - x_offset + x_firmware_offset;
}

double position::get_gcode_y() const
{
	return y - y_offset + y_firmware_offset;
}

double position::get_gcode_z() const
{
	return z - z_offset + z_firmware_offset;
}

extruder& position::get_current_extruder() const
{
	int tool_number = current_tool;
	if (current_tool > num_extruders-1)
		tool_number = num_extruders - 1;
	else if (current_tool < 0)
		tool_number = 0;
	return p_extruders[tool_number];
}

extruder& position::get_extruder(int index) const
{
	if (index >= num_extruders)
		index = num_extruders - 1;
	else if (index < 0)
		index = 0;
	return p_extruders[index];
}

void position::reset_state()
{
	is_layer_change = false;
	is_height_change = false;
	is_height_increment_change = false;
	is_xy_travel = false;
	is_xyz_travel = false;
	has_position_changed = false;
	has_received_home_command = false;
	gcode_ignored = true;
	
	//is_in_bounds = true; // I dont' think we want to reset this every time since it's only calculated if the current position
	// changes.
	p_extruders[current_tool].e_relative = 0;
	z_relative = 0;
	feature_type_tag = 0;
}
