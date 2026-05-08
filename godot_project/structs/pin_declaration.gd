extends Resource
class_name PinDeclaration

@export var pin_name : int = Enums.Pins.NC #TODO: set min/max for this
@export var analog : bool = false
@export var inactive_value : int = 0x1 #HIGH  #TODO: set min/max for this

func _init(_pin_name : int = Enums.Pins.NC, _analog : bool = false, _inactive_value : int = 0x1) -> void:
	pin_name = _pin_name
	analog = _analog
	inactive_value = _inactive_value

#serialization functions

const BYTE_SIZE = 4 #make sure this matches the #declare in .h

func to_bytes() -> PackedByteArray:
	return PackedByteArray([
		pin_name & 0xFF,
		analog,
		inactive_value >> 8 & 0xFF,
		inactive_value & 0xFF,
	])

static func from_bytes(bytes : PackedByteArray) -> PinDeclaration:
	if bytes.size() != BYTE_SIZE:
		push_error("Passed byte array does not match the byte size of PinDeclaration.")
	
	var _pin_name : int = bytes[0]
	var _analog : bool = bytes[1]
	var _inactive_value : int = (bytes[2] << 8) + bytes[3]
	
	return PinDeclaration.new(_pin_name, _analog, _inactive_value)

static func from_byte_array(bytes : PackedByteArray) -> Array[PinDeclaration]:
	var array : Array[PinDeclaration] = []
	
	for index in range(0, bytes.size(), BYTE_SIZE):
		array.append(from_bytes(bytes.slice(index, index + BYTE_SIZE)))
	
	return array

func _to_string() -> String:
	return "PinDeclaration(pin_name=%s, analog=%s, inactive_value=%s)" % [pin_name, analog, inactive_value]
