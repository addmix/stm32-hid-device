extends Resource
class_name InputMapping

@export var pin_name : int = Enums.Pins.NC #TODO: set min/max for this
@export var input_type : int = 0
@export var input_id : int = 0
@export var invert : bool = false
@export var max_report_value : int = 255
@export var activation_value : int = 255
@export var scale : float = float(max_report_value) / 1024.0 + 0.02
@export var deadzone : int = int(float(max_report_value) * 0.04)
@export var change_amount_before_update : int = int(float(max_report_value) * 0.01)
@export var quick_release : bool = false #unimplemented
@export var counter_strafe_help_time_ms : int = 0 #unimplemented

func _init(
	_pin_name : int = pin_name, 
	_input_type : int = input_type,
	_input_id : int = input_id,
	_invert : bool = invert,
	_max_report_value : int = max_report_value,
	_activation_value : int = activation_value,
	_scale : float = scale,
	_deadzone : int = deadzone,
	_change_amount_before_update : int = change_amount_before_update,
	_quick_release : bool = quick_release,
	_counter_strafe_help_time_ms : int = counter_strafe_help_time_ms,
	) -> void:
	pin_name = _pin_name
	input_type = _input_type
	input_id = _input_id
	invert = _invert
	max_report_value = _max_report_value
	activation_value = _activation_value
	scale = _scale
	deadzone = _deadzone
	change_amount_before_update = _change_amount_before_update
	quick_release = _quick_release
	counter_strafe_help_time_ms = _counter_strafe_help_time_ms

#serialization functions

const BYTE_SIZE = 17 #make sure this matches the #declare in .h

func to_bytes() -> PackedByteArray:
	var scale_bytes = PackedByteArray([0, 0, 0, 0])
	scale_bytes.encode_float(0, scale)
	
	return PackedByteArray([
		pin_name & 0xFF,
		input_type & 0xFF,
		input_id >> 8 & 0xFF,
		input_id & 0xFF,
		invert,
		max_report_value >> 8 & 0xFF,
		max_report_value & 0xFF,
		activation_value >> 8 & 0xFF,
		activation_value & 0xFF,
		scale_bytes[3],
		scale_bytes[2],
		scale_bytes[1], #TODO: make sure these bytes are in the correct order
		scale_bytes[0],
		#*return_buffer++ = scale_bits >> 24 & 0xFF;
		#*return_buffer++ = scale_bits >> 16 & 0xFF;
		#*return_buffer++ = scale_bits >> 8 & 0xFF;
		#*return_buffer++ = scale_bits & 0xFF;
		deadzone & 0xFF,
		change_amount_before_update & 0xFF,
		quick_release,
		counter_strafe_help_time_ms & 0xFF,
	])

static func from_bytes(bytes : PackedByteArray) -> InputMapping:
	if bytes.size() != BYTE_SIZE:
		push_error("Passed byte array does not match the byte size of PinDeclaration.")
	
	var _pin_name : int = bytes[0]
	var _input_type : int = bytes[1]
	var _input_id : int = (bytes[2] << 8) + bytes[3]
	var _invert : bool = bytes[4]
	var _max_report_value : int = (bytes[5] << 8) + bytes[6]
	var _activation_value : int = (bytes[7] << 8) + bytes[8]
	
	var scale_bytes := bytes.slice(9, 13)
	scale_bytes.reverse()
	var _scale : float = scale_bytes.decode_float(0) #10, 11, 12 #TODO: verify this is correct
	var _deadzone : int = bytes[13]
	var _change_amount_before_update : int = bytes[14]
	var _quick_release : bool = bytes[15]
	var _counter_strafe_help_time_ms : int = bytes[16]
	
	return InputMapping.new(_pin_name, _input_type, _input_id, _invert, _max_report_value,\
	_activation_value, _scale, _deadzone, _change_amount_before_update, _quick_release, \
	_counter_strafe_help_time_ms)

static func from_byte_array(bytes : PackedByteArray) -> Array[InputMapping]:
	var array : Array[InputMapping] = []
	
	for index in range(0, bytes.size(), BYTE_SIZE):
		array.append(from_bytes(bytes.slice(index, index + BYTE_SIZE)))
	
	return array

func _to_string() -> String:
	return "InputMapping(pin_name=%s, input_type=%s, input_id=%s, invert=%s, max_report_value=%s, \
	activation_value=%s, scale=%s, deadzone=%s, change_amount_before_update=%s, quick_release=%s, \
	counter_strafe_help_time_ms=%s)" % [pin_name, input_type, input_id, invert, max_report_value, \
	activation_value, scale, deadzone, change_amount_before_update, quick_release, counter_strafe_help_time_ms]
