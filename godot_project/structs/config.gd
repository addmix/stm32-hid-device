extends Resource
class_name Config

#TODO - maybe add a config identifier name/checksum so that the UI can automatically load the
# - config that is currently on the device? - currently the UI will automatically pull the config from the
# device, however it shows as a blank preset, which isn't the best UX

@export var pin_map : Dictionary[int, PinDeclaration] = {
	
}
func add_pin(pin : PinDeclaration) -> void:
	pin_map[pin.pin_name] = pin
func add_pins(pins : Array[PinDeclaration]) -> void:
	for pin : PinDeclaration in pins:
		add_pin(pin)

@export var pin_bindings : Array[InputMapping] = [
	
]
func add_input_mapping(binding : InputMapping) -> void:
	pin_bindings.append(binding)
func add_input_mappings(bindings : Array[InputMapping]) -> void:
	pin_bindings += bindings

@export var augmentations : Array[Augmentation] = [
	
]
func add_augmentation(aug : Augmentation) -> void:
	augmentations.append(aug)
func add_augmentations(augs : Array[Augmentation]) -> void:
	augmentations += augs

var byte_size : int:
	get:
		return Enums.SECTION_HEADER_BYTE_SIZE + get_pin_data_size() + Enums.SECTION_HEADER_BYTE_SIZE + get_mapping_data_size() + Enums.SECTION_HEADER_BYTE_SIZE + get_augment_data_size()

func to_bytes() -> PackedByteArray:
	var data_buffer := PackedByteArray()#u_int8_t data_buffer[data_length];
	
	data_buffer += get_pin_data()
	data_buffer += get_mapping_data() #get_mapping_data(data_buffer_index);
	data_buffer += get_augment_data() #get_augment_data(data_buffer_index);
	
	return data_buffer

#TODO: UNTESTED
static func from_bytes(bytes : PackedByteArray) -> Config:
	var config := Config.new()
	
	var array := bytes as Array
	
	#this logic should probably be reusable,
	#TODO: make some sort of ByteBuffer class to handle this stuff.
	
	var data_type : int = array.pop_front()
	print("data type=", data_type, "=", Enums.Commands.find_key(data_type))
	var section_length : int = (array.pop_front() << 8) + array.pop_front()
	print("section length=", section_length)
	print(array.slice(0, section_length))
	config.add_pins(PinDeclaration.from_byte_array(array.slice(0, section_length)))
	array = array.slice(section_length)
	
	#InputMapping
	data_type = array.pop_front()
	print("data type=", data_type, "=", Enums.Commands.find_key(data_type))
	section_length = (array.pop_front() << 8) + array.pop_front()
	print("section length=", section_length)
	print(array.slice(0, section_length))
	config.add_input_mappings(InputMapping.from_byte_array(array.slice(0, section_length)))
	array = array.slice(section_length)
	
	#Augmentation
	data_type = array.pop_front()
	print("data type=", data_type, "=", Enums.Commands.find_key(data_type))
	section_length = (array.pop_front() << 8) + array.pop_front()
	print("section length=", section_length)
	print(array.slice(0, section_length))
	config.add_augmentations(Augmentation.from_byte_array(array.slice(0, section_length)))
	array = array.slice(section_length)
	
	print("remaining bytes in buffer=", array)
	
	return config






func get_pin_data_size(index : int = 255) -> int:
	if not index == 255:
		return PinDeclaration.BYTE_SIZE
	
	return pin_map.size() * PinDeclaration.BYTE_SIZE

func get_pin_data(index : int = 255) -> PackedByteArray:
	var data := PackedByteArray()
	
	data.append(Enums.Commands.PinDeclaration)
	var data_size : int = get_pin_data_size(index)
	data.append(data_size >> 8 & 0xFF)#*return_buffer++ = data_size >> 8 & 0xFF; //this bitwise and may be redundant
	data.append(data_size & 0xFF)#*return_buffer++ = data_size & 0xFF; //this bitwise and may be redundant
	
	if not index == 255:
		var pin : PinDeclaration = pin_map.get(index, null)
		if pin:
			data += pin.to_bytes()
		else:
			var pin_bytes := PackedByteArray()
			pin_bytes.resize(PinDeclaration.BYTE_SIZE)
			data += pin_bytes
		
		return data
	
	for pin : PinDeclaration in pin_map.values():
		data += pin.to_bytes()
	
	return data

func get_mapping_data_size(index : int = 255) -> int:
	if not index == 255:
		return InputMapping.BYTE_SIZE
	
	return pin_bindings.size() * InputMapping.BYTE_SIZE

func get_mapping_data(index : int = 255) -> PackedByteArray:
	var data := PackedByteArray()
	
	data.append(Enums.Commands.InputMapping)
	var data_size : int = get_mapping_data_size(index)
	data.append(data_size >> 8 & 0xFF)#*return_buffer++ = data_size >> 8 & 0xFF; //this bitwise and may be redundant
	data.append(data_size & 0xFF)#*return_buffer++ = data_size & 0xFF; //this bitwise and may be redundant
	
	if not index == 255:
		var mapping : InputMapping = pin_bindings.get(index)
		if mapping:
			data += mapping.to_bytes()
		else:
			var pin_bytes := PackedByteArray()
			pin_bytes.resize(InputMapping.BYTE_SIZE)
			data += pin_bytes
		
		return data
	
	for mapping : InputMapping in pin_bindings:
		data += mapping.to_bytes()
	
	return data

func get_augment_data_size(index : int = 255) -> int:
	if not index == 255:
		return Augmentation.BYTE_SIZE
	
	return augmentations.size() * Augmentation.BYTE_SIZE

func get_augment_data(index : int = 255) -> PackedByteArray:
	var data := PackedByteArray()
	
	data.append(Enums.Commands.Augmentation)
	var data_size : int = get_augment_data_size(index)
	data.append(data_size >> 8 & 0xFF)#*return_buffer++ = data_size >> 8 & 0xFF; //this bitwise and may be redundant
	data.append(data_size & 0xFF)#*return_buffer++ = data_size & 0xFF; //this bitwise and may be redundant
	
	if not index == 255:
		var augment : Augmentation = augmentations.get(index)
		if augment:
			data += augment.to_bytes()
		else:
			var pin_bytes := PackedByteArray()
			pin_bytes.resize(Augmentation.BYTE_SIZE)
			data += pin_bytes
		
		return data
	
	for augment : Augmentation in augmentations:
		data += augment.to_bytes()
	
	return data
