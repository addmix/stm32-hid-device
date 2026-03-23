class_name SerialCommandBuilder

static func create_command(command : int, data : Array = []) -> PackedByteArray:
	var byte_array = PackedByteArray([0xFF, 0x00]) #starting bits
	
	var payload_array = PackedByteArray([command] + data)
	
	#payload length
	var length_byte_1 : int = payload_array.size() >> 8 & 0xFF
	var length_byte_2 : int = payload_array.size() & 0xFF
	
	byte_array.append(length_byte_1)
	byte_array.append(length_byte_2)
	
	byte_array += payload_array
	
	#checksum
	var sum : int = 0
	for byte in payload_array:
		sum += byte
	
	var checksum : int = sum & 0xFF
	
	byte_array.append(checksum)
	
	return byte_array

static func receive_command(data : PackedByteArray) -> void:
	pass #TODO
