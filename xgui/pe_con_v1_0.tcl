# Definitional proc to organize widgets for parameters.
proc init_gui { IPINST } {
  ipgui::add_param $IPINST -name "Component_Name"
  #Adding Page
  set Page_0 [ipgui::add_page $IPINST -name "Page 0"]
  ipgui::add_param $IPINST -name "DATA_WIDTH" -parent ${Page_0}
  ipgui::add_param $IPINST -name "L_RAM_SIZE" -parent ${Page_0}
  ipgui::add_param $IPINST -name "R_RAM_SIZE" -parent ${Page_0}
  ipgui::add_param $IPINST -name "VECTOR_SIZE" -parent ${Page_0}


}

proc update_PARAM_VALUE.DATA_WIDTH { PARAM_VALUE.DATA_WIDTH } {
	# Procedure called to update DATA_WIDTH when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.DATA_WIDTH { PARAM_VALUE.DATA_WIDTH } {
	# Procedure called to validate DATA_WIDTH
	return true
}

proc update_PARAM_VALUE.L_RAM_SIZE { PARAM_VALUE.L_RAM_SIZE } {
	# Procedure called to update L_RAM_SIZE when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.L_RAM_SIZE { PARAM_VALUE.L_RAM_SIZE } {
	# Procedure called to validate L_RAM_SIZE
	return true
}

proc update_PARAM_VALUE.R_RAM_SIZE { PARAM_VALUE.R_RAM_SIZE } {
	# Procedure called to update R_RAM_SIZE when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.R_RAM_SIZE { PARAM_VALUE.R_RAM_SIZE } {
	# Procedure called to validate R_RAM_SIZE
	return true
}

proc update_PARAM_VALUE.VECTOR_SIZE { PARAM_VALUE.VECTOR_SIZE } {
	# Procedure called to update VECTOR_SIZE when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.VECTOR_SIZE { PARAM_VALUE.VECTOR_SIZE } {
	# Procedure called to validate VECTOR_SIZE
	return true
}


proc update_MODELPARAM_VALUE.DATA_WIDTH { MODELPARAM_VALUE.DATA_WIDTH PARAM_VALUE.DATA_WIDTH } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.DATA_WIDTH}] ${MODELPARAM_VALUE.DATA_WIDTH}
}

proc update_MODELPARAM_VALUE.VECTOR_SIZE { MODELPARAM_VALUE.VECTOR_SIZE PARAM_VALUE.VECTOR_SIZE } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.VECTOR_SIZE}] ${MODELPARAM_VALUE.VECTOR_SIZE}
}

proc update_MODELPARAM_VALUE.L_RAM_SIZE { MODELPARAM_VALUE.L_RAM_SIZE PARAM_VALUE.L_RAM_SIZE } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.L_RAM_SIZE}] ${MODELPARAM_VALUE.L_RAM_SIZE}
}

proc update_MODELPARAM_VALUE.R_RAM_SIZE { MODELPARAM_VALUE.R_RAM_SIZE PARAM_VALUE.R_RAM_SIZE } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.R_RAM_SIZE}] ${MODELPARAM_VALUE.R_RAM_SIZE}
}

