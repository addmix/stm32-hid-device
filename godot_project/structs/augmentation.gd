extends Resource
class_name Augmentation

enum AugmentType{
	NONE,
	ROTATION,
};

@export var pin : int = Enums.Pins.NC #TODO: set min/max for this
@export var secondary_pin : int = Enums.Pins.NC
@export var type : int = AugmentType.NONE
@export var control_rotation : float = 0.0

func _init(
	_pin : int = pin,
	_secondary_pin : int = secondary_pin,
	_type : bool = type,
	_control_rotation : float = control_rotation
	) -> void:
	pin = _pin
	secondary_pin = _secondary_pin
	type = _type
	control_rotation = _control_rotation

#serialization functions

const BYTE_SIZE = 7 #make sure this matches the #declare in .h

func to_bytes() -> PackedByteArray:
	var rotation_bytes = PackedByteArray([0, 0, 0, 0])
	rotation_bytes.encode_float(0, control_rotation)
	
	return PackedByteArray([
		pin & 0xFF,
		secondary_pin & 0xFF,
		type & 0xFF,
		
		rotation_bytes[3],
		rotation_bytes[2],
		rotation_bytes[1], #TODO: make sure these bytes are in the correct order
		rotation_bytes[0],
	])

func _to_string() -> String:
	return "Augmentation(pin=%s, secondary_pin=%s, type=%s, control_rotation=%s)" % \
	[pin, secondary_pin, type, control_rotation]


static func from_bytes(bytes : PackedByteArray) -> Augmentation:
	if bytes.size() != BYTE_SIZE:
		push_error("Passed byte array does not match the byte size of Augmentation.")
	
	var _pin : int = bytes[0]
	var _secondary_pin : bool = bytes[1]
	var _type : int = bytes[2]
	
	var rotation_bytes := bytes.slice(3, 7)
	rotation_bytes.reverse()
	var _control_rotation : float = rotation_bytes.decode_float(0)
	
	return Augmentation.new(_pin, _secondary_pin, _type, _control_rotation)

static func from_byte_array(bytes : PackedByteArray) -> Array[Augmentation]:
	var array : Array[Augmentation] = []
	
	for index in range(0, bytes.size(), BYTE_SIZE):
		array.append(from_bytes(bytes.slice(index, index + BYTE_SIZE)))
	
	return array
